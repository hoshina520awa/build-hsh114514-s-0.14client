#pragma once
#include "Item.h"
#include <string>

//int PLAYER_INVENTORY_OFFSET = 3488;
class ItemRenderChunkType;
class CompoundTag {
public:
    ~CompoundTag();
};

class Block;
class ItemInstance {
public:
    unsigned char count; //0
    short damage; //2
    unsigned char something; // 4
    char filler[8-5]; // 5
    CompoundTag* tag; // 8
    Item* item; // 12
    Block* block; //16
    //char filler2[12]; // 5

    ItemInstance();
    ItemInstance(int, int, int);
    ItemInstance(ItemInstance const&);
    ItemInstance(Item const*); // _ZN12ItemInstanceC1EPK4Item
    ItemInstance(Item const*, int); // _ZN12ItemInstanceC1EPK4Itemi
    ItemInstance(Block const*); // _ZN12ItemInstanceC1EPK5Block
    ItemInstance(Block const*, int); // _ZN12ItemInstanceC2EPK5Blocki
    ItemInstance& operator=(ItemInstance const&);
    bool operator==(ItemInstance const&) const;
    bool operator!=(ItemInstance const&) const;
    //ItemEnchants getEnchantsFromUserData() const;
    bool hasCustomHoverName() const;
    std::string getCustomName() const;
    void setCustomName(std::string const&);
    void init(int, int, int);
    int getId();
    //TextureUVCoordinateSet* getIcon(int, bool) const;
    std::string getName();
    int getMaxStackSize() const;
    void remove(int);
    TextureUVCoordinateSet* getIcon(int, bool) const; // _ZNK12ItemInstance7getIconEib
    bool isFoil() const; // _ZNK12ItemInstance6isFoilEv
    void setNull(); // _ZN12ItemInstance7setNullEv
};


namespace mce {
    class TexturePtr;
};


