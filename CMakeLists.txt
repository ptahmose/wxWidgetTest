# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules")

project ("wxWidgetsTest")

#[[
The story is something like: 
* The CMake-bundled "find-wxWidget"-function does not work with CMake older than 3.24
* Therefore, we bring our own FindwxWidget-script in this repository
* this is suggested here -> https://docs.wxwidgets.org/trunk/overview_cmake.html

On Linux, the wxWidgets-development files can be installed with:
sudo apt install libwxgtk3.0-gtk3-dev
]]#


find_package(wxWidgets  REQUIRED COMPONENTS net core base)
include(${wxWidgets_USE_FILE})

# Include sub-projects.
add_subdirectory ("wxWidgetsTest")
