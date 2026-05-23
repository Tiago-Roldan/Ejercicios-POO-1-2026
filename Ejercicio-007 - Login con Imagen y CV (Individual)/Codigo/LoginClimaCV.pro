QT += core gui widgets network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
TEMPLATE = app
TARGET = LoginClimaCV

SOURCES += \
    appcontroller.cpp \
    backgrounddownloader.cpp \
    clima.cpp \
    configmanager.cpp \
    logger.cpp \
    login.cpp \
    main.cpp \
    pantalla.cpp \
    ventana.cpp

HEADERS += \
    appconfig.h \
    appcontroller.h \
    backgrounddownloader.h \
    clima.h \
    configmanager.h \
    logger.h \
    login.h \
    pantalla.h \
    ventana.h

FORMS += \
    login.ui \
    ventana.ui
