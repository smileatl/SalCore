#include "cpp_plugin.h"
#include "base.h"
#include "kill_bunny.h"
#include "base_platform.h"
#include <iostream>

BASE_BEGIN_EXTERN_C

BASELIB_API void helloCppPlugin() {
    std::cout << "hello, cpp_plugin" << std::endl;
}

PLUGIN_API base_int32_t ExitFunc()
{
    std::cout << "hello, cpp_plugin: ExitFunc" << std::endl;
    return 0;
}

PLUGIN_API Base_ExitFunc Base_initPlugin(const Base_PlatformServices * params)
{
    std::cout << "hello, cpp_plugin: Base_initPlugin" << std::endl;

  int res = 0;
    
  Base_RegisterParams rp;
  rp.version.major = 1;
  rp.version.minor = 0;
  rp.programmingLanguage = Base_ProgrammingLanguage_CPP;
  
  // Regiater KillerBunny
  rp.createFunc = KillerBunny::create;
  rp.destroyFunc = KillerBunny::destroy;
  res = params->registerObject((const base_byte_t *)"KillerBunny", &rp);
  if (res < 0)
    return NULL;

  return ExitFunc;
}

BASE_END_EXTERN_C