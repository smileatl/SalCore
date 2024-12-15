project(core_base)

#库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

# 头文件
include_directories(${base_srcdir}/thirdparty_sal/libmodule/include)
file(GLOB_RECURSE SOURCE_HEAD  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成动态库
add_library(libmodule SHARED ${SOURCE_SRC} ${SOURCE_HEAD})

# 为 Windows 平台添加 BASELIB_EXPORTS 预处理器定义
if(WIN32)
    target_compile_definitions(libmodule PRIVATE MYLIBRARY_EXPORTS)
endif()
