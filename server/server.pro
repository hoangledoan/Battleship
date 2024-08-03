QT -= gui
QT += network

CONFIG += c++20 console
CONFIG -= app_bundle

QMAKE_CFLAGS += -Werror
QMAKE_CXXFLAGS += -Werror

DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000

SOURCES += \
    source/main.cpp \
    source/kabumm_server.cpp \
    source/connection.cpp

HEADERS += \
    source/kabumm_server.h \
    source/connection.h \
    source/message/message.h \
    source/message/kinds.h \
    source/message/concepts.h

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
