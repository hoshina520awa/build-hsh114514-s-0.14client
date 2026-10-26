#pragma once
#include <GLES2/gl2.h>

extern bool g_MotionBlurEnabled;
extern float g_MotionBlurIntensity;

void InitMotionBlur(int width, int height);
void ApplyMotionBlur();
void CleanupMotionBlur();