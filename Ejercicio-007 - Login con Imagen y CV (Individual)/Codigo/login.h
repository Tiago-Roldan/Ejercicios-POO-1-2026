#ifndef LOGIN_H
#define LOGIN_H

#include "appconfig.h"
#include "pantalla.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class Login;
}
QT_END_NAMESPACE

class Clima;
class QTimer;

class Login : public Pantalla
{
    Q_OBJECT

public:
    explicit Login(QWidget *parent = nullptr);
    ~Login() override;

    void aplicarConfiguracion(const AppConfig &config) override;
    void actualizarContenido() override;
    void setLoadingState(bool loading, const QString &message = QString());

signals:
    void loginSuccess();

private slots:
    void handleLogin();
    void updateLockState();

private:
    void resetLock();

    Ui::Login *ui = nullptr;
    Clima *m_clima = nullptr;
    AppConfig m_config;
    int m_failedAttempts = 0;
    int m_remainingLockSeconds = 0;
    QTimer *m_lockTimer = nullptr;
};

#endif
