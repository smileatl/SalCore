#include "base.h"

BASE_BEGIN_EXTERN_C

BASELIB_API PluginManagerHandle createPluginManager() 
{
    try {
        return &PluginManager::getInstance(); // 创建 PluginManager 实例
    } catch (...) {
        return nullptr; // 出现异常时返回空指针
    }
}

// 销毁 PluginManager 实例（没有必要显式销毁单例，但保留接口）
BASELIB_API void destroyPluginManager(PluginManagerHandle handle) {
    // 由于使用单例，销毁操作是无效的，单例会在程序退出时自动销毁
    // 这里保留这个接口仅作为形式上的销毁操作
    // 以后销毁时如需做什么操作，就可以在这里实现
}

BASELIB_API int loadAllPlugins(PluginManagerHandle handle, const char* pluginsDir)
{
    if (!handle || !pluginsDir) {
        return -1;
    }

    return static_cast<PluginManager*>(handle)->loadAll(pluginsDir);
}

BASELIB_API int initializeStaticPlugin(PluginManagerHandle handle, Base_InitFunc initFunc)
{
    if (!handle || !initFunc) {
        return -1;
    }

    return static_cast<PluginManager*>(handle)->initializePlugin(initFunc);

}

BASELIB_API void setInvokeService(PluginManagerHandle handle, Base_InvokeServiceFunc invokeServiceFunc)
{
    if (!handle || !invokeServiceFunc) {
        return;
    }

    static_cast<PluginManager*>(handle)->getPlatformServices().invokeService = invokeServiceFunc;

}


BASE_END_EXTERN_C
