#include "listen.h"
#include <istream>
#include <string>

MListen::MListen(int port, std::string &address)
    : _port(port), _address(address) {
    _url = _address + ":" + std::to_string(port);
}

MListen::~MListen() {}

bool MListen::Listen() { return true; }
