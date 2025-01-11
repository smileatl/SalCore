#include "base.h"
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#endif

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

    return static_cast<PluginManager*>(handle)->loadAll(Path::makeAbsolute(pluginsDir));
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

BASELIB_API void base_core_runtime_loop(bool backend)
{
#ifdef WIN32
    HANDLE shutdown_event;
    char path[256] = "";
#endif
    if (backend) { // 后台程序
#ifdef WIN32
        snprintf(path, sizeof(path), "Global\\SalCore.%d", _getpid()); // 生成一个全局命名的事件对象路径。
        shutdown_event = CreateEvent(NULL, FALSE, FALSE, path); // 创建一个事件对象
        if (shutdown_event) {
            WaitForSingleObject(shutdown_event, INFINITE); // 等待事件对象被触发，超时时间为 INFINITE，表示无限等待。
        }
#else 
        ;
#endif
    } else {
        char input[256];
        while (1) {
            printf("Enter command: ");
            if (fgets(input, sizeof(input), stdin) == NULL) {
                break;
            }
            printf("You entered: %s", input);
        }; // 前台程序
    }

}

BASE_END_EXTERN_C
