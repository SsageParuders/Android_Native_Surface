//
// Created by fgsqme on 2022/9/9.
//

#ifndef NATIVESURFACE_EXTERN_FUNCTION_H
#define NATIVESURFACE_EXTERN_FUNCTION_H
// System libs
#include <iostream>
#include <cstdlib>
#include <android/api-level.h>
// User libs
#include "utils.h"
#include <android/native_window.h>

struct MDisplayInfo {
    uint32_t width{0};
    uint32_t height{0};
    uint32_t orientation{0};
};

// 方法指针
struct FuncPointer {
    void *func_createNativeWindow;
    //    更多可选参数
    void *func_more_createNativeWindow;
    void *func_getDisplayInfo;
//    void *func_setSurfaceWH;
    void *func_runRecord;
    void *func_stopRecord;
    void *func_initRecord;
    void *func_getRecordNativeWindow;
};

class ExternFunction {
public:
    ExternFunction();

    /**
     * 创建 native surface
     * @param surface_name 创建名称
     * @param screen_width 创建宽度
     * @param screen_height 创建高度
     * @param author 是否打印作者信息
     */
    ANativeWindow *
    createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author);

    /**
     * (更多可选参数)创建 native surface
     * @param surface_name 创建名称
     * @param screen_width 创建宽度
     * @param screen_height 创建高度
     * @param format format
     * @param flags flags
     * @param author 是否打印作者信息
     * @return
     */
    ANativeWindow *
    createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, uint32_t format,
                       uint32_t flags, bool author);

    /**
     * 获取屏幕宽高以及旋转状态
     */
    MDisplayInfo getDisplayInfo();

    /**
     * 设置画布大小(没啥用暂时屏蔽)
     * @param width
     * @param height
     */
//    void setSurfaceWH(uint32_t width, uint32_t height);

    /**
     * 录屏初始化
     */
    void initRecord(const char *bitRate, float fps,
                    uint32_t videoWidth = 0, uint32_t videoHeight = 0);

    /**
     * 开始录屏
     * @param gStopRequested
     * @param callback
     */
    void runRecord(bool *flag, void callback(uint8_t *, size_t));

    /**
     * 录屏结束调用
     */
    void stopRecord();

    /**
     * 获取录屏window
     * @return
     */
    ANativeWindow *getRecordNativeWindow();


};


#endif //NATIVESURFACE_EXTERN_FUNCTION_H
