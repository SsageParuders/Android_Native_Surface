/*
 * Codes from SsageParuders[https://github.com/SsageParuders]
 * 代码由泓清提供
 */
#include "aosp_surface.h"
#include "aosp_record.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <utils/StrongPointer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <ui/DisplayInfo.h>
#include <gui/ISurfaceComposer.h>

using namespace android;
using namespace std;
sp <SurfaceComposerClient> gSurfaceComposerClient;
sp <SurfaceControl> gSurfaceControl;
sp <IBinder> token;

/*
 * Next codes mostly withaout changes from Khronos intro
 */

/* Already defined in frameworks/native/opengl/include/EGL/eglplatform.h like
 * typedef struct ANativeWindow*           EGLNativeWindowType;
 * ...
 * typedef EGLNativeWindowType  NativeWindowType;
 */
// typedef ... NativeWindowType;

NativeWindowType
createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author) {
    return createNativeWindow(surface_name, screen_width, screen_height, android::PIXEL_FORMAT_RGBA_8888, 0, author);
}

NativeWindowType createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height,
                                    uint32_t format, uint32_t flags, bool author) {

    if (author) {
        std::cout << "Free SoftWare From GitHub: https://github.com/SsageParuders/Android_Native_Surface" << std::endl;
        std::cout << "Made by SsageParuders[泓清]" << std::endl;
    }

    NativeWindowType ret = nullptr;
    gSurfaceComposerClient = android::sp < android::SurfaceComposerClient > {new android::SurfaceComposerClient()};
    gSurfaceControl = gSurfaceComposerClient->createSurface(android::String8(surface_name),
                                                            screen_width,
                                                            screen_height,
                                                            format,
                                                            flags);
    if (!gSurfaceControl) {
        std::cout << "!gSurfaceControl" << std::endl;
    } else if (!gSurfaceControl->isValid()) {
        std::cout << "!gSurfaceControl->isValid()" << std::endl;
    } else {
        // TODO: !BUGS FINDED : Vector<> have different types
        // fixed bugs from aosp & miui | see https://github.com/DeviceFarmer/minicap/issues/6
        /* patch frameworks/native/libs/gui/include/gui/LayerState.h below
         * [ ] uint32_t width, height;
         * [+] uint8_t _dummy[16]; // patch Google AOSP & MIUI
         * [ ] status_t write(Parcel& output) const;
         * [ ] status_t read(const Parcel& input);
         */
        // android::SurfaceComposerClient::Transaction{}
        //         .setLayer(gSurfaceControl, INT_MAX)
        //         .show(gSurfaceControl)
        //         .apply();
        // TODO: !BUGS FINDED
        ret = gSurfaceControl->getSurface().get();
    }
    return ret;
}

void setSurfaceWH(uint32_t width, uint32_t height) {
    android::SurfaceComposerClient::Transaction{}
            .setLayer(gSurfaceControl, INT_MAX)
            .show(gSurfaceControl)
            .setPosition(gSurfaceControl, width, height)
            .apply();
}


sp <IBinder> getMDisplayToken() {
    if (!token) {
        sp <IBinder> binder(SurfaceComposerClient::getBuiltInDisplay(
            ISurfaceComposer::eDisplayIdMain));
        if (binder) {
            // std::cout << "getDisplayToken ok...." << std::endl;
            token = binder;
        } else {
            std::cout << "getDisplayToken err...." << std::endl;
            return NULL;
        }
    }
    return token;
}

MDisplayInfo getDisplayInfo() {
    sp <IBinder> localToken = getMDisplayToken();
    DisplayInfo mainDisplayInfo;
    // 获取手机的屏幕信息
    status_t err = SurfaceComposerClient::getDisplayInfo(localToken, &mainDisplayInfo);
    MDisplayInfo mDisplayInfo;
    if (err != NO_ERROR) {
        std::cout << "getDisplayInfo err...." << std::endl;
        return mDisplayInfo;
    }
    if (mainDisplayInfo.orientation == DISPLAY_ORIENTATION_90 ||
        mainDisplayInfo.orientation == DISPLAY_ORIENTATION_270) {
        //屏幕宽
        mDisplayInfo.width = mainDisplayInfo.h;
        //屏幕高
        mDisplayInfo.height = mainDisplayInfo.w;
    } else {
        //屏幕宽
        mDisplayInfo.width = mainDisplayInfo.w;
        //屏幕高
        mDisplayInfo.height = mainDisplayInfo.h;
    }
    mDisplayInfo.orientation = mainDisplayInfo.orientation;
    return mDisplayInfo;
}

void runRecord(bool *runFlag, void callback(uint8_t *, size_t)) {
    runEncoder(runFlag, callback);
}

NativeWindowType getRecordNativeWindow() {
    return getRecordWindow();
}

void initRecord(const char *bitRate, float fps,
                uint32_t videoWidth, uint32_t videoHeight) {
    initScreenrecord(bitRate, fps, videoWidth, videoHeight);
}

void stopRecord() {
    stopScreenrecord();
}

// void destroy1(){
//     if (gSurfaceControl && gSurfaceControl->isValid()) {
//         gSurfaceControl->destroy();
//     }
// }
/* backed
    // if ( NULL == gSurfaceComposerClient.get() ) {
    //     printf("Unable to establish connection to Surface Composer \n");
    //     return NULL;
    // }

    // auto transaction = new android::SurfaceComposerClient::Transaction();
    // std::cout << "New transaction is OK ~~" << std::endl;
    // const android::Parcel  parcel;
    // transaction->createFromParcel(&parcel);
    // std::cout << "createFromParcel is OK ~~" << std::endl;
    // if ( NULL == gSurfaceControl.get() ) {
    //     printf("Unable to create preview surface \n");
    //     return nullptr;
    // }

    // std::cout << "Everything is OK ~~" << std::endl;

    // transaction->setLayer(gSurfaceControl, 0x7fffffff);
    // std::cout << "setLayer is OK ~~" << std::endl;
    // transaction->show(gSurfaceControl);
    // std::cout << "show is OK ~~" << std::endl;
    // transaction->setPosition

    // transaction->apply();
    // std::cout << "apply is OK ~~" << std::endl;
*/