#ifndef BASE_OBJECT_ADAPTER_H
#define BASE_OBJECT_ADAPTER_H

#include "base_plugin.h"

// This interface is used to adapt C plugin objects to C++ plugin objects.
// It must be passed to the PluginManager::createObject() function.
// �˽ӿ����ڽ� C �����������Ϊ C++ �������
// �����봫�ݸ� PluginManager::createObject() ������
struct IObjectAdapter
{
    virtual ~IObjectAdapter() {}
    virtual void * adapt(void * object, Base_DestroyFunc df) = 0;

};

// This template should be used if the object model implements the
// dual C/C++ object design pattern. Otherwise you need to provide
// your own object adapter class that implements IObjectAdapter
// �������ģ��ʵ����˫C/C++�������ģʽ��Ӧʹ�ô�ģ�塣
// ��������Ҫ�ṩ�Լ��Ķ�������������ʵ�� IObjectAdapter��
template<typename T, typename U>
struct ObjectAdapter : public IObjectAdapter
{
    virtual void * adapt(void * object, Base_DestroyFunc df)
    {
        return new T((U *)object, df);
    }
};

#endif // BASE_OBJECT_ADAPTER_H