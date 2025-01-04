#ifndef BASE_PLUGIN_MANAGER_API_H
#define BASE_PLUGIN_MANAGER_API_H

#include "base.h"

BASE_BEGIN_EXTERN_C

// 不透明指针是一种设计模式，使用 void* 类型在 C 接口中表示类的实例
// 用户通过创建和销毁函数来管理类实例，并通过接口函数操作该实例
typedef void* PluginManagerHandle;

// 创建 PluginManager 实例
BASELIB_API PluginManagerHandle createPluginManager();

// 销毁 PluginManager 实例
BASELIB_API void destroyPluginManager(PluginManagerHandle handle);

// 加载所有插件
BASELIB_API int loadAllPlugins(PluginManagerHandle handle, const char* pluginsDir);

// 初始化静态插件
BASELIB_API int initializeStaticPlugin(PluginManagerHandle handle, Base_InitFunc initFunc);

// 设置invoke函数指针
BASELIB_API void setInvokeService(PluginManagerHandle handle, Base_InvokeServiceFunc invokeServiceFunc);

BASE_END_EXTERN_C

#endif // BASE_PLUGIN_MANAGER_API_H