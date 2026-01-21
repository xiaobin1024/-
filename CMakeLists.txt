cmake_minimum_required(VERSION 3.16)

project(network_dictionary VERSION 1.0.0 LANGUAGES CXX)

# 在最开始设置编码相关选项
if(MSVC)
    # 设置源码和执行字符集都为UTF-8
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    # 确保Windows API使用Unicode版本
    add_definitions(-DUNICODE -D_UNICODE)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 查找Qt6组件 - 修改：添加Core模块
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Core)

# 设置输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 包含路径
include_directories(${CMAKE_SOURCE_DIR}/include)

add_subdirectory(src/core)
add_subdirectory(src/network)
add_subdirectory(src/ui)
# 启用测试
option(BUILD_TESTING "Build tests" ON)
if(BUILD_TESTING)
   enable_testing()
   add_subdirectory(src/tests)
endif()


