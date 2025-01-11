#include "core_test.h"

#ifdef WIN32
#include <windows.h>
#endif

int main()
{
    bool backend = true;
    // 是否是后台程序
    int a = 100;
    if (backend) {
#ifdef WIN32
        a = FreeConsole(); // 后台程序则关闭console
#else
        ;
#endif 
    }

    while (1) {

    }
}