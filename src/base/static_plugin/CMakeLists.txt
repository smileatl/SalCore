project(static_plugin)

# 库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

# 添加编译选项
add_definitions(-DWIN32)
add_definitions(-DPF_PLATFORM_WINDOWS)
# 静态库不需要定义导出符号

# 包含头文件
include_directories(${base_srcdir}/base/static_plugin/include)
include_directories(${base_srcdir}/core_base/include)
file(GLOB_RECURSE SOURCE_HEAD  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成静态库
add_library(static_plugin STATIC ${SOURCE_SRC} ${SOURCE_HEAD})


add_dependencies(static_plugin core_base)
target_link_libraries(static_plugin ${base_srcdir}/out/core_base.lib)