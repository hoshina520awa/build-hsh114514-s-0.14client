#pragma once
#include "Entity.h"
#include "EntityType.h"
struct EntityClassTree {
public:
    //bool isMob(Entity const&); // _ZN15EntityClassTree5isMobERK6Entity
    bool isPlayer(Entity const&); // _ZN15EntityClassTree8isPlayerERK6Entity
    static bool isInstanceOf(Entity const&, EntityType); // _ZN15EntityClassTree12isInstanceOfERK6Entity10EntityType
    void getEntityTypeIdLegacy(EntityType); // _ZN15EntityClassTree21getEntityTypeIdLegacyE10EntityType
    void isItemEntity(Entity const&); // _ZN15EntityClassTree12isItemEntityERK6Entity
    static bool isMob(EntityType); // _ZN15EntityClassTree5isMobE10EntityType
    void isMobLegacy(EntityType); // _ZN15EntityClassTree11isMobLegacyE10EntityType
    static bool isTypeInstanceOf(EntityType, EntityType); // _ZN15EntityClassTree16isTypeInstanceOfE10EntityTypeS0_
    void getMobCategory(EntityType); // _ZN15EntityClassTree14getMobCategoryE10EntityType
    void isOfType(EntityType, EntityType); // _ZN15EntityClassTree8isOfTypeE10EntityTypeS0_
    void isHangingEntity(Entity const&); // _ZN15EntityClassTree15isHangingEntityERK6Entity
    void isMinecart(Entity const&); // _ZN15EntityClassTree10isMinecartERK6Entity
};


