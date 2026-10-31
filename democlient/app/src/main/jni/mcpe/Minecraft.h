#ifndef MINECRAFT_H
#define MINECRAFT_H
#include "Level.h"
#include "Timer.h"
#include "GameMode.h"
#include "PacketSender.h"
#include "ServerCommandParser.h"

class Minecraft {
public:
    char filler1[64];
	ServerCommandParser* cmdParser;
    Level* getLevel();
    Timer* getTimer();
    GameMode* getGameMode();
    int setHurtTime(int time);
    void initAsDedicatedServer();
    PacketSender* getPacketSender();
    ServerCommandParser* getCommandParser(); // _ZN9Minecraft16getCommandParserEv
	void tick(int, int); // _ZN9Minecraft4tickEii
};


#endif //MINECRAFT_H
