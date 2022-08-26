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
// User libs
//#include <touch.h>
#include <native_surface.h>
#include <imgui.h>
#include <font/Font.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_android.h>
// namespace
using namespace std;
using namespace std::chrono_literals;
extern void *handle;// 动态库方案
extern EGLDisplay display;
extern EGLConfig config;
extern EGLSurface surface;
extern NativeWindowType native_window;
extern NativeWindowType (*createNativeWindow)(const char *surface_name ,uint32_t screen_width ,uint32_t screen_height);
extern EGLContext context;
struct Screen {
    float ScreenX;
    float ScreenY;
};
extern Screen full_screen;
extern int Orientation;
extern int screen_x, screen_y;
extern int init_screen_x, init_screen_y;
extern bool g_Initialized;
// Func
string exec(string command);
int init_egl(int _screen_x,int _screen_y, bool log = false);
void screen_config();
void ImGui_init();
void tick();
void shutdown();
#endif //NATIVESURFACE_DRAW_H
