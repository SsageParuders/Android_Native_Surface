//
// Created by fgsqme on 2021/9/26 0026.
//

#ifndef EP_TCPSERVER_H
#define EP_TCPSERVER_H

#include "SocketBase.h"
#include "TCPClient.h"

class TCPServer {
private:
    mFd tcp_fd = -1;
    int port;
public:
    ~TCPServer();
    TCPServer(int port);

    TCPClient *accept();

    ssize_t close();
};


#endif //EP_TCPSERVER_H
