#pragma once
class Mob;
class JumpControl {
public:
    JumpControl(Mob&); // _ZN11JumpControlC2ER3Mob
    ~JumpControl(); // _ZN11JumpControlD2Ev
    void tick(); // _ZN11JumpControl4tickEv
    void jump(); // _ZN11JumpControl4jumpEv
};


