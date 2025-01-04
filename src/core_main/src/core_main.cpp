#include <base.h>
#include <iostream>
#include <string>

#include "core_main.h"

base_int32_t DummyInvokeService(const base_byte_t* serviceName, void* serviceParams)
{
    return 0;
}

int main(int argc, char* argv[]) {
    sayHello();
    std::cout << "Hello SalCore!" << std::endl;
    

    ////检测输入参数的个数
    //if (argc != 2)
    //{
    //    std::cout << "Usage: great_game <plugins dir>" << std::endl;
    //    return -1;
    //}

    // apr函数库初始化，是一个第三方的，apr的全称是Apache Portable Runtime Project
    base_apr_initialize();

    // 获取PluginManager单例实例
    PluginManagerHandle pm = createPluginManager();
    if (!pm) {
        std::cerr << "Failed to initialize PluginManager" << std::endl;
        return -1;
    }

    // 赋值一个函数指针
    setInvokeService(pm, DummyInvokeService);



    destroyPluginManager(pm);

    base_apr_terminate();

    return 0;
}