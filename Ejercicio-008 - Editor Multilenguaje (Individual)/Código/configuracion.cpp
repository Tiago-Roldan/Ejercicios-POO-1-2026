#include "configuracion.h"

#include <QDir>
#include <QFileInfo>
#include <QSettings>

Configuracion Configuracion::cargarDesdeArchivo(const QString &rutaArchivo)
{
    Configuracion configuracion;
    const QFileInfo info(rutaArchivo);
    QSettings settings(rutaArchivo, QSettings::IniFormat);

    configuracion.usuarioInicial =
        settings.value("auth/usuarioInicial", configuracion.usuarioInicial).toString();
    configuracion.passwordInicial =
        settings.value("auth/passwordInicial", configuracion.passwordInicial).toString();
    configuracion.tiempoBloqueoSegundos =
        settings.value("auth/tiempoBloqueoSegundos", configuracion.tiempoBloqueoSegundos).toInt();
    configuracion.lenguajePorDefecto =
        settings.value("editor/lenguajePorDefecto", configuracion.lenguajePorDefecto).toString();
    configuracion.rutaExportacion =
        settings.value("editor/rutaExportacion", configuracion.rutaExportacion).toString();
    configuracion.rutaCv = settings.value("perfil/rutaCv", configuracion.rutaCv).toString();

    const QDir baseDir = info.absoluteDir().exists() ? info.absoluteDir() : QDir::current();
    if (QDir::isRelativePath(configuracion.rutaExportacion)) {
        configuracion.rutaExportacion = baseDir.absoluteFilePath(configuracion.rutaExportacion);
    }
    if (QDir::isRelativePath(configuracion.rutaCv)) {
        configuracion.rutaCv = baseDir.absoluteFilePath(configuracion.rutaCv);
    }

    return configuracion;
}
