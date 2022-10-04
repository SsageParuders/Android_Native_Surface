//
// Created by fgsqme on 2021/8/4 0004.
//

#ifndef EW_TIMETOOLS_H
#define EW_TIMETOOLS_H


#include "Type.h"

class TimeTools {
public:
    static void getFormatTime(char *str_time);
    static void sleep_s(int s);               //毫秒延时
    static void sleep_ms(int ms);               //毫秒延时
    static void sleep_us(int us);               //微秒延时
    static mlong getCurrentTime();           //获取系统时间
};


#endif //EW_TIMETOOLS_H
