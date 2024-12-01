# CMakeList.txt : CMake project for core_main, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
#库生成路径
include_directories(${base_srcdir}/base_foss_core)
include_directories(${base_srcdir}/thirdparty/apr-trunk/include)
include_directories(${base_srcdir}/thirdparty/apr-trunk/include/arch/win32)

# Add source to this project's executable.
add_executable (core_main "core_main.h" "core_main.cpp" )
# 指定 core_main 目标依赖于 base_foss_core 目标，这意味着在构建 core_main 之前，必须先构建 base_foss_core。这主要用于管理构建顺序和依赖关系，但不会自动链接库。
#add_dependencies(core_main base_foss_core)
# 为 Windows 平台添加 FREESWITCHCORE_EXPORTS 预处理器定义
# 将 base_foss_core 库链接到 core_main 目标，确保 core_main 在编译时链接到 base_foss_core 库。这不仅管理构建顺序，还确保链接器链接到正确的库。
target_link_libraries(core_main ${base_srcdir}/out/base_foss_core.lib)


set_target_properties(core_main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${base_srcdir}/out"
)
