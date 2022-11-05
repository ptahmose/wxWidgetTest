# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

cmake_policy(SET CMP0057 NEW)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

project ("wxWidgetsTest")

#find_package(IPP CONFIG REQUIRED)
#[[
The story is something like: 
* The CMake-bundled "find-wxWidget"-function does not work with CMake older than 3.24
* Therefore, we bring our own FindwxWidget-script in this repository

sudo apt install libwxgtk3.0-gtk3-dev

]]#


find_package(wxWidgets  REQUIRED  COMPONENTS net core base)
message("wxWidgets_FOUND           : ${wxWidgets_FOUND}")
message("wxWidgets_INCLUDE_DIRS    : ${wxWidgets_INCLUDE_DIRS}")
message("wxWidgets_LIBRARY_DIRS    : ${wxWidgets_LIBRARY_DIRS}")
message("wxWidgets_LIBRARIES       : ${wxWidgets_LIBRARIES}")
message("wxWidgets_CXX_FLAGS       : ${wxWidgets_CXX_FLAGS}")
message("wxWidgets_USE_FILE        : ${wxWidgets_USE_FILE}")
include(${wxWidgets_USE_FILE})

# Include sub-projects.
add_subdirectory ("wxWidgetsTest")
