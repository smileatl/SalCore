# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.5)

project ("core_main")

include_directories("${PROJECT_BINARY_DIR}")

# Include sub-projects.
add_subdirectory ("core_main")


