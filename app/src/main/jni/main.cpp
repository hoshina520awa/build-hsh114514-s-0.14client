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
#include <GLES2/gl2.h>
#include <android/asset_manager_jni.h>
#include <android/asset_manager.h>

// ==========================================
// 引入 Spine 和 stb_image
// ==========================================
#include <spine/spine.h>
#include <spine/SkeletonRenderer.h>
#include <spine/Extension.h>
#include <Includes/stb_image.h>

using namespace spine;

spine::Skeleton* g_skeleton = nullptr;
spine::AnimationState* g_state = nullptr;
spine::SkeletonRenderer* g_renderer = nullptr;

// 给官方源码擦屁股：实现它缺的 getDefaultExtension
namespace spine {
    SpineExtension* getDefaultExtension() {
        static DefaultSpineExtension* extension = new DefaultSpineExtension();
        return extension;
    }
}

// 全局 AAssetManager
AAssetManager* g_AssetManager = nullptr;

// ==========================================
// Spine 纹理加载器 (适配新版 API)
// ==========================================
class MyTextureLoader : public TextureLoader {
public:
    void load(AtlasPage& page, const String& path) override {
        String fullPath = String("test/");
        fullPath.append(path); // 新版 String 拼接方式
        LOGD("Spine 尝试加载纹理: %s", fullPath.buffer());

        if (!g_AssetManager) {
            LOGD("错误：g_AssetManager 为空！读不了文件！");
            return;
        }

        AAsset* asset = AAssetManager_open(g_AssetManager, fullPath.buffer(), AASSET_MODE_BUFFER);
        if (!asset) {
            LOGD("错误：找不到文件 %s", fullPath.buffer());
            return;
        }

        int len = AAsset_getLength(asset);
        void* data = malloc(len);
        AAsset_read(asset, data, len);
        AAsset_close(asset);

        int width, height, channels;
        unsigned char* pixels = stbi_load_from_memory((unsigned char*)data, len, &width, &height, &channels, 4);
        free(data);

        if (!pixels) {
            LOGD("错误：stb_image 解码图片失败！");
            return;
        }

        GLuint textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        stbi_image_free(pixels);
        
        // 新版直接存到 page.texture
        page.texture = (void*)(intptr_t)textureID;
        LOGD("纹理加载成功 ID: %d", textureID);
    }

    // 修复报错：新版 unload 参数是 void*
    void unload(void* texture) override {
        GLuint id = (GLuint)(intptr_t)texture;
        glDeleteTextures(1, &id);
    }
};

// ==========================================
// 初始化 Spine 函数 (适配新版 API)
// ==========================================
void InitSpine() {
    LOGD("开始初始化 Spine...");
    if (!g_AssetManager) {
        LOGD("致命错误: g_AssetManager 为空，无法读取 assets 目录！");
        return;
    }

    // 新版直接 new Atlas，不用 spAtlas_createFromFile
    Atlas* atlas = new Atlas("test/CH0335_home.atlas", new MyTextureLoader());
    
    // 新版传引用 &atlas
    SkeletonBinary binary(*atlas);
    SkeletonData* skeletonData = binary.readSkeletonData("test/CH0335_home.skel");
    if (!skeletonData) {
        LOGD("Skel 加载失败！检查路径或版本: %s", binary.getError().buffer());
        return;
    }

    // 新版传引用 &skeletonData
    g_skeleton = new Skeleton(*skeletonData);
    g_state = new AnimationState(new AnimationStateData(*skeletonData));
    g_renderer = new SkeletonRenderer();

    // 打印动画名字（方便你找正确的动画名）
    for (size_t i = 0; i < skeletonData->getAnimations().size(); i++) {
        LOGD("找到动画: %s", skeletonData->getAnimations()[i]->getName().buffer());
    }

    // 播放动画（如果你不知道名字，根据上面的日志改，比如改成 "idle"）
    g_state->setAnimation(0, "animation", true); 

    // 设置大小和位置
    g_skeleton->setScaleX(1.0f); 
    g_skeleton->setScaleY(1.0f);
    g_skeleton->setX(500); 
    g_skeleton->setY(500);

    LOGD("Spine 初始化成功！");
}

// ==========================================

JavaVM *Main::vm = nullptr;
unsigned long Main::baseAddress = 0L;

int shadowhook_hook_func_addr(void *symbol, void *replace, void **result) {
    MSHookFunction(symbol, replace, result);
    return (result != nullptr);
}

template<typename T>
T *getSymbolArray(const char *symbol_name) {
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    void *symbol_address = dlsym(handle, symbol_name);
    if (!symbol_address)return nullptr;
    return reinterpret_cast<T *>(symbol_address);
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
    *(short *) (p + 56) = 0;
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
    if (entity == nullptr) return -1;
    Entity *e = entity.get();
    e->setPos(pos);
    if (type != 93) level->addEntity(std::move(entity));
    else level->addGlobalEntity(std::move(entity));
    return e->getUniqueID();
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

// ==========================================
// 通过 JNI 反射拿到 AssetManager
// ==========================================
void InitAssetManager() {
    JNIEnv *env = Main::currentEnv();
    if (!env) return;
    
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThreadMethod = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject activityThread = env->CallStaticObjectMethod(activityThreadClass, currentActivityThreadMethod);
    
    jmethodID getApplicationMethod = env->GetMethodID(activityThreadClass, "getApplication", "()Landroid/app/Application;");
    jobject application = env->CallObjectMethod(activityThread, getApplicationMethod);
    
    jclass contextClass = env->GetObjectClass(application);
    jmethodID getAssetsMethod = env->GetMethodID(contextClass, "getAssets", "()Landroid/content/res/AssetManager;");
    jobject assets = env->CallObjectMethod(application, getAssetsMethod);
    
    g_AssetManager = AAssetManager_fromJava(env, assets);
    LOGD("AssetManager 获取结果: %p", g_AssetManager);
}

jint JNI_OnLoad(JavaVM *vm, void *unused) {
    Main::vm = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }
    Main::baseAddress = Main::findLibBaseAddress("libminecraftpe.so");

    // 1. 获取 AssetManager
    InitAssetManager();

    const char *init_method_name = "_ZN15MinecraftClient4initEv";
    void *init_method_address = Main::findSymbol(init_method_name);
    if (!init_method_address) return JNI_ERR;

    if (shadowhook_hook_func_addr(init_method_address, (void *) MinecraftClient::_initMinecraftClient, (void **) &MinecraftClient::_initMinecraftClientOrigin)) {
        LOGD("Hook Successfully, baseAddress=0x%s", Main::toHexString(Main::baseAddress).c_str());
        ImGuiIME::Init(vm);
        
        // 修复报错：明确指出是 mcpe::Block，避免和 spine::Block 冲突
        mBlocks = getSymbolArray<mcpe::Block *>("_ZN5Block7mBlocksE");
        mItems = getSymbolArray<mcpe::Item *>("_ZN4Item6mItemsE");
    } else {
        LOGD("Hook Failed!");
    }

    // 2. 初始化 Spine
    InitSpine();

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
    if (ifstream.bad() || !ifstream.is_open()) return 0L;
    unsigned long localBaseAddress = 0L;
    std::string line;
    while (getline(ifstream, line)) {
        if (line.find(libName) != std::string::npos && line.find("x") != std::string::npos) {
            sscanf(line.c_str(), "%lx-%*lx", &localBaseAddress);
            break;
        }
    }
    ifstream.close();
    return localBaseAddress;
}

static int mem_patch(const void *addr, const void *data, size_t len) {
    long page = sysconf(_SC_PAGESIZE);
    uintptr_t start = (uintptr_t) addr & ~(page - 1);
    size_t len2 = ((uintptr_t) addr + len - start + page - 1) & ~(page - 1);
    if (mprotect((void *) start, len2, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) return -1;
    int fd = open("/proc/self/mem", O_RDWR);
    if (fd < 0) return -1;
    if (pwrite64(fd, data, len, (uintptr_t) addr) != (ssize_t) len) {
        close(fd); return -1;
    }
    unsigned char verify[len];
    if (pread64(fd, verify, len, (uintptr_t) addr) != (ssize_t) len || memcmp(verify, data, len) != 0) {
        close(fd); return -1;
    }
    close(fd);
    msync((void *) start, len2, MS_INVALIDATE);
    __builtin___clear_cache((char *) addr, (char *) addr + len);
    return 0;
}

void *Main::findSymbol(const char *symbol_name) {
    void *handle = dlopen("libminecraftpe.so", RTLD_LAZY);
    if (!handle) return nullptr;
    void *symbol_address = dlsym(handle, symbol_name);
    dlclose(handle);
    return symbol_address;
}

void *Main::findSymbol2(const char *symbol_name, const char *lib_name) {
    void *handle = dlopen(lib_name, RTLD_LAZY);
    if (!handle) return nullptr;
    void *symbol_address = dlsym(handle, symbol_name);
    dlclose(handle);
    return symbol_address;
}

jobject Main::getApplication() {
    JNIEnv *env = currentEnv();
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThreadMethod = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject activityThread = env->CallStaticObjectMethod(activityThreadClass, currentActivityThreadMethod);
    jmethodID getApplicationMethod = env->GetMethodID(activityThreadClass, "getApplication", "()Landroid/app/Application;");
    return env->CallObjectMethod(activityThread, getApplicationMethod);
}

jobject Main::getCurrentActivity() {
    JNIEnv *env = currentEnv();
    jclass activityThreadClass = env->FindClass("android/app/ActivityThread");
    jmethodID currentActivityThreadMethod = env->GetStaticMethodID(activityThreadClass, "currentActivityThread", "()Landroid/app/ActivityThread;");
    jobject activityThread = env->CallStaticObjectMethod(activityThreadClass, currentActivityThreadMethod);
    jmethodID getTopActivityMethod = env->GetMethodID(activityThreadClass, "getTopActivity", "()Landroid/app/Activity;");
    return env->CallObjectMethod(activityThread, getTopActivityMethod);
}

void Main::showToast(std::string message) {
    JNIEnv *env = currentEnv();
    jclass toastClass = env->FindClass("android/widget/Toast");
    jmethodID makeTextMethod = env->GetStaticMethodID(toastClass, "makeText", "(Landroid/content/Context;Ljava/lang/CharSequence;I)Landroid/widget/Toast;");
    jstring messageStr = env->NewStringUTF(message.c_str());
    jobject toast = env->CallStaticObjectMethod(toastClass, makeTextMethod, getApplication(), messageStr, 0);
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
        jmethodID getAppInfoMethod = env->GetMethodID(contextClass, "getApplicationInfo", "()Landroid/content/pm/ApplicationInfo;");
        if (!getAppInfoMethod) return "";
        jobject appInfo = env->CallObjectMethod(context, getAppInfoMethod);
        if (!appInfo) return "";
        jclass appInfoClass = env->GetObjectClass(appInfo);
        if (!appInfoClass) return "";
        jfieldID sourceDirField = env->GetFieldID(appInfoClass, "sourceDir", "Ljava/lang/String;");
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
