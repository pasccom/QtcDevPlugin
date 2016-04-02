DEFINES += QTCPLUGINTEST_LIBRARY
# Build plugin into user config directory
USE_USER_DESTDIR = yes

SOURCES += \
    qtcplugintest.cpp

HEADERS += \
    qtcplugintest_global.h \
    qtcplugintestconstants.h \
    qtcplugintest.h

# Qt Creator linking

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(../../QtCreator.local.pri) {
    include(../../QtCreator.local.pri)
}

# If the plugin can be depended upon by other plugins, this code needs to be outsourced to
# <dirname>_dependencies.pri, where <dirname> is the name of the directory containing the
# plugin's sources.

QTC_PLUGIN_NAME = QtcPluginTest1
QTC_LIB_DEPENDS += \
    # nothing here at this time

QTC_PLUGIN_DEPENDS += \
    coreplugin

QTC_PLUGIN_RECOMMENDS += \
    # optional plugin dependencies. nothing here at this time

###### End _dependencies.pri contents ######

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)

# Added stuff to bypass IDE_BUILD_TREE
DESTDIR = ./bin

unix {
    LIBS+= -L$$IDE_BUILD_TREE
    LIBS+= -L$$IDE_BUILD_TREE/plugins
} else:win32  {
    LIBS+= -L$$IDE_BUILD_TREE/bin
}
