#pragma once
#include <string>

class GuiMessage {
public:
    GuiMessage(std::string const&, std::string const&, int, bool); // _ZN10GuiMessageC2ERKSsS1_ib
    GuiMessage(); // _ZN10GuiMessageC2Ev
    bool isCommand() const; // _ZNK10GuiMessage9isCommandEv
    std::string* getString() const; // _ZNK10GuiMessage9getStringEv
    void getUser() const; // _ZNK10GuiMessage7getUserEv
    std::string getMessage(); // _ZNK10GuiMessage10getMessageEv
    bool isForceVisible() const; // _ZNK10GuiMessage14isForceVisibleEv
    void getTicks() const; // _ZNK10GuiMessage8getTicksEv
    void getMaxTicks() const; // _ZNK10GuiMessage11getMaxTicksEv
    void getPercentComplete() const; // _ZNK10GuiMessage18getPercentCompleteEv
    void isDead() const; // _ZNK10GuiMessage6isDeadEv
    void isNew() const; // _ZNK10GuiMessage5isNewEv
    void setOld(); // _ZN10GuiMessage6setOldEv
    void incrementTick(); // _ZN10GuiMessage13incrementTickEv
};

