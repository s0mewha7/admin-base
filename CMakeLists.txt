cmake_minimum_required(VERSION 3.28)
project(admin-base)

set(CMAKE_CXX_STANDARD 23)

find_package(Qt6 REQUIRED COMPONENTS Widgets Sql)
qt_standard_project_setup()

qt_add_executable(admin-base
        main.cpp
        src/mainwindow.h
        src/mainwindow.cpp
        src/database.h
        src/database.cpp
)

target_include_directories(admin-base PRIVATE
        include
        src
)

target_link_libraries(admin-base PRIVATE
        Qt6::Widgets
        Qt6::Sql
)


