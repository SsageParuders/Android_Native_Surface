//
// Created by fgsqme on 2022/9/8.
//


#include <hmutex.h>
#include <HttpServer.h>
#include <hssl.h>

using namespace hv;

void httpService() {
    http_server_t wserver;
    HttpService router;
    router.GET("/", [&router](HttpRequest *req, HttpResponse *resp) {
        resp->body = "ok";
        return 200;
    });
    wserver.service = &router;
    wserver.port = 8080;
    http_server_run(&wserver);
}

/**
 * hv 库测试
 */
int main(int argv, char **argc) {
    httpService();
    return 0;
}