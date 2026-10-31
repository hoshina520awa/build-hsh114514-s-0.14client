#include "ImguiManager.h"
#include "imgui.h"
#include "main.h"
#include "backends/imgui_impl_android.h"
#include "backends/imgui_impl_opengl3.h"
#include "font/font.h"
#include "MinecraftGame.h"
#include "Minecraft.h"
#include "LevelData.h"
#include "ImGuiInputIME.h"
#include <mutex>
#include <thread>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <jni.h>
#include <android/input.h>
#include <android/native_window_jni.h>
#include <android/sensor.h>
#include "xhook/xhook.h"

// ==========================================
// 引入 Spine 头文件和外部变量声明
// ==========================================
#include <spine/spine.h>
#include <spine/SkeletonRenderer.h>

extern spine::Skeleton* g_skeleton;
extern spine::AnimationState* g_state;
extern spine::SkeletonRenderer* g_renderer;
// ==========================================


int imgui_Width = 0;
int imgui_Height = 0;

std::mutex imgui_ImGuiMutex;

void ImGui_ImplAndroid_UpdateDeltaTime() {
    static double g_Time = 0.0;
    ImGuiIO& io = ImGui::GetIO();
    struct timespec current_timepiece {};
    clock_gettime(CLOCK_MONOTONIC, &current_timepiece);
    double current_time = (double)(current_timepiece.tv_sec) + (current_timepiece.tv_nsec / 1000000000.0);
    io.DeltaTime = g_Time > 0.0 ? (float)(current_time - g_Time) : (float)(1.0f / 60.0f);
    g_Time = current_time;
}


using eglSwapBuffers_ = EGLBoolean(*)(EGLDisplay display, EGLSurface surface);
static eglSwapBuffers_ o_eglSwapBuffers = nullptr;

static EGLBoolean f_eglSwapBuffers(EGLDisplay display, EGLSurface surface)
{
    static bool init = false;
    static EGLDisplay lastDisplay = nullptr;
    static EGLSurface lastSurface = nullptr;
    static EGLContext lastContext = nullptr;

    EGLContext currentContext = eglGetCurrentContext();
    if (currentContext == EGL_NO_CONTEXT) {
        if (o_eglSwapBuffers == nullptr) o_eglSwapBuffers = (eglSwapBuffers_)dlsym(RTLD_NEXT, "eglSwapBuffers");
        return o_eglSwapBuffers ? o_eglSwapBuffers(display, surface) : EGL_FALSE;
    }

    if (currentContext != lastContext) {
        if (init) {
            std::lock_guard<std::mutex> lock(imgui_ImGuiMutex);
            ImGui_ImplOpenGL3_Shutdown();
            if (ImGui::GetCurrentContext() != nullptr) {
                ImGui::DestroyContext();
            }
            init = false;
        }
        lastContext = currentContext;
    }

    if (!init) {
        std::lock_guard<std::mutex> lock(imgui_ImGuiMutex);
        if (ImGui::GetCurrentContext() == nullptr) {
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO &io = ImGui::GetIO();
            io.IniFilename = nullptr;
            io.LogFilename = nullptr;
            io.BackendPlatformName = "mcpe_manual_hook";

            ImGui::StyleColorsDark();
            ImGuiStyle *style = &ImGui::GetStyle();
            style->FramePadding = ImVec2(5, 5);
            style->WindowRounding = 3.0f;
            style->ScrollbarRounding = 2.0f;
            style->FrameRounding = 2.0f;
            style->GrabRounding = 2.0f;
            style->ScaleAllSizes(2.3f);

            ImFontConfig fontConf;
            fontConf.FontDataOwnedByAtlas = false;
            fontConf.OversampleH = 2;
            fontConf.OversampleV = 2;
            fontConf.PixelSnapH = true;

            io.Fonts->AddFontFromMemoryTTF((void *) OPPOSans_H, (int) OPPOSans_H_size, 32.0f,
                                           &fontConf, io.Fonts->GetGlyphRangesChineseFull());
            io.Fonts->Build();
        }
        ImGui_ImplOpenGL3_Init("#version 300 es");
        init = true;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &imgui_Width);
    eglQuerySurface(display, surface, EGL_HEIGHT, &imgui_Height);

    if (imgui_Width > 0 && imgui_Height > 0) {
        std::lock_guard<std::mutex> lock(imgui_ImGuiMutex);

        ImGuiIO &io = ImGui::GetIO();
        io.DisplaySize = ImVec2((float)imgui_Width, (float)imgui_Height);
        io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

        ImGui_ImplAndroid_UpdateDeltaTime();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SetNextWindowSize(ImVec2(400, 300), ImGuiCond_FirstUseEver);
        ImGui::Begin("MCPE Mod Menu");
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::Text("Resolution: %dx%d", imgui_Width, imgui_Height);
        
        if (g_skeleton) {
            ImGui::TextColored(ImVec4(0, 1, 0, 1), "Spine: 已加载");
        } else {
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Spine: 未加载");
        }

        static char text[32] = "Text";
        if (ImGuiIME::InputText("Input", text, sizeof(text))) {
            LOGD("Input: %s", text);
        }

        if (ImGui::Button("收起键盘")) {
            ImGuiIME::HideKeyboard();
        }

        if (ImGui::Button("Test")) {
            auto* mcc = MinecraftClient::getInstance();
            auto* guiData = mcc->getGuiData();
            if (guiData) guiData->displayClientMessage("Hello World");
        }

        ImGui::End();

        // ==========================================
        // Spine 渲染
        // ==========================================
        if (g_skeleton && g_state && g_renderer) {
            // 1. 更新动画
            g_state->update(0.016f);
            g_state->apply(*g_skeleton);
            
            // 修正报错1：新版需要指定 Physics 参数
            g_skeleton->updateWorldTransform(spine::Physics_Update);

            // 2. 保存当前状态
            GLboolean depth_test = glIsEnabled(GL_DEPTH_TEST);
            glDisable(GL_DEPTH_TEST); 
            glViewport(0, 0, imgui_Width, imgui_Height);

            // 修正报错2：新版把 draw 改成了 render
            g_renderer->render(*g_skeleton);

            // 3. 恢复状态
            if (depth_test) glEnable(GL_DEPTH_TEST);
        }
        // ==========================================

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }

    if (o_eglSwapBuffers == nullptr) {
        o_eglSwapBuffers = (eglSwapBuffers_)dlsym(RTLD_NEXT, "eglSwapBuffers");
    }

    if (o_eglSwapBuffers != nullptr) {
        return o_eglSwapBuffers(display, surface);
    }

    return EGL_FALSE;
}


static bool s_TouchCapturedByImGui = false;

bool ImGui_HandleInput(AInputEvent *event) {
    std::lock_guard<std::mutex> lock(imgui_ImGuiMutex);
    if (ImGui::GetCurrentContext() != nullptr) {
        ImGui_ImplAndroid_HandleInputEvent(event);
        if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
            int32_t action = AMotionEvent_getAction(event) & AMOTION_EVENT_ACTION_MASK;
            if (action == AMOTION_EVENT_ACTION_DOWN) {
                s_TouchCapturedByImGui = ImGui::GetIO().WantCaptureMouse;
            } else if (action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_CANCEL) {
                bool ret = s_TouchCapturedByImGui;
                s_TouchCapturedByImGui = false;
                return ret;
            }
        }
        return s_TouchCapturedByImGui;
    }
    return false;
}

void *main_thread(void *) {
    xhook_register("libminecraftpe.so", "eglSwapBuffers", (void *)f_eglSwapBuffers,
                   (void **)&o_eglSwapBuffers);
    xhook_refresh(1);
    pthread_exit(nullptr);
}

void GL_Hook_Init() {
    pthread_t t;
    pthread_create(&t, nullptr, main_thread, nullptr);
}
