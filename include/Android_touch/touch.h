//
// Created by Administrator on 2022/2/21.
//

#ifndef KKONDK_TOUCH_H
#define KKONDK_TOUCH_H

#include "touchInput.h"

class touch {
    int         read_fb;
    int         write_fb;
    float       *sensitivity;
    bool        mainio;
    bool        *touchIsChange;
    Screen      *screen;
    Screen      FullScreen;
    vec2        TouchProportion;
    char sdDir[1024];
public:

    static int getTouchEventNum(bool a,string dir) 
    {
        char          name[64];           /* RATS: Use ok, but could be better */
        char          buf[256] = {0,};  /* RATS: Use ok */
        unsigned char mask[EV_MAX / 8 + 1];
        int           fd       = 0;
        int           i, j;
        int           version;
        int           va       = 0;
        char msg[4096];
        bool resio = false;
        string msgdata;
        msgdata.clear();
        for (i = 0; i < 32; i++) {
            sprintf(name, "/dev/input/event%d", i);
            if ((fd = open(name, O_RDONLY, 0)) >= 0) {
                ioctl(fd, EVIOCGVERSION, &version);
                ioctl(fd, EVIOCGNAME(sizeof(buf)), buf);
                ioctl(fd, EVIOCGBIT(0, sizeof(mask)), mask);
                //printf("%s\n", name);
                //printf("    evdev version: %d.%d.%d\n", version >> 16, (version >> 8) & 0xff, version & 0xff);
                //printf("    name: %s\n", buf);
                //printf("    features:");
                memset(msg,0,4096);

                sprintf(msg,"%s\n    evdev version: %d.%d.%d\n    name: %s\n    features:",name, version >> 16,
                        (version >> 8) & 0xff, version & 0xff, buf);
                va     = 0;
                msgdata += msg;
                for (j = 0; j < EV_MAX; j++) {
                    if (test_bit(j)) {
                        const char *type = "unknown";
                        switch (j) {
                            case EV_KEY: type = "keys/buttons";
                                ++va;
                                break;
                            case EV_REL: type = "relative";
                                break;
                            case EV_ABS: type = "ABSolute";
                                ++va;
                                break;
                            case EV_MSC: type = "reserved";
                                break;
                            case EV_LED: type = "leds";
                                break;
                            case EV_SND: type = "sound";
                                break;
                            case EV_REP: type = "repeat";
                                break;
                            case EV_FF: type = "feedback";
                                break;
                            case EV_SYN: type = "EV_SYN";
                                ++va;
                                break;
                            case EV_SW: type = "EV_SW";
                                break;
                            case EV_PWR: type = "EV_PWR";
                                break;
                            case EV_FF_STATUS: type = "EV_FF_STATUS";
                                break;
                        }
                        msgdata += type;
                        msgdata += "|";
                        //printf(" %s", type);
                    }
                }
                resio = print_possible_events(fd,msgdata);
                ::close(fd);
                //printf("\n");
                msgdata += "\n";
            }
            if (a){
                if (va >= 3) {
                    return i;
                }
            }else{
                if (resio){return i;}
            }
        }
        //printf("%s",msgdata.c_str());
        string msgDir =  dir;
        msgDir += "/eventsMsg";
        FILE *msgFd = fopen(msgDir.c_str(),"wb+");
        fwrite(msgdata.c_str(),msgdata.size(),1,msgFd);
        fflush(msgFd);
        fsync(fileno(msgFd));
        fclose(msgFd);
        return -1;
    }

    static int print_possible_events(int fd, struct events *outdata) {
        int        ress      = 0;
        uint8_t    *bits     = NULL;
        ssize_t    bits_size = 0;
        const char *label;
        int        i, j, k;
        int        res, res2;
        memset(outdata, 0, sizeof(events) * EV_MAX + 1);
        for (i = 0; i <= EV_MAX; i++) {
            int count = 0;
            while (1) {
                res = ioctl(fd, EVIOCGBIT(i, bits_size), bits);
                if (res < bits_size) {
                    break;
                }
                bits_size = res + 16;
                bits      = (uint8_t *) realloc(bits, bits_size * 2);
                if (bits == NULL) {
                    fprintf(stderr, "failed to allocate buffer of size %ld\n", bits_size);
                    return ress;
                }
            }
            res2      = 0;
            switch (i) {
                case EV_SYN: label = "SYN";
                    break;
                case EV_KEY: res2 = ioctl(fd, EVIOCGKEY(res), bits + bits_size);
                    label         = "KEY";
                    break;
                case EV_REL: label = "REL";
                    break;
                case EV_ABS: label = "ABS";
                    break;
                case EV_MSC: label = "MSC";
                    break;
                case EV_LED: res2 = ioctl(fd, EVIOCGLED(res), bits + bits_size);
                    label         = "LED";
                    break;
                case EV_SND: res2 = ioctl(fd, EVIOCGSND(res), bits + bits_size);
                    label         = "SND";
                    break;
                case EV_SW: res2 = ioctl(fd, EVIOCGSW(bits_size), bits + bits_size);
                    label        = "SW ";
                    break;
                case EV_REP: label = "REP";
                    break;
                case EV_FF: label = "FF ";
                    break;
                case EV_PWR: label = "PWR";
                    break;
                default: res2 = 0;
                    label     = "???";
            }
            for (j    = 0; j < res; j++) {
                for (k = 0; k < 8; k++)
                    if (bits[j] & 1 << k) {
                        char down;
                        if (j < res2 && (bits[j + bits_size] & 1 << k)) {
                            down = '*';
                        } else {
                            down = ' ';
                        }
                        if (count == 0) {
                            outdata[i].ID                     = i;
                            outdata[i].io                     = true;
                            outdata[i].eventmsg[j * 8 + k].ID = j * 8 + k;
                            outdata[i].eventmsg[j * 8 + k].io = true;
                            //printf("    %s (%04x):", label, i);
                        } else if ((count & 0x7) == 0 || i == EV_ABS) {
                            outdata[i].ID = i;
                        }
                        outdata[i].io                     = true;
                        outdata[i].eventmsg[j * 8 + k].ID = j * 8 + k;
                        outdata[i].eventmsg[j * 8 + k].io = true;
                        //printf(" %04x%c", j * 8 + k, down);
                        if (i == EV_ABS) {
                            outdata[i].ID                     = i;
                            outdata[i].io                     = true;
                            outdata[i].eventmsg[j * 8 + k].ID = j * 8 + k;
                            outdata[i].eventmsg[j * 8 + k].io = true;
                            struct input_absinfo abs;
                            if (ioctl(fd, EVIOCGABS(j * 8 + k), &abs) == 0) {
                                ++ress;
                                outdata[i].infoio                   = true;
                                outdata[i].eventmsg[j * 8 + k].data = abs;
                               // printf(" value %d, min %d, max %d, fuzz %d flat %d", abs.value, abs.minimum, abs.maximum, abs.fuzz, abs.flat);
                            }
                        }
                        count++;
                    }
            }
            //if (count)
                //printf("\n");
        }
        free(bits);
        return ress;
    }

    static bool print_possible_events(int fd,string &msgOut) {
        int        ress      = 0;
        int  pos = 0;
        uint8_t    *bits     = NULL;
        ssize_t    bits_size = 0;
        const char *label;
        int        i, j, k;
        int        res, res2;
        char buf[512];
        msgOut += "  events:\n";
        for (i = 0; i <= EV_MAX; i++) {
            int count = 0;
            while (1) {
                res = ioctl(fd, EVIOCGBIT(i, bits_size), bits);
                if (res < bits_size) {
                    break;
                }
                bits_size = res + 16;
                bits      = (uint8_t *) realloc(bits, bits_size * 2);
                if (bits == NULL) {
                    fprintf(stderr, "failed to allocate buffer of size %ld\n", bits_size);


                    return ress;
                }
            }
            res2      = 0;
            switch (i) {
                case EV_SYN: label = "SYN";
                    break;
                case EV_KEY: res2 = ioctl(fd, EVIOCGKEY(res), bits + bits_size);
                    label         = "KEY";
                    break;
                case EV_REL: label = "REL";
                    break;
                case EV_ABS: label = "ABS";
                    break;
                case EV_MSC: label = "MSC";
                    break;
                case EV_LED: res2 = ioctl(fd, EVIOCGLED(res), bits + bits_size);
                    label         = "LED";
                    break;
                case EV_SND: res2 = ioctl(fd, EVIOCGSND(res), bits + bits_size);
                    label         = "SND";
                    break;
                case EV_SW: res2 = ioctl(fd, EVIOCGSW(bits_size), bits + bits_size);
                    label        = "SW ";
                    break;
                case EV_REP: label = "REP";
                    break;
                case EV_FF: label = "FF ";
                    break;
                case EV_PWR: label = "PWR";
                    break;
                default: res2 = 0;
                    label     = "???";
            }
            for (j    = 0; j < res; j++) {
                for (k = 0; k < 8; k++)
                    if (bits[j] & 1 << k) {
                        char down;
                        if (j < res2 && (bits[j + bits_size] & 1 << k)) {
                            down = '*';
                        } else {
                            down = ' ';
                        }
                        if (count == 0) {
                            //printf("    %s (%04x):", label, i);
                            sprintf(buf,"    %s (%04x):", label, i);
                            msgOut += buf;
                        } else if ((count & 0x7) == 0 || i == EV_ABS) {

                        }
                        sprintf(buf," %04x%c", j * 8 + k, down);
                        msgOut += buf;
                        //printf(" %04x%c", j * 8 + k, down);
                        if (strstr(label,"KEY")){
                            if (j * 8 + k == 0x14a){
                                ++pos;
                            }
                        }
                        if (strstr(label,"ABS")){
                            if (j * 8 + k == 0x2f||
                                    j * 8 + k == 0x35||
                                    j * 8 + k == 0x36||
                                    j * 8 + k == 0x39){
                                ++pos;
                            }
                        }
                        if (i == EV_ABS) {
                            struct input_absinfo abs;
                            if (ioctl(fd, EVIOCGABS(j * 8 + k), &abs) == 0) {
                                ++ress;
                                sprintf(buf," value %d, min %d, max %d, fuzz %d flat %d", abs.value, abs.minimum, abs.maximum, abs.fuzz, abs.flat);
                                msgOut += buf;
                               // printf(" value %d, min %d, max %d, fuzz %d flat %d", abs.value, abs.minimum, abs.maximum, abs.fuzz, abs.flat);
                            }
                        }
                        count++;
                    }
            }
            if (count){
               // printf("\n");
                msgOut += "\n";
            }
        }
        free(bits);
        if (pos>=5){
            return true;
        }else{
            return false;
        }
    }

    int initInput() {
        int num = getTouchEventNum(false,sdDir);
        if (num == -1) {
            printf("No Touch Event\n");
            return -1;
        }
        char name[64];
        sprintf(name, "/dev/input/event%d", num);
        read_fb = open(name, O_RDWR | O_SYNC | O_NONBLOCK);
        if (read_fb < 0) {
            printf("Open dev Error");
            return read_fb;
        }

        return read_fb;
    }

    void getTouchProportion() {
        FullScreen.ScreenX = screen->ScreenX * 2;
        FullScreen.ScreenY = screen->ScreenY * 2;
        TouchProportion.x  = (screen->ScreenX -1) / FullScreen.ScreenY;
        TouchProportion.y  = (screen->ScreenY -1) / FullScreen.ScreenX;
        //printf("FullScreen.x=%f,FullScreen.y %f\n", FullScreen.ScreenX, FullScreen.ScreenY);
        //printf("TouchProportion.x=%f,TouchProportion.y %f\n", TouchProportion.x, TouchProportion.y);
    }

    bool touch_io = false;
    void gettouch(Screen *screen, int *Orientation) {
        touchInput input(screen,&touch_io);
        input.setOrientation(Orientation);
        input.setScreen(screen);
        this->screen = screen;
        getTouchProportion();
        input.setTouchProportion(&TouchProportion);
        if (initInput() < 0) {
            printf("touch failed to open ");
            return;
        } else {
            printf("touch open Event ");
        }
        input.setReadfd(&read_fb);
        //int result = ioctl(read_fb, EVIOCGRAB, 1);//allow imgui input only
        for (;;) {
            input.getRightTouch();
            input.readEvent();
            input.getOutEvent();
        }
    }


};


#endif //KKONDK_TOUCH_H
