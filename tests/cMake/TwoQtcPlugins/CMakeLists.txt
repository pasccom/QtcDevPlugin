cmake_minimum_required(VERSION 3.24)

project(TwoQtcPlugins)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QtCreator COMPONENTS Core REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_qtc_plugin(QtcPluginTest1
  PLUGIN_PATH
    "$<PATH:GET_PARENT_PATH,${CMAKE_CURRENT_LIST_DIR}>/plugins"
  PLUGIN_DEPENDS
    QtCreator::Core
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    qtcplugintest1constants.h
    qtcplugintest1_global.h
    qtcplugintest1.h
    qtcplugintest1.cpp
)

add_qtc_plugin(QtcPluginTest2
  PLUGIN_PATH
    "$<PATH:GET_PARENT_PATH,${CMAKE_CURRENT_LIST_DIR}>/plugins"
  PLUGIN_DEPENDS
    QtCreator::Core
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    qtcplugintest2constants.h
    qtcplugintest2_global.h
    qtcplugintest2.h
    qtcplugintest2.cpp
)
