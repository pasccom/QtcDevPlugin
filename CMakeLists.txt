cmake_minimum_required(VERSION 3.16)

project(QtcDevPlugin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(QtCreator COMPONENTS Core ProjectExplorer QmakeProjectManager REQUIRED)
find_package(Qt6 COMPONENTS Widgets REQUIRED LinguistTools)

option(WITH_TESTS "Builds with tests" NO)
if(WITH_TESTS)
  find_package(Qt6 REQUIRED COMPONENTS Test)
  # Tell CMake functions like add_qtc_plugin about the QtTest component.
  set(IMPLICIT_DEPENDS Qt::Test)
endif()

option(USE_USER_DESTDIR "Install in user config directory" YES)
if (USE_USER_DESTDIR)
  if (WIN32)
    set(USER_APP_NAME qtcreator)
    set(USER_DATA_BASE "$ENV{APPDATA}")
    if (NOT USER_DATABASE)
      set(USER_DATA_BASE "$ENV{USERPROFILE}\Local Settings\Application Data")
    endif()
    set(USER_DESTDIR_BASE "$ENV{USERPROFILE}\Local Settings\Application Data")
  elseif (APPLE)
    set(USER_APP_NAME "Qt Creator")
    set(USER_DATA_BASE "$ENV{HOME}/Library/Application Support")
    set(USER_DESTDIR_BASE "$ENV{HOME}/Library/Application Support")
  else()
    set(USER_APP_NAME qtcreator)
    set(USER_DATA_BASE "$ENV{XDG_DATA_HOME}")
    if (NOT USER_DATA_BASE)
      set(USER_DATA_BASE "$ENV{HOME}/.config")
    else()
      set(USER_DATA_BASE "${USER_DATA_BASE}/data")
    endif()
    set(USER_DESTDIR_BASE "$ENV{XDG_DATA_HOME}")
    if (NOT USER_DESTDIR_BASE)
      set(USER_DESTDIR_BASE "$ENV{HOME}/.local/share/data")
    else()
      set(USER_DESTDIR_BASE "${USER_DESTDIR_BASE}/data")
    endif()
  endif()

  set(PLUGIN_DESTDIR "${USER_DESTDIR_BASE}/QtProject/${USER_APP_NAME}/plugins/${IDE_VERSION}")
  set(TRANSLATIONS_DESTDIR "${USER_DATA_BASE}/QtProject/${USER_APP_NAME}/translations")
else()
  set(PLUGIN_DESTDIR "${IDE_PLUGIN_PATH}")
  set(TRANSLATIONS_DESTDIR "${IDE_DATA_PATH}/translations")
endif()

message(STATUS "Plugin install path:       \"${PLUGIN_DESTDIR}\"")
message(STATUS "Translations install path: \"${TRANSLATIONS_DESTDIR}\"")

add_qtc_plugin(QtcDevPlugin
  PLUGIN_PATH
    "${PLUGIN_DESTDIR}"
  PLUGIN_DEPENDS
    QtCreator::Core
    QtCreator::ProjectExplorer
    QtCreator::QmakeProjectManager
  DEPENDS
    Qt::Widgets
    QtCreator::ExtensionSystem
    QtCreator::Utils
  SOURCES
    README.md
    Doxyfile
    Doxyfile.internal
    qtcreator-test.sh
    qtcdevplugin_global.h
    qtcdevpluginconstants.h
    qtcdeveloperplugin.h
    qtcdeveloperplugin.cpp
    qtcrunconfigurationfactory.h
    qtcrunconfigurationfactory.cpp
    qtcrunconfiguration.h
    qtcrunconfiguration.cpp
    qtctestrunconfiguration.h
    qtctestrunconfiguration.cpp
    pathaspect.h
    pathaspect.cpp
    Widgets/filetypevalidatinglineedit.h
    Widgets/filetypevalidatinglineedit.cpp
)

extend_qtc_plugin(QtcDevPlugin
  CONDITION WITH_TESTS
  SOURCES
    Test/testhelper.h
    Test/testhelper.cpp
    Test/qtcrunconfigurationfactorytest.h
    Test/qtcrunconfigurationfactorytest.cpp
    Test/qtcrunconfigurationtest.h
    Test/qtcrunconfigurationtest.cpp
    Test/qtcpluginrunnertest.h
    Test/qtcpluginrunnertest.cpp
  DEFINES BUILD_TESTS
    TESTS_DIR=\"${CMAKE_CURRENT_LIST_DIR}/tests\"
)

if (USE_USER_DESTDIR)
  set_target_properties(QtcDevPlugin PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
    ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
    RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_PLUGIN_PATH}"
  )
endif()


add_translation_targets(qtcdevplugin
  TS_LANGUAGES fr en
  QM_LANGUAGES fr en
  OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/${IDE_DATA_PATH}/translations"
  INSTALL_DESTINATION "${TRANSLATIONS_DESTDIR}"
)
