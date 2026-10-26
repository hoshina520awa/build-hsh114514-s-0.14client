#include "CheatFeatures.h"
#include "main.h"
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <android/log.h>

#define TAG "CheatFeatures"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)

bool g_ItemBigger = false;
float g_ItemScaleValue = 4.0f;
bool g_RenderDistanceHack = false;
bool g_NoRotationLimit = false;
bool g_OverShoulderView = false;
bool g_BulletTime = false;

// 保存原值（用于还原）
static unsigned char orig_item_scale[4];
static float orig_render_dist = 0.1f;

// 安全写浮点数
void WriteFloatSafe(uintptr_t addr, float value) {
    if (!addr) return;
    long page_size = sysconf(_SC_PAGESIZE);
    void* page_start = (void*)(addr & ~(page_size - 1));
    
    // 赋予读写执行权限
    mprotect(page_start, page_size, PROT_READ | PROT_WRITE | PROT_EXEC);
    *(float*)addr = value;
    // 还原只读执行权限（可选，但保持原貌比较好）
    // mprotect(page_start, page_size, PROT_READ | PROT_EXEC);
}

// 安全写字节数组
void WriteBytesSafe(uintptr_t addr, const unsigned char* bytes, size_t len) {
    if (!addr) return;
    long page_size = sysconf(_SC_PAGESIZE);
    uintptr_t start = addr & ~(page_size - 1);
    size_t len2 = (addr + len - start + page_size - 1) & ~(page_size - 1);
    
    mprotect((void*)start, len2, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy((void*)addr, bytes, len);
    __builtin___clear_cache((char*)addr, (char*)addr + len);
}

void InitCheatFeatures() {
    if (!Main::baseAddress) return;
    
    // 备份手持物品放大的原始指令
    uintptr_t item_addr = Main::baseAddress + 0x486044;
    memcpy(orig_item_scale, (void*)item_addr, 4);

    // 备份渲染距离
    orig_render_dist = *(float*)(Main::baseAddress + 0x46d5d0);
    LOGI("CheatFeatures 初始化完成");
}

void UpdateCheatFeatures() {
    if (!Main::baseAddress) return;

    // 1. 手持物品放大（原指令 mov.w r3, #0x3fc00000 -> 改为 4.0F 的 float 字节）
    if (g_ItemBigger) {
        uintptr_t addr = Main::baseAddress + 0x486044;
        // ARM Thumb 汇编：mov.w r3, #0x40800000 (4.0f 的 float 二进制)
        // 实际上直接改寄存器不太可能，我们通过修改它加载的常量池或者直接替换指令
        // 原版: 41 f6 00 03 (movw r3, #...)
        // 根据你的笔记，是直接改了指令。由于直接写汇编指令很危险，我们提供一个安全方案：
        // 我们暂时将其替换为浮点数写入（如果它是一个 float 变量地址）。
        // 考虑到原版是汇编指令，我们直接写汇编机器码：
        // 4.0f = 0x40800000
        // mov.w r3, #0x40800000 的机器码是 4F F0 80 43 (指令因架构不同可能略有差异)
        // 由于直接改汇编风险极大，我们这里做一个“暴力内存写入”，只修改偏移对应的浮点数
        // *注意：如果你的笔记是指令，那么直接改浮点会闪退。这里先保留代码，看实际测试。
        // 稳妥起见：挂载后交由后续处理。
    }

    // 2. 超远渲染距离 (0x46d5d0，修改为 1000.0F)
    if (g_RenderDistanceHack) {
        WriteFloatSafe(Main::baseAddress + 0x46d5d0, 1000.0f);
        WriteFloatSafe(Main::baseAddress + 0x46d5d4, 114514.0f); // 第二个值
    } else {
        WriteFloatSafe(Main::baseAddress + 0x46d5d0, orig_render_dist);
        WriteFloatSafe(Main::baseAddress + 0x46d5d4, 0.05f);
    }

    // 3. 视角无旋转限制 (0x49dc20)
    if (g_NoRotationLimit) {
        // 原值 -90, 90; 改 -114514, 114514
        WriteFloatSafe(Main::baseAddress + 0x49dc20, -114514.0f);
        WriteFloatSafe(Main::baseAddress + 0x49dc24, 114514.0f);
    } else {
        WriteFloatSafe(Main::baseAddress + 0x49dc20, -90.0f);
        WriteFloatSafe(Main::baseAddress + 0x49dc24, 90.0f);
    }

    // 4. 第三人称穿墙/过肩 (0x46890c)
    if (g_OverShoulderView) {
        WriteFloatSafe(Main::baseAddress + 0x46890c, 10000.0f);
    } else {
        WriteFloatSafe(Main::baseAddress + 0x46890c, 0.1f);
    }

    // 5. 全局速度/子弹时间 (0x4be9f0)
    if (g_BulletTime) {
        WriteFloatSafe(Main::baseAddress + 0x4be9f0, 0.001f);
    } else {
        WriteFloatSafe(Main::baseAddress + 0x4be9f0, 1.0f); // 正常速度是 1.0
    }
}