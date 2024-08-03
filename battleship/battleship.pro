
QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    main.cpp \
    state.cpp \
    kabumm.cpp \
    ui/mainwindow.cpp \
    net/kinds.cpp \
    net/message.cpp \
    net/network.cpp \
    logic/game.cpp

HEADERS += \
    kabumm.h \
    state.h \
    ui/mainwindow.h \
    ui/zustand.h \
    net/kinds.h \
    net/message.h \
    net/network.h \
    logic/game.hpp \

FORMS += \
    ui/mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    ui/GUI/Img_file.qrc

RC_ICONS += ui/GUI/Img_file/app.ico

DISTFILES += \
    README.md

