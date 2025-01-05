#include <stdio.h>
#include "c_plugin.h"
//#include "base_types.h"
//#include "base_plugin.h"
//#include "mellow_monster.h"
//#include "base_types.h"
//#include "base_plugin.h"

BASELIB_API void helloCPlugin(void)
{
    printf("Hello\n");
}



//PLUGIN_API base_int32_t ExitFunc()
//{
//    return 0;
//}
//
//PLUGIN_API Base_ExitFunc PF_initPlugin(const Base_PlatformServices* params)
//{
//    int res = 0;
//
//    Base_RegisterParams rp;
//    rp.version.major = 1;
//    rp.version.minor = 0;
//
//    // Regiater MellowMonster
//    /*rp.createFunc = MellowMonster_create;
//    rp.destroyFunc = MellowMonster_destroy;*/
//    rp.programmingLanguage = Base_ProgrammingLanguage_C;
//
//    res = params->registerObject((const base_byte_t *)"MellowMonster", &rp);
//    if (res < 0)
//        return NULL;
//
//    return ExitFunc;
//
//}