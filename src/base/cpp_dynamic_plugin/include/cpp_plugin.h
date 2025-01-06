#include "base.h"

//#ifdef WIN32
//#ifdef CPP_PLUGIN_EXPORTS
//#define PLUGIN_API __declspec(dllexport)
//#else
//#define PLUGIN_API __declspec(dllimport)
//#endif
//#endif

BASE_BEGIN_EXTERN_C

BASELIB_API void helloCppPlugin();

//PLUGIN_API base_int32_t ExitFunc();
//
//PLUGIN_API Base_ExitFunc Base_initPlugin(const Base_PlatformServices* params);

BASE_END_EXTERN_C