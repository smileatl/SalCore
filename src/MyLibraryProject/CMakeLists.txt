cmake_minimum_required(VERSION 3.10)
project(MyLibrary)

#库生成路径
set(LIBRARY_OUTPUT_PATH ${base_srcdir}/out)

# 定义用于 DLL 导出的宏
add_definitions(-DMYLIBRARY_EXPORTS)

# 包含头文件目录

add_library(MyLibraryProject SHARED "MyLibraryProject.cpp" "MyLibraryProject.h")


