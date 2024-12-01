cmake_minimum_required(VERSION 3.10)
project(MyApplication)

include_directories("../MyLibraryProject")

add_executable(main "main.cpp" "main.h")

target_link_libraries(main ${base_srcdir}/out/MyLibraryProject.lib)

# 设置可执行文件的输出路径
set_target_properties(main PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${base_srcdir}/out"
)
