#ifndef __m_server_h__
#define __m_server_h__

#include <bits/stdc++.h>
#include <shared_mutex>

#include "listen.h"

class MServer {
  public:
    MServer(std::string &address, uint32_t port);
    ~MServer();

  public:
    void init();
    void Start();

  private:
    std::shared_ptr<MListen> _listensp;
};

#endif // !DEBUG
