# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

cmake_policy(SET CMP0057 NEW)

project ("wxWidgetsTest")

find_package(wxWidgets CONFIG REQUIRED)

# Include sub-projects.
add_subdirectory ("wxWidgetsTest")
