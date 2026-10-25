#pragma once

#include <bits/stringfwd.h>
#include <cstdint>

class InputHandler;
class Options;
class LocalPlayer;

class MoveInputHandler {
public:
    void** vtable;
    float x;
    float y;
    char padding_0[56-8];
    bool isAutoJumping;    // +60
    bool isSneaking;      // +61
    bool isJumping;       // +62
    bool unknown_flag;    // +63
    char padding_1[28];
    float moveStrafe;     // +92 计算后输入X?
    float moveForward;    // +96 计算后输入Y?
public:
    MoveInputHandler(InputHandler&, Options const&); // _ZN16MoveInputHandlerC2ER12InputHandlerRK7Options
    ~MoveInputHandler(); // _ZN16MoveInputHandlerD2Ev
    void clearInputState(); // _ZN16MoveInputHandler15clearInputStateEv
    void clearMovementState(); // _ZN16MoveInputHandler18clearMovementStateEv
    void _toggleSneak(); // _ZN16MoveInputHandler12_toggleSneakEv
    void _updateButtonDown(bool*, bool); // _ZN16MoveInputHandler17_updateButtonDownEPbb
    void _updateMoveVector(float, float); // _ZN16MoveInputHandler17_updateMoveVectorEff
    void _updateGGVector(short, float, float, float); // _ZN16MoveInputHandler15_updateGGVectorEsfff
    bool isMovingForward() const; // _ZNK16MoveInputHandler15isMovingForwardEv
    void _registerButtonStateTracking(std::string const&, bool*); // _ZN16MoveInputHandler28_registerButtonStateTrackingERKSsPb
    void _updateXY(bool); // _ZN16MoveInputHandler9_updateXYEb
    void tick(LocalPlayer&); // _ZN16MoveInputHandler4tickER11LocalPlayer

    inline float getMoveX() {
        uintptr_t base = reinterpret_cast<uintptr_t>(this);
        float x = *reinterpret_cast<float*>(base + 4);
        return x;
    }

    inline float getMoveY() {
        uintptr_t base = reinterpret_cast<uintptr_t>(this);
        float x = *reinterpret_cast<float*>(base + 8);
        return x;
    }
};


