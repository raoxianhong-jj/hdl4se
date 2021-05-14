
cmake_minimum_required (VERSION 3.8)
add_executable (testpreprocess "testpreprocess.c")
TARGET_LINK_LIBRARIES(testpreprocess lcom preprocess)
include_directories("../lcom/include")
include_directories("../preprocess")