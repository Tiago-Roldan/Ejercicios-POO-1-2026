QT += core gui widgets

CONFIG += c++17 console
CONFIG -= app_bundle

TEMPLATE = app
TARGET = EditorMultilenguaje

SOURCES += \
    configuracion.cpp \
    editorcodigo.cpp \
    editorprincipal.cpp \
    gestoraplicacion.cpp \
    logger.cpp \
    login.cpp \
    main.cpp \
    modobloqueado.cpp \
    pantalla.cpp \
    perfildatos.cpp \
    validadorsintaxis.cpp

HEADERS += \
    configuracion.h \
    editorcodigo.h \
    editorprincipal.h \
    gestoraplicacion.h \
    logger.h \
    login.h \
    modobloqueado.h \
    pantalla.h \
    perfildatos.h \
    validadorsintaxis.h

win32 {
    QMAKE_POST_LINK += copy /Y \"$$PWD\\config.ini\" \"$$OUT_PWD\\config.ini\" $$escape_expand(\\n\\t)
    QMAKE_POST_LINK += copy /Y \"$$PWD\\CV_Carlos_Ruiz.pdf\" \"$$OUT_PWD\\CV_Carlos_Ruiz.pdf\"
}
