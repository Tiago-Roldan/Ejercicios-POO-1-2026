#ifndef APPCONTROLLER_H
#define APPCONTROLLER_H

#include "appconfig.h"

#include <QObject>

class BackgroundDownloader;
class Login;
class QPixmap;
class Ventana;

class AppController : public QObject
{
    Q_OBJECT

public:
    explicit AppController(QObject *parent = nullptr);
    void start();

private slots:
    void onLoginSuccess();
    void onBackgroundReady(const QPixmap &pixmap, bool offlineMode);
    void onProgressMessage(const QString &message);

private:
    AppConfig m_config;
    Login *m_login = nullptr;
    Ventana *m_ventana = nullptr;
    BackgroundDownloader *m_backgroundDownloader = nullptr;
};

#endif
