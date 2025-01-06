project(core_test)

# 头文件
include_directories(${base_srcdir}/core_test/include)
file(GLOB_RECURSE SOURCE_HEAD  ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h )

# 源文件
aux_source_directory(./src SOURCE_SRC)

# 生成可执行文件
add_executable(core_test ${SOURCE_SRC} ${SOURCE_HEAD})

# 清空子系统设置
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SubSystem:Console")

# 设置可执行文件的输出路径
set_target_properties(core_test PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${base_srcdir}/out"
)

