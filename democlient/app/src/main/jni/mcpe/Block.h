#ifndef BLOCK_H
#define BLOCK_H
#include "Vec.h"
#include "mcpe/BlockSource.h"

class TextureFile;

struct TextureUVCoordinateSet {
    float bounds[4];
    unsigned short size[2]; // 16
    void* filler; // 20
    TextureFile* textureFile; // 24
public:
    TextureUVCoordinateSet(); // _ZN22TextureUVCoordinateSetC1Ev
    TextureUVCoordinateSet(float, float, float, float, unsigned short, unsigned short, TextureFile); // _ZN22TextureUVCoordinateSetC1Efffftt11TextureFile
    void fromOldSystem(int); // _ZN22TextureUVCoordinateSet13fromOldSystemEi
    void offsetWidthPixel(float, float) const; // _ZNK22TextureUVCoordinateSet16offsetWidthPixelEff
    void offsetHeightPixel(float, float) const; // _ZNK22TextureUVCoordinateSet17offsetHeightPixelEff
    void offsetWidth(float, float) const; // _ZNK22TextureUVCoordinateSet11offsetWidthEff
    void offsetHeight(float, float) const; // _ZNK22TextureUVCoordinateSet12offsetHeightEff
    void pixelWidth() const; // _ZNK22TextureUVCoordinateSet10pixelWidthEv
    void pixelHeight() const; // _ZNK22TextureUVCoordinateSet11pixelHeightEv
    void inverseOffsetHeight(float, float) const; // _ZNK22TextureUVCoordinateSet19inverseOffsetHeightEff
    void inverseOffsetWidth(float, float) const; // _ZNK22TextureUVCoordinateSet18inverseOffsetWidthEff
    void subTexture(float, float, int, int) const; // _ZNK22TextureUVCoordinateSet10subTextureEffii
    void offset(float) const; // _ZNK22TextureUVCoordinateSet6offsetEf
};


class Block {
public:
    void** vtable; //0
    unsigned char id; // 4
    char filler0[16-5]; //5
    TextureUVCoordinateSet texture; //16
    char filler[52-44]; //48 (insert rude joke here)
    int renderLayer; //52
    int renderType; //56 Block::getBlockShape()
    char filler2[84-60]; //60
    void* material; //84
    char filler3[112-88]; // 88
    float destroyTime; //112
    float explosionResistance; //116

    float getDestroySpeed();
    float getFriction();
    void setFriction(float);
    void setSolid(bool);
    void setVisualShape(Vec3 const&, Vec3 const&);
    void setRenderLayer(int);
};
#define Tile Block

typedef void (*setFriction_t)(Block*, float);
typedef void (*setVisualShape_t)(Block *,Vec3 const&, Vec3 const&);
typedef void (*setRenderLayer_t)(Block*, int);


#endif //BLOCK_H
