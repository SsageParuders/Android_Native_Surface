/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <inttypes.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>

#include <termios.h>
#include <unistd.h>

#define LOG_TAG "ScreenRecord"
#define ATRACE_TAG ATRACE_TAG_GRAPHICS
//#define LOG_NDEBUG 0
#include <utils/Log.h>

#include <binder/IPCThreadState.h>
#include <utils/Errors.h>
#include <utils/SystemClock.h>
#include <utils/Timers.h>
#include <utils/Trace.h>

#include <gui/ISurfaceComposer.h>
#include <gui/Surface.h>
#include <gui/SurfaceComposerClient.h>
#include <gui/ISurfaceComposer.h>
#include <media/MediaCodecBuffer.h>
#include <media/NdkMediaCodec.h>
#include <media/NdkMediaFormatPriv.h>
#include <media/openmax/OMX_IVCommon.h>
#include <media/stagefright/MediaCodec.h>
#include <media/stagefright/MediaCodecConstants.h>
#include <media/stagefright/MediaErrors.h>
#include <media/stagefright/PersistentSurface.h>
#include <media/stagefright/foundation/ABuffer.h>
#include <media/stagefright/foundation/AMessage.h>
#include <mediadrm/ICrypto.h>
#include <ui/DisplayInfo.h>
#include <thread>
#include "aosp_record.h"

using android::ABuffer;
using android::ALooper;
using android::AMessage;
using android::AString;
using android::IBinder;
using android::DisplayInfo;
using android::IGraphicBufferProducer;
using android::ISurfaceComposer;
using android::MediaCodec;
using android::MediaCodecBuffer;
using android::PersistentSurface;
using android::PhysicalDisplayId;
using android::ProcessState;
using android::Rect;
using android::String8;
using android::SurfaceComposerClient;
using android::Vector;
using android::Surface;
using android::sp;
using android::status_t;

using android::DISPLAY_ORIENTATION_0;
using android::DISPLAY_ORIENTATION_180;
using android::DISPLAY_ORIENTATION_90;
using android::INVALID_OPERATION;
using android::INVALID_OPERATION;
using android::NAME_NOT_FOUND;
using android::NO_ERROR;
using android::UNKNOWN_ERROR;

namespace ui = android::ui;

using namespace std;


static const uint32_t kMinBitRate = 100000;         // 0.1Mbps
static const uint32_t kMaxBitRate = 200 * 1000000;  // 200Mbps
static const uint32_t kFallbackWidth = 1280;        // 720p
static const uint32_t kFallbackHeight = 720;

static const char *kMimeTypeAvc = "video/avc";

// Command-line parameters.
static bool gVerbose = false;           // chatty on stdout
static bool gRotate = false;            // rotate 90 degrees
static bool gMonotonicTime = false;     // use system monotonic time for timestamps
static bool gPersistentSurface = false; // use persistent surface

static AString gCodecName = "";         // codec name override
static bool gSizeSpecified = false;     // was size explicitly requested?
static bool gWantInfoScreen = false;    // do we want initial info screen?
static bool gWantFrameTime = false;     // do we want times on each frame?
static uint32_t gVideoWidth = 0;        // default width+height
static uint32_t gVideoHeight = 0;
static uint32_t gBitRate = 20000000;     // 20Mbps
static float gfps = 60;                  // fps
static uint32_t gBframes = 0;
static PhysicalDisplayId gPhysicalDisplayId;
// Set by signal handler to stop recording.
// static volatile bool gStopRequested = false;

// Previous signal handler state, restored after first hit.
static struct sigaction gOrigSigactionINT;
static struct sigaction gOrigSigactionHUP;
bool gStopRequested = false;

sp <MediaCodec> encoder;
sp <IBinder> display;
sp <IBinder> virtualDpy;
uint8_t orientation;
sp <IGraphicBufferProducer> bufferProducer;
sp <ANativeWindow> window;

/*
 * Catch keyboard interrupt signals.  On receipt, the "stop requested"
 * flag is raised, and the original handler is restored (so that, if
 * we get stuck finishing, a second Ctrl-C will kill the process).
 */
static void signalCatcher(int signum) {
    gStopRequested = true;
    switch (signum) {
        case SIGINT:
        case SIGHUP:
            sigaction(SIGINT, &gOrigSigactionINT, NULL);
            sigaction(SIGHUP, &gOrigSigactionHUP, NULL);
            break;
        default:
            abort();
            break;
    }
}

/*
 * Configures signal handlers.  The previous handlers are saved.
 *
 * If the command is run from an interactive adb shell, we get SIGINT
 * when Ctrl-C is hit.  If we're run from the host, the local adb process
 * gets the signal, and we get a SIGHUP when the terminal disconnects.
 */
static status_t configureSignals() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = signalCatcher;
    if (sigaction(SIGINT, &act, &gOrigSigactionINT) != 0) {
        status_t err = -errno;
        fprintf(stderr, "Unable to configure SIGINT handler: %s\n",
                strerror(errno));
        return err;
    }
    if (sigaction(SIGHUP, &act, &gOrigSigactionHUP) != 0) {
        status_t err = -errno;
        fprintf(stderr, "Unable to configure SIGHUP handler: %s\n",
                strerror(errno));
        return err;
    }
    signal(SIGPIPE, SIG_IGN);
    return NO_ERROR;
}

/*
 * Configures and starts the MediaCodec encoder.  Obtains an input surface
 * from the codec.
 */
status_t prepareEncoder(float displayFps, sp <MediaCodec> *pCodec,
                        sp <IGraphicBufferProducer> *pBufferProducer) {
    status_t err;

    if (gVerbose) {
        printf("Configuring recorder for %dx%d %s at %.2fMbps\n",
               gVideoWidth, gVideoHeight, kMimeTypeAvc, gBitRate / 1000000.0);
        fflush(stdout);
    }

    sp <AMessage> format = new AMessage;
    // 录制宽高
    format->setInt32(KEY_WIDTH, gVideoWidth);
    format->setInt32(KEY_HEIGHT, gVideoHeight);
    format->setString(KEY_MIME, kMimeTypeAvc);
    format->setInt32(KEY_COLOR_FORMAT, OMX_COLOR_FormatAndroidOpaque);
    // 录制码率
    format->setInt32(KEY_BIT_RATE, gBitRate);
    // 录制帧率
    format->setFloat(KEY_FRAME_RATE, displayFps);

    // 关键帧间隔时间，单位秒
    format->setInt32(KEY_I_FRAME_INTERVAL, 10);
    format->setInt32(KEY_MAX_B_FRAMES, gBframes);
    if (gBframes > 0) {
        format->setInt32(KEY_PROFILE, AVCProfileMain);
        format->setInt32(KEY_LEVEL, AVCLevel41);
    }

    sp <android::ALooper> looper = new android::ALooper;
    looper->setName("screenrecord_looper");
    looper->start();
    ALOGV("Creating codec");
    sp <MediaCodec> codec;
    if (gCodecName.empty()) {
        codec = MediaCodec::CreateByType(looper, kMimeTypeAvc, true);
        if (codec == NULL) {
            fprintf(stderr, "ERROR: unable to create %s codec instance\n",
                    kMimeTypeAvc);
            return UNKNOWN_ERROR;
        }
    } else {
        codec = MediaCodec::CreateByComponentName(looper, gCodecName);
        if (codec == NULL) {
            fprintf(stderr, "ERROR: unable to create %s codec instance\n",
                    gCodecName.c_str());
            return UNKNOWN_ERROR;
        }
    }

    err = codec->configure(format, NULL, NULL,
                           MediaCodec::CONFIGURE_FLAG_ENCODE);
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to configure %s codec at %dx%d (err=%d)\n",
                kMimeTypeAvc, gVideoWidth, gVideoHeight, err);
        codec->release();
        return err;
    }

    ALOGV("Creating encoder input surface");
    sp <IGraphicBufferProducer> bufferProducer;
    if (gPersistentSurface) {
        sp <PersistentSurface> surface = MediaCodec::CreatePersistentInputSurface();
        bufferProducer = surface->getBufferProducer();
        err = codec->setInputSurface(surface);
    } else {
        err = codec->createInputSurface(&bufferProducer);
    }
    if (err != NO_ERROR) {
        fprintf(stderr,
                "ERROR: unable to %s encoder input surface (err=%d)\n",
                gPersistentSurface ? "set" : "create",
                err);
        codec->release();
        return err;
    }
    // Surface *surface = new Surface(bufferProducer);

    ALOGV("Starting codec");
    err = codec->start();
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to start codec (err=%d)\n", err);
        codec->release();
        return err;
    }

    ALOGV("Codec prepared");
    *pCodec = codec;
    *pBufferProducer = bufferProducer;
    return 0;
}

/*
 * Sets the display projection, based on the display dimensions, video size,
 * and device orientation.
 */
static status_t setDisplayProjection(
        SurfaceComposerClient::Transaction& t,
        const sp<IBinder>& dpy,
        const DisplayInfo& mainDpyInfo) {

    // Set the region of the layer stack we're interested in, which in our
    // case is "all of it".
    Rect layerStackRect(mainDpyInfo.viewportW, mainDpyInfo.viewportH);

    // We need to preserve the aspect ratio of the display.
    float displayAspect = (float) mainDpyInfo.viewportH / (float) mainDpyInfo.viewportW;


    // Set the way we map the output onto the display surface (which will
    // be e.g. 1280x720 for a 720p video).  The rect is interpreted
    // post-rotation, so if the display is rotated 90 degrees we need to
    // "pre-rotate" it by flipping width/height, so that the orientation
    // adjustment changes it back.
    //
    // We might want to encode a portrait display as landscape to use more
    // of the screen real estate.  (If players respect a 90-degree rotation
    // hint, we can essentially get a 720x1280 video instead of 1280x720.)
    // In that case, we swap the configured video width/height and then
    // supply a rotation value to the display projection.
    uint32_t videoWidth, videoHeight;
    uint32_t outWidth, outHeight;
    if (!gRotate) {
        videoWidth = gVideoWidth;
        videoHeight = gVideoHeight;
    } else {
        videoWidth = gVideoHeight;
        videoHeight = gVideoWidth;
    }
    if (videoHeight > (uint32_t)(videoWidth * displayAspect)) {
        // limited by narrow width; reduce height
        outWidth = videoWidth;
        outHeight = (uint32_t)(videoWidth * displayAspect);
    } else {
        // limited by short height; restrict width
        outHeight = videoHeight;
        outWidth = (uint32_t)(videoHeight / displayAspect);
    }
    uint32_t offX, offY;
    offX = (videoWidth - outWidth) / 2;
    offY = (videoHeight - outHeight) / 2;
    Rect displayRect(offX, offY, offX + outWidth, offY + outHeight);

    if (gVerbose) {
        if (gRotate) {
            printf("Rotated content area is %ux%u at offset x=%d y=%d\n",
                    outHeight, outWidth, offY, offX);
            fflush(stdout);
        } else {
            printf("Content area is %ux%u at offset x=%d y=%d\n",
                    outWidth, outHeight, offX, offY);
            fflush(stdout);
        }
    }

    t.setDisplayProjection(dpy,
            gRotate ? DISPLAY_ORIENTATION_90 : DISPLAY_ORIENTATION_0,
            layerStackRect, displayRect);

    return NO_ERROR;
}
#include <GLES/egl.h>
#include <GLES/gl.h>

// 测试画图
void test(sp <ANativeWindow> &native_window) {

    EGLint windowConfigAttribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_RECORDABLE_ANDROID, 1,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            // no alpha
            EGL_NONE
    };
    EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    // status_t err = eglSetupContext(false);
    // if (err != NO_ERROR) {
    //     return;
    // }

    EGLDisplay display;
    EGLConfig config;
    EGLContext context;
    EGLSurface surface;
    // NativeWindowType native_window;
    EGLint num_config = 0;
    /* get an EGL display connection */
    display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    /* initialize the EGL display connection */
    eglInitialize(display, NULL, NULL);
    /* get an appropriate EGL frame buffer configuration */
    eglChooseConfig(display, windowConfigAttribs, &config, 1, &num_config);
    /* create an EGL rendering context */

    context = eglCreateContext(display, config, EGL_NO_CONTEXT, contextAttribs);
    /* create a native window */
    // native_window = createNativeWindow();
    /* create an EGL window surface */
    surface = eglCreateWindowSurface(display, config, native_window.get(), NULL);
    if (surface == EGL_NO_SURFACE) {
        printf("eglCreateWindowSurface  error = %u\n", glGetError());
    }
    /* connect the context to the surface */
    eglMakeCurrent(display, surface, surface, context);
    /* clear the color buffer */
    glClearColor(1.0, 1.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    glFlush();


    eglSwapBuffers(display, surface);
    // sleep(10);

}

/*
 * Configures the virtual display.  When this completes, virtual display
 * frames will start arriving from the buffer producer.
 */

static status_t prepareVirtualDisplay(const DisplayInfo& mainDpyInfo,
        const sp<IGraphicBufferProducer>& bufferProducer,
        sp<IBinder>* pDisplayHandle, sp <ANativeWindow> *window) {
    *window = new Surface(bufferProducer);
    // test(*window);

    sp<IBinder> dpy = SurfaceComposerClient::createDisplay(
            String8("ScreenRecorder"), false /*secure*/);

    SurfaceComposerClient::Transaction t;
    t.setDisplaySurface(dpy, bufferProducer);
    setDisplayProjection(t, dpy, mainDpyInfo);
    t.setDisplayLayerStack(dpy, 0);    // default stack
    t.apply();

    *pDisplayHandle = dpy;

    return NO_ERROR;
}

/*
 * Writes an unsigned integer byte-by-byte in little endian order regardless
 * of the platform endianness.
 */
template<typename UINT>
static void writeValueLE(UINT value, uint8_t *buffer) {
    for (int i = 0; i < sizeof(UINT); ++i) {
        buffer[i] = static_cast<uint8_t>(value);
        value >>= 8;
    }
}

uint64_t getCurrentTime() {
    timeval tv{};
    gettimeofday(&tv, nullptr);
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}


void fpsDelay(uint64_t time, int fps) {
    if ((1000 / fps) > time) {
        int fp = (1000 / fps) - time;
        // printf("sleep: %d\n",fp);
        std::this_thread::sleep_for(std::chrono::milliseconds(fp));
    }
}


status_t runEncoder(bool *runFlag, void callback(uint8_t *, size_t)) {
    static int kTimeout = 250000;   // be responsive on signal 响应信号
    status_t err;
    uint32_t debugNumFrames = 0;
    int64_t startWhenNsec = systemTime(CLOCK_MONOTONIC);
    Vector <int64_t> timestamps;
    bool firstFrame = true;

    Vector <sp<MediaCodecBuffer>> buffers;
    err = encoder->getOutputBuffers(&buffers);
    if (err != NO_ERROR) {
        fprintf(stderr, "Unable to get output buffers (err=%d)\n", err);
        return err;
    }
    // int64_t  start = 0;
    // int64_t end = 0;
    // Run until we're signaled.
    while ((*runFlag) && !gStopRequested) {
        size_t bufIndex, offset, size;
        int64_t ptsUsec;
        uint32_t flags;

        if (firstFrame) {
            ATRACE_NAME("first_frame");
            firstFrame = false;
        }
        ALOGV("Calling dequeueOutputBuffer");
        err = encoder->dequeueOutputBuffer(&bufIndex, &offset, &size, &ptsUsec,
                                           &flags, kTimeout);
        ALOGV("dequeueOutputBuffer returned %d", err);
        switch (err) {
            case NO_ERROR:
                if (size != 0) {
                    ALOGV("Got data in buffer %zu, size=%zu, pts=%" PRId64,
                          bufIndex, size, ptsUsec);
                 
                    { // scope
                        ATRACE_NAME("orientation");
                        // Check orientation, update if it has changed.
                        //
                        // Polling for changes is inefficient and wrong, but the
                        // useful stuff is hard to get at without a Dalvik VM.
                        DisplayInfo mainDpyInfo;
                        err = SurfaceComposerClient::getDisplayInfo(display,
                                &mainDpyInfo);
                        if (err != NO_ERROR) {
                            ALOGW("getDisplayInfo(main) failed: %d", err);
                        } else if (orientation != mainDpyInfo.orientation) {
                            ALOGD("orientation changed, now %d", mainDpyInfo.orientation);
                            SurfaceComposerClient::Transaction t;
                            setDisplayProjection(t, virtualDpy, mainDpyInfo);
                            t.apply();
                            orientation = mainDpyInfo.orientation;
                        }
                    }
                    // 如果虚拟显示器没有为我们提供时间戳，那就设置为当前系统时间戳来保证时间戳不为空
                    if (ptsUsec == 0) {
                        ptsUsec = systemTime(SYSTEM_TIME_MONOTONIC) / 1000;
                    }
                    if (callback) {
                        callback(buffers[bufIndex]->data(), size);
                    }
                    debugNumFrames++;
                }
                err = encoder->releaseOutputBuffer(bufIndex);
                if (err != NO_ERROR) {
                    fprintf(stderr, "Unable to release output buffer (err=%d)\n",
                            err);
                    return err;
                }
                if ((flags & MediaCodec::BUFFER_FLAG_EOS) != 0) {
                    // Not expecting EOS from SurfaceFlinger.  Go with it.
                    ALOGI("Received end-of-stream");
                    gStopRequested = true;
                    *runFlag = false;
                }
                break;
            case -EAGAIN:                       // INFO_TRY_AGAIN_LATER
                ALOGV("Got -EAGAIN, looping");
                break;
            case android::INFO_FORMAT_CHANGED:    // INFO_OUTPUT_FORMAT_CHANGED
            {
                // Format includes CSD, which we must provide to muxer.
                // ALOGV("Encoder format changed");
                // sp <AMessage> newFormat;
                // encoder->getOutputFormat(&newFormat);
            }
                break;
            case android::INFO_OUTPUT_BUFFERS_CHANGED:   // INFO_OUTPUT_BUFFERS_CHANGED
                // Not expected for an encoder; handle it anyway.
                ALOGV("Encoder buffers changed");
                err = encoder->getOutputBuffers(&buffers);
                if (err != NO_ERROR) {
                    fprintf(stderr,
                            "Unable to get new output buffers (err=%d)\n", err);
                    return err;
                }
                break;
            case INVALID_OPERATION:
                ALOGW("dequeueOutputBuffer returned INVALID_OPERATION");
                return err;
            default:
                fprintf(stderr,
                        "Got weird result %d from dequeueOutputBuffer\n", err);
                return err;
        }
    }
    ALOGV("Encoder stopping (req=%d)", gStopRequested);
    if (gVerbose) {
        printf("Encoder stopping; recorded %u frames in %" PRId64 " seconds\n",
               debugNumFrames, nanoseconds_to_seconds(
                        systemTime(CLOCK_MONOTONIC) - startWhenNsec));
        fflush(stdout);
    }
    return NO_ERROR;
}


static inline uint32_t floorToEven(uint32_t num) {
    return num & ~1;
}

static status_t recordScreen() {
    status_t err;

    // 配置信号处理
    err = configureSignals();
    if (err != NO_ERROR) return err;

    // 启动线程池,MediaCodec需要接收来自mediaserver的消息。
    sp <ProcessState> self = ProcessState::self();
    self->startThreadPool();

    // 获取主屏幕参数
    display = SurfaceComposerClient::getPhysicalDisplayToken(
            gPhysicalDisplayId);
    if (display == nullptr) {
        fprintf(stderr, "ERROR: no display\n");
        return NAME_NOT_FOUND;
    }
    
    DisplayInfo mainDpyInfo;
    err = SurfaceComposerClient::getDisplayInfo(display, &mainDpyInfo);
    if (err != NO_ERROR) {
        fprintf(stderr, "ERROR: unable to get display characteristics\n");
        return err;
    }

    if (gVerbose) {
        printf("Main display is %dx%d @%.2ffps (orientation=%u)\n",
                mainDpyInfo.viewportW, mainDpyInfo.viewportH, mainDpyInfo.fps,
                mainDpyInfo.orientation);
        fflush(stdout);
    }

    // 编码器无法将奇数作为配置 
    if (gVideoWidth == 0) {
        gVideoWidth = floorToEven(mainDpyInfo.viewportW);
    }
    if (gVideoHeight == 0) {
        gVideoHeight = floorToEven(mainDpyInfo.viewportH);
    }


    // 配置并启动编码器。
    err = prepareEncoder(gfps, &encoder, &bufferProducer);
    if (err != NO_ERROR && !gSizeSpecified) {
        // fallback is defined for landscape; swap if we're in portrait
        bool needSwap = gVideoWidth < gVideoHeight;
        uint32_t newWidth = needSwap ? kFallbackHeight : kFallbackWidth;
        uint32_t newHeight = needSwap ? kFallbackWidth : kFallbackHeight;
        if (gVideoWidth != newWidth && gVideoHeight != newHeight) {
            ALOGV("Retrying with 720p");
            fprintf(stderr, "WARNING: failed at %dx%d, retrying at %dx%d\n",
                    gVideoWidth, gVideoHeight, newWidth, newHeight);
            gVideoWidth = newWidth;
            gVideoHeight = newHeight;
            err = prepareEncoder(gfps, &encoder, &bufferProducer);
        }
    }

    if (err != NO_ERROR) return err;
    err = prepareVirtualDisplay(mainDpyInfo, bufferProducer, &virtualDpy, &window);
    if (err != NO_ERROR) {
        if (encoder != NULL) encoder->release();
        return err;
    }
    orientation = mainDpyInfo.orientation;
    printf("runEncoder\n");
    return err;
}

/*
 * Accepts a string with a bare number ("4000000") or with a single-character
 * unit ("4m").
 *
 * Returns an error if parsing fails.
 */
static status_t parseValueWithUnit(const char *str, uint32_t *pValue) {
    long value;
    char *endptr;

    value = strtol(str, &endptr, 10);
    if (*endptr == '\0') {
        // bare number
        *pValue = value;
        return NO_ERROR;
    } else if (toupper(*endptr) == 'M' && *(endptr + 1) == '\0') {
        *pValue = value * 1000000;  // check for overflow?
        return NO_ERROR;
    } else {
        fprintf(stderr, "Unrecognized value: %s\n", str);
        return UNKNOWN_ERROR;
    }
}

void stopScreenrecord() {
    // Shut everything down, starting with the producer side.
    // 结束
    SurfaceComposerClient::destroyDisplay(virtualDpy);
    if (encoder != NULL) encoder->stop();
    if (encoder != NULL) encoder->release();
}

ANativeWindow *getRecordWindow() {
    return window.get();
}

/*
 * Parses args and kicks things off.
 */
int initScreenrecord(const char *bitRate, float fps, uint32_t videoWidth, uint32_t videoHeight) {
    std::cout << "screenrecord" << std::endl;
    std::optional<PhysicalDisplayId> displayId = SurfaceComposerClient::getInternalDisplayId();
    if (!displayId) {
        fprintf(stderr, "Failed to get token for internal display\n");
        return 1;
    }

    gPhysicalDisplayId = *displayId;

    gVerbose = false;
    gfps = fps;
    // 视频大小
    gVideoWidth = videoWidth;
    gVideoHeight = videoHeight;
    gSizeSpecified = false;

    // 码率
    if (parseValueWithUnit(bitRate, &gBitRate) != NO_ERROR) {
        return 2;
    }

    // 日志显示
    gWantInfoScreen = false;
    gWantFrameTime = false;
    // 视频显示基本信息
    gWantInfoScreen = false;
    // 旋转
    gRotate = false;

    gMonotonicTime = false;

    if (gBitRate < kMinBitRate || gBitRate > kMaxBitRate) {
        fprintf(stderr,
                "Bit rate %dbps outside acceptable range [%d,%d]\n",
                gBitRate, kMinBitRate, kMaxBitRate);
        return 2;
    }
    status_t err = recordScreen();
    ALOGD(err == NO_ERROR ? "success" : "failed");
    return (int) err;
}
