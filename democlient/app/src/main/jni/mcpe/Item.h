#pragma once
#include <string>
class Item;
typedef void (*initItems_t)(Item *);

class CreativeItemCategory {
public:
    CreativeItemCategory(int v) : value(v) {}
    operator int() const { return value; }
private:
    int value;
};
/*
namespace CreativeItemCategory {
    constexpr CreativeItemCategory CONSTRUCTION(1);
    constexpr CreativeItemCategory NATURE(2);
    constexpr CreativeItemCategory EQUIPMENT(3);
}
*/


class Item{
private:
    static Item *instance;
public:
    //void** vtable; //0
    char filler0[18-4]; //4
    short itemId; //18
    char filler1[26-20]; // 20
    unsigned char attribs; // 26
    char filler[32-27]; //27
    unsigned char useAnimation; // 32
    char filler3[64-33]; // 33
	virtual ~Item();
    Item(std::string const&, short); // _ZN4ItemC1ERKSss
    //void setIcon(TextureUVCoordinateSet const&); // _ZN4Item7setIconERK22TextureUVCoordinateSet                    
    void setIcon(std::string const&, int); // _ZN4Item7setIconERKSsi
    void setCategory(CreativeItemCategory); // _ZN4Item11setCategoryE20CreativeItemCategory
    static initItems_t initItemsOrigin;
    static void initItems(Item *item);
    static Item *getInstance();
    void setMaxStackSize(unsigned char); // _ZN4Item15setMaxStackSizeEh
    static void addCreativeItem(short,short);
    void setStackedByData(bool); // _ZN4Item16setStackedByDataEb
    std::string getInteractText() const; // _ZNK4Item15getInteractTextEv
};

