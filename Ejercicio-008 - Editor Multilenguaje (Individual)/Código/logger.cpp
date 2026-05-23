#include "logger.h"

#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>

Logger::Logger(const QString &rutaArchivo)
    : rutaArchivo_(rutaArchivo)
{
    const QFileInfo info(rutaArchivo_);
    QDir().mkpath(info.absolutePath());
}

void Logger::registrar(const QString &accion)
{
    QFile archivo(rutaArchivo_);
    if (!archivo.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream salida(&archivo);
    salida << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
           << " | " << accion << "\n";
}
