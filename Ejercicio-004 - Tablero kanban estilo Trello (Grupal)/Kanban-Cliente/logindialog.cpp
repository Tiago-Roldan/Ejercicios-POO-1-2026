#include "logindialog.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "kanbanapi.h"

LoginDialog::LoginDialog(KanbanApi *apiClient, QWidget *parent) : QDialog(parent), api(apiClient)
{
    setupUi();
    connectSignals();
}

void LoginDialog::setupUi()
{
    setWindowTitle("Login Kanban");
    setModal(true);
    setFixedSize(480, 380);

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(22, 22, 22, 22);
    mainLayout->setSpacing(12);

    auto *titleLabel = new QLabel("Kanban Colaborativo");
    titleLabel->setObjectName("AppTitle");
    auto *subtitleLabel = new QLabel("Inicia sesion para acceder al tablero.");
    subtitleLabel->setObjectName("SectionTitle");

    auto *userLabel = new QLabel("Usuario");
    usernameEdit = new QLineEdit;
    usernameEdit->setPlaceholderText("Tu usuario");
    usernameEdit->setMinimumHeight(42);
    auto *passLabel = new QLabel("Password");
    passwordEdit = new QLineEdit;
    passwordEdit->setPlaceholderText("Tu password");
    passwordEdit->setMinimumHeight(42);
    passwordEdit->setEchoMode(QLineEdit::Password);
    loginButton = new QPushButton("Ingresar");
    loginButton->setObjectName("PrimaryButton");
    loginButton->setMinimumHeight(38);
    registerButton = new QPushButton("Registrarse");
    registerButton->setMinimumHeight(38);
    statusLabel = new QLabel("Ingresa con tu usuario.");

    auto *buttonsLayout = new QHBoxLayout;
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->setSpacing(8);
    buttonsLayout->addWidget(loginButton);
    buttonsLayout->addWidget(registerButton);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addSpacing(8);
    mainLayout->addWidget(userLabel);
    mainLayout->addWidget(usernameEdit);
    mainLayout->addSpacing(34);
    mainLayout->addWidget(passLabel);
    mainLayout->addWidget(passwordEdit);
    mainLayout->addSpacing(34);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addItem(new QSpacerItem(0, 4));
    mainLayout->addWidget(statusLabel);
}

void LoginDialog::connectSignals()
{
    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::tryLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::tryRegister);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::tryLogin);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::tryLogin);
}

void LoginDialog::updateStatus(const QString &message, bool isError)
{
    statusLabel->setText(message);
    statusLabel->setStyleSheet(isError ? "color: #a00000;" : "color: #1d3557;");
}

void LoginDialog::tryLogin()
{
    if (api == nullptr)
    {
        updateStatus("Error interno de login.", true);
        return;
    }

    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();
    if (username.isEmpty() || password.isEmpty())
    {
        updateStatus("Completa usuario y password.", true);
        return;
    }

    QString errorMessage;
    if (!api->login(username, password, errorMessage))
    {
        updateStatus("Login invalido: " + errorMessage, true);
        return;
    }

    accept();
}

void LoginDialog::tryRegister()
{
    if (api == nullptr)
    {
        updateStatus("Error interno de registro.", true);
        return;
    }

    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();
    if (username.isEmpty() || password.isEmpty())
    {
        updateStatus("Completa usuario y password.", true);
        return;
    }

    QString errorMessage;
    if (!api->registerUser(username, password, errorMessage))
    {
        updateStatus("Registro invalido: " + errorMessage, true);
        return;
    }

    updateStatus("Registro exitoso. Ahora presiona Ingresar.");
}
