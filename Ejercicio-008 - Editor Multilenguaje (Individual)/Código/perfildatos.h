#ifndef PERFILDATOS_H
#define PERFILDATOS_H

#include <QString>
#include <QStringList>

struct PerfilDatos {
    QString nombre;
    QString titular;
    QString descripcion;
    QStringList habilidades;
    QString contacto;
    QString rutaPdf;

    static PerfilDatos cargarDesdePdf(const QString &rutaPdf);
};

#endif
