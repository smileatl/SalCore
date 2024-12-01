#pragma once

#ifdef MYLIBRARY_EXPORTS
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif

extern "C" {
    MYLIB_API void sayHello();
}
