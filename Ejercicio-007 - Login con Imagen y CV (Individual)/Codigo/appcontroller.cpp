#include "appcontroller.h"

#include "backgrounddownloader.h"
#include "configmanager.h"
#include "logger.h"
#include "login.h"
#include "ventana.h"

AppController::AppController(QObject *parent)
    : QObject(parent)
{
}

void AppController::start()
{
    m_config = ConfigManager::load();
    Logger::setLogFilePath(m_config.logFilePath);
    Logger::write(QStringLiteral("Aplicacion iniciada."));

    m_login = new Login();
    m_ventana = new Ventana();
    m_backgroundDownloader = new BackgroundDownloader(this);

    m_login->aplicarConfiguracion(m_config);
    m_ventana->aplicarConfiguracion(m_config);
    m_backgroundDownloader->applyConfiguration(m_config);

    connect(m_login, &Login::loginSuccess, this, &AppController::onLoginSuccess);
    connect(m_backgroundDownloader, &BackgroundDownloader::backgroundReady,
            this, &AppController::onBackgroundReady);
    connect(m_backgroundDownloader, &BackgroundDownloader::progressMessage,
            this, &AppController::onProgressMessage);

    m_login->actualizarContenido();
    m_login->show();
}

void AppController::onLoginSuccess()
{
    m_login->setLoadingState(true, QStringLiteral("Usuario valido. Esperando la imagen principal..."));
    m_backgroundDownloader->prepareBackground();
}

void AppController::onBackgroundReady(const QPixmap &pixmap, bool offlineMode)
{
    m_ventana->setBackground(pixmap, offlineMode);
    m_ventana->showFullScreen();
    m_ventana->actualizarContenido();
    m_login->close();
    Logger::write(QStringLiteral("Ventana principal abierta en pantalla completa."));
}

void AppController::onProgressMessage(const QString &message)
{
    m_login->setLoadingState(true, message);
}
