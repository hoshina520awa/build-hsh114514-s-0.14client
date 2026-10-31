#ifndef MOB_H
#define MOB_H
#include"Entity.h"
#include"Attribute.h"
#include "ItemRenderer.h"
#include "MobEffect.h"
#include "JumpControl.h"

class Mob: public Entity {
public:
	Mob* getTarget();
	void setTarget(Mob*);
    void setSneaking(bool);
    void canAttack(Mob *,bool);
    void setInvisible(bool);
    void setSprinting(bool);
    //void hurt(EntityDamageSource const&, int);
    void serializationSetHealth(int);
    ItemInstance* getArmor(int);
    int getHealth()const;
    int getMaxHealth()const;
    void removeEffect(int); // _ZN3Mob12removeEffectEi
    void addEffect(MobEffectInstance const&); // _ZN3Mob9addEffectERK17MobEffectInstance
    AttributeInstance* getAttribute(Attribute const&);
    void jumpFromGround(); // _ZN3Mob14jumpFromGroundEv
    JumpControl* getJumpControl(); // _ZN3Mob14getJumpControlEv
    bool isSprinting() const; // _ZNK3Mob11isSprintingEv
    bool isSneaking() const; // _ZNK3Mob10isSneakingEv
    bool onLadder(bool allowVine); // _ZN3Mob8onLadderEb
    inline void Jump(){
        JumpControl* jc =this->getJumpControl();
        jc->jump();
    }
    void lookAt(Entity*, float, float); // _ZN3Mob6lookAtEP6Entityff
   
};
typedef void (*setSneaking_t)(Mob *,bool);
typedef void (*setInvisible_t)(Mob *,bool);
typedef void (*setSprinting_t)(Mob *,bool);
typedef void (*canAttack_t)(Mob *,Mob *,bool);
typedef void (*serializationSetHealth_t)(Mob *,int);
typedef ItemInstance* (*getArmor_t)(Mob *,int);
typedef AttributeInstance* (*getAttribute_t)(Entity*, Attribute const&);

#endif //MOB_H
