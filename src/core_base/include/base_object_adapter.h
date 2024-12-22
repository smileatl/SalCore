#ifndef BASE_OBJECT_ADAPTER_H
#define BASE_OBJECT_ADAPTER_H

#include "base_plugin.h"

// This interface is used to adapt C plugin objects to C++ plugin objects.
// It must be passed to the PluginManager::createObject() function.
// 此接口用于将 C 插件对象适配为 C++ 插件对象。
// 它必须传递给 PluginManager::createObject() 函数。
struct IObjectAdapter
{
    virtual ~IObjectAdapter() {}
    virtual void * adapt(void * object, Base_DestroyFunc df) = 0;

};

// This template should be used if the object model implements the
// dual C/C++ object design pattern. Otherwise you need to provide
// your own object adapter class that implements IObjectAdapter
// 如果对象模型实现了双C/C++对象设计模式，应使用此模板。
// 否则，你需要提供自己的对象适配器类来实现 IObjectAdapter。
template<typename T, typename U>
struct ObjectAdapter : public IObjectAdapter
{
    virtual void * adapt(void * object, Base_DestroyFunc df)
    {
        return new T((U *)object, df);
    }
};

#endif // BASE_OBJECT_ADAPTER_H