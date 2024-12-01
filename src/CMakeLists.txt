# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
# 允许在 CMake 脚本中使用松散的循环结构
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
# 在 CMake 中设置 Microsoft Visual Studio 编译器的增量链接选项。增量链接可以加快链接时间，因为它只链接更改过的部分，而不是整个程序。
SET(MSVC_INCREMENTAL_DEFAULT ON)


project ("SalCore")

set(base_srcdir ${PROJECT_SOURCE_DIR})

# Include sub-projects.
add_subdirectory("base_foss_core")
add_subdirectory ("core_main")
add_subdirectory("MyLibraryProject")
add_subdirectory("main")

option(BUILD_THIRDPARTY "Set when you want to build the thirdpart" ON)
if (BUILD_THIRDPARTY)
    add_subdirectory("thirdparty")
endif(BUILD_THIRDPARTY)