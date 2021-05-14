
cmake_minimum_required (VERSION 3.8)
add_executable (testpreprocess "testpreprocess.c")
target_link_libraries(testpreprocess preprocess lcom)

include_directories("../lcom/include")
include_directories("../preprocess")
