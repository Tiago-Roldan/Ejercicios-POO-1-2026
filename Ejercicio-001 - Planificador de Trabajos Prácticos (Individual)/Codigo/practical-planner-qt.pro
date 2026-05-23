QT += core gui widgets

CONFIG += c++17

TARGET = practical-planner-qt
TEMPLATE = app

SOURCES += \
    src/auth/authmanager.cpp \
    src/main.cpp \
    src/storage/storagemanager.cpp \
    src/ui/logindialog.cpp \
    src/ui/mainwindow.cpp \
    src/ui/registerdialog.cpp \
    src/ui/taskdialog.cpp

HEADERS += \
    src/auth/authmanager.h \
    src/models/historyentry.h \
    src/models/practicalwork.h \
    src/models/user.h \
    src/planneroptions.h \
    src/storage/storagemanager.h \
    src/ui/logindialog.h \
    src/ui/mainwindow.h \
    src/ui/registerdialog.h \
    src/ui/taskdialog.h

RESOURCES += \
    src/resources/resources.qrc
