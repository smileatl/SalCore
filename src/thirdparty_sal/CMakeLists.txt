option(BUILD_LIBMODULE "Build Apr thirdparty module, only applied when BUILD_THIRDPARTY is ON" OFF)

if(BUILD_LIBMODULE)
    add_subdirectory("libbasemodule")
endif(BUILD_LIBMODULE)

