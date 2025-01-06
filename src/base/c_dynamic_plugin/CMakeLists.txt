project(c_dynamic_plugin)

#库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

# 添加编译选项
add_definitions(-DWIN32)
add_definitions(-DBASELIB_EXPORTS)
add_definitions(-DPLUGIN_EXPORTS)
add_definitions(-DPF_PLATFORM_WINDOWS)


# 头文件
include_directories(${base_srcdir}/base/c_dynamic_plugin/include)
include_directories(${base_srcdir}/core_base/include)
file(GLOB_RECURSE SOURCE_HEAD ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成动态库
add_library(c_dynamic_plugin SHARED ${SOURCE_SRC} ${SOURCE_HEAD})

add_dependencies(c_dynamic_plugin core_base)
target_link_libraries(c_dynamic_plugin ${base_srcdir}/out/core_base.lib)

# 设置目标库为 C 编译，cmake会自动将.c文件当作C语言文件来编译，因此通常不需要显式指定
# 如果是C和CPP混合，最好显式指定CXX编译
#set_target_properties(c_dynamic_plugin PROPERTIES LINKER_LANGUAGE C)

# 为 Windows 平台添加 BASELIB_EXPORTS 预处理器定义
#if(WIN32)
#    target_compile_definitions(core_base PRIVATE BASELIB_EXPORTS)
#endif()
