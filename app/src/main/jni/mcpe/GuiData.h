#pragma once
#include <string>
#include <vector>
#include "GuiMessage.h"
class GuiData;


class GuiData {
public:
    static void addMessage(GuiData *, std::string const &, std::string const &, int, bool);
    void displayClientMessage(std::string const &message);
    void showTipMessage(std::string const &message);
    void showPopupNotice(std::string const&, std::string const&);
    void clearMessages();
    int getScreenWidth();
    int getScreenHeight();
    std::vector<GuiMessage>* getMessageList();
    void setGuiScale(float);
    void setShowProgress(bool b);
    int getPointerX() const; // _ZNK7GuiData11getPointerXEv
    int getPointerY() const; // _ZNK7GuiData11getPointerYEv
    void postError(int); // _ZN7GuiData9postErrorEi
    bool getShowProgress() const; // _ZNK7GuiData15getShowProgressEv
    float getGuiScale(); // _ZN7GuiData11getGuiScaleEv
    void getInvGuiScale(); // _ZN7GuiData14getInvGuiScaleEv
};

