project(core_main)

# 头文件
include_directories(${base_srcdir}/core_main/include)
include_directories(${base_srcdir}/core_base/include)
#include_directories(${base_srcdir}/thirdparty/${libapr_dir}/include)
#include_directories(${base_srcdir}/thirdparty/${libapr_dir}/include/arch/win32)
file(GLOB_RECURSE SOURCE_HEAD  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成可执行文件
add_executable(core_main ${SOURCE_SRC} ${SOURCE_HEAD})
# 指定 core_main 目标依赖于 core_base 目标，这意味着在构建 core_main 之前，必须先构建 core_base。这主要用于管理构建顺序和依赖关系，但不会自动链接库。
#add_dependencies(core_main core_base)
# 将 core_base 库链接到 core_main 目标，确保 core_main 在编译时链接到 core_base 库。这不仅管理构建顺序，还确保链接器链接到正确的库。
target_link_libraries(core_main ${base_srcdir}/out/core_base.lib)

# 设置可执行文件的输出路径
set_target_properties(core_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${base_srcdir}/out"
)



