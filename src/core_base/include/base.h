#ifndef BASE_H
#define BASE_H

#ifdef __cplusplus
#define BASE_BEGIN_EXTERN_C       extern "C" {
#define BASE_END_EXTERN_C         }
#else
#define BASE_BEGIN_EXTERN_C
#define BASE_END_EXTERN_C
#endif

#ifdef BASELIB_EXPORTS
#define BASELIB_API __declspec(dllexport)
#else
#define BASELIB_API __declspec(dllimport)
#endif

#include <stdio.h>

BASELIB_API void sayHello();

#endif // BASE_H