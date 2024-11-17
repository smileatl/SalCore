# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.5)

project ("SalCore")

include_directories("${PROJECT_BINARY_DIR}")

option(CORE_BUILD_SHARED "build shared library" ON)

set(base_srcdir ${PROJECT_SOURCE_DIR})

# Include sub-projects.
add_subdirectory ("core_main")
add_subdirectory ("base_foss_core")

