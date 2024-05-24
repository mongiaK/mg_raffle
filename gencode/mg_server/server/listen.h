#ifndef __listen_h__
#define __listen_h__

#include "net_header.h"
#include <bits/stdc++.h>

class MListen {
  public:
    MListen(int port, std::string &address);
    ~MListen();
    bool Listen();
    int Socket() const { return _sock; }
    int Port() const { return _port; }

    std::string Address() const { return _address; }
    std::string Url() const { return _url; }

  private:
    int _sock;
    int _port;
    std::string _address;
    std::string _url;
};

#endif // !__listen_h__
