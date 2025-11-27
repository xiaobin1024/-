cmake_minimum_required(VERSION 3.16)

project(core LANGUAGES CXX)

#启动自动处理Qt元对象系统
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#查找Qt6组件
find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core)


#创建静态库而不是共享库
add_library(core STATIC
  core.cpp
  core.h
  message.h
  message.cpp
  base_manager.h base_manager.cpp
  app_config.h
  utils.h
  utils.cpp
)

#链接Qt Core模块
target_link_libraries(core PRIVATE Qt${QT_VERSION_MAJOR}::Core)

#设置包含目录
target_include_directories(core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR})


