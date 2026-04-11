#include "logindialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include "../auth/authmanager.h"
#include "registerdialog.h"

LoginDialog::LoginDialog(AuthManager *authManager, QWidget *parent)
    : QDialog(parent)
    , m_authManager(authManager)
    , m_usernameEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_feedbackLabel(new QLabel(this))
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
    m_usernameEdit->setPlaceholderText("Usuario");
    m_passwordEdit->setPlaceholderText("Contrasena");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    formLayout->addRow("Usuario:", m_usernameEdit);
    formLayout->addRow("Contrasena:", m_passwordEdit);

    m_feedbackLabel->setStyleSheet("color: #b42318;");
    m_feedbackLabel->setWordWrap(true);

    auto *buttonBox = new QDialogButtonBox(this);
    QPushButton *loginButton = buttonBox->addButton("Ingresar", QDialogButtonBox::AcceptRole);
    QPushButton *registerButton = buttonBox->addButton("Registrarse", QDialogButtonBox::ActionRole);
    buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);

    mainLayout->addWidget(titleLabel);
    mainLayout->addWidget(subtitleLabel);
    mainLayout->addWidget(credentialsLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_feedbackLabel);
    mainLayout->addWidget(buttonBox);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::attemptLogin);
    connect(registerButton, &QPushButton::clicked, this, &LoginDialog::openRegisterDialog);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &LoginDialog::reject);
    connect(m_usernameEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);
    connect(m_passwordEdit, &QLineEdit::returnPressed, this, &LoginDialog::attemptLogin);

    m_usernameEdit->setFocus();
}

void LoginDialog::attemptLogin()
{
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        m_feedbackLabel->setStyleSheet("color: #b42318;");
        m_feedbackLabel->setText("Debes completar usuario y contrasena.");
        return;
    }

    if (!m_authManager->login(username, password)) {
        m_feedbackLabel->setStyleSheet("color: #b42318;");
        m_feedbackLabel->setText(
            "No fue posible iniciar sesion. Revisa las credenciales o la sesion local.");
        return;
    }

    accept();
}

void LoginDialog::openRegisterDialog()
{
    RegisterDialog registerDialog(m_authManager, this);
    if (registerDialog.exec() != QDialog::Accepted) {
        return;
    }

    m_usernameEdit->setText(registerDialog.registeredUsername());
    m_passwordEdit->setText(registerDialog.registeredPassword());
    m_feedbackLabel->setStyleSheet("color: #027a48;");
    m_feedbackLabel->setText("Usuario registrado. Se iniciara sesion automaticamente.");
    attemptLogin();
}
