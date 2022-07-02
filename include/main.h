//
// Created by Ssage on 2022/3/18.
//

#ifndef SSAGEPARUDERS_NATIVE_SURFACE_H
#define SSAGEPARUDERS_NATIVE_SURFACE_H
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
#include <touch.h>
#include <native_surface.h>
#include <imgui.h>
#include <font/Font.h>
#include <imgui_internal.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_android.h>
// namespace
using namespace std;
using namespace std::chrono_literals;
// Var
void *handle;// 动态库方案
EGLDisplay display = EGL_NO_DISPLAY;
EGLConfig config;
EGLContext context = EGL_NO_CONTEXT;
EGLSurface surface = EGL_NO_SURFACE;
NativeWindowType native_window;
//struct Screen { // FixMe: DO
//    float ScreenX;
//    float ScreenY;
//}

Screen full_screen;
int Orientation = 0;
int screen_x = 0, screen_y = 0;
int init_screen_x = 0, init_screen_y = 0;
bool g_Initialized = false;
touch mytouch;
// Func
string exec(string command);
void screen_config();
void ImGui_init();
void tick();
void shutdown();
NativeWindowType (*createNativeWindow)(const char *surface_name ,uint32_t screen_width ,uint32_t screen_height);
#endif //SSAGEPARUDERS_NATIVE_SURFACE_H