#pragma once

class Attribute {
public:
    char filler[0x14];
};

class SharedAttributes {
public:
    static Attribute ABSORPTION;
    static Attribute ATTACK_DAMAGE;
    static Attribute MOVEMENT_SPEED;
    static Attribute KNOCKBACK_RESISTANCE;
    static Attribute FOLLOW_RANGE;
    static Attribute HEALTH;
};

class AttributeInstance {
public:
    char filler[76-0];
    float value;
    float getMaxValue() const;
    void setMaxValue(float);
};

class CompoundTag;
class GlobalPermissionsLevel;
class UserPermissionsLevel;
class PermissionsHandler;
//player +3316
class Abilities{
public:
    bool invulnerable;
    bool flying;
    bool mayfly;
    bool instabuild;
    bool idk_bool; 
    bool lightning;
    char pad[2];
    float walkSpeed;
    float flySpeed;
    PermissionsHandler* permissionsHandler;
    
    Abilities(); // _ZN9AbilitiesC2Ev
    void getUserPermissions() const; // _ZNK9Abilities18getUserPermissionsEv
    void setGlobalPermissions(GlobalPermissionsLevel); // _ZN9Abilities20setGlobalPermissionsE22GlobalPermissionsLevel
    bool isFlying() const; // _ZNK9Abilities8isFlyingEv
    float getFlyingSpeed() const; // _ZNK9Abilities14getFlyingSpeedEv
    void setFlyingSpeed(float); // _ZN9Abilities14setFlyingSpeedEf
    void setUserPermissions(UserPermissionsLevel); // _ZN9Abilities18setUserPermissionsE20UserPermissionsLevel
    float getWalkingSpeed() const; // _ZNK9Abilities15getWalkingSpeedEv
    void getGlobalPermissions() const; // _ZNK9Abilities20getGlobalPermissionsEv
    void setWalkingSpeed(float); // _ZN9Abilities15setWalkingSpeedEf
    void addSaveData(CompoundTag&) const; // _ZNK9Abilities11addSaveDataER11CompoundTag
    void loadSaveData(CompoundTag const&); // _ZN9Abilities12loadSaveDataERK11CompoundTag
};
