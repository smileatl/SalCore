#include "static_plugin.h"
#include "fidgety_phantom.h"
#include <iostream>

BASE_BEGIN_EXTERN_C

base_int32_t StaticPlugin_ExitFunc()
{
    std::cout << "hello, StaticPlugin_ExitFunc" << std::endl;
    return 0;
}

Base_ExitFunc StaticPlugin_InitPlugin(const Base_PlatformServices * params)
{
    std::cout << "hello, StaticPlugin_InitPlugin" << std::endl;

    int res = 0;
    Base_RegisterParams rp;
    rp.version.major = 1;
    rp.version.minor = 0;
    rp.programmingLanguage = Base_ProgrammingLanguage_CPP;

    // Regiater FidgetyPhantom
    rp.createFunc = FidgetyPhantom::create;
    rp.destroyFunc = FidgetyPhantom::destroy;
    res = params->registerObject((const base_byte_t *)"FidgetyPhantom", &rp);
    if (res < 0) {
        return NULL;
    }


    return StaticPlugin_ExitFunc;
}

BASE_END_EXTERN_C

