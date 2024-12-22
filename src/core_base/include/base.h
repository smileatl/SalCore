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

#ifndef PLUGIN_API
  #ifdef WIN32
    #define PLUGIN_API __declspec(dllimport)
  #else
    #define PLUGIN_API
  #endif
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <setjmp.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#include "base_types.h"
#include "base_util.h"
#include "base_apr.h"
#include "base_path.h"
#include "base_directory.h"
#include "base_plugin.h"
#include "base_plugin_manager.h"
#include "base_object_adapter.h"

BASE_BEGIN_EXTERN_C

BASELIB_API void sayHello();

BASE_END_EXTERN_C

#endif // BASE_H
