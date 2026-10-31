#ifndef MAIN_H
#define MAIN_H

#include <Substrate/SubstrateHook.h>
#include <Substrate/CydiaSubstrate.h>
#include <jni.h>
#include <string>
#include <fstream>
#include <sstream>
#include <android/log.h>
#include <sys/mman.h>
#include <unordered_map>
#include "Block.h"
#include "EntityUniqueID.h"
#include "Entity.h"
#include "Attribute.h"
#include "mcpe/Inventory.h"
#include "mcpe/Player.h"
#include "mcpe/Item.h"

#define TAG "START"
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)

int shadowhook_hook_func_addr(void *symbol, void *replace, void **result);

static Block **mBlocks;
static Item **mItems;

class Main {
private:
    typedef void (*CreatePacket_t)(void *outPacket, int id);

    inline static CreatePacket_t cpfunc;
public:
    Main() = delete;

    static JavaVM *vm;

    static unsigned long baseAddress;

    static JNIEnv *currentEnv();

    static int myPid();

    static void *findSymbol(const char *symbol_name);

    static void *findSymbol2(const char *symbol_name, const char *lib_name);

    static unsigned long findLibBaseAddress(std::string libName);

    static jobject getApplication();

    static jobject getCurrentActivity();

    static void showToast(std::string message);

    static std::string getApplicationPath();

    static std::string toHexString(long value);

    static inline void *absoluteAddress(unsigned long offset) {
        return reinterpret_cast<void *>(Main::baseAddress + offset);
    }

    static inline void *CreatePacket(int packetId) {
        if (!cpfunc)cpfunc = (CreatePacket_t) findSymbol("_ZN16MinecraftPackets12createPacketEi");
        void *pkt = nullptr;
        cpfunc(&pkt, packetId);
        return pkt;
    }

};

#endif //MAIN_H
