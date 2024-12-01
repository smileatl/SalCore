cmake_minimum_required(VERSION 3.10)

project(base_foss_core)

#库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)


include_directories(${base_srcdir}/base_foss_core)
#include_directories(${base_srcdir}/thirdparty/apr/include)
#include_directories(${base_srcdir}/thirdparty/apr/include/arch/win32)
include_directories(${base_srcdir}/thirdparty/apr-trunk/include)
include_directories(${base_srcdir}/thirdparty/apr-trunk/include/arch/win32)

add_library(base_foss_core SHARED
    "base.h"  
    "base_platform.h"  
    "base_types.h" 
    "base_apr.h" 
    "base_apr.cpp"
    "base_util.h"
    "base_util.cpp"
    "base_core.h"
	"base_core.cpp" )

add_dependencies(base_foss_core libapr-2)
target_link_libraries(base_foss_core ${base_srcdir}/out/libapr-2.lib)


# 为 Windows 平台添加 FREESWITCHCORE_EXPORTS 预处理器定义
if(WIN32)
    target_compile_definitions(base_foss_core PRIVATE FREEBASECORE_EXPORTS)
endif()

