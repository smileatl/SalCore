#ifndef TEST_H
#define TEST_H

#include <stdio.h>

#ifdef MYLIBRARY_EXPORTS
#define MYLIB_API __declspec(dllexport)
#else
#define MYLIB_API __declspec(dllimport)
#endif


MYLIB_API void sayHello();


#endif // TEST_H