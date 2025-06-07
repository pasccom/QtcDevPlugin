cmake_minimum_required(VERSION 3.24)

project(ProjectTest)

find_package(Qt6 COMPONENTS Core)

add_executable(ProjectTest
  main.cpp
)

target_link_libraries(ProjectTest Qt6::Core)
