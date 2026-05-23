#include "registerwidget.h"
#include "ui_registerwidget.h"
#include <QCryptographicHash>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>

RegisterWidget::RegisterWidget(QWidget *parent) : QWidget(parent), ui(new Ui::RegisterWidget)
{
    ui->setupUi(this);
    ui->passwordEdit->setEchoMode(QLineEdit::Password);
    ui->confirmPasswordEdit->setEchoMode(QLineEdit::Password);
    connect(ui->registerButton, &QPushButton::clicked, this, &RegisterWidget::onRegisterClicked);
    connect(ui->backButton, &QPushButton::clicked, this, &RegisterWidget::onBackClicked);
}

RegisterWidget::~RegisterWidget()
{
    delete ui;
}

void RegisterWidget::onRegisterClicked()
{
    const QString firstName = ui->firstNameEdit->text().trimmed();
    const QString lastName = ui->lastNameEdit->text().trimmed();
    const QString username = ui->usernameEdit->text().trimmed();
    const QString password = ui->passwordEdit->text();
    const QString confirmPassword = ui->confirmPasswordEdit->text();

    if (firstName.isEmpty() || lastName.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        ui->statusLabel->setText("Complete todos los campos.");
        return;
    }

    if (password != confirmPassword) {
        ui->statusLabel->setText("Las contraseñas no coinciden.");
        return;
    }

    if (usernameExists(username)) {
        ui->statusLabel->setText("El nombre de usuario ya existe.");
        return;
    }

    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen() && !db.open()) {
        ui->statusLabel->setText("No se pudo conectar a la base de datos.");
        return;
    }

    QSqlQuery query(db);
    query.prepare("INSERT INTO users (first_name, last_name, username, password_hash) VALUES (:first_name, :last_name, :username, :password_hash)");
    query.bindValue(":first_name", firstName);
    query.bindValue(":last_name", lastName);
    query.bindValue(":username", username);
    query.bindValue(":password_hash", hashPassword(password));

    if (!query.exec()) {
        ui->statusLabel->setText("Error al guardar el usuario.");
        return;
    }

    ui->statusLabel->setText("Registro exitoso. Inicie sesión.");
    emit registrationDone();
}

void RegisterWidget::onBackClicked()
{
    emit requestLogin();
}

QString RegisterWidget::hashPassword(const QString &password) const
{
    return QString(QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}

bool RegisterWidget::usernameExists(const QString &username) const
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen() && !db.open()) {
        return false;
    }

    QSqlQuery query(db);
    query.prepare("SELECT COUNT(1) FROM users WHERE username = :username");
    query.bindValue(":username", username);
    if (!query.exec()) {
        return false;
    }
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}
