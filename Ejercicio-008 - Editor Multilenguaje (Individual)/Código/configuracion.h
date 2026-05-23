#ifndef CONFIGURACION_H
#define CONFIGURACION_H

#include <QString>

struct Configuracion {
    QString usuarioInicial = "admin";
    QString passwordInicial = "1234";
    int tiempoBloqueoSegundos = 20;
    QString lenguajePorDefecto = "Python";
    QString rutaExportacion = ".";
    QString rutaCv = "CV_Carlos_Ruiz.pdf";

    static Configuracion cargarDesdeArchivo(const QString &rutaArchivo);
};

#endif
