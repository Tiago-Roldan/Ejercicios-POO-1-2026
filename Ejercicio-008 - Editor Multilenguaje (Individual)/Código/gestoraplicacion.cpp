#include "gestoraplicacion.h"

#include "editorprincipal.h"
#include "logger.h"
#include "login.h"
#include "modobloqueado.h"
#include "pantalla.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

namespace {
QString resolverRutaConfig()
{
    QDir directorio(QCoreApplication::applicationDirPath());
    for (int i = 0; i < 6; ++i) {
        const QString candidata = directorio.filePath("config.ini");
        if (QFileInfo::exists(candidata)) {
            return candidata;
        }

        if (!directorio.cdUp()) {
            break;
        }
    }

    return QDir(QCoreApplication::applicationDirPath()).filePath("config.ini");
}
}

GestorAplicacion::GestorAplicacion(QObject *parent)
    : QObject(parent)
{
    const QString base = QCoreApplication::applicationDirPath();
    configuracion_ = Configuracion::cargarDesdeArchivo(resolverRutaConfig());
    logger_ = new Logger(QDir(base).filePath("logs/eventos.log"));
    logger_->registrar("Sistema | Aplicacion iniciada");
}

GestorAplicacion::~GestorAplicacion()
{
    delete pantallaActual_;
    delete logger_;
}

void GestorAplicacion::iniciar()
{
    mostrarPantalla(new Login(configuracion_, logger_));
}

void GestorAplicacion::mostrarPantalla(Pantalla *nuevaPantalla, bool fullScreen)
{
    if (pantallaActual_) {
        pantallaActual_->hide();
        pantallaActual_->deleteLater();
    }

    pantallaActual_ = nuevaPantalla;
    conectarPantalla(pantallaActual_);
    if (fullScreen) {
        pantallaActual_->showFullScreen();
    } else {
        pantallaActual_->show();
    }
}

void GestorAplicacion::conectarPantalla(Pantalla *pantalla)
{
    connect(pantalla, &Pantalla::solicitarLogin, this, [this]() {
        mostrarPantalla(new Login(configuracion_, logger_));
    });
    connect(pantalla, &Pantalla::solicitarEditor, this, [this]() {
        mostrarPantalla(new EditorPrincipal(configuracion_, logger_), true);
    });
    connect(pantalla, &Pantalla::solicitarBloqueo, this, [this]() {
        mostrarPantalla(new ModoBloqueado(configuracion_, logger_));
    });
}
