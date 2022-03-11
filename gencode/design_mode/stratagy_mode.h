/*================================================================
*
*  文件名称：stratagy_mode.h
*  创 建 者: mongia
*  创建日期：2021年01月16日
*  邮    箱：mr_pengmj@outlook.com
*
================================================================*/

#pragma once

#include <iostream>
#include <memory>

// 设计模式：策略模式
// 设计思想：通常if else 非常多的情况下，可以考虑这个设计模式
//           以时间轴的方式去判断各类变化
// 使用场景：通常if else比较多的时候会用到
namespace DesignMode {

class Language {
   public:
    Language();
    virtual ~Language(){};

    virtual void explain() = 0;
};

class EnglishLanguage : public Language {
   public:
    EnglishLanguage();
    virtual ~EnglishLanguage(){};

    virtual void explain() { std::cout << "english language" << std::endl; }
};

class JapanLanguage : public Language {
   public:
    JapanLanguage(){};
    virtual ~JapanLanguage(){};

    virtual void explain() { std::cout << "japan language" << std::endl; }
};

class LanguageFactory {
   public:
    static std::shared_ptr<Language> createLanguageParse(int type) {
        std::shared_ptr<Language> parse(nullptr);
        switch (type) {
            case 1:
                parse = std::make_shared<Language>(new EnglishLanguage());
                break;
            case 2:
                parse = std::make_shared<Language>(new JapanLanguage());
                break;
            default:
                break;
        }
        return parse;
    }
};

static void stratagy_mode() {
    int type = 2;
    std::shared_ptr<Language> parse =
        LanguageFactory::createLanguageParse(type);
    parse->explain();
}

}  // namespace DesignMode
