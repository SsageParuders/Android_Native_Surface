/*
 * Codes from SsageParuders[https://github.com/SsageParuders]
 * 代码由泓清提供
*/
#include <EGL/egl.h>
#include <GLES/gl.h>


struct MDisplayInfo {
    uint32_t width{0};
    uint32_t height{0};
    uint32_t orientation{0};
};

NativeWindowType
createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height, bool author);

NativeWindowType createNativeWindow(const char *surface_name, uint32_t screen_width, uint32_t screen_height,
                                    uint32_t format, uint32_t flags, bool author);
void* getDisplayToken();
void setSurfaceWH(uint32_t width, uint32_t height);
MDisplayInfo getDisplayInfo();
void initRecord(const char* bitRate,float fps,
uint32_t videoWidth, uint32_t videoHeight);
void stopRecord();
NativeWindowType getRecordNativeWindow();
void runRecord(bool *runFlag,void callback(uint8_t*,size_t));