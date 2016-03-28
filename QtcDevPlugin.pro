DEFINES += QTCDEVPLUGIN_LIBRARY
# Build plugin into user config directory
USE_USER_DESTDIR = yes

SOURCES += \
    qtcdeveloperplugin.cpp \
    qtcrunconfigurationfactory.cpp \
    qtcrunconfiguration.cpp \
    qtctestrunconfiguration.cpp \
    Widgets/filetypevalidatinglineedit.cpp

HEADERS += \
    qtcdevplugin_global.h \
    qtcdevpluginconstants.h \
    qtcdeveloperplugin.h \
    qtcrunconfigurationfactory.h \
    qtcrunconfiguration.h \
    qtctestrunconfiguration.h \
    Widgets/filetypevalidatinglineedit.h

# Qt Creator from environment
# Set the QTC_SOURCE environment variable to override the setting here
QTCREATOR_SOURCES = $$(QTC_SOURCE)
# Set the QTC_BUILD environment variable to override the setting here
IDE_BUILD_TREE = $$(QTC_BUILD)

# Qt Creator from local pri file if it exists
exists(QtCreator.local.pri) {
    include(QtCreator.local.pri)
}

include($$QTCREATOR_SOURCES/src/qtcreatorplugin.pri)
INSTALLS =

###### Special tuning for output dir on Win32
win32 {
    MOC_DIR = ./.moc
    OBJECTS_DIR = ./.obj_win
}

###### Special tunings for DESTDIR on Win32
# Bug in QtCreator source from commit 1d5091e48f6df341dbd4436843c25afe25cbf4ce
# corrected in 7ea6b21b6d9480f6ed5975d75b2d83273856944a (included in v3.5.1)
defineTest(qtCreatorVersionLessThan) {
    # Extracts major, minor and patch numbers
    QTC_VERSION=$$split(QTCREATOR_VERSION, .)
    MAJOR_QTC_VERSION=$$member(QTC_VERSION, 0)
    MINOR_QTC_VERSION=$$member(QTC_VERSION, 1)
    PATCH_QTC_VERSION=$$member(QTC_VERSION, 2)
    MAX_VERSION=$$1
    MAX_VERSION=$$split(MAX_VERSION, .)
    MAJOR_MAX_VERSION=$$member(MAX_VERSION, 0)
    MINOR_MAX_VERSION=$$member(MAX_VERSION, 1)
    PATCH_MAX_VERSION=$$member(MAX_VERSION, 2)
    #message(QTC: M:$$MAJOR_QTC_VERSION m:$$MINOR_QTC_VERSION p:$$PATCH_QTC_VERSION)
    #message(MAX: M:$$MAJOR_MAX_VERSION m:$$MINOR_MAX_VERSION p:$$PATCH_MAX_VERSION)
    # Test if QtCreator version is smaller than MAX
    greaterThan(MAJOR_MAX_VERSION, $$MAJOR_QTC_VERSION) {
        return(true)
    }
    isEqual(MAJOR_MAX_VERSION, $$MAJOR_QTC_VERSION) {
        greaterThan(MINOR_MAX_VERSION, $$MINOR_QTC_VERSION) {
            return(true)
        }
        isEqual(MINOR_MAX_VERSION, $$MINOR_QTC_VERSION) {
            greaterThan(PATCH_MAX_VERSION, $$PATCH_QTC_VERSION) {
                return(true)
            }
        }
    }
    return(false)
}

win32 {
    !isEmpty(USE_USER_DESTDIR) {
        qtCreatorVersionLessThan(3.5.1) {
            !qtCreatorVersionLessThan(3.3.0) {
            message("Corrected bug affecting windows between verions 3.3.0 included and 3.5.1 excluded")
            DESTDIRAPPNAME = "qtcreator"
            DESTDIRBASE = "$$(LOCALAPPDATA)"
            isEmpty(DESTDIRBASE):DESTDIRBASE="$$(USERPROFILE)\Local Settings\Application Data"
            DESTDIR = "$$DESTDIRBASE/data/QtProject/$$DESTDIRAPPNAME/plugins/$$QTCREATOR_VERSION"
            }
        }
    }
}
###### Added stuff to bypass IDE_BUILD_TREE
target.path = $$DESTDIR
INSTALLS += target
DESTDIR = ./bin

unix {
    LIBS+= -L$$IDE_BUILD_TREE
    LIBS+= -L$$IDE_BUILD_TREE/plugins
} else:win32  {
    LIBS+= -L$$IDE_BUILD_TREE/bin
}
