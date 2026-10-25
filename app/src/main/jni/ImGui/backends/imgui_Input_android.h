#pragma once
#include "imgui.h"

#ifndef IM_DOWN
#define IM_DOWN 0 //按下
#endif
#ifndef IM_MOVE
#define IM_MOVE 2 //移动
#endif
#ifndef IM_UP
#define IM_UP 1   //放开
#endif


struct ImGuInput_LinuxEvent {
    int Action_type;
    int fingerIndex;
    ImVec2 pos;
};

IMGUI_IMPL_API int32_t ImGui_ImplLinux_HandleInputEvent(ImGuInput_LinuxEvent event);
