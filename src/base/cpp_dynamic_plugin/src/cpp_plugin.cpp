#include "cpp_plugin.h"
#include "base.h"
#include <iostream>

BASE_BEGIN_EXTERN_C

BASELIB_API void helloCppPlugin() {
    std::cout << "hello, cpp_plugin" << std::endl;
}

BASE_END_EXTERN_C