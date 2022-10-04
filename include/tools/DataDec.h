//
// Created by fgsqme on 2021/5/10.
//



#ifndef WZ_CHEAT_DATADEC_H
#define WZ_CHEAT_DATADEC_H

#include <string>
#include "Type.h"

using namespace std;

/**
 * 数据朝拆包
 */
class DataDec {
private:
    static const int HEADER_LEN = 12;
    mbyte *m_bytes;
    int index = HEADER_LEN;
    int m_byteLen = 0;

public:
    int getCmd();

    mbyte getByteCmd();

    int getCount();

    int getLength();

    DataDec();

    DataDec(mbyte *bytes, int bytelen);

    void setData(mbyte *bytes, int bytelens);

    int getInt();                      //获取一个int
    mlong getLong();                   //获取一个long
    mbyte getByte();                   //获取一个byte
    bool getBool();                   //获取一个byte
    int getStrLen();                   //获取接下来的字符串长度
    char *getStr();                   //获取字符 字符内存空间为new 需要自行清理
    string getString();                   //获取字符 字符内存空间为new 需要自行清理
    void getStr(char *buff);                    //获取下标的字符
    mbyte *getSurplusBytes();

    void getSurplusBytes(mbyte *buff);

    float getFloat();                  //获取一个float
    double getDouble();                //获取一个double

    int getInt(int i);                      //获取下标的int
    mlong getLong(int i);                   //获取下标的long
    mbyte getByte(int i);                   //获取下标的byte
    char *getStr(int i);                    //获取下标的字符 字符内存空间为new 需要自行清理
    float getFloat(int i);                  //获取下标的float
    double getDouble(int i);                //获取下标的double

    static int headerSize();            //头大小
    void reset();                       //重置读取下标
    void skip(int off);                 //偏移读取下标
    int getDataIndex() const;

    void setDataIndex(int i);
};


#endif //WZ_CHEAT_DATADEC_H
