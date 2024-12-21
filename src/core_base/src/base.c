#include "base.h"

BASE_BEGIN_EXTERN_C

BASELIB_API void sayHello() {
    printf("Hello from DLL!\n");
}

BASE_END_EXTERN_C
