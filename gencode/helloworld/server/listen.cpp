#include "listen.h"
#include "util.h"

MListen::MListen(int port, std::string &address)
    : _port(port), _address(address) {
    _url = _address + ":" + std::to_string(port);
}

MListen::~MListen() {}

bool MListen::Listen() {
    int sock = -1;
    struct sockaddr_in server;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        mlog_info("create socket failed.");
        return false;
    }

    bzero(&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(_port);
    server.sin_addr.s_addr = _address.c_str();

    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        mlog_info("bind failed.");
        close(sock);
        return false;
    }

    if (listen(sock, 1024) < 0) {
        mlog_info("listen failed.");
        close(sock);
        return false;
    }
    return true;
}
