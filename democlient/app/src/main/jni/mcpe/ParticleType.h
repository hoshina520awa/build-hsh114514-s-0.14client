//
// Created by hxf16 on 2026/7/17.
//

#ifndef CLIENT_PARTICLETYPE_H
#define CLIENT_PARTICLETYPE_H


#pragma once
#include <cstdint>


enum class ParticleType : int
{
#define PP_MAP(_) \
        _(PARTICLE_POPLE,              1)  \
        _(PARTICLE_STAR_YELLOW,        2)  \
        _(PARTICLE_BLCAK_SMOKE,        3)  \
        _(PARTICLE_WHITE_SMOKE,        4)  \
        _(PARTICLE_WHITE_SOMKE_2,      5)  \
        _(PARTICLE_FIRE,               6)  \
        _(PARTICLE_LAVA_STAR,          7)  \
        _(PARTICLE_BLACK_SMOKE_2,      8)  \
        _(PARTICLE_RED_STONE,          9)  \
        _(PARTICLE_ITEM_BREAK,         10) \
        _(PARTICLE_SNOW_BALL,          11) \
        _(PARTICLE_EXPLOSION_SMOKE,    12) \
        _(PARTICLE_HUGE_EXPLOSION_SMOKE,13)\
        _(PARTICLE_HUGE_FIRE,          14) \
        _(PARTICLE_LOVE,               15) \
        _(PARTICLE_WATER_VERY_SLOW,    17) \
        _(PARTICLE_NEITHER,            18) \
        _(PARTICLE_WATER_SLOW,         19) \
        _(PARTICLE_WATER_FAST,         20) \
        _(PARTICLE_WATER_VERY_FAST,    21) \
        _(PARTICLE_LAVA,               22) \
        _(PARTICLE_SMOKE_VERY_BLACK,   23) \
        _(PARTICLE_POTION,             24) \
        _(PARTICLE_POTION_2,           25) \
        _(PARTICLE_STAR,               26) \
        _(PARTICLE_SMOKE,              27) \
        _(PARTICLE_SLIME,              28) \
        _(PARTICLE_WATER_MIDDLE,       29) \
        _(PARTICLE_VILLAGER_ANGRY,     30) \
        _(PARTICLE_GREEN_STAR,         31) \
        _(PARTICLE_ENCHANT,            32) \
        _(PARTICLE_MUSIC,              34)

#define GEN_ENUM(name,value) name = value,
    PP_MAP(GEN_ENUM)
#undef GEN_ENUM
};

inline ParticleType toParticleType(int v) noexcept{
#define CHECK_VALUE(name,val) case val: return ParticleType::name;
    switch (v){
        PP_MAP(CHECK_VALUE)
        default: return static_cast<ParticleType>(1);
    }
#undef CHECK_VALUE
}


#endif //CLIENT_PARTICLETYPE_H
