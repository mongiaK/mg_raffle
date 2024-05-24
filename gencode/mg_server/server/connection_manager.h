/*================================================================
*  
*  文件名称：connection_manager.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __CONNECTION_MANAGER_H__
#define __CONNECTION_MANAGER_H__

#include "util.h"

#include <map>

class MConnManager {
  public:
      MConnManager(int epfd);
      ~MConnManager();
    void CreateConn(int32_t& fd, std::string &client_ip, long& client_port);

    MConnectionSP GetConn(int fd);

    void CloseConn(int fd);
  private:
    std::map<int, MConnectionSP> _conns;
    int _epfd; 
};

typedef std::shared_ptr<MConnManager> MConnManagerSP;

#endif // __CONNECTION_MANAGER_H__
