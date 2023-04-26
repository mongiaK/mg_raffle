/*================================================================
*  
*  文件名称：HelloRPCTask.cpp
*  创 建 者: mongia
*  创建日期：2023年04月23日
*  邮    箱：mongiaK@outlook.com
*  
================================================================*/

#include "hello_rpc_task.h"
#include <memory>

HelloTask::HelloTask(std::shared_ptr<M::HelloReq> req, std::shared_ptr<M::HelloRes> res)
    :_req(req)
     ,_res(res)
{}

HelloTask::~HelloTask()
{}

void HelloTask::Run()
{
    _res->set_name("world");
}
