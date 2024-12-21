project(core_base)

#库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

add_definitions(-DWIN32)
add_definitions(-DBASELIB_EXPORTS)


# 头文件
include_directories(${base_srcdir}/core_base/include)
include_directories(${base_srcdir}/thirdparty/${libapr_dir}/include)
include_directories(${base_srcdir}/thirdparty/${libapr_dir}/include/arch/win32)
file(GLOB_RECURSE SOURCE_HEAD  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成动态库
add_library(core_base SHARED ${SOURCE_SRC} ${SOURCE_HEAD})

add_dependencies(core_base libapr-2)
target_link_libraries(core_base ${base_srcdir}/out/libapr-2.lib)

# 为 Windows 平台添加 BASELIB_EXPORTS 预处理器定义
#if(WIN32)
#    target_compile_definitions(core_base PRIVATE BASELIB_EXPORTS)
#endif()
