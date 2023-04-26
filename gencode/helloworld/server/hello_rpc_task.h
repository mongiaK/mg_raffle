/*================================================================
*  
*  文件名称：HelloRPCTask.h
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#ifndef __HELLORPCTASK_H__
#define __HELLORPCTASK_H__

#include "rpc.pb.h"
#include <memory>

class HelloTask {
  public:
    HelloTask(std::shared_ptr<M::HelloReq> req,
              std::shared_ptr<M::HelloRes> res);
    ~HelloTask();

    void Run();

  private:
    std::shared_ptr<M::HelloReq> _req;
    std::shared_ptr<M::HelloRes> _res;
};

typedef std::shared_ptr<HelloTask> HelloTaskSP;
#endif // __HELLORPCTASK_H__
