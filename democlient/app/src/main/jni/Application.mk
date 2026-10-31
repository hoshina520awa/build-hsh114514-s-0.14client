APP_ABI := armeabi-v7a 

# 0.14.3 时代是 android-14，但现代 NDK 支持的最低通常是 android-16 或 19。
# 考虑到 libc++ 兼容性，先用 android-16。
APP_PLATFORM := android-16

# 使用 c++_static 静态库！
# 这样会把 C++ 标准库直接打进你的 Mod.so 里，不需要额外推 libc++_shared.so，避免和老游戏环境冲突。
APP_STL := c++_static

# 强制新版 Spine 要求的标准
APP_CPPFLAGS := -std=c++17 -frtti -fexceptions

APP_OPTIM := release
APP_THIN_ARCHIVE := true
APP_PIE := true