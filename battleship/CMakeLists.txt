cmake_minimum_required(VERSION 3.16)

project(kabumm_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror")

find_package(Qt6 REQUIRED COMPONENTS Core Network Widgets )
qt_standard_project_setup()

set(CMAKE_AUTORCC ON)
set(QT_USE_QTMULTIMEDIA TRUE)

qt_add_executable(kabumm
        main.cpp
        kabumm.cpp
        kabumm.h
        state.cpp
        state.h
        ui/mainwindow.cpp
        ui/mainwindow.h
        ui/zustand.h
        ui/mainwindow.ui
        ui/GUI/Img_file.qrc
        net/network.h
        net/network.cpp
        net/message.cpp
        net/message.h
        net/kinds.h
        net/kinds.cpp
        logic/game.cpp
        logic/game.hpp
)

target_include_directories(kabumm PRIVATE .)

target_link_libraries(kabumm PRIVATE Qt6::Core)
target_link_libraries(kabumm PRIVATE Qt6::Network)
target_link_libraries(kabumm PRIVATE Qt6::Widgets)
