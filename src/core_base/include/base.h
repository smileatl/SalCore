#ifndef BASE_H
#define BASE_H



//#ifdef BASELIB_EXPORTS
//#define BASELIB_API __declspec(dllexport)
//#else
//#define BASELIB_API __declspec(dllimport)
//#endif

//#ifdef WIN32
//#ifdef PLUGIN_EXPORTS
//#define PLUGIN_API __declspec(dllexport)
//#else
//#define PLUGIN_API __declspec(dllimport)
//#endif
//#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stddef.h>
#include <assert.h>
#include <setjmp.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <math.h>

#include "base_platform.h"
#include "base_types.h"
#include "base_util.h"
#include "base_apr.h"
#include "base_path.h"
#include "base_directory.h"
#include "base_dynamic_library.h"
#include "base_plugin.h"
#include "base_plugin_manager.h"
#include "base_object_adapter.h"
#include "base_plugin_manager_api.h"

BASE_BEGIN_EXTERN_C

BASELIB_API void sayHello();

BASE_END_EXTERN_C

#endif // BASE_H
