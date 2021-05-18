# CMakeList.txt: 顶层 CMake 项目文件，在此处执行全局配置
# 并包含子项目。
#
cmake_minimum_required (VERSION 3.8)

add_executable (counter "include/counter.h" "src/counter.c" "../../../glfw/deps/glad_gl.c" "src/main.c")
target_link_libraries(counter hdl4sesim preprocess lcom hdl4secell glfw)

include_directories("../../../lcom/include")
include_directories("../../hdl4sesim/include")
include_directories("../../hdl4secell/include")
include_directories("../../preprocess/include")
include_directories("../counter/include")
include_directories("../../../glfw/include")
include_directories("../../../glfw/deps")
 