
# DEFINES += LICENSE_NOT_REQUIRED
DEFINES += LICENSE_NOT_REQUIRED

greaterThan(QT_MAJOR_VERSION, 4) {
	QT += widgets
}

QMAKE_EXTRA_TARGETS += version

QT += network
QT -= testlib

DESTDIR = out
MOC_DIR = tmp/moc
UI_DIR = tmp/ui

INCLUDEPATH += tmp/ui

QMAKE_CXXFLAGS += -std=c++11


CONFIG(debug, debug|release) {
        TARGET = workclock.debug
        OBJECTS_DIR = tmp/debug
        RCC_DIR = tmp/debug/rcc
}
CONFIG(release, debug|release) {
        TARGET = workclock
        OBJECTS_DIR = tmp/release
        RCC_DIR = tmp/release/rcc
        QT -= testlib
}

unix {
}

# Define how to create version.h
win32 {
	version.target = version.h
#	equals(QMAKE_CXX, g++):version.commands = C:\\Program Files\\TortoiseSVN\\bin\\SubWCRev.exe ./.. "version.tmpl" "version.h"
	equals(QMAKE_CXX, g++):version.commands = versionit.bat
	!equals(QMAKE_CXX, g++):version.commands = ./subwcrev.sh "version.tmpl" "version.h"
	version.depends = FORCE
}



FORMS += clockit.ui \
    preferences.ui \
    viewentries.ui \

HEADERS = clockit.h \
    preferences.h \
    viewentries.h \

SOURCES = main.cpp \
    clockit.cpp \
    preferences.cpp \
    viewentries.cpp \

RESOURCES = workclock.qrc

