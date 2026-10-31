#pragma once

#include <string>
#include <jni.h>

namespace ImGuiIME {
    void Init(JavaVM *vm);

    bool InputText(const char *label, char *buf, int bufSize, float width = -1.0f);

    void HideKeyboard();

    void CheckExternalClick(float clickX, float clickY);

    void ResetText(char *buf, int bufSize);

    void Shutdown();
} // namespace ImGuiIME