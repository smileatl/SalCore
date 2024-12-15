cmake_minimum_required(VERSION 3.8)
# 允许在 CMake 脚本中使用松散的循环结构
set(CMAKE_ALLOW_LOOSE_LOOP_CONSTRUCTS true)
# 在 CMake 中设置 Microsoft Visual Studio 编译器的增量链接选项。增量链接可以加快链接时间，因为它只链接更改过的部分，而不是整个程序。
SET(MSVC_INCREMENTAL_DEFAULT ON)

project(SalCore)

# 项目基础路径
set(base_srcdir ${PROJECT_SOURCE_DIR})
# 第三方库目录变量
set(libapr_dir "apr-trunk")
set(libarputil_dir "apr-util-1.7.x")

# 第三方库
option(BUILD_THIRDPARTY "Set when you want to build the thirdpart" ON)
# 自己写的第三方库
option(BUILD_THIRDPARTY_SAL "Set when you want to build the thirdpart" ON)


# 第三方库编译
if (BUILD_THIRDPARTY)
    add_subdirectory("thirdparty")
endif(BUILD_THIRDPARTY)
# 自己的第三方库编译
if (BUILD_THIRDPARTY_SAL)
    add_subdirectory("thirdparty_sal")
endif(BUILD_THIRDPARTY_SAL)

add_subdirectory(core_base)
add_subdirectory(core_main)
