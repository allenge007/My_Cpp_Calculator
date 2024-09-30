cmake_minimum_required(VERSION 3.10)

project(SymbolicCalculator)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(SymEngine REQUIRED)
find_package(Python3 COMPONENTS Interpreter Development REQUIRED)

# 添加自动生成步骤
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# 添加可执行文件
add_executable(SymbolicCalculator
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

# 链接库
target_link_libraries(SymbolicCalculator Qt5::Widgets symengine gmp ${Python3_LIBRARIES} ${SymEngine_LIBRARIES})

# 添加 Python 包含路径
target_include_directories(SymbolicCalculator PRIVATE ${Python3_INCLUDE_DIRS})

# 设置生成的安装包类型
set(CPACK_PACKAGE_NAME "SymbolicCalculator")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_CONTACT "2438745244@qq.com")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "A symbolic calculator application")
set(CPACK_PACKAGE_VENDOR "SYSU")
set(CPACK_PACKAGE_EXECUTABLES "SymbolicCalculator" "SymbolicCalculator")
set(CPACK_GENERATOR "TGZ;ZIP")

# 设置安装目录
install(TARGETS SymbolicCalculator DESTINATION bin)
install(FILES mainwindow.ui DESTINATION share/SymbolicCalculator)

# 启用 CPack
include(CPack)