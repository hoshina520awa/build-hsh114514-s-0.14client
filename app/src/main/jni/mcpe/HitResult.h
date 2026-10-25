#pragma once
#include "Entity.h"
#include "Vec.h"
typedef struct {
    int type; //0
    int side; //4
    int x; //8
    int y; //12
    int z; //16
    Vec3 hitVec; //20
    Entity* entity; //32
    unsigned char filler1; //36
    float distanceTo(Entity const&) const; // _ZNK9HitResult10distanceToERK6Entity
} HitResult;


