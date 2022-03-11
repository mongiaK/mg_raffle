/*================================================================
*
*  文件名称：watch_mode.h
*  创 建 者: mongia
*  创建日期：2021年01月16日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <iostream>
#include <memory>

namespace DesignMode {

// 设计模式：观察者模式
// 设计思想：不要依赖实现细节，要依赖抽象（通俗点，不要依赖子类，要依赖父类）
// 使用场景：定义对象间的一种一对多的依赖关系，以便当一个对象的状态发生改变时，所有依赖它的对象都会得到通知并自动更新
//           reactor 模型
//
class IProgress {
   public:
    IProgress();
    virtual ~IProgress(){};

    virtual void doProcess();
};

class FilterSplit {
   private:
    IProgress* _process;

   public:
    FilterSplit(IProgress* process) : _process(process){};
    virtual ~FilterSplit();

    void split() {
        std::cout << "notify process" << std::endl;

        notify();
    }

   protected:
    void notify() {
        std::cout << "notify" << std::endl;

        _process->doProcess();
    }
};

// 观察者
class Observer : public IProgress {
   public:
    Observer(){};
    virtual ~Observer(){};

    void run() {
        std::shared_ptr<FilterSplit> obj(new FilterSplit(this));
        obj->split();
    }

    virtual void doProcess() {
        std::cout << "observer do process" << std::endl;
    }
};

static void watch_mode() {
    Observer ob;
    ob.run();
}

}  // namespace DesignMode
