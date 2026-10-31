#include "imgui.h"
#include "imgui_Input_android.h"
#include <time.h>
#include <android/native_window.h>
#include <android/input.h>
#include <android/keycodes.h>


int32_t ImGui_ImplLinux_HandleInputEvent(ImGuInput_LinuxEvent event) {
    ImGuiIO &io = ImGui::GetIO();
    if (event.Action_type == IM_DOWN || event.Action_type == IM_UP) {
        io.MouseDown[0] = (event.Action_type == IM_DOWN);
        io.MousePos = event.pos;
    } else {
        io.MousePos = event.pos;
    }
    return 0;
}
