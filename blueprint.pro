QT += quick

CONFIG += c++17

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/backend.cpp \
    src/control_device.cpp \
    src/mainwindow.cpp \
    src/control_midi.cpp \
    src/tinamicomm.cpp

RESOURCES += src/qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    3rdparty/hidapi/hidapi.h \
    src/backend.h \
    src/control_device.h \
    src/defs.h \
    src/control_midi.h \
    src/tinamicomm.h

INCLUDEPATH += ./3rdparty

#-------------------------------------------------
# Add the correct HIDAPI library according to what
# OS is being used
#-------------------------------------------------
win32: LIBS += $$PWD/3rdparty/hidapi/win32/libhidapi.a
macx: LIBS += $$PWD/3rdparty/hidapi/mac/libhidapi.a
unix: !macx: LIBS += $$PWD/3rdparty/hidapi/gnu_linux/libhidapi-libusb.a

#-------------------------------------------------
# Make sure to add the required libraries or
# frameoworks for the hidapi to work depending on
# what OS is being used
#-------------------------------------------------
win32: LIBS += -lSetupAPI
macx: LIBS += -framework CoreFoundation -framework IOkit
unix: !macx: LIBS += -lusb-1.0

macx{
    CONFIG(debug, debug|release) {
        DEST_DIRECTORY = $$PWD/mac/debug
    }
    CONFIG(release, debug|release) {
        DEST_DIRECTORY = $$PWD/mac/release
    }
}
unix:!mac{
    CONFIG(debug, debug|release) {
        DEST_DIRECTORY = $$PWD/linux/debug
    }
    CONFIG(release, debug|release) {
        DEST_DIRECTORY = $$PWD/linux/release
    }
}
win32{
    CONFIG(debug, debug|release) {
        DEST_DIRECTORY = $$PWD/win32/debug
    }
    CONFIG(release, debug|release) {
        DEST_DIRECTORY = $$PWD/win32/release
    }
}

MOC_DIR     += $${DEST_DIRECTORY}/generated/mocs
UI_DIR      += $${DEST_DIRECTORY}/generated/uis
RCC_DIR     += $${DEST_DIRECTORY}/generated/rccs
OBJECTS_DIR += $${DEST_DIRECTORY}/generated/objs
DLLDESTDIR = $${DEST_DIRECTORY}
DESTDIR    = $${DEST_DIRECTORY}

DISTFILES += \
    3rdparty/hidapi/gnu_linux/libhidapi-libusb.a \
    3rdparty/hidapi/win32/libhidapi.a
