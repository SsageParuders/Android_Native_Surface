//
// Created by fgsqme on 2021/9/26 0026.
//

#include "TCPClient.h"

#include <utility>


TCPClient::~TCPClient() {
    close();
}


TCPClient::TCPClient(int tcp_fd) : tcp_fd(tcp_fd) {

}

TCPClient::TCPClient(string ip, int port) : ip(std::move(ip)), port(port) {

}


bool TCPClient::connect() {
    tcp_fd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in servAddr{};
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    servAddr.sin_port = htons(port);
    if (::connect(tcp_fd, (struct sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        puts("connect  error!");
        return false;
    } else {
        puts("connect success!");
        return true;
    }
}


ssize_t TCPClient::send(const void *buff, int len, int flag) const {
    return ::send(tcp_fd, static_cast<const char *>(buff), len, flag);
}

ssize_t TCPClient::recv(void *buff, int len, int flag) const {
    return ::recv(tcp_fd, static_cast<char *>(buff), len, flag);
}

ssize_t TCPClient::recvo(void *buff, size_t len, int flag) const {
    return recvo(buff, 0, len, flag);
}


ssize_t TCPClient::recvo(void *buff, int index, size_t len, int flag) const {
    auto *tempBuff = (unsigned char *) buff;
    int totalRecv = 0;
    int off = index;
    size_t size = len;
    while (size > 0) {
        ssize_t i = ::recv(tcp_fd, reinterpret_cast<char *>(&tempBuff[off]), size, flag);
        if (i == 0) {
            return i;
        } else if (i == -1) {
            // 数据接收错误，可能客户端断开连接
            printf("error during recvall: %d\n", (int) i);
            return i;
        }
        totalRecv += (int) i;
        off += (int) i;
        size -= i;
    }
    return totalRecv;
}

ssize_t TCPClient::close() const {
#if defined(PLATFORM_WINDOWS)
    return ::closesocket(tcp_fd);
#else
    return ::close(tcp_fd);
#endif
}



