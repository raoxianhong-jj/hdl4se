# CMakeList.txt: preprocess 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# 将源代码添加到此项目的可执行文件。
add_library (preprocess STATIC "filestack.h" "preprocess.c" "preprocess.h" "stringlist.h" "verilog_preprocess.h" "verilog_preprocess.c")
include_directories("../lcom/include")