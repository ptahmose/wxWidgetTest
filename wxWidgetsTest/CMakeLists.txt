# CMakeList.txt : CMake project for wxWidgetsTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (wxWidgetsTest WIN32 
  "wxWidgetsTest.cpp" 
  "wxWidgetsTest.h" 
#  "../liboperation/compressionOptions.h" 
#  "../liboperation/DoOperation.h" 
#  "../liboperation/DoOperation.cpp"
  "wxProgressInfoClientData.h"
  "wxProgressInfoClientData.cpp" 
#  "../liboperation/FileEnumerator.h" 
#  "../liboperation/FileEnumerator.cpp" 
  "../liboperation/utilities.h" 
  "../liboperation/utilities.cpp")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(TARGET wxWidgetsTest PROPERTY CXX_STANDARD 17)
#target_link_libraries(wxWidgetsTest PRIVATE stdc++fs)
#target_include_directories(wxWidgetsTest PRIVATE liboperation )
#target_include_directories(wxWidgetsTest PRIVATE liboperation)
target_link_libraries(wxWidgetsTest PRIVATE liboperation ${wxWidgets_LIBRARIES})
 
# TODO: Add tests and install targets if needed.
