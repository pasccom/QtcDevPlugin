cmake_minimum_required(VERSION 3.24)

project(ProjectTest2)

find_package(Qt6 COMPONENTS Core)

add_executable(ProjectTest2
  main.cpp
)

target_link_libraries(ProjectTest2 Qt6::Core)
