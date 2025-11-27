cmake_minimum_required(VERSION 3.16)

project(network_dictionary VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#查找Qt6组件
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Network)

#设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

#包含路径
include_directories(${CMAKE_SOURCE_DIR}/include)


add_subdirectory(src/core)

#启用测试
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
enable_testing()
add_subdirectory(src/tests)
endif()

