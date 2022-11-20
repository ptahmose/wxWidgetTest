# CMakeList.txt : CMake project for wxWidgetsTest, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable(wxWidgetsTest2 WIN32 
  "webView.cpp" 
  "webView.h"  
  "htmlpage.h" 
  "htmlpage.cpp"
  "customEvents.cpp"
  "customEvents.h")

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set_property(TARGET wxWidgetsTest2 PROPERTY CXX_STANDARD 17)
#target_link_libraries(wxWidgetsTest PRIVATE stdc++fs)
add_dependencies(wxWidgetsTest2 rapidjson)
target_include_directories(wxWidgetsTest2 PRIVATE ${RAPIDJSON_INCLUDE_DIR})
target_link_libraries(wxWidgetsTest2 PRIVATE liboperation ${wxWidgets_LIBRARIES})
 
# TODO: Add tests and install targets if needed.
