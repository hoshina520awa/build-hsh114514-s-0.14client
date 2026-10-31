#pragma once
#include <memory>
#include "EntityType.h"
class TileSource;
class BlockEntityType;

class MobFactory {
public:
    static std::unique_ptr<Entity> CreateMob(EntityType, TileSource&, Vec3 const&, Vec2 const&);
};

class EntityFactory {
public:
    static std::unique_ptr<Entity> CreateEntity(EntityType, TileSource&);
};

class BlockEntityFactory {
public:
    static std::unique_ptr<Entity> createBlockEntity(BlockEntityType, BlockPos const&); // _ZN18BlockEntityFactory17createBlockEntityE15BlockEntityTypeRK8BlockPos
};


