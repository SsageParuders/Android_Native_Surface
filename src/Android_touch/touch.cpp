//
// Created by 泓清 on 2022/8/26.
//
#include <touch.h>
int isa_event_device(const struct dirent* dir){
    return strncmp("event", dir->d_name, 5) == 0;
}
std::string getTouchScreenDevice(){
    struct dirent** namelist;
    int i, ndev;
    ndev = scandir("/dev/input", &namelist, isa_event_device, alphasort);
    if(ndev <= 0){
        return "";
    }
    for(i = 0; i < ndev; i++){
        char fname[64];
        int fd = -1;
        unsigned long keybit[NBITS(KEY_CNT)];
        unsigned long propbit[INPUT_PROP_MAX];
        snprintf(fname, sizeof(fname), "%s/%s", "/dev/input", namelist[i]->d_name);
        fd = open(fname, O_RDONLY);
        if(fd < 0){
            continue;
        }
        memset(keybit, 0, sizeof(keybit));
        memset(propbit, 0, sizeof(propbit));
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybit)), keybit);
        ioctl(fd, EVIOCGPROP(INPUT_PROP_MAX), propbit);
        close(fd);
        free(namelist[i]);
        if(test_bit(propbit, INPUT_PROP_DIRECT) && (test_bit(keybit, BTN_TOUCH) || test_bit(keybit, BTN_TOOL_FINGER))){
            return std::string(fname);
        } else if(test_bit(keybit, BTN_TOUCH) || test_bit(keybit, BTN_TOOL_FINGER)){
            return std::string(fname);
        }
    }
    return "";
}
Vector2 rotatePointx(uint32_t orientation, float x, float y, int32_t displayWidth,int32_t displayHeight) {
    if (orientation == 0) {
        return Vector2(x, y);
    }
    Vector2 xy(x, y);
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
Vector2 getTouchScreenDimension(int fd){
    int abs_x[6], abs_y[6] = {0};
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_X), abs_x);
    ioctl(fd, EVIOCGABS(ABS_MT_POSITION_Y), abs_y);
    return Vector2(abs_x[2], abs_y[2]);
}
void touch_config(){
    std::string device = getTouchScreenDevice();
    // printf("touch event : %s\n",device.c_str());
    if (device.length() < 2) {
        printf("No Touch Event\n");
    }
    int touch_device_fd = open(device.c_str(), O_RDWR | O_SYNC | O_NONBLOCK);
    if (touch_device_fd < 0) {
        printf("Open dev Error\n");
    }
    Vector2 touch_screen_size = getTouchScreenDimension(touch_device_fd); // + 1.0f;
    int x_now = 0 , y_now = 0;
    while(true) {
        if(g_Initialized){
            ImGuiIO& io = ImGui::GetIO();
            struct input_event oneevent;
            if (read(touch_device_fd, &oneevent, sizeof(oneevent)) > 0) {
                if(oneevent.code==ABS_MT_POSITION_X){
                    x_now = oneevent.value;
                }
                if(oneevent.code==ABS_MT_POSITION_Y){
                    y_now = oneevent.value;
                }
                if(oneevent.code==ABS_MT_TRACKING_ID) {
                    if((int)oneevent.value <= 0){
                        io.MouseDown[0] = false;
                    } else {
                        io.MouseDown[0] = true;
                    }
                }
                Vector2 point = rotatePointx(Orientation,(float)x_now,(float)y_now,touch_screen_size.x,touch_screen_size.y);
                io.MousePos = ImVec2((point.x * full_screen.ScreenX) / touch_screen_size.x,(point.y * full_screen.ScreenY) / touch_screen_size.y);
            }
            // printf("press:%d pos: %f | %f \n",io.MouseDown[0],io.MousePos.x,io.MousePos.y);
        }
        std::this_thread::sleep_for(0.0001s);
    }
    pthread_exit(0);
}
void Init_touch_config(){ // 初始化触摸设置
    std::thread touch_thread(touch_config);
    touch_thread.detach();
}