#ifndef LEVEL_H
#define LEVEL_H
#include "Entity.h"
#include "Player.h"
#include "EntityUniqueID.h"
#include "HitResult.h"
#include "BlockSource.h"
#include "LevelData.h"
#include "ParticleType.h"
#include <memory>
#include <vector>
#include <unordered_set>

class Level;




class Level {
public:
	void** vtable; //0
	char filler[12-4]; //4
	bool isRemote; //12 PrimedTnT::normalTick
	char filler2[2908-13];//13
    
    Mob* getMob(EntityUniqueID) const;
    Entity* getEntity(EntityUniqueID, bool) const;
	void addEntity(std::unique_ptr<Entity>);
	void addGlobalEntity(std::unique_ptr<Entity>);
    void addPlayer(std::unique_ptr<Player>);
    void explode(BlockSource&, Entity*, Vec3 const&, float, bool);
    void setNightMode(bool);
    void setTime(int);
    int getTime() const;
    void addParticle(ParticleType, Vec3 const&, Vec3 const&, int); // _ZN5Level11addParticleE12ParticleTypeRK4Vec3S3_i                  
    void playSound(Vec3 const&, std::string const&, float, float); // _ZN5Level9playSoundERK4Vec3RKSsff
    void setIsClientSide(bool);
    LevelData* getLevelData();
    HitResult const& getHitResult();
    bool isClientSide();
    bool isDayCycleActive(); // _ZN5Level16isDayCycleActiveEv
    void setDayCycleActive(bool); // _ZN5Level17setDayCycleActiveEb
    unsigned int getSeed(); // _ZN5Level7getSeedEv
    void tick(); // _ZN5Level4tickEv
    LevelChunk* onChunkLoaded(LevelChunk&); // _ZN5Level13onChunkLoadedER10LevelChunk
    void findPath(Entity&, int, int, int, float, bool, bool, bool, bool); // _ZN5Level8findPathER6Entityiiifbbbb
    //void findPath(Entity&, Entity&, float, bool, bool, bool, bool); // _ZN5Level8findPathER6EntityS1_fbbbb
};



#endif // LEVEL_H
