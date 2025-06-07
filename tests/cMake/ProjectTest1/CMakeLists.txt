cmake_minimum_required(VERSION 3.24)

project(ProjectTest1)

find_package(Qt6 COMPONENTS Core)

add_executable(ProjectTest1
  main.cpp
)

target_link_libraries(ProjectTest1 Qt6::Core)
