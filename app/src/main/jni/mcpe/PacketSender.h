#pragma once
#include "main.h"
class Packet {
public:
    virtual ~Packet();
};

typedef struct {
    char filler[16]; //0
    char* text; //16;
} RakString;

class TextPacket : public Packet {
public:
    char filler[8-4]; // 4
    int shouldBeOne; // 8
    unsigned char shouldBeZero;
    unsigned char type; //13
    char filler2[16-14]; // 14
    std::string username; // 16
    std::string message; // 20
    char filler3[36-24]; // 24
    virtual ~TextPacket() override;
};

class AnimatePacket : public Packet {
public:
    void** vtable;           // 0x00
    char _pad[0x10];         // 0x04 - 0x0F
    uint8_t action;          // 0x10
    char _pad2[7];           // 0x11 - 0x17
    uint64_t eid;            // 0x18
    virtual ~AnimatePacket() override;
};

class MinecraftPackets {
public:
    //void createPacket(int); // _ZN16MinecraftPackets12createPacketEi
};

namespace RakNet {
    class RakNetGUID;
};
class PacketSender {
public:
    virtual ~PacketSender();
    virtual void send(Packet const&);
    virtual void send(RakNet::RakNetGUID const&, Packet const&);
    virtual void sendBroadcast(RakNet::RakNetGUID const&, Packet const&);
};


