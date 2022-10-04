//
// Created by fgsqme on 2021/9/26 0026.
//

#include "TCPServer.h"

TCPServer::~TCPServer() {
    close();
}

TCPServer::TCPServer(int port) : port(port) {

#if defined(PLATFORM_WINDOWS)
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        printf("Failed to load Winsock.\n");
        return;
    }
#endif
    tcp_fd = socket(PF_INET, SOCK_STREAM, 0);
    sockaddr_in servAddr{};
    memset(&servAddr, 0, sizeof(servAddr));
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(port);
    if (::bind(tcp_fd, (sockaddr *) &servAddr, sizeof(servAddr)) == -1) {
        puts("bind error!");
        return;
    }
    if (::listen(tcp_fd, 5) == -1) {
        puts("listen error!");
        return;
    }
    puts("init TCPServer success!");
}

TCPClient *TCPServer::accept() {
    int newClient = ::accept(tcp_fd, nullptr, nullptr);
    if (-1 == newClient) {
        puts("acept error!");
        return nullptr;
    } else {
        puts("new client connect");
    }
    return new TCPClient(newClient);
}


ssize_t TCPServer::close() {
#if defined(PLATFORM_WINDOWS)
    return ::closesocket(tcp_fd);
#else
    return ::close(tcp_fd);
#endif
}



