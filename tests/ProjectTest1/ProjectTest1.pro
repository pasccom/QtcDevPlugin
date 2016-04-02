TEMPLATE = app

CONFIG += console c++11
CONFIG -= app_bundle

SOURCES += main.cpp

MOC_DIR = .moc
OBJECTS_DIR = .obj

CONFIG(release): DESTDIR=./release
else: DESTIR=./debug
