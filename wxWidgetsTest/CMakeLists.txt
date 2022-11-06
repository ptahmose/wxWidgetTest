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

  target_link_libraries(wxWidgetsTest PRIVATE ${wxWidgets_LIBRARIES})
 
# TODO: Add tests and install targets if needed.
