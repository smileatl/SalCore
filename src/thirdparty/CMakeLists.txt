option(BUILD_APR "Build Apr thirdparty module, only applied when BUILD_THIRDPARTY is ON" ON)
option(BUILD_APR_UTIL "Build Apr thirdparty module, only applied when BUILD_THIRDPARTY is ON" ON)

if(BUILD_APR)
    add_subdirectory(${libapr_dir})
endif(BUILD_APR)

