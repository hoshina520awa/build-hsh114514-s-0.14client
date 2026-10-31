#pragma once
#include <android/native_window.h>
#include <android/input.h>

void GL_Hook_Init();
bool ImGui_HandleInput(AInputEvent *event);