#pragma once
#include "Player.h"
#include "HitResult.h"
#include "MoveInputHandler.h"
#include <memory>
class MinecraftClient;
class User;
class Side;
class IContainerManager;
class EntityDamageSource;
class ArmorSlot;
class HudContainerManagerModel;
class DimensionId;
class ChangeDimensionPacket;
class Tick;
namespace mce{
    class UUID;
}
namespace ui{
    class GameEventNotification;
}
namespace RakNet{
    class RakNetGUID;
}

class LocalPlayer: public Player {
public:
    class RegionListener;
public:
    //LocalPlayer(MinecraftClient*, Level&, User const&, GameType, RakNet::RakNetGUID const&, mce::UUID); // _ZN11LocalPlayerC1EP15MinecraftClientR5LevelRK4User8GameTypeRKN6RakNet10RakNetGUIDEN3mce4UUIDE
    ~LocalPlayer(); // _ZN11LocalPlayerD2Ev
    void requestChunkRadius(int); // _ZN11LocalPlayer18requestChunkRadiusEi
    void startPaddling(Side); // _ZN11LocalPlayer13startPaddlingE4Side
    void stopPaddling(Side); // _ZN11LocalPlayer12stopPaddlingE4Side
    void pickBlockCreative(HitResult const&); // _ZN11LocalPlayer17pickBlockCreativeERK9HitResult
    void clearMovementState(); // _ZN11LocalPlayer18clearMovementStateEv
    void getMinecraftClient() const; // _ZNK11LocalPlayer18getMinecraftClientEv
    void closeScreen(); // _ZN11LocalPlayer11closeScreenEv
    void registerFramewiseActionOrStop(); // _ZN11LocalPlayer29registerFramewiseActionOrStopEv
    void isLocalPlayer() const; // _ZNK11LocalPlayer13isLocalPlayerEv
    void setContainerData(IContainerManager&, int, int); // _ZN11LocalPlayer16setContainerDataER17IContainerManagerii
    void refreshContainer(IContainerManager&, std::vector<ItemInstance, std::allocator<ItemInstance> > const&); // _ZN11LocalPlayer16refreshContainerER17IContainerManagerRKSt6vectorI12ItemInstanceSaIS3_EE
    void stopLoading(); // _ZN11LocalPlayer11stopLoadingEv
    void isInTrialMode(); // _ZN11LocalPlayer13isInTrialModeEv
    void getControllingPlayer() const; // _ZNK11LocalPlayer20getControllingPlayerEv
    void getTelemetry() const; // _ZNK11LocalPlayer12getTelemetryEv
    void displayLocalizableMessage(std::string const&, std::vector<std::string, std::allocator<std::string> > const&); // _ZN11LocalPlayer25displayLocalizableMessageERKSsRKSt6vectorISsSaISsEE
    void travel(float, float); // _ZN11LocalPlayer6travelEff
    void applyFinalFriction(float); // _ZN11LocalPlayer18applyFinalFrictionEf
    void hurt(EntityDamageSource const&, int); // _ZN11LocalPlayer4hurtERK18EntityDamageSourcei
    void setPos(Vec3 const&); // _ZN11LocalPlayer6setPosERK4Vec3
    void drop(ItemInstance const*, bool); // _ZN11LocalPlayer4dropEPK12ItemInstanceb
    void handleInsidePortal(BlockPos const&); // _ZN11LocalPlayer18handleInsidePortalERK8BlockPos
    void onPrepChangeDimension(); // _ZN11LocalPlayer21onPrepChangeDimensionEv
    void onDimensionChanged(); // _ZN11LocalPlayer18onDimensionChangedEv
    void setArmor(ArmorSlot, ItemInstance const*); // _ZN11LocalPlayer8setArmorE9ArmorSlotPK12ItemInstance
    void canStartSleepInBed(); // _ZN11LocalPlayer18canStartSleepInBedEv
    void resetPos(bool); // _ZN11LocalPlayer8resetPosEb
    void setSneaking(bool); // _ZN11LocalPlayer11setSneakingEb
    void setSprinting(bool); // _ZN11LocalPlayer12setSprintingEb
    void slotChanged(IContainerManager&, int, ItemInstance const&, bool); // _ZN11LocalPlayer11slotChangedER17IContainerManageriRK12ItemInstanceb
    void _checkMovementStatistiscs(Vec3 const&); // _ZN11LocalPlayer25_checkMovementStatistiscsERK4Vec3
    void respawn(); // _ZN11LocalPlayer7respawnEv
    void addAdditionalSaveData(CompoundTag&); // _ZN11LocalPlayer21addAdditionalSaveDataER11CompoundTag
    void readAdditionalSaveData(CompoundTag const&); // _ZN11LocalPlayer22readAdditionalSaveDataERK11CompoundTag
    void actuallyHurt(int, EntityDamageSource const*); // _ZN11LocalPlayer12actuallyHurtEiPK18EntityDamageSource
    void stopRiding(bool); // _ZN11LocalPlayer10stopRidingEb
    void getGameModeString() const; // _ZNK11LocalPlayer17getGameModeStringEv
    void sendGameSessionHeartBeat(bool); // _ZN11LocalPlayer24sendGameSessionHeartBeatEb
    void _calculateFlight(float, float, float); // _ZN11LocalPlayer16_calculateFlightEfff
    void getViewBlockRadius() const; // _ZNK11LocalPlayer18getViewBlockRadiusEv
    void _RotationVec2Modulus(Vec2&); // _ZN11LocalPlayer20_RotationVec2ModulusER4Vec2
    void isFlying() const; // _ZNK11LocalPlayer8isFlyingEv
    void getFieldOfViewModifier(); // _ZN11LocalPlayer22getFieldOfViewModifierEv
    void chat(std::string const&); // _ZN11LocalPlayer4chatERKSs
    void _updateArmorTypeHash(); // _ZN11LocalPlayer20_updateArmorTypeHashEv
    void getLoadingState() const; // _ZNK11LocalPlayer15getLoadingStateEv
    void sendPosition(); // _ZN11LocalPlayer12sendPositionEv
    void sendInput(); // _ZN11LocalPlayer9sendInputEv
    void isAutoJumpEnabled() const; // _ZNK11LocalPlayer17isAutoJumpEnabledEv
    void _updateAutoJump(float, float, float, float); // _ZN11LocalPlayer15_updateAutoJumpEffff
    void move(Vec3 const&); // _ZN11LocalPlayer4moveERK4Vec3
    void _PostJumpFrameOfReferenceAdjustCheck(bool); // _ZN11LocalPlayer36_PostJumpFrameOfReferenceAdjustCheckEb
    void checkFallDamage(float, bool); // _ZN11LocalPlayer15checkFallDamageEfb
    void _setRideInputFor(Entity&); // _ZN11LocalPlayer16_setRideInputForER6Entity
    void startRiding(Entity&); // _ZN11LocalPlayer11startRidingER6Entity
    void getBob() const; // _ZNK11LocalPlayer6getBobEv
    void getBobOld() const; // _ZNK11LocalPlayer9getBobOldEv
    void getPreloadingProgress(); // _ZN11LocalPlayer21getPreloadingProgressEv
    void removeRegionListener(LocalPlayer::RegionListener*); // _ZN11LocalPlayer20removeRegionListenerEPNS_14RegionListenerE
    void onGameEventNotification(ui::GameEventNotification); // _ZN11LocalPlayer23onGameEventNotificationEN2ui21GameEventNotificationE
    void die(EntityDamageSource const&); // _ZN11LocalPlayer3dieERK18EntityDamageSource
    void addItemCreative(ItemInstance const*); // _ZN11LocalPlayer15addItemCreativeEPK12ItemInstance
    void getInventoryMenu(); // _ZN11LocalPlayer16getInventoryMenuEv
    void getPortalEffectTime() const; // _ZNK11LocalPlayer19getPortalEffectTimeEv
    void setPortalEffectTime(float); // _ZN11LocalPlayer19setPortalEffectTimeEf
    void getOPortalEffectTime() const; // _ZNK11LocalPlayer20getOPortalEffectTimeEv
    void setOPortalEffectTime(float); // _ZN11LocalPlayer20setOPortalEffectTimeEf
    void swing(); // _ZN11LocalPlayer5swingEv
    void updateAi(); // _ZN11LocalPlayer8updateAiEv
    void getHudContainerManagerModel(); // _ZN11LocalPlayer27getHudContainerManagerModelEv
    void setHudContainerManagerModel(std::shared_ptr<HudContainerManagerModel>); // _ZN11LocalPlayer27setHudContainerManagerModelESt10shared_ptrI24HudContainerManagerModelE
    void stopSleepInBed(bool, bool); // _ZN11LocalPlayer14stopSleepInBedEbb
    void deleteContainerManager(); // _ZN11LocalPlayer22deleteContainerManagerEv
    void openTextEdit(BlockEntity*); // _ZN11LocalPlayer12openTextEditEP11BlockEntity
    void startSleepInBed(BlockPos const&); // _ZN11LocalPlayer15startSleepInBedERK8BlockPos
    void _init(); // _ZN11LocalPlayer5_initEv
    void addRegionListener(LocalPlayer::RegionListener*); // _ZN11LocalPlayer17addRegionListenerEPNS_14RegionListenerE
    void _crit(Entity&); // _ZN11LocalPlayer5_critER6Entity
    void openEnchanter(int, BlockPos const&); // _ZN11LocalPlayer13openEnchanterEiRK8BlockPos
    void rideTick(); // _ZN11LocalPlayer8rideTickEv
    void _modifyDeltaByHMDPosition(Vec3&, float); // _ZN11LocalPlayer25_modifyDeltaByHMDPositionER4Vec3f
    void localPlayerRenderFrequencyMove(float); // _ZN11LocalPlayer30localPlayerRenderFrequencyMoveEf
    void localPlayerTurn(Vec2 const&); // _ZN11LocalPlayer15localPlayerTurnERK4Vec2
    void changeDimension(DimensionId); // _ZN11LocalPlayer15changeDimensionE11DimensionId
    void changeDimension(ChangeDimensionPacket&); // _ZN11LocalPlayer15changeDimensionER21ChangeDimensionPacket
    void openAnvil(BlockPos const&); // _ZN11LocalPlayer9openAnvilERK8BlockPos
    void openBrewingStand(int, BlockPos const&); // _ZN11LocalPlayer16openBrewingStandEiRK8BlockPos
    void openContainer(int, BlockPos const&); // _ZN11LocalPlayer13openContainerEiRK8BlockPos
    void openContainer(int, EntityUniqueID const&); // _ZN11LocalPlayer13openContainerEiRK14EntityUniqueID
    void startCrafting(BlockPos const&); // _ZN11LocalPlayer13startCraftingERK8BlockPos
    void openFurnace(int, BlockPos const&); // _ZN11LocalPlayer11openFurnaceEiRK8BlockPos
    void openHopper(int, EntityUniqueID const&); // _ZN11LocalPlayer10openHopperEiRK14EntityUniqueID
    void openDispenser(int, BlockPos const&, bool); // _ZN11LocalPlayer13openDispenserEiRK8BlockPosb
    void openHopper(int, BlockPos const&); // _ZN11LocalPlayer10openHopperEiRK8BlockPos
    void openPortfolio(); // _ZN11LocalPlayer13openPortfolioEv
    void tickWorld(Tick const&); // _ZN11LocalPlayer9tickWorldERK4Tick
    void outOfWorld(); // _ZN11LocalPlayer10outOfWorldEv
    void hurtTo(int); // _ZN11LocalPlayer6hurtToEi
    void normalTick(); // _ZN11LocalPlayer10normalTickEv
    void aiStep(); // _ZN11LocalPlayer6aiStepEv
    void destroyRegion(); // _ZN11LocalPlayer13destroyRegionEv
    MoveInputHandler* _input(); // _ZN11LocalPlayer6_inputEv
};


