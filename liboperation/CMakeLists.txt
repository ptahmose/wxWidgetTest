ADD_LIBRARY( liboperation STATIC
              "compressionOptions.h" 
              "DoOperation.h" 
              "DoOperation.cpp"
              "FileEnumerator.h" 
              "FileEnumerator.cpp")

set_property(TARGET liboperation PROPERTY CXX_STANDARD 17)
target_include_directories(liboperation  PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})