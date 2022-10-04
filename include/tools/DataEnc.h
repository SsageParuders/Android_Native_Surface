//
// Created by fgsqme on 2021/5/10.
//

#include "Type.h"
#include <string>

#ifndef WZ_CHEAT_DATAENC_H
#define WZ_CHEAT_DATAENC_H

using namespace std;

/**
 * 数据打包
 */
class DataEnc {
private:
    static const int HEADER_LEN = 12;//头长度

    mbyte *m_bytes;
    int index = HEADER_LEN;

    int m_byteLen = 0;

public:
    DataEnc();

    DataEnc(mbyte *bytes, int bytelen);

    void setData(mbyte *bytes, int bytelen);

    void setCmd(int cmd);

    void setByteCmd(mbyte cmd);

    void setCount(int count);

    void setLength(int len);


    DataEnc &putInt(int val);                 //往数据包添加int
    DataEnc &putLong(mlong val);              //往数据包添加long
    DataEnc &putByte(mbyte val);              //往数据包添加byte
    DataEnc &putBytes(mbyte *val, int len);              //往数据包添加byte
    DataEnc &putBool(bool val);              //往数据包添加bool
    DataEnc &putFloat(float val);             //往数据包添加flaot
    DataEnc &putDouble(double val);           //往数据包添加double
    DataEnc &putStr(const char *str, int len);       //往数据包添加字符
    DataEnc &putStr(const char *str);       //往数据包添加字符
    DataEnc &putString(const string &str);       //往数据包添加字符

    DataEnc &putInt(int val, int i);                 //往数据包添加int
    DataEnc &putLong(mlong val, int i);              //往数据包添加long
    DataEnc &putByte(mbyte val, int i);              //往数据包添加byte
    DataEnc &putFloat(float val, int i);             //往数据包添加flaot
    DataEnc &putDouble(double val, int i);           //往数据包添加double
    DataEnc &putStr(const char *str, int len, int i);       //往数据包添加字符

    int getDataLen();

    mbyte *getData();

    void reset();

    static int headerSize();

    int getDataIndex() const;

    void setDataIndex(int i);
};


#endif //WZ_CHEAT_DATAENC_H
