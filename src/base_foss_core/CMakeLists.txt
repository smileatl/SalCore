project(base_foss_core)

set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBEDIT_CFLAGS}")

# 为 Windows 平台添加 FREESWITCHCORE_EXPORTS 预处理器定义
if(WIN32)
    add_definitions(-FREESWITCHCORE_EXPORTS)
endif()

# include
include_directories(./include)
include_directories(${base_srcdir}/thirdparty/apr/include)
include_directories(${base_srcdir}/thirdparty/apr/include/arch/win32)
include_directories(${base_srcdir}/thirdparty/sqlite-autoconf-3150200)
include_directories(${base_srcdir}/thirdparty/libteletone/src)

# 源文件
aux_source_directory(./src SOURCE_SRC)
# 头文件
file(GLOB_RECURSE MOD_HEADERS  ./include/*.h)
# 在可视化工具中创建自定义的源文件组织结构，而不是按照实际文件夹结构显示这些头文件。(工程本身已经组织好了，无需这么做)
# source_group("include" FILES ${MOD_HEADERS})

# 生成库
if(CORE_BUILD_SHARED)
    add_library(base_foss_core SHARED ${SOURCE_SRC} ${MOD_HEADERS})
endif()
