//
// Created by fgsqme on 2021/9/26 0026.
//

#ifndef EP_TCPCLIENT_H
#define EP_TCPCLIENT_H

#include "SocketBase.h"
#include <string>
#include <unistd.h>

using namespace std;

class TCPClient {
private:
    int tcp_fd = -1;
    string ip;
    int port = -1;
public:
    ~TCPClient();

    TCPClient(string ip, int port);

    explicit TCPClient(int tcp_fd);

    bool connect();

    ssize_t send(const void *buff, int len, int flag = 0) const;

    ssize_t recv(void *buff, int len, int flag = 0) const;

    ssize_t recvo(void *buff, size_t len, int flag = 0) const;

    ssize_t recvo(void *buff, int index, size_t len, int flag = 0) const;

    ssize_t close() const;
};


#endif //EP_TCPCLIENT_H
