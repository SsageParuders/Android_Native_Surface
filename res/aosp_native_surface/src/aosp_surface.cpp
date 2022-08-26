/*
 * Codes from SsageParuders[https://github.com/SsageParuders]
 * 代码由泓清提供
*/
#include "aosp_surface.h"

#include <iostream>
#include <thread>
#include <chrono>

#include <utils/StrongPointer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>

android::sp<android::SurfaceComposerClient> gSurfaceComposerClient;
android::sp<android::SurfaceControl>        gSurfaceControl;

/*
 * Next codes mostly withaout changes from Khronos intro
 */

/* Already defined in frameworks/native/opengl/include/EGL/eglplatform.h like
 * typedef struct ANativeWindow*           EGLNativeWindowType;
 * ...
 * typedef EGLNativeWindowType  NativeWindowType;
 */
// typedef ... NativeWindowType;

NativeWindowType createNativeWindow(const char *surface_name ,uint32_t screen_width ,uint32_t screen_height)
{
    NativeWindowType ret = nullptr;

    gSurfaceComposerClient = android::sp<android::SurfaceComposerClient> {new android::SurfaceComposerClient()};

    gSurfaceControl = gSurfaceComposerClient->createSurface(android::String8(surface_name),
                                                            screen_width,
                                                            screen_height,
                                                            0); // set format to 0 because it is specified by attribute_list like RGBA-format



    if(!gSurfaceControl)
        std::cout << "!gSurfaceControl" << std::endl;
    else if(!gSurfaceControl->isValid())
        std::cout << "!gSurfaceControl->isValid()" << std::endl;
    else
    {
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