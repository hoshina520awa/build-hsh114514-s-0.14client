#ifndef GAMEMODE_H
#define GAMEMODE_H
#include "Player.h"
#include "BlockPos.h"

class GameMode {
public:
    void buildBlock(Player&, BlockPos, signed char);
    void destroyBlock(Player&, BlockPos, signed char);
    void _destroyBlockInternal(Player&, BlockPos, signed char);
};

typedef void (*_destroyBlockInternal_t)(GameMode *, Player&, BlockPos, signed char);
typedef void (*destroyBlock_t)(GameMode *,Player&, BlockPos, signed char);
typedef void (*buildBlock_t)(GameMode *,Player&, BlockPos, signed char);


#endif //GAMEMODE_H
