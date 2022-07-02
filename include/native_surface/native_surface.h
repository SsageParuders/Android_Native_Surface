//
// Created by Ssage on 2022/6/29.
//

#ifndef NATIVESURFACE_NATIVE_SURFACE_H
#define NATIVESURFACE_NATIVE_SURFACE_H
// System libs
#include <iostream>
#include <cstdlib>
#include <android/api-level.h>
// User libs
#include "utils.h"
#include "aosp_11/native_surface_11.h"
#include "aosp_10/native_surface_10.h"
#include "aosp_12/native_surface_12.h"
void* get_createNativeWindow(){
    void *handle;// 动态库方案
    void* sy;
    if (get_android_api_level() == __ANDROID_API_S__){ // 安卓12支持
#ifdef __aarch64__
        handle = dlblob(&native_surface_12_64, sizeof(native_surface_12_64)); // 64位支持
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#elif
        handle = dlblob(&native_surface_12_32, sizeof(native_surface_12_32)); // 32位支持 <<-- 其实很没必要 未测试
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#endif
    } else if (get_android_api_level() == __ANDROID_API_R__) { // 安卓11支持
#ifdef __aarch64__
        handle = dlblob(&native_surface_11_64, sizeof(native_surface_11_64)); // 64位支持
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#elif
        handle = dlblob(&native_surface_11_32, sizeof(native_surface_11_32)); // 32位支持 <<-- 其实很没必要 未测试
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#endif
    } else if (get_android_api_level() == __ANDROID_API_Q__){ // 安卓10支持
#ifdef __aarch64__
        handle = dlblob(&native_surface_10_64, sizeof(native_surface_10_64)); // 64位支持
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#elif
        handle = dlblob(&native_surface_10_32, sizeof(native_surface_10_32)); // 32位支持 <<-- 其实很没必要 未测试
        sy = dlsym(handle,"_Z18createNativeWindowPKcjj");
#endif
    } else { // 不在安卓10-12中 不在支持列表
        printf("Sorry, Don't Support~");
        exit(0);
    }
    return sy;
}
#endif //NATIVESURFACE_NATIVE_SURFACE_H
