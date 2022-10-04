//
// Created by fgsqme on 2022/3/9 0009.
//

#ifndef LANSHARE_WIN_SOCKETBASE_H
#define LANSHARE_WIN_SOCKETBASE_H

#include "Type.h"

#if defined(PLATFORM_WINDOWS)

#include <winsock2.h>
#include <cstdio>
#include <ws2tcpip.h>

typedef SOCKET mFd;
#elif defined(PLATFORM_ANDROID) || defined(PLATFORM_LINUX)
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
typedef int mFd;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <unistd.h>
typedef int mFd;
#endif


#endif //LANSHARE_WIN_SOCKETBASE_H
