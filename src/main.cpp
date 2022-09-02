//
// Created by Ssage on 2022/3/18.
//
#include <main.h>

int main(){
    printf("Pid is %d\n", getpid());
    screen_config();
    init_screen_x = screen_x + screen_y;
    init_screen_y = screen_y + screen_x;
    if(!init_egl(init_screen_x,init_screen_y)){
        exit(0);
    }
    ImGui_init();
    Init_touch_config();
    for (;;) {
        tick();
    }
    shutdown();
    return 0;
}