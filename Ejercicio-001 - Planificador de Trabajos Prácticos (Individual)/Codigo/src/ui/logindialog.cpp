#include "logindialog.h"

#include <QDialogButtonBox>
#include <QFont>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../auth/authmanager.h"
#include "registerdialog.h"

LoginDialog::LoginDialog(AuthManager *authManager, QWidget *parent)
    : QDialog(parent)
    , authManager(authManager)
    , usernameEdit(new QLineEdit(this))
    , passwordEdit(new QLineEdit(this))
    , feedbackLabel(new QLabel(this))
{
    setWindowTitle("Ingreso al planificador");
    setModal(true);
    resize(420, 240);

    auto *mainLayout = new QVBoxLayout(this);

    auto *titleLabel = new QLabel("Practical Planner Qt", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(titleFont.pointSize() + 4);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);

    auto *subtitleLabel = new QLabel(
        "Inicia sesion con un usuario local guardado en JSON. "
        "La sesion queda recordada por 5 minutos en este equipo.",
        this);
    subtitleLabel->setWordWrap(true);

    auto *credentialsLabel = new QLabel(
        "Usuarios de prueba:\n"
        "admin / admin123\n"
        "lucia / lucia123\n"
        "profe / profe123",
        this);

    auto *formLayout = new QFormLayout();
    usernameEdit->setPlaceholderText("Usuario");
    passwordEdit->setPlaceholderText("Contrasena");
    passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Usuario:", usernameEdit);
    formLayout->addRow("Contrasena:", passwordEdit);

    feedbackLabel->setStyleSheet("color: #b42318;");
    feedbackLabel->setWordWrap(true);

    auto *buttonBox = new QDialogButtonBox(this);
    QPushButton *loginButton = buttonBox->addButton("Ingresar", QDialogButtonBox::AcceptRole);
    QPushButton *registerButton = buttonBox->addButton("Registrarse", QDialogButtonBox::ActionRole);
    buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(credentialsLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addWidget(buttonBox);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::openRegisterDialog);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
    connect(usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
    connect(passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);

    usernameEdit->setFocus();
}

void LoginDialog::attemptLogin()
{
    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        feedbackLabel->setStyleSheet("color: #b42318;");
        feedbackLabel->setText("Debes completar usuario y contrasena.");
        return;
    }

    if (!authManager->login(username, password)) {
        feedbackLabel->setStyleSheet("color: #b42318;");
        feedbackLabel->setText(
            "No fue posible iniciar sesion. Revisa las credenciales o la sesion local.");
        return;
    }

    accept();
}

void LoginDialog::openRegisterDialog()
{
    RegisterDialog registerDialog(authManager, this);
    if (registerDialog.exec() != QDialog::Accepted) {
        return;
    }

    usernameEdit->setText(registerDialog.registeredUsername());
    passwordEdit->setText(registerDialog.registeredPassword());
    feedbackLabel->setStyleSheet("color: #027a48;");
    feedbackLabel->setText("Usuario registrado. Se iniciara sesion automaticamente.");
    attemptLogin();
}
