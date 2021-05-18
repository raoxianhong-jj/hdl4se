# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

add_library (hdl4secell STATIC 
  "include/hdl4secell.h" 
  "src/hdl4secell.c"
)

include_directories("../../lcom/include")
include_directories("../hdl4secell/include")



