#include "ImguiInput.h"
#include <android/native_activity.h>
#include <android_native_app_glue.h>
#include <android/input.h>
#include <android/log.h>
#include <dlfcn.h>
#include "main.h"
#include "MinecraftGame.h"
#include "imgui_impl_android.h"
#include "ImguiManager.h"

int32_t (*orig_AInputQueue_getEvent)(AInputQueue *queue, AInputEvent **outEvent) = nullptr;
int32_t my_AInputQueue_getEvent(AInputQueue *queue, AInputEvent **outEvent) {
    int32_t result = orig_AInputQueue_getEvent(queue, outEvent);
    if (result >= 0 && outEvent != nullptr && *outEvent != nullptr) {
        AInputEvent *event = *outEvent;
        bool handled_by_imgui = ImGui_HandleInput(event);
        if (handled_by_imgui) {
            AInputQueue_finishEvent(queue, event, 1);
            return -1;
        }
    }
    return result;
}


void initImGuiInput() {
    void *AInputQueue_getEvent = Main::findSymbol2("AInputQueue_getEvent", "libandroid.so");
    if (AInputQueue_getEvent) {
        shadowhook_hook_func_addr(AInputQueue_getEvent, (void *) my_AInputQueue_getEvent,
                                  (void **) &orig_AInputQueue_getEvent);
    }
}
