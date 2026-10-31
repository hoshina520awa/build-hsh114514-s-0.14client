#pragma once

#include "string"
#include "BlockPos.h"
#include "GameType.h"
struct GCC_String {
    struct Rep {
        size_t length;
        size_t capacity;
        int refcount;
    };
    char* _M_dataplus;
    const char* c_str() const {
        return _M_dataplus;
    }
};

class LevelData {
public:
    void setSpawn(BlockPos const&);
    void setGameType(GameType);
    GameType getGameType() const;
    std::string* getLevelName() const;
};

typedef void (*setSpawn_t)(LevelData *, BlockPos const&);
typedef void (*setGameType_t)(LevelData *,GameType);
