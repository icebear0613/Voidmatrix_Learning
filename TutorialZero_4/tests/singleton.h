#pragma once

//单例模式：
//通过私有其构造函数和拷贝函数等创建对象的方法统一对外暴露一个获取类示例的静态方法

class Singleton
{
public:
    static Singleton* instance();

private:
    static Singleton* instance;

private:
    Singleton() {};
};