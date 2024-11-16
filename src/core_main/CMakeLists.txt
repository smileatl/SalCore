# CMakeList.txt : CMake project for SalCore, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.5)

include_directories(./include)

# Add source to this project's executable.
add_executable (core_main "src/core_main.cpp")

# TODO: Add tests and install targets if needed.
