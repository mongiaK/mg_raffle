/*================================================================
*
*  文件名称：template_method.h
*  创 建 者: mongia
*  创建日期：2021年01月16日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <iostream>
#include <memory>

// 设计模式：模板方法
// 设计思想：底层调用上层
// 使用场景：多层接口设计的时候

namespace DesignMode {

class Library {
   public:
    Library(){};
    virtual ~Library(){};

    void run() {
        step1();

        step2();

        step3();

        step4();
    }

   protected:
    virtual void step1() { std::cout << __func__ << std::endl; }

    virtual void step2() { std::cout << __func__ << std::endl; }

    virtual void step3() { std::cout << __func__ << std::endl; }

    virtual void step4() { std::cout << __func__ << ::std::endl; }
};

class Application : public Library {
   public:
    Application(){};
    virtual ~Application(){};

   protected:
    virtual void step1() { std::cout << __func__ << ::std::endl; }

    virtual void step3() { std::cout << __func__ << ::std::endl; }
};

static void template_method() {
    std::shared_ptr<Application> app(new Application());
    app->run();
}

}  // namespace TemplateMethod
