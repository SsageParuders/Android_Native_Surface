//
// Created by fgsqme on 2021/5/10.
//

#include "ByteUtils.h"
#include "Type.h"

void ByteUtils::ByteArrCopy(const mbyte *d, int d_index, mbyte *t, int t_index, int length) {
    for (int i = 0; i < length; i++) {
        t[t_index + i] = d[d_index + i];
    }
}

void ByteUtils::intToBytes(int i, mbyte *b, int index) {
    b[0 + index] = (mbyte) ((i >> 24) & 0xFF);
    b[1 + index] = (mbyte) ((i >> 16) & 0xFF);
    b[2 + index] = (mbyte) ((i >> 8) & 0xFF);
    b[3 + index] = (mbyte) (i & 0xFF);
}

int ByteUtils::bytesToInt(mbyte *buf, int offset) {
    int i = 0;
    i = i | ((buf[offset] & 0xFF) << 24);
    i = i | ((buf[offset + 1] & 0xFF) << 16);
    i = i | ((buf[offset + 2] & 0xFF) << 8);
    i = i | (buf[offset + 3] & 0xFF);
    return i;
}

/*
 * long to byte[]
 */

void ByteUtils::longToBytes(mlong i, mbyte *b, int index) {
    b[0 + index] = (mbyte) ((i >> 56) & 0xFF);
    b[1 + index] = (mbyte) ((i >> 48) & 0xFF);
    b[2 + index] = (mbyte) ((i >> 40) & 0xFF);
    b[3 + index] = (mbyte) ((i >> 32) & 0xFF);
    b[4 + index] = (mbyte) ((i >> 24) & 0xFF);
    b[5 + index] = (mbyte) ((i >> 16) & 0xFF);
    b[6 + index] = (mbyte) ((i >> 8) & 0xFF);
    b[7 + index] = (mbyte) (i & 0xFF);
}

mlong ByteUtils::bytesToLong(mbyte *buf, int offset) {
    mlong i = 0;
    i = i | (((mlong) buf[offset] & 0xFF) << 56)
        | (((mlong) buf[offset + 1] & 0xFF) << 48)
        | (((mlong) buf[offset + 2] & 0xFF) << 40)
        | (((mlong) buf[offset + 3] & 0xFF) << 32)
        | (((mlong) buf[offset + 4] & 0xFF) << 24)
        | (((mlong) buf[offset + 5] & 0xFF) << 16)
        | (((mlong) buf[offset + 6] & 0xFF) << 8)
        | ((mlong) buf[offset + 7] & 0xFF);
    return i;
}
