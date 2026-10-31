#include "main.h"
#include "mcpe/MinecraftGame.h"
#include "mcpe/I18n.h"
#include "mcpe/Item.h"
#include "mcpe/ItemRenderer.h"
#include "mcpe/GuiMessage.h"
#include "mcpe/PacketSender.h"
#include "mcpe/Enchant.h"
#include "mcpe/EntityClassTree.h"
#include "MobFactory.h"
#include "MobEffect.h"
#include "StrBuild.h"
#include "Hook/VtableHook.h"
#include "ImGuiInputIME.h"
#include "ImguiInput.h"
#include "ImguiManager.h"
#include <string>
#include <sstream>
#include <Includes/obfuscate.h>
#include <Includes/Utils.h>
#include <android/log.h>
#include <fstream>
#include <vector>
#include <csignal>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <cerrno>
#include <fcntl.h>
#include <cstring>
#include <iterator>
#include <algorithm>
#include <elf.h>
#include <link.h>
#include <dlfcn.h>
#include <new>
#include <thread>

// ==========================================
// 引入 Spine 头文件和全局变量实体
// ==========================================
#include <spine/spine.h>
#include <spine/SkeletonRenderer.h>
#include <spine/Extension.h>

spine::Skeleton* g_skeleton = nullptr;
spine::AnimationState* g_state = nullptr;
spine::SkeletonRenderer* g_renderer = nullptr;
// ==========================================

JavaVM *Main::vm = nullptr;
unsigned long Main::baseAddress = 0L;

int shadowhook_hook_func_addr(void *symbol, void *replace, void **result) {
    MSHookFunction(symbol, replace, result);
    if (result != nullptr) {
        return true;
    } else {
        return false;
    }
}

template<typename T>
T *getSymbolArray(const char *symbol_name) {
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    void *symbol_address = dlsym(handle, symbol_name);
    if (!symbol_address)return nullptr;
    T *array_pointer = reinterpret_cast<T *>(symbol_address);
    return array_pointer;
}

void PacketSend(Packet *packet) {
    MinecraftClient *minecraftGame = MinecraftClient::getInstance();
    Minecraft *minecraft = minecraftGame->getServer();
    PacketSender *packetsend = minecraft->getPacketSender();
    packetsend->send(*packet);
}


void SendMessage(std::string message) {
    MinecraftClient *minecraftGame = MinecraftClient::getInstance();
    Player *player = minecraftGame->getLocalPlayer();
    std::string *nametag = player->getNameTag();
    TextPacket pkt;
    pkt.type = 1;
    memset(pkt.filler2, 0, sizeof(pkt.filler2));
    memset(pkt.filler3, 0, sizeof(pkt.filler3));
    pkt.shouldBeZero = 0;
    pkt.shouldBeOne = 1;
    pkt.username = *nametag;
    pkt.message = message;
    PacketSend(&pkt);
}

void SendInteract(uint8_t action, uint64_t targetRuntimeId) {
    void *pkt = Main::CreatePacket(169);
    uintptr_t p = (uintptr_t) pkt;
    *(int *) (p + 4) = 1;
    *(int *) (p + 8) = 2;
    *(uint8_t *) (p + 12) = 1;
    *(uint8_t *) (p + 13) = action;
    *(long long *) (p + 16) = targetRuntimeId;
    PacketSend((Packet *) pkt);
}

void SendUseItem(int x, int y, int z, int face, float px, float py, float pz) {
    void *pkt = Main::CreatePacket(170);
    if (!pkt) return;
    uintptr_t p = (uintptr_t) pkt;
    *(int *) (p + 4) = 2;
    *(int *) (p + 8) = 1;
    *(int *) (p + 16) = x;
    *(int *) (p + 20) = y;
    *(int *) (p + 24) = z;
    *(uint8_t *) (p + 28) = (uint8_t) face;
    *(float *) (p + 32) = px;
    *(float *) (p + 36) = py;
    *(float *) (p + 40) = pz;
    *(float *) (p + 44) = 0.5f;
    *(float *) (p + 48) = 0.5f;
    *(float *) (p + 52) = 0.5f;
    *(short *) (p + 56) = 0;//TODO write ItemInstance
    *(int *) (p + 76) = -1;
    PacketSend((Packet *) pkt);
}



std::unordered_map<Entity *, EntityUniqueID> getAllPlayer() {
    std::unordered_map<Entity *, EntityUniqueID> result;
    MinecraftClient *mc = MinecraftClient::getInstance();
    if (!mc) return result;
    Minecraft *server = mc->getServer();
    if (!server) return result;
    Level *level = server->getLevel();
    if (!level) return result;
    Entity *local = (Entity *) mc->getLocalPlayer();
    if (!local) return result;
    Player **begin = *(Player ***) ((uintptr_t) level + 12);
    Player **end = *(Player ***) ((uintptr_t) level + 16);
    for (Player **it = begin; it != end; ++it) {
        Player *player = *it;
        if (!player) continue;
        if ((Entity *) player == local) continue;
        EntityUniqueID uid = player->getUniqueID();
        if (uid.id == 0) continue;
        result.emplace((Entity *) player, uid);
    }
    return result;
}

EntityUniqueID SpawnEntity(float x, float y, float z, int type) {
    MinecraftClient *minecraftGame = MinecraftClient::getInstance();
    Minecraft *minecraft = minecraftGame->getServer();
    if (!minecraft)return -1;
    Level *level = minecraft->getLevel();
    if (!level)return -1;
    Player *player = minecraftGame->getLocalPlayer();
    if (!player)return -1;
    std::unique_ptr<Entity> entity;
    Vec3 pos;
    if (x == -1 || y == -1 || z == -1) {
        pos.x = player->getX();
        pos.y = player->getY();
        pos.z = player->getZ();
    } else {
        pos.x = x;
        pos.y = y;
        pos.z = z;
    }
    Vec2 rot{0, 0};
    if (type < 64) {
        entity = MobFactory::CreateMob((EntityType) type, *player->getRegion(), pos, rot);
    } else {
        entity = EntityFactory::CreateEntity((EntityType) type, *player->getRegion());
    }

    if (entity == nullptr) {
        return -1;
    }
    Entity *e = entity.get();
    e->setPos(pos);

    if (type != 93) {
        level->addEntity(std::move(entity));
    } else {
        level->addGlobalEntity(std::move(entity));
    }
    EntityUniqueID id = e->getUniqueID();
    return id;
}

void AddEffect(int potionId, int duration, int amplifier, bool ambient, bool showParticles) {
    MinecraftClient *minecraftGame = MinecraftClient::getInstance();
    if (!minecraftGame)return;
    Minecraft *minecraft = minecraftGame->getServer();
    if (!minecraft)return;
    Level *level = minecraft->getLevel();
    if (!level)return;
    Player *player = minecraftGame->getLocalPlayer();
    MobEffectInstance effect(potionId, duration, amplifier, ambient, showParticles);
    player->addEffect(effect);
}

jint JNI_OnLoad(JavaVM *vm, void *unused) {
    Main::vm = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        LOGE("JNI_VERSION is not 1.6");
        return JNI_ERR;
    }
    Main::baseAddress = Main::findLibBaseAddress("libminecraftpe.so");

    const char *init_method_name = "_ZN15MinecraftClient4initEv";
    void *init_method_address = Main::findSymbol(init_method_name);
    if (!init_method_address) {
        LOGE("Failed to find init method address");
        return JNI_ERR;
    }

    if (shadowhook_hook_func_addr(
            init_method_address,
            (void *) MinecraftClient::_initMinecraftClient,
            (void **) &MinecraftClient::_initMinecraftClientOrigin)) {

        std::ostringstream oss;
        oss << "Hook Successfully, pid=" << Main::myPid()
            << ", baseAddress=0x" << Main::toHexString(Main::baseAddress);
        LOGD(oss.str().c_str());

        ImGuiIME::Init(vm);

        mBlocks = getSymbolArray<Block *>("_ZN5Block7mBlocksE");
        mItems = getSymbolArray<Item *>("_ZN4Item6mItemsE");

    } else {
        std::ostringstream oss;
        oss << "Hook Failed, pid=" << Main::myPid()
            << ", baseAddress=0x" << Main::toHexString(Main::baseAddress);
        LOGD(oss.str().c_str());
    }

    GL_Hook_Init();
    initImGuiInput();
    return JNI_VERSION_1_6;
}

void JNI_OnUnload(JavaVM *vm, void *unused) {
    Main::vm = nullptr;
}

JNIEnv *Main::currentEnv() {
    JNIEnv *env = nullptr;
    vm->AttachCurrentThread(&env, nullptr);
    return env;
}

int Main::myPid() {
    JNIEnv *env = currentEnv();
    jclass processClass = env->FindClass("android/os/Process");
    jmethodID myPidMethod = env->GetStaticMethodID(processClass, "myPid", "()I");
    return env->CallStaticIntMethod(processClass, myPidMethod);
}

unsigned long Main::findLibBaseAddress(std::string libName) {
    int pid = myPid();
    std::ifstream ifstream;
    std::ostringstream ostringstream;
    ostringstream << "/proc/" << pid << "/maps";

    ifstream.open(ostringstream.str(), std::ios::in);
    if (ifstream.bad() || !ifstream.is_open()) {
        ifstream.close();
        return 0L;
    }

    unsigned long localBaseAddress = 0L;
    std::string line;
    while (getline(ifstream, line)) {
        if (line.find(libName) != std::string::npos && line.find("x") != std::string::npos) {
            sscanf(line.c_str(), "%lx-%*lx", &localBaseAddress);
            break;
        }
    }

    ifstream.close();
    ostringstream.clear();
    return localBaseAddress;
}

static int mem_patch(const void *addr, const void *data, size_t len) {
    long page = sysconf(_SC_PAGESIZE);
    uintptr_t start = (uintptr_t) addr & ~(page - 1);
    size_t len2 = ((uintptr_t) addr + len - start + page - 1) & ~(page - 1);

    if (mprotect((void *) start, len2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        return -1;
    }

    int fd = open("/proc/self/mem", O_RDWR);
    if (fd < 0) {
        return -1;
    }

    if (pwrite64(fd, data, len, (uintptr_t) addr) != (ssize_t) len) {
        close(fd);
        return -1;
    }

    unsigned char verify[len];
    if (pread64(fd, verify, len, (uintptr_t) addr) != (ssize_t) len ||
        memcmp(verify, data, len) != 0) {
        close(fd);
        return -1;
    }

    close(fd);
    msync((void *) start, len2, MS_INVALIDATE);
    __builtin___clear_cache((char *) addr, (char *) addr + len);
    return 0;
}

void *Main::findSymbol(const char *symbol_name) {
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    if (!handle) {
        return nullptr;
    }

    void *symbol_address = dlsym(handle, symbol_name);
    if (!symbol_address) {
        dlclose(handle);
        return nullptr;
    }

    dlclose(handle);
    return symbol_address;
}

void *Main::findSymbol2(const char *symbol_name, const char *lib_name) {

    void *handle = dlopen(lib_name, RTLD_LAZY);
    if (!handle) {
        return nullptr;
    }

    void *symbol_address = dlsym(handle, symbol_name);
    if (!symbol_address) {
        dlclose(handle);
        return nullptr;
    }

    dlclose(handle);
    return symbol_address;
}

jobject Main::getApplication() {
    JNIEnv *env = currentEnv();
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThreadMethod = env->GetStaticMethodID(activityThreadClass,
                                                                   "currentActivityThread",
                                                                   "()Landroid/app/ActivityThread;");
    jobject activityThread = env->CallStaticObjectMethod(activityThreadClass,
                                                         currentActivityThreadMethod);
    jmethodID getApplicationMethod = env->GetMethodID(activityThreadClass,
                                                      "getApplication",
                                                      "()Landroid/app/Application;");
    jobject application = env->CallObjectMethod(activityThread, getApplicationMethod);
    return application;
}

jobject Main::getCurrentActivity() {
    JNIEnv *env = currentEnv();
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThreadMethod = env->GetStaticMethodID(activityThreadClass,
                                                                   "currentActivityThread",
                                                                   "()Landroid/app/ActivityThread;");
    jobject activityThread = env->CallStaticObjectMethod(activityThreadClass,
                                                         currentActivityThreadMethod);

    jmethodID getTopActivityMethod = env->GetMethodID(activityThreadClass,
                                                      "getTopActivity",
                                                      "()Landroid/app/Activity;");
    jobject activity = env->CallObjectMethod(activityThread, getTopActivityMethod);
    return activity;
}


void Main::showToast(std::string message) {
    JNIEnv *env = currentEnv();
    jclass toastClass = env->FindClass("android/widget/Toast");
    jmethodID makeTextMethod = env->GetStaticMethodID(toastClass, "makeText",
                                                      "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    jstring messageStr = env->NewStringUTF(message.c_str());
    jobject toast = env->CallStaticObjectMethod(toastClass, makeTextMethod,
                                                getApplication(), messageStr, 0);
    jmethodID showMethod = env->GetMethodID(toastClass, "show", "()V");
    env->CallVoidMethod(toast, showMethod);
}

std::string Main::getApplicationPath() {
    JNIEnv *env = currentEnv();
    if (!env) return "";
    try {
        jobject context = getApplication();
        if (!context) return "";

        jclass contextClass = env->GetObjectClass(context);
        if (!contextClass) return "";

        jmethodID getAppInfoMethod = env->GetMethodID(
                contextClass, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
        if (!getAppInfoMethod) return "";

        jobject appInfo = env->CallObjectMethod(context, getAppInfoMethod);
        if (!appInfo) return "";

        jclass appInfoClass = env->GetObjectClass(appInfo);
        if (!appInfoClass) return "";

        jfieldID sourceDirField = env->GetFieldID(
                appInfoClass, "sourceDir", "Ljava/lang/String;");
        if (!sourceDirField) return "";

        jstring sourceDir = (jstring) env->GetObjectField(appInfo, sourceDirField);
        if (!sourceDir) return "";

        const char *path = env->GetStringUTFChars(sourceDir, nullptr);
        std::string result(path ? path : "");
        if (path) {
            env->ReleaseStringUTFChars(sourceDir, path);
        }

        env->DeleteLocalRef(contextClass);
        env->DeleteLocalRef(appInfo);
        env->DeleteLocalRef(appInfoClass);
        env->DeleteLocalRef(sourceDir);

        return result;
    } catch (...) {
        return "";
    }
}


std::string Main::toHexString(long value) {
    std::ostringstream ostringstream;
    ostringstream << std::hex << value;
    return ostringstream.str();
}

extern "C" {

}
