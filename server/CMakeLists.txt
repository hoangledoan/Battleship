cmake_minimum_required(VERSION 3.25)

project(kabumm_server VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS_DEBUG "-Wall -Werror")

find_package(Qt6 REQUIRED COMPONENTS Core Network)
qt_standard_project_setup()

qt_add_executable(kabumm_server
        source/main.cpp
        source/kabumm_server.cpp
        source/connection.cpp
        source/message/message.h
        source/message/kinds.h
        source/message/concepts.h
)

target_link_libraries(kabumm_server PRIVATE Qt6::Core)
target_link_libraries(kabumm_server PRIVATE Qt6::Network)