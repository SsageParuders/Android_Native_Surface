//
// Created by 泓清 on 2022/8/26.
//
#ifndef NATIVESURFACE_DRAW_H
#define NATIVESURFACE_DRAW_H
// System libs
#include <iostream>
#include <thread>
#include <chrono>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include <dlfcn.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <android/native_window.h>
#include <Egl/eglext.h>
#include <GLES3/gl3platform.h>
#include <GLES3/gl3ext.h>
#include <GLES3/gl32.h>

// User libs
//#include <touch.h>
#include "native_surface/extern_function.h"
#include <imgui.h>
#include <font/Font.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_android.h>

// namespace
using namespace std;
using namespace std::chrono_literals;
//extern EGLDisplay display;
//extern EGLConfig config;
//extern EGLSurface surface;
//extern ANativeWindow *native_window;
extern ExternFunction externFunction;
//extern EGLContext context;
// 屏幕信息
extern MDisplayInfo displayInfo;
extern bool g_Initialized;

// Func
bool init_egl(uint32_t _screen_x, uint32_t _screen_y, bool log = false);

bool initDraw(uint32_t _screen_x, uint32_t _screen_y, bool log = false);

bool initDraw(bool log = false);

bool ImGui_init();

void screen_config();

void drawBegin();

void drawEnd();

void shutdown();

#endif //NATIVESURFACE_DRAW_H
