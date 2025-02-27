### CMakeLists.txt

cmake_minimum_required(VERSION 3.10)
project(EventDriven)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 添加源文件
set(SOURCES
    test/test1.cpp
    include/timer_manager.cpp
)

# 包含头文件目录
include_directories(include)

# 添加可执行文件
add_executable(test1 ${SOURCES})

# 链接线程库
find_package(Threads REQUIRED)
target_link_libraries(test1 Threads::Threads)
