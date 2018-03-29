# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("postgres-lib" VERSION 0.1 LANGUAGES CXX)

# Include sub-projects.
add_subdirectory ("postgres-lib")
add_subdirectory ("test")
