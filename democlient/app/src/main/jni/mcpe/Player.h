#ifndef PLAYER_H
#define PLAYER_H
#include "Mob.h"
#include "Block.h"
#include "ItemRenderer.h"

class Player: public Mob {
public:
	int getScore();
	void addExperience(int);
	void addLevels(int);
    void attack(Entity&);
    void drop(ItemInstance const*, bool);
    void SetStepHeight(float height);
    float getDestroySpeed(Block&);//_ZN6Player15getDestroySpeedER5Block
    bool IsCreative() const; // _ZNK6Player10IsCreativeEv
    void normalTick(); // _ZN6Player10normalTickEv
    void jumpFromGround(); // _ZN6Player14jumpFromGroundEv
    
    //void isLocalPlayer() const; // _ZNK6Player13isLocalPlayerEv
    inline bool isLP() {
        constexpr size_t TYPEID_SLOT = 255;
        using Fn = bool(__thiscall*)(Player*);
        return ((Fn)(this->vtable[TYPEID_SLOT]))(this);
    }
    
    inline Abilities& getAbilities() {
        return *reinterpret_cast<Abilities*>(reinterpret_cast<char*>(this) + 3316);
    }
};


#endif //PLAYER_H
