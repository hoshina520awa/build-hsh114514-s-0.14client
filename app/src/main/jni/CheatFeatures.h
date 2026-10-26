#pragma once
#include <jni.h>
#include <GLES2/gl2.h>

// 菜单里的开关全局变量
extern bool g_ItemBigger;
extern float g_ItemScaleValue; // 默认 4.0f

extern bool g_RenderDistanceHack;
extern bool g_NoRotationLimit;
extern bool g_OverShoulderView;
extern bool g_BulletTime;

// 初始化（保存原始值，方便关闭时还原）
void InitCheatFeatures();

// 每帧刷新（如果开着功能，就持续向内存写入）
void UpdateCheatFeatures();

// 安全写内存函数（处理 mprotect，防止闪退）
void WriteFloatSafe(uintptr_t addr, float value);
void WriteBytesSafe(uintptr_t addr, const unsigned char* bytes, size_t len);