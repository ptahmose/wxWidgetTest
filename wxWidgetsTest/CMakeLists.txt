# CMakeList.txt : CMake project for wxWidgetsTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (wxWidgetsTest WIN32 
  "wxWidgetsTest.cpp" 
  "wxWidgetsTest.h" 
  "compressionOptions.h" 
  "DoOperation.h" 
  "DoOperation.cpp"
  "wxProgressInfoClientData.h"
  "wxProgressInfoClientData.cpp")

 #target_link_libraries(wxWidgetsTest PRIVATE wx::core wx::base)
 target_link_libraries(wxWidgetsTest PRIVATE ${wxWidgets_LIBRARIES})
 #target_include_directories(wxWidgetsTest PRIVATE ${wxWidgets_INCLUDE_DIRS})

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET wxWidgetsTest PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
