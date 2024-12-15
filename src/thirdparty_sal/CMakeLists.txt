option(BUILD_LIBMODULE "Build Apr thirdparty module, only applied when BUILD_THIRDPARTY is ON" ON)

if(BUILD_LIBMODULE)
    add_subdirectory("libmodule")
endif(BUILD_LIBMODULE)

