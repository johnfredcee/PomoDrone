cmake_minimum_required(VERSION 3.16)
project(PomoDrone)

set(CMAKE_PREFIX_PATH H:\\Dev\\sdks\\qt\\6.8.2\\msvc2022_64)

set(CMAKE_CXX_STANDARD 17)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

set(CMAKE_AUTOMOC ON) 
set(CMAKE_AUTORCC ON)

add_executable(PomoDrone
    main.cpp 
    mainwindow.cpp 
    mainwindow.h 
    resources.qrc
)


set_target_properties(PomoDrone PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(PomoDrone PRIVATE Qt6::Core Qt6::Gui  Qt6::Widgets)