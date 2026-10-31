#include <main.h>
#include "MinecraftGame.h"

MinecraftClient *MinecraftClient::instance = nullptr;
_initMinecraftClient_t MinecraftClient::_initMinecraftClientOrigin = nullptr;

void MinecraftClient::_initMinecraftClient(MinecraftClient *minecraftGame) {
    LOGE("MinecraftClientOrigin == %p", _initMinecraftClientOrigin);
    instance = minecraftGame;
    if (_initMinecraftClientOrigin) {
        _initMinecraftClientOrigin(minecraftGame);
    }
}

MinecraftClient *MinecraftClient::getInstance() {
    return instance;
}


