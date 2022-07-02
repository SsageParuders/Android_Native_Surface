
#ifndef KKONDK_TOUCHINPUT_H
#define KKONDK_TOUCHINPUT_H

#include "unistd.h"
#include <android/input.h>
#include <android/keycodes.h>
#include <string.h>
#include <cstdio>
#include <malloc.h>
#include <cstdio>
#include <cstdlib>
#include <math.h>
#include <iostream>
#include <sys/system_properties.h>

#include <linux/input.h>
#include <linux/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "linux/uinput.h"
#include <imgui.h>
using namespace std;
#define test_bit(bit) (mask[(bit)/8] & (1 << ((bit)%8)))

struct vec2{
    vec2(float x, float y) {
        this->x = x;
        this->y = y;
    }

    vec2() {}

    float x;
    float y;
    bool operator == (const vec2 &t) const
    {
        if ( this->x == t.x && this->y == t.y ) return true;
        return false;
    }
    bool operator != (const vec2 &t) const
    {
        if ( this->x != t.x || this->y != t.y ) return true;
        return false;
    }
    vec2 & SetNewAim(const vec2 &t,int fps){
        if ( abs(this->x - t.x) < 0.1f  &&   abs(this->y - t.y) < 0.1f ){
            this->x = t.x;
            this->y = t.y;
        } else{
            this->x -= ((this->x - t.x) / fps);
            this->y -= ((this->y - t.y) / fps);
        }
        return *this;
    }
};

struct Screen {
    float ScreenX;
    float ScreenY;
};

enum TouchEvent {
    TOUCH_DOWN,
    TOUCH_UP,
    TOUCH_MOVE,
    TOUCH_CANCEL,
    TOUCH_OUTSIDE
};
struct Touch {
    int TRACKING_ID;
    int POSITION_X;
    int POSITION_Y;
    int SLOT;
    bool io;
    bool Action;
    bool run;
    bool isbot;
    bool runing;
};
struct eventinfo {
    int ID;
    bool io = false;
    struct input_absinfo data;
};
struct events {
    int ID;
    bool io = false;
    bool infoio = false;
    eventinfo eventmsg[KEY_MAX + 1];
} eventdata[EV_MAX + 1];

struct touchInput {
    Touch              *touchNow  = nullptr;
    Touch              *touchLast = nullptr;
    Touch              touch[16];
    Touch              *outTouch[16];
    Screen             *screen;
    struct input_event oneevent;
    int                *Orientation;
    int                *read_fb;
    int                un;
    int                in;
    int                SYN;
    int                pos;
    size_t             l, outsize;
    bool               *touchio   = nullptr;
    vec2               *TouchProportion;

    touchInput(Screen *screen,bool *touchio) {
        bzero(touch, sizeof(touch));
        this->screen = screen;
        if (touchNow == nullptr) {
            touchNow = &touch[0];
        }
        pos         = 0;
        for (int l  = 0; l < 16; ++l) {
            touch[l].SLOT = l;
        }
        SLOTio = false;
        this->touchio = touchio;
        outsize = 0;
    }

    void setTouchProportion(vec2 *TouchProportion_) {
        this->TouchProportion = TouchProportion_;
    }
    void setScreen(Screen *screen) {
        this->screen = screen;
    }

    void setOrientation(int *Orientation) {
        this->Orientation = Orientation;
    }

    void setReadfd(int *read_fb_) {
        this->read_fb = read_fb_;
    }

    Touch *opennew(int s) {
        for (in = 0; in < 16; ++in) {
            if (!touch[in].io) {
                break;
            }
        }
        touch[in].io     = true;
        touch[in].Action = true;
        touch[in].run    = true;
        touchLast = &touch[in];
        touchLast->TRACKING_ID = s;
        ++this->pos;
        return touchLast;
    }

    bool SLOTio = false;

    Touch *SLOT_getnow(int idi) {
        SLOTio          = true;
        if (!touch[idi].io) {
            ++this->pos;
            touch[idi].Action = true;
            touch[idi].run    = true;
        }
        touch[idi].io   = true;
        touch[idi].SLOT = idi;
        touchLast = &touch[idi];
        return touchLast;
    }

    void closenow(Touch *last) {
        if (this->pos > 0) {
            --this->pos;
        }
        last->io          = false;
        last->TRACKING_ID = -1;
    }

    Touch *TRACKING_ID_findnow(int ss) {
        bool a  = false;
        if (ss == -1) {
            closenow(touchNow);
            SLOTio = false;
            return touchNow;
        }
        if (SLOTio) {
            touchLast->TRACKING_ID = ss;
            SLOTio = false;
            return touchLast;
        }
        for (in = 0; in < 16; ++in) {
            if (touch[in].TRACKING_ID == ss) {
                a = true;
                break;
            }
        }
        if (a) {
            touchLast = &touch[in];
            SLOTio    = false;
            return touchLast;
        } else {
            SLOTio = false;
            return opennew(ss);
        }
    }

    void ABS_SetX(int x) {
        touchNow->POSITION_X = x;
        touchNow->runing     = true;
    }

    void ABS_SetY(int y) {
        touchNow->POSITION_Y = y;
        touchNow->runing     = true;
    }

    int add_event(input_event &oneevent) {
        SYN = 0;

        switch (oneevent.code) {
            case ABS_MT_POSITION_X: {
                ABS_SetX(oneevent.value);
            }
                break;
            case ABS_MT_POSITION_Y: {
                ABS_SetY(oneevent.value);
            }
                break;
            case ABS_MT_TRACKING_ID: {
                touchNow = TRACKING_ID_findnow(oneevent.value);
            }
                break;
            case ABS_MT_SLOT: {
                touchNow = SLOT_getnow(oneevent.value);
            }
                break;
            case SYN_REPORT: {
                SYN = 1;
            }
                break;
        }
        if (SYN) {
            return 1;
        }
        return 0;
    }

    bool  jk     = true;
    int   thindx[9];
    Touch tmp[9];
    int   tmpcon = 0;
    int   tmpsize;
    int   upcon;

    void getRightTouch() {
        if (jk) {
            tmpcon  = 0;
            for (l  = 0; l < outsize; ++l) {
                if (outTouch[l]->io) {
                    if (*Orientation == 270) {
                        if (!outTouch[l]->isbot && outTouch[l]->POSITION_Y / TouchProportion->y > screen->ScreenX) {
                            tmp[tmpcon]    = *outTouch[l];
                            thindx[tmpcon] = l;
                            *this->touchio = false;
                            jk = false;
                            ++tmpcon;
                        }
                    } else {
                        if (!outTouch[l]->isbot && outTouch[l]->POSITION_Y / TouchProportion->y < screen->ScreenX) {
                            tmp[tmpcon]    = *outTouch[l];
                            thindx[tmpcon] = l;
                            *this->touchio = false;
                            jk = false;
                            ++tmpcon;
                        }
                    }
                }
            }
            tmpsize = tmpcon;
        }
    }


    vec2 rotatePoint(uint32_t orientation, float x, float y, int32_t displayWidth = 0,int32_t displayHeight = 0) {
        if (orientation == 0) {
            return {x, y};
        }
        vec2 xy(x, y);
        if (orientation == 3) {
            xy.x = (float)displayHeight - y;
            xy.y = x;
        } else if (orientation == 2) {
            xy.x = (float)displayWidth - x;
            xy.y =(float) displayHeight - y;
        } else if (orientation == 1) {
            xy.x = y;
            xy.y =(float) displayWidth - x;
        }
        return xy;
    }


    void getOutEvent() {
        un     = 0;
        for (l = 0; l < 10; ++l) {
            if (touch[l].run)
            {
                outTouch[un] = &touch[l];
                vec2 point = rotatePoint(*Orientation,(float)touch[l].POSITION_X,(float)touch[l].POSITION_Y,screen->ScreenX,screen->ScreenY);
                ImGuiIO& io = ImGui::GetIO();
                if (touch[l].SLOT == 0&&touch[l].TRACKING_ID!=-1){
                    if (touch[l].Action){
                        io.MousePos.x = point.x;
                        io.MousePos.y = point.y;
                        io.MouseDown[0] = true;

                    } else{
                        io.MousePos.x = point.x;
                        io.MousePos.y = point.y;
                    }
                } else{
                    std::this_thread::sleep_for(0.01s);
                    io.MouseDown[0] = false;
                }
                ++un;
            }
        }
        outsize = un;
    }

    void readEvent() {
        for (;;) {
            if (read(*read_fb, &oneevent, sizeof(oneevent)) > 0) {
                if (this->add_event(oneevent)) {
                    break;
                }
            } else {
                break;
            }
        }
    }

};


#endif //KKONDK_TOUCHINPUT_H
