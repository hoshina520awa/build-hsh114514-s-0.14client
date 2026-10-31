#pragma once
#include "Vec.h"
#include "AABB.h"
#include <string>
#include "EntityUniqueID.h"
#include "BlockSource.h"
#include "EntityType.h"
#include <vector>

class Entity;
typedef void (*remove_t)(Entity *);

class Entity {
public:
	void** vtable; //0
	int filler3;//4
	float x; //8
	float y; //12
	float z; //16
	char filler2[44-20]; // 20
	float motionX; //44 
	float motionY; //48
	float motionZ; //52
	float pitch; //56 
	float yaw; //60
	float prevPitch; //64
	float prevYaw; //68
    //char fill8[12];
    //SynchedEntityData* syncdata;//80
    char filler4[144-72];//72
    AABB aabb;//144
    float width;//165
    float height;//168
    char filler7[244-168];//172
	//char filler4[244-72]; //72
	int renderType; //244
	char filler5[260-248]; // 248
	struct Entity* rider; //260
	struct Entity* riding; //264 
    const Vec3& getPos() const;
    const Vec3& getPosOld() const;
	BlockSource* getRegion() const;
	void setRot(Vec2 const&);
    void setPos(Vec3 const&);//_ZN6Entity6setPosERK4Vec3
    void turn(Vec2 const&,bool);
    float getEntityYaw();
    float getEntityPitch();
    std::vector<float> getEntityPos();
	EntityUniqueID const& getUniqueID() const;//_ZNK6Entity11getUniqueIDEv
    static remove_t removeOrigin;
    static void remove(Entity *);
    
    void onLightningHit();
    void setSize(float,float);
    std::string* getNameTag() ;
	void setNameTag(std::string const&);
    void setNameTagVisible(bool);
    bool isInvisible(); // _ZN6Entity11isInvisibleEv
    bool canShowNameTag(); // _ZN6Entity14canShowNameTagEv
    float getX();
    float getY();
    float getZ();
    //bool isSneaking() const; // _ZNK6Entity10isSneakingEv
    float distanceToSqr(Entity const&);
    Vec3 getInterpolatedPosition(float) const; // _ZNK6Entity23getInterpolatedPositionEf
    Vec3 getInterpolatedPosition2(float) const;
    Vec2 getInterpolatedRotation(float) const;

    void enableAutoSendPosRot(bool); // _ZN6Entity20enableAutoSendPosRotEb
    bool isAutoSendEnabled() const; // _ZNK6Entity17isAutoSendEnabledEv
    Vec3 getVelocity() const; // _ZNK6Entity11getVelocityEv
    
    inline bool isPlayerEntity() {
        constexpr size_t TYPEID_SLOT = 83;
        using Fn = int(__thiscall*)(const Entity*);
        return 319 == ((Fn)(this->vtable[TYPEID_SLOT]))(this);
    }
    
    inline int getRenderId(){
        int renderType = *reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this) + 260);
        return renderType;
    }
    
    inline int getTypeId() const {
        constexpr size_t TYPEID_SLOT = 83;
        using Fn = int(__thiscall*)(const Entity*);
        return ((Fn)(this->vtable[TYPEID_SLOT]))(this);
    }
    
    inline void* getNameTagVtb() const {
        constexpr size_t TYPEID_SLOT = 36;
        using Fn = void*(__thiscall*)(const Entity*);
        return ((Fn)(this->vtable[TYPEID_SLOT]))(this);
    }
    
    inline bool onGround(){
        return *reinterpret_cast<uint8_t*>(reinterpret_cast<uintptr_t>(this) + 297) != 0;
    }
    
   static inline std::string getEntityName(EntityType type) {
    switch (type) {
        case EntityType::PLAYER:            return "玩家";
        case EntityType::OCLEOT:            return "豹猫";
        case EntityType::TRUPODCAMERA:      return "摄像机";
        case EntityType::CHICKEN:           return "鸡";
        case EntityType::COW:               return "牛";
        case EntityType::MUSHROMCOW:        return "哞菇";
        case EntityType::PIG:               return "猪";
        case EntityType::SHEEP:             return "羊";
        case EntityType::WOLF:              return "狼";
        case EntityType::BAT:               return "蝙蝠";
        case EntityType::RABBIT:            return "兔子";
        case EntityType::SQUID:             return "鱿鱼";
        case EntityType::VILLAGER:          return "村民";
        case EntityType::IRONGOLEM:         return "铁傀儡";
        case EntityType::SNOWGOLEM:         return "雪傀儡";
        case EntityType::CREEPER:           return "苦力怕";
        case EntityType::ZOMBIE:            return "僵尸";
        case EntityType::ZOMBIEVILLAGER:    return "僵尸村民";
        case EntityType::SKELETON:          return "骷髅";
        case EntityType::SPIDER:            return "蜘蛛";
        case EntityType::CAVESPIDER:        return "洞穴蜘蛛";
        case EntityType::SLIME:             return "史莱姆";
        case EntityType::ENDERMAN:          return "末影人";
        case EntityType::PIGZOMBIE:         return "僵尸猪人";
        case EntityType::SILVERFISH:        return "蠹虫";
        case EntityType::GHAST:             return "恶魂";
        case EntityType::LAVASLIME:         return "岩浆怪";
        case EntityType::BLAZE:             return "烈焰人";
        case EntityType::WITCH:             return "女巫";
        case EntityType::ITEMENTITY:        return "物品实体";
        case EntityType::PRIMEDTNT:         return "点燃的TNT";
        case EntityType::FALLINGBLOCK:      return "下落的方块";
        case EntityType::EXPERIENCEPOTION:  return "经验药水";
        case EntityType::EXPERIENCEORB:     return "经验球";
        case EntityType::FISHINGHOOK:       return "鱼钩";
        case EntityType::ARROW:             return "箭";
        case EntityType::SNOWBALL:          return "雪球";
        case EntityType::THROWNEGG:         return "投掷的蛋";
        case EntityType::PAINTING:          return "画";
        case EntityType::LARGEFIREBALL:     return "大火球";
        case EntityType::THROWNPOTION:      return "喷溅药水";
        case EntityType::THROWNENDERPEARL:  return "末影珍珠";
        case EntityType::BOAT:              return "船";
        case EntityType::LIGHTNINGBOLT:     return "雷电";
        case EntityType::SMALLFIREBALL:     return "小火球";
        case EntityType::MINECARFTRIDEABLE: return "矿车";
        case EntityType::MINECARTHOPPER:    return "漏斗矿车";
        case EntityType::MINECARTTNT:       return "TNT矿车";
        case EntityType::MINECARCHEST:      return "运输矿车";
        default:                            return "未知实体";
    }
    /*
    inline AABB getAABB(){
        return *reinterpret_cast<AABB*>(reinterpret_cast<uintptr_t>(this) + 144);;
    }
    */
}

};



