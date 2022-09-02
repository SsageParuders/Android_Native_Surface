//
// Created by 泓清 on 2022/8/26.
//
#include <native_surface/native_surface.h>
void* get_createNativeWindow(){
    void *handle;// 动态库方案
    void* sy;
    if (get_android_api_level() == __ANDROID_API_S__){ // 安卓12支持
        exec_native_surface("settings put global block_untrusted_touches 0");
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