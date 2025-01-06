//#include "base.h"
//#ifdef WIN32
//#ifdef C_PLUGIN_EXPORTS
//#define PLUGIN_API __declspec(dllexport)
//#else
//#define PLUGIN_API __declspec(dllimport)
//#endif
//#endif
#include "base_platform.h"

BASE_BEGIN_EXTERN_C

BASELIB_API void helloCPlugin(void);

BASE_END_EXTERN_C

//BASELIB_API void helloCPlugin();