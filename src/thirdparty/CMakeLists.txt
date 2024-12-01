option(BUILD_APR "Build Apr thirdparty module, only applied when BUILD_THIRDPARTY is ON" ON)

if(BUILD_APR)
    add_subdirectory("apr-trunk")
endif()