//
// Created by fgsqme on 2021/5/10.
//

#include "DataEnc.h"
#include "ByteUtils.h"
#include <unistd.h>
#include <cstring>


DataEnc::DataEnc() {

}

DataEnc::DataEnc(mbyte *bytes, int bytelen) {
    setData(bytes, bytelen);
}

int DataEnc::headerSize() {
    return HEADER_LEN;
}

void DataEnc::setData(mbyte *bytes, int bytelen) {
    if (bytes != NULL) {
        m_bytes = bytes;
        m_byteLen = bytelen;
    }
}

void DataEnc::setCmd(int cmd) {
    putInt(cmd, 0);
}

void DataEnc::setByteCmd(mbyte cmd) {
    putByte(cmd, 0);
}


void DataEnc::setCount(int count) {
    putInt(count, 4);
}

void DataEnc::setLength(int length) {
    putInt(length, 8);
}


DataEnc &DataEnc::putInt(int val) {
    if ((index + 4) <= m_byteLen) {
        ByteUtils::intToBytes(val, m_bytes, index);
        index += 4;
    }
    return *this;
}

DataEnc &DataEnc::putLong(mlong val) {
    if ((index + 8) <= m_byteLen) {
        ByteUtils::longToBytes(val, m_bytes, index);
        index += 8;
    }
    return *this;
}

DataEnc &DataEnc::putByte(mbyte val) {
    if ((index + 1) <= m_byteLen) {
        m_bytes[index] = val;
        index += 1;
    }
    return *this;
}

DataEnc &DataEnc::putBytes(mbyte *val, int len) {
    if ((index + len + 4) <= m_byteLen) {
        putInt(len);
        ByteUtils::ByteArrCopy(val, 0, m_bytes, index, len);
        index += len;
    }
    return *this;
}

DataEnc &DataEnc::putBool(bool val) {
    return putByte(val);
}

DataEnc &DataEnc::putFloat(float val) {
    return putInt((int) (val * 1000));
}

DataEnc &DataEnc::putDouble(double val) {
    return putLong((mlong) (val * 1000000));
}

DataEnc &DataEnc::putStr(const char *str, int len) {
    return putBytes((mbyte *) str, len);;
}

DataEnc &DataEnc::putStr(const char *str) {
    return putStr(str, strlen(str));
}

DataEnc &DataEnc::putString(const string &str) {
    return putStr(str.c_str(), str.size());
}

int DataEnc::getDataLen() {
    return index;
}

void DataEnc::reset() {
    index = HEADER_LEN;
}

mbyte *DataEnc::getData() {
    setLength(index - HEADER_LEN);
    return m_bytes;
}

DataEnc &DataEnc::putInt(int val, int i) {
    if ((i + 4) <= m_byteLen) {
        ByteUtils::intToBytes(val, m_bytes, i);
    }
    return *this;
}

DataEnc &DataEnc::putLong(mlong val, int i) {
    if ((i + 8) <= m_byteLen) {
        ByteUtils::longToBytes(val, m_bytes, i);
    }
    return *this;
}

DataEnc &DataEnc::putByte(mbyte val, int i) {
    if ((i + 1) <= m_byteLen) {
        m_bytes[i] = val;
    }
    return *this;
}

DataEnc &DataEnc::putFloat(float val, int i) {
    putInt(val * 1000, i);
    return *this;
}

DataEnc &DataEnc::putDouble(double val, int i) {
    putLong(val * 1000000, i);
    return *this;
}

DataEnc &DataEnc::putStr(const char *str, int len, int i) {
    if ((i + len + 4) <= m_byteLen) {
        putInt(len, i);
        i += 4;
        ByteUtils::ByteArrCopy((mbyte *) str, 0, m_bytes, i, len);
    }
    return *this;
}

int DataEnc::getDataIndex() const {
    return index - HEADER_LEN;
}

void DataEnc::setDataIndex(int i) {
    index = i + HEADER_LEN;
}




