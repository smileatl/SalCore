project(base_foss_core)

set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LIBEDIT_CFLAGS}")

# 为 Windows 平台添加 FREESWITCHCORE_EXPORTS 预处理器定义
if(WIN32)
    add_definitions(-DFREEBASECORE_EXPORTS)
endif()

# include
include_directories(./include)
include_directories(${base_srcdir}/thirdparty/apr/include)
include_directories(${base_srcdir}/thirdparty/apr/include/arch/win32)

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成库
if(CORE_BUILD_SHARED)
    add_library(base_foss_core SHARED ${SOURCE_SRC})
endif()
