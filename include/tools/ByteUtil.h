
#include "Type.h"

#ifndef BYTEUTIL_H
#define BYTEUTIL_H

class ByteUtil {
public:
    static void ByteArrCopy(const mbyte *d, int d_index, mbyte *t, int t_index, int length);

    static void intToBytes(int i, mbyte *b, int index = 0);

    static int bytesToInt(mbyte *buf, int offset);

    static void longToBytes(mlong i, mbyte *b, int index = 0);

    static mlong bytesToLong(mbyte *buf, int offset);
};

#endif
