#include "loginwidget.h"
#include "ui_loginwidget.h"
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QFile>
#include <QTextStream>
#include <QCoreApplication>

LoginWidget::LoginWidget(QWidget *parent) : QWidget(parent), ui(new Ui::LoginWidget)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    connect(ui->loginButton, &QPushButton::clicked, this, &LoginWidget::onLoginClicked);
    connect(ui->registerButton, &QPushButton::clicked, this, &LoginWidget::onRegisterClicked);
}

LoginWidget::~LoginWidget()
{
    delete ui;
}

void LoginWidget::onLoginClicked()
{
    const QString username = ui->usernameEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();
    if (username.isEmpty() || password.isEmpty()) {
        ui->statusLabel->setText("Complete todos los campos.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen() && !db.open()) {
        ui->statusLabel->setText("No se pudo conectar a la base de datos.");
        return;
    }

    const QString hashed = hashPassword(password);
    QSqlQuery query(db);
    query.prepare("SELECT password_hash FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        ui->statusLabel->setText("Error al consultar la base de datos.");
        logAttempt(username, false);
        return;
    }

    if (query.next()) {
        const QString storedHash = query.value(0).toString();
        if (storedHash == hashed) {
            ui->statusLabel->setText("Ingreso exitoso.");
            logAttempt(username, true);
            emit loginSuccess(username);
            return;
        }
    }

    ui->statusLabel->setText("Usuario o contraseña incorrectos.");
    logAttempt(username, false);
}

void LoginWidget::onRegisterClicked()
{
    emit requestRegister();
}

QString LoginWidget::hashPassword(const QString &password) const
{
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

void LoginWidget::logAttempt(const QString &username, bool success)
{
    const QString logFile = QCoreApplication::applicationDirPath() + "/accesos.log";
    QFile file(logFile);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream out(&file);
    const QString timestamp = QDateTime::currentDateTime().toString(Qt::ISODate);
    out << timestamp << " - " << username << " - " << (success ? "EXITO" : "FALLA") << "\n";
    file.close();
}
