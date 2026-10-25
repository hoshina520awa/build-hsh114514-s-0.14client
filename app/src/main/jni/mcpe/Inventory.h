#pragma once

class ItemInstance;
class FillingContainer {
public:
    ItemInstance* getItem(int slot);
    int getLinkedSlotsCount() const;
    int getLinkedSlot(int slot) const;
    ItemInstance* getLinked(int slot) const;
    void dropAll2(bool);
    int** getLinkedSlots();
};

class Inventory : public FillingContainer {
public:
    int getLinkedSlotForItem(int slot);
    void clearSlot(int slot);
    void add(ItemInstance&, bool);
    void dropAll(bool);
    void doDrop(ItemInstance&, bool);
    int getSelectedSlot() const; // _ZNK9Inventory15getSelectedSlotEv
    void selectSlot(int); // _ZN9Inventory10selectSlotEi                                        
    ItemInstance* getSelectedItem() const; // _ZNK9Inventory15getSelectedItemEv
    void moveToSelectedSlot(int); // _ZN9Inventory18moveToSelectedSlotEi
    void moveToSelectionSlot(int, int); // _ZN9Inventory19moveToSelectionSlotEii
    void setContainerSize(int); // _ZN9Inventory16setContainerSizeEi
    int getSelectionSize(); // _ZN9Inventory16getSelectionSizeEv
};

typedef void (*add_t)(Inventory *, ItemInstance&, bool);
typedef void (*dropAll_t)(Inventory *,bool);
typedef void (*dropAll2_t)(FillingContainer *,bool);
typedef void (*doDrop_t)(Inventory *,ItemInstance&, bool);
typedef ItemInstance* (*getItem_t)(FillingContainer* ,int);

