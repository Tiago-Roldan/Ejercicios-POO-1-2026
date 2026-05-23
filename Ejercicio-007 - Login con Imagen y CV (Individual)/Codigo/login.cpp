#include "login.h"

#include "clima.h"
#include "logger.h"
#include "ui_login.h"

#include <QTimer>
#include <QVBoxLayout>

Login::Login(QWidget *parent)
    : Pantalla(parent)
    , ui(new Ui::Login)
    , m_lockTimer(new QTimer(this))
{
    ui->setupUi(this);

    m_clima = new Clima(this);
    auto *containerLayout = new QVBoxLayout(ui->weatherContainer);
    containerLayout->setContentsMargins(0, 0, 0, 0);
    containerLayout->addWidget(m_clima);

    ui->messageLabel->clear();
    ui->statusLabel->setText(QStringLiteral("Ingrese sus credenciales para continuar."));
    ui->usernameLineEdit->setText(QStringLiteral("admin"));
    ui->passwordLineEdit->setText(QStringLiteral("1234"));

    connect(ui->loginButton, &QPushButton::clicked, this, &Login::handleLogin);
    connect(m_lockTimer, &QTimer::timeout, this, &Login::updateLockState);
}

Login::~Login()
{
    delete ui;
}

void Login::aplicarConfiguracion(const AppConfig &config)
{
    m_config = config;
    m_clima->aplicarConfiguracion(config);
}

void Login::actualizarContenido()
{
    m_clima->actualizarContenido();
}

void Login::setLoadingState(bool loading, const QString &message)
{
    ui->loginButton->setEnabled(!loading && m_remainingLockSeconds == 0);
    ui->usernameLineEdit->setEnabled(!loading && m_remainingLockSeconds == 0);
    ui->passwordLineEdit->setEnabled(!loading && m_remainingLockSeconds == 0);
    ui->statusLabel->setText(message.isEmpty()
                                 ? QStringLiteral("Ingrese sus credenciales para continuar.")
                                 : message);
}

void Login::handleLogin()
{
    if (m_remainingLockSeconds > 0) {
        ui->messageLabel->setText(QStringLiteral("El acceso esta bloqueado temporalmente."));
        return;
    }

    const QString user = ui->usernameLineEdit->text().trimmed();
    const QString password = ui->passwordLineEdit->text();

    if (user == QStringLiteral("admin") && password == QStringLiteral("1234")) {
        Logger::write(QStringLiteral("Login exitoso del usuario admin."));
        ui->messageLabel->setStyleSheet(QStringLiteral("color: #1f7a52;"));
        ui->messageLabel->setText(QStringLiteral("Acceso concedido. Preparando ventana principal..."));
        resetLock();
        emit loginSuccess();
        return;
    }

    ++m_failedAttempts;
    Logger::write(QStringLiteral("Intento fallido de login. Total: %1").arg(m_failedAttempts));
    ui->messageLabel->setStyleSheet(QStringLiteral("color: #bf3c3c;"));

    if (m_failedAttempts >= m_config.failedAttemptsLimit) {
        m_remainingLockSeconds = m_config.lockDurationSeconds;
        ui->messageLabel->setText(QStringLiteral("Se alcanzo el limite de intentos. Bloqueo temporal activado."));
        ui->statusLabel->setText(QStringLiteral("Espere unos segundos antes de volver a intentar."));
        ui->loginButton->setEnabled(false);
        ui->usernameLineEdit->setEnabled(false);
        ui->passwordLineEdit->setEnabled(false);
        m_lockTimer->start(1000);
        updateLockState();
        Logger::write(QStringLiteral("Bloqueo temporal del login activado."));
        return;
    }

    const int attemptsLeft = m_config.failedAttemptsLimit - m_failedAttempts;
    ui->messageLabel->setText(QStringLiteral("Usuario o clave incorrectos. Intentos restantes: %1").arg(attemptsLeft));
}

void Login::updateLockState()
{
    if (m_remainingLockSeconds <= 0) {
        resetLock();
        ui->messageLabel->setStyleSheet(QStringLiteral("color: #1f7a52;"));
        ui->messageLabel->setText(QStringLiteral("Puede volver a intentar iniciar sesion."));
        ui->statusLabel->setText(QStringLiteral("Ingrese sus credenciales para continuar."));
        Logger::write(QStringLiteral("Bloqueo temporal finalizado."));
        return;
    }

    ui->statusLabel->setText(QStringLiteral("Login bloqueado por %1 segundos.").arg(m_remainingLockSeconds));
    --m_remainingLockSeconds;
}

void Login::resetLock()
{
    m_failedAttempts = 0;
    m_remainingLockSeconds = 0;
    m_lockTimer->stop();
    ui->loginButton->setEnabled(true);
    ui->usernameLineEdit->setEnabled(true);
    ui->passwordLineEdit->setEnabled(true);
}
