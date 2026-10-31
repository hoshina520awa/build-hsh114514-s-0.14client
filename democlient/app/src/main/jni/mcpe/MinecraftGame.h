#ifndef MINECRAFTGAME_H
#define MINECRAFTGAME_H

#include "GuiData.h"
#include "Minecraft.h"
#include "GameType.h"
#include "Player.h"
#include "LocalPlayer.h"
class LevelRenderer;
struct BuildActionIntention {
    uint32_t value = 0;
    BuildActionIntention() = default;
    explicit BuildActionIntention(uint32_t v) : value(v) {}
};

static_assert(sizeof(BuildActionIntention) == 4, "size mismatch");
static_assert(alignof(BuildActionIntention) == 4, "align mismatch");

class MinecraftClient;
typedef void (*_initMinecraftClient_t)(MinecraftClient *);

class MinecraftClient {
private:
    static MinecraftClient *instance;
    
public:
    char filler1[100];
	Minecraft& minecraft;
    
    static _initMinecraftClient_t _initMinecraftClientOrigin;
    void setGameMode(GameType gameType);
    static void _initMinecraftClient(MinecraftClient *minecraftGame);
    static MinecraftClient *getInstance();
    GuiData *getGuiData();
    bool isInGame();
    Entity *getCameraEntity();
    void setCameraEntity(Entity *entity);
    Minecraft* getServer();
    LocalPlayer* getLocalPlayer();
    Options* getOptions();
    void requestScreenshot(); // _ZN15MinecraftClient17requestScreenshotEv
    void reloadShaders(); // _ZN15MinecraftClient13reloadShadersEv
    void _reloadLanguages(); // _ZN15MinecraftClient16_reloadLanguagesEv
    void _reloadFancy(bool); // _ZN15MinecraftClient12_reloadFancyEb
    LevelRenderer* getLevelRenderer() const; // _ZNK15MinecraftClient16getLevelRendererEv
    void handleBuildAction(BuildActionIntention&); // _ZN15MinecraftClient17handleBuildActionER20BuildActionIntention
    void popScreen(int); // _ZN15MinecraftClient9popScreenEi
    std::string getScreenName(); // _ZN15MinecraftClient13getScreenNameEv
    typedef void(*setGameMode_t)(MinecraftClient *, GameType);
    static void onClientStartedLevel(MinecraftClient* minecraft, std::unique_ptr<Level> levelPtr, std::unique_ptr<LocalPlayer> localPlayerPtr);            
    static Level *v_level;
    void handleReloadUIDefinitions(); // _ZN15MinecraftClient25handleReloadUIDefinitionsEv
    std::shared_ptr<InputHandler> getInput() const; // _ZNK15MinecraftClient8getInputEv
    void getCurrentInputMode(); // _ZN15MinecraftClient19getCurrentInputModeEv
    int getWidth() const; // _ZNK15MinecraftClient8getWidthEv
    int getHeight() const; // _ZNK15MinecraftClient9getHeightEv
};

#endif //MINECRAFTGAME_H

