cmake_minimum_required(VERSION 3.5)

project(SymbolicCalculator)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)

# 手动指定SymEngine库的路径
include_directories(/usr/local/include/symengine)
link_directories(/usr/local/lib)

add_executable(SymbolicCalculator
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

target_link_libraries(SymbolicCalculator Qt5::Widgets symengine gmp)