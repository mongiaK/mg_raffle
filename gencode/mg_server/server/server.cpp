#include "server.h"
#include "listen.h"
#include <cstdint>

MServer::MServer(std::string &address, uint32_t port)
    : _listensp(new MListen(address, port)) {
    init();
}

void MServer::init() { _listensp->Listen(); }
