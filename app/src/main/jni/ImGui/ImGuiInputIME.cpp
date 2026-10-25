#include "ImGuiInputIME.h"

#include "imgui.h"
#include "main.h"
#include <cstring>
#include <android/log.h>


namespace ImGuiIME {

    static JavaVM* g_vm = nullptr;
    static jclass g_cls = nullptr;
    static jmethodID g_showKeyboard = nullptr;
    static jmethodID g_hideKeyboard = nullptr;
    static jmethodID g_getInputText = nullptr;
    static jmethodID g_isKeyboardVisible = nullptr;
    static jmethodID g_copyToClipboard = nullptr;
    static jmethodID g_pasteFromClipboard = nullptr;

    static bool g_initialized = false;
    static ImGuiID g_activeInputID = 0;
    static ImVec2 g_inputBoxPos = ImVec2(0, 0);
    static ImVec2 g_inputBoxSize = ImVec2(0, 0);
    static double g_pressStartTime = 0;
    static bool g_isPointerDown = false;

    static JNIEnv* GetEnv() {
        JNIEnv* env = nullptr;
        if (g_vm) {
            int status = g_vm->GetEnv((void**)&env, JNI_VERSION_1_6);
            if (status < 0) {
                g_vm->AttachCurrentThread(&env, nullptr);
            }
        }
        return env;
    }

    static void CopyToClipboard(const std::string& text) {
        JNIEnv* env = GetEnv();
        if (env && g_cls && g_copyToClipboard) {
            jstring jtext = env->NewStringUTF(text.c_str());
            env->CallStaticVoidMethod(g_cls, g_copyToClipboard, jtext);
            env->DeleteLocalRef(jtext);
        }
    }

    static std::string PasteFromClipboard() {
        JNIEnv* env = GetEnv();
        if (env && g_cls && g_pasteFromClipboard) {
            auto result = (jstring)env->CallStaticObjectMethod(g_cls, g_pasteFromClipboard);
            if (!result) return "";
            const char* str = env->GetStringUTFChars(result, nullptr);
            std::string ret(str ? str : "");
            env->ReleaseStringUTFChars(result, str);
            env->DeleteLocalRef(result);
            return ret;
        }
        return "";
    }

    static void CallShowKeyboard(const std::string& text) {
        JNIEnv* env = GetEnv();
        if (!env || !g_cls || !g_showKeyboard) {
            LOGE("CallShowKeyboard failed: env=%p, cls=%p, method=%p", env, g_cls, g_showKeyboard);
            return;
        }
        LOGD("Calling showKeyboard with text: '%s'", text.c_str());
        jstring jtext = env->NewStringUTF(text.c_str());
        env->CallStaticVoidMethod(g_cls, g_showKeyboard, jtext);
        if (env->ExceptionCheck()) {
            LOGE("showKeyboard threw exception");
            env->ExceptionClear();
        }
        env->DeleteLocalRef(jtext);
    }

    static void CallHideKeyboard() {
        JNIEnv* env = GetEnv();
        if (!env || !g_cls || !g_hideKeyboard) return;
        LOGD("Calling hideKeyboard");
        env->CallStaticVoidMethod(g_cls, g_hideKeyboard);
    }

    static std::string CallGetText() {
        JNIEnv* env = GetEnv();
        if (!env || !g_cls || !g_getInputText) return "";
        auto result = (jstring)env->CallStaticObjectMethod(g_cls, g_getInputText);
        if (!result) return "";
        const char* str = env->GetStringUTFChars(result, nullptr);
        std::string ret(str);
        env->ReleaseStringUTFChars(result, str);
        env->DeleteLocalRef(result);
        return ret;
    }

    static bool CallIsVisible() {
        JNIEnv* env = GetEnv();
        if (!env || !g_cls || !g_isKeyboardVisible) return false;
        return (bool)env->CallStaticBooleanMethod(g_cls, g_isKeyboardVisible);
    }

    void Init(JavaVM* vm) {
        if (g_initialized) return;
        g_vm = vm;

        JNIEnv* env = GetEnv();
        if (!env) return;

        jclass localCls = env->FindClass("com/irene/algui/IMEHelper");
        if (!localCls) {
            env->ExceptionClear();
            LOGE("Failed to find IMEHelper class");
            return;
        }

        g_cls = (jclass)env->NewGlobalRef(localCls);
        env->DeleteLocalRef(localCls);

        g_showKeyboard = env->GetStaticMethodID(g_cls, "showKeyboard", "(Ljava/lang/String;)V");
        g_hideKeyboard = env->GetStaticMethodID(g_cls, "hideKeyboard", "()V");
        g_getInputText = env->GetStaticMethodID(g_cls, "getInputText", "()Ljava/lang/String;");
        g_isKeyboardVisible = env->GetStaticMethodID(g_cls, "isKeyboardVisible", "()Z");

        g_copyToClipboard = env->GetStaticMethodID(g_cls, "copyToClipboard", "(Ljava/lang/String;)V");
        g_pasteFromClipboard = env->GetStaticMethodID(g_cls, "pasteFromClipboard", "()Ljava/lang/String;");

        if (!g_showKeyboard || !g_hideKeyboard || !g_getInputText || !g_isKeyboardVisible || !g_copyToClipboard || !g_pasteFromClipboard) {
            LOGE("Failed to get method IDs");
            return;
        }

        g_initialized = true;
        LOGD("IME initialized successfully");
    }

    bool InputText(const char* label, char* buf, int bufSize, float width) {
        ImGuiIO& io = ImGui::GetIO();
        ImGuiID id = ImGui::GetID(label);
        io.MouseDoubleClickTime = 0.55f;
        io.MouseDoubleClickMaxDist = 20.0f;
        bool isActive = (g_activeInputID == id);
        bool value_changed = false;

        if (width > 0) ImGui::PushItemWidth(width);

        ImVec2 pos = ImGui::GetCursorScreenPos();
        float w = (width > 0) ? width : ImGui::GetContentRegionAvail().x;
        float h = ImGui::GetFrameHeight();

        ImDrawList* drawList = ImGui::GetWindowDrawList();
        ImU32 bgColor = ImGui::GetColorU32(isActive ? ImGuiCol_FrameBgActive : ImGuiCol_FrameBg);
        ImU32 borderColor = ImGui::GetColorU32(ImGuiCol_Border);

        drawList->AddRectFilled(pos, ImVec2(pos.x + w, pos.y + h), bgColor);
        drawList->AddRect(pos, ImVec2(pos.x + w, pos.y + h), borderColor);

        ImVec2 padding = ImGui::GetStyle().FramePadding;
        ImVec2 textPos = ImVec2(pos.x + padding.x, pos.y + padding.y);

        bool isEmpty = (buf[0] == '\0');
        if (isEmpty) {
            drawList->AddText(textPos, IM_COL32(128, 128, 128, 255), "点击输入...");
        } else {
            drawList->AddText(textPos, ImGui::GetColorU32(ImGuiCol_Text), buf);
        }

        if (isActive) {
            float cursorX = textPos.x + ImGui::GetFont()->CalcTextSizeA(
                    ImGui::GetFontSize(), FLT_MAX, 0, buf).x;
            if (fmod(ImGui::GetTime(), 1.0f) < 0.5f) {
                drawList->AddLine(
                        ImVec2(cursorX + 1, pos.y + padding.y),
                        ImVec2(cursorX + 1, pos.y + h - padding.y),
                        ImGui::GetColorU32(ImGuiCol_Text), 1.0f);
            }
        }

        bool clicked = ImGui::InvisibleButton(label, ImVec2(w, h));
        bool hovered = ImGui::IsItemHovered();
        bool active = ImGui::IsItemActive();

        char popupLabel[64];
        snprintf(popupLabel, sizeof(popupLabel), "##IME_Menu_%u", id);

        ImVec2 clickPos = ImGui::GetIO().MousePos;

        bool isDoubleClicked = hovered && ImGui::IsMouseDoubleClicked(0);

        bool isLongPressed = false;
        if (active && hovered) {
            if (ImGui::IsMouseClicked(0)) {
                g_pressStartTime = ImGui::GetTime();
                g_isPointerDown = true;
            }
            if (g_isPointerDown) {
                double elapsed = ImGui::GetTime() - g_pressStartTime;
                if (elapsed >= 0.35f) {
                    g_isPointerDown = false;
                    isLongPressed = true;
                }
            }
        } else {
            g_isPointerDown = false;
        }

        if (isDoubleClicked || isLongPressed) {
            LOGD("PopupMenu (DoubleClicked: %d, LongPressed: %d)", isDoubleClicked, isLongPressed);
            ImGui::OpenPopup(popupLabel);
        }

        if (clicked && !isDoubleClicked) {
            if (g_activeInputID != 0 && g_activeInputID != id) {
                CallHideKeyboard();
            }
            g_activeInputID = id;
            g_inputBoxPos = pos;
            g_inputBoxSize = ImVec2(w, h);

            CallShowKeyboard(std::string(buf));
        }

        ImGui::SetNextWindowPos(ImVec2(clickPos.x - 50, clickPos.y - 100), ImGuiCond_Appearing);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(10, 0));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 8.0f);

        if (ImGui::BeginPopup(popupLabel)) {
            ImVec2 btnSize(100.0f, 75.0f);

            if (ImGui::Button("复制", btnSize)) {
                CopyToClipboard(std::string(buf));
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button("剪切", btnSize)) {
                CopyToClipboard(std::string(buf));
                buf[0] = '\0';
                CallShowKeyboard("");
                value_changed = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();

            if (ImGui::Button("粘贴", btnSize)) {
                std::string pasteText = PasteFromClipboard();
                if (!pasteText.empty()) {
                    strncat(buf, pasteText.c_str(), bufSize - strlen(buf) - 1);
                    CallShowKeyboard(std::string(buf));
                    value_changed = true;
                }
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }
        ImGui::PopStyleVar(3);

        if (g_activeInputID == id) {
            if (!clicked) {
                std::string imeText = CallGetText();
                if (imeText != buf) {
                    strncpy(buf, imeText.c_str(), bufSize - 1);
                    buf[bufSize - 1] = '\0';
                    value_changed = true;
                }
            }

            bool isVisible = CallIsVisible();
            if (!isVisible && g_activeInputID == id) {
                g_activeInputID = 0;
            }
        }

        if (width > 0) ImGui::PopItemWidth();

        return value_changed;
    }

    void HideKeyboard() {
        if (g_activeInputID != 0) {
            LOGD("HideKeyboard!");
            CallHideKeyboard();
            g_activeInputID = 0;
        }
    }

    void ResetText(char* buf, int bufSize) {
        if (buf) {
            memset(buf, 0, bufSize);
        }

        JNIEnv* env = GetEnv();
        if (env && g_cls && g_showKeyboard) {
            jstring jtext = env->NewStringUTF("");
            env->CallStaticVoidMethod(g_cls, g_showKeyboard, jtext);
            env->DeleteLocalRef(jtext);
        }
    }


    void CheckExternalClick(float clickX, float clickY) {
        if (g_activeInputID == 0) return;

        bool isInsideBox = (clickX >= g_inputBoxPos.x &&
                            clickX <= g_inputBoxPos.x + g_inputBoxSize.x &&
                            clickY >= g_inputBoxPos.y &&
                            clickY <= g_inputBoxPos.y + g_inputBoxSize.y);

        if (!isInsideBox) {
            LOGD("在区域外!,点击区域(%.0f, %.0f), 输入框位置(%.0f,%.0f,%.0f,%.0f)",
                 clickX, clickY, g_inputBoxPos.x, g_inputBoxPos.y, g_inputBoxSize.x, g_inputBoxSize.y);
            HideKeyboard();
        } else {
            LOGD("在区域内!,点击区域(%.0f, %.0f)", clickX, clickY);
        }
    }

    void Shutdown() {
        HideKeyboard();
        JNIEnv* env = GetEnv();
        if (env && g_cls) {
            env->DeleteGlobalRef(g_cls);
            g_cls = nullptr;
        }
        g_vm = nullptr;
        g_initialized = false;
    }

} // namespace ImGuiIME