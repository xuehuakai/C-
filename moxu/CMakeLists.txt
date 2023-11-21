cmake_minimum_required(VERSION 3.12)
project(moxu)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -rdynamic -O3 -g -std=c++11 -Wno-deprecated -Werror -Wno-unused-function")

# 设置可执行文件的输出路径
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# 设置库文件的输出路径
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# 设置静态库文件的输出路径
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)

# 全局设置链接目录
link_directories(${CMAKE_BINARY_DIR}/build)


include_directories(.)
include_directories(../../yaml-cpp/include)

# 设置CMAKE_PREFIX_PATH
#set(CMAKE_PREFIX_PATH ../../yaml-cpp)

# 查找yaml-cpp库
find_library(YAMLCPP libyaml-cpp.a)

# 添加源文件
set(LIB_SRC 
    src/log.cpp
    src/util.cpp
    src/config.cpp
)

# 生成共享库
add_library(moxu SHARED ${LIB_SRC})



# 生成可执行文件 test
add_executable(test test/test.cpp)
add_dependencies(test moxu)
target_link_libraries(test moxu -L/a/workspace/yaml-cpp -lyaml-cpp )

# 生成可执行文件 test_config
add_executable(test_config test/test_config.cpp)
add_dependencies(test_config moxu)
target_link_libraries(test_config moxu -L/a/workspace/yaml-cpp -lyaml-cpp )
# 设置输出路径
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

