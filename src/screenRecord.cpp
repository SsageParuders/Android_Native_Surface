//
// Created by fgsqme on 2022/9/29.
//
#include "extern_function.h"
#include "DataEnc.h"
#include "TCPClient.h"
#include "ByteUtils.h"
#include "TimeTools.h"
#include <thread>

FILE *fp;
// 录屏flag，设置false退出录屏
bool flag = true;
TCPClient *tcpClient;
mlong currentTime = TimeTools::getCurrentTime();
int fps = 0;
int ffps = 0;
mlong totalTime = 0;

/**
 * 录屏回调
 * @param buff 录屏数据包
 * @param size 数据长度
 */
void callback(uint8_t *buff, size_t size) {
    // fps 计算(大概，不精准)
    mlong end = TimeTools::getCurrentTime();
    totalTime += (end - currentTime);
    if (totalTime >= 1000) {
        fps = ffps;
        ffps = 0;
        totalTime = 0;
        printf(" fps: %d\n", fps);
    }
    currentTime = end;
    ffps++;
    // 将buff写入文件
    fwrite(buff, 1, size, fp);

    mbyte byte[DataEnc::headerSize() + size];
    memcpy(byte + DataEnc::headerSize(), buff, size);
    // 数据打包
    auto *dataEnc = new DataEnc(byte, DataEnc::headerSize() + size);
    // 设置数据包下标
    dataEnc->setDataIndex((int) size);
    //printf("size: %zu getDataLen:%d fps:%d\n", size, dataEnc->getDataLen(), fps);
    // 将buff发送
    if (!tcpClient->send(dataEnc->getData(), dataEnc->getDataLen())) {
        // 发送失败退出录屏
        printf("Failed to send buffer\n");
        flag = false;
    }
}

/**
 * h264录屏测试
 * 运行后会保存录屏数据并且使用tcp发送数据流
 * 代码 recordReceive.cpp 作为接收端
 */
int main(int argc, char *argv[]) {
    // tcp客户端
    tcpClient = new TCPClient("192.168.31.108", 6656);
    if (!tcpClient->connect()) {
        return -1;
    }
    ExternFunction functionRecord;
    // 录屏文件保存路径
    fp = fopen("/sdcard/test.h264", "w");
    // 初始化录屏，帧率设置无用待解决
    functionRecord.initRecord("1M", 60.0F, 720, 1280);
    // 开始录屏
    functionRecord.runRecord(&flag, callback);
    functionRecord.stopRecord();
    fclose(fp);
    return 0;
}