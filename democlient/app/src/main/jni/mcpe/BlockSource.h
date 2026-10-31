#pragma once
#include "BlockPos.h"
#include "mcpe/AABB.h"
class Level;
class BlockEntity;
class Biome;
class LevelChunk;
class Dimension;
class ChunkSource;
class ChunkPos;
#define FullTile FullBlock
struct FullTile {
    unsigned char id;
    unsigned char data;
    FullTile(): id(0), data(0) {
    }
    FullTile(FullTile const& other): id(other.id), data(other.data) {
    }
    FullTile(unsigned char _id, unsigned char _data): id(_id), data(_data) {}

};

struct BlockBrightness {
    unsigned char value;
    BlockBrightness(BlockBrightness const& other): value(other.value) {}
};

struct BlockID {
    unsigned char id;
    operator unsigned char() const {
        return id;
    }
    BlockID(BlockID const& other): id(other.id) {
    }
    BlockID(unsigned char id_): id(id_) {
    }
};

class BlockSource {
public:
    BlockSource(Level&, Dimension&, ChunkSource&, bool, bool);
    void        setBlockAndData(int, int, int, FullTile, int);
    BlockID     getBlockID(int, int, int);
    BlockID     getBlockID(BlockPos const&);
    int     getData(int, int, int);
    int     getData(BlockPos const&);
    BlockBrightness getRawBrightness(int, int, int, bool);
    BlockEntity*    getBlockEntity(int, int, int);
    int     getGrassColor(BlockPos const&);
    void        setGrassColor(int, BlockPos const&, int);
    Biome*      getBiome(BlockPos const&);
    LevelChunk* getChunk(int, int);
    LevelChunk* getChunk(ChunkPos const&); // _ZN11BlockSource8getChunkERK8ChunkPos
    void        fireBlockEvent(int, int, int, int, int);
    Dimension*  getDimension();
    bool        canSeeSky(int, int, int);
    bool isEmptyBlock(const BlockPos&);
    bool setBlock(const BlockPos&, BlockID, int);
    Level*      getLevel();
    bool isSolidBlockingBlock(BlockPos const&); // _ZN11BlockSource20isSolidBlockingBlockERK8BlockPos
    bool isTopSolidBlocking(int, int, int); // _ZN11BlockSource18isTopSolidBlockingEiii
    bool isTopSolidBlocking(BlockPos const&); // _ZN11BlockSource18isTopSolidBlockingERK8BlockPos
    void* fetchAABBs(AABB const&, bool); // _ZN11BlockSource10fetchAABBsERK4AABBb
};
#define TileSource BlockSource
