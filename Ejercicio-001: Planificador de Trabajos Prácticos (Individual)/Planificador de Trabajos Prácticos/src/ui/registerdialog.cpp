#include "registerdialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

#include "../auth/authmanager.h"

RegisterDialog::RegisterDialog(AuthManager *authManager, QWidget *parent)
    : QDialog(parent)
    , m_authManager(authManager)
    , m_fullNameEdit(new QLineEdit(this))
    , m_usernameEdit(new QLineEdit(this))
    , m_passwordEdit(new QLineEdit(this))
    , m_confirmPasswordEdit(new QLineEdit(this))
    , m_feedbackLabel(new QLabel(this))
{
    setWindowTitle("Registro de usuario");
    setModal(true);
    resize(430, 280);

    auto *mainLayout = new QVBoxLayout(this);

    auto *subtitleLabel = new QLabel(
        "Crea un usuario nuevo para que quede guardado en el archivo JSON local.", this);
    subtitleLabel->setWordWrap(true);

    auto *formLayout = new QFormLayout();
    m_fullNameEdit->setPlaceholderText("Nombre y apellido");
    m_usernameEdit->setPlaceholderText("Usuario");
    m_passwordEdit->setPlaceholderText("Contrasena");
    m_confirmPasswordEdit->setPlaceholderText("Repetir contrasena");
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Nombre completo:", m_fullNameEdit);
    formLayout->addRow("Usuario:", m_usernameEdit);
    formLayout->addRow("Contrasena:", m_passwordEdit);
    formLayout->addRow("Confirmar:", m_confirmPasswordEdit);

    m_feedbackLabel->setStyleSheet("color: #b42318;");
    m_feedbackLabel->setWordWrap(true);

    auto *buttonBox = new QDialogButtonBox(this);
    QPushButton *createButton = buttonBox->addButton("Crear usuario", QDialogButtonBox::AcceptRole);
    buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);

    mainLayout->addWidget(subtitleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_feedbackLabel);
    mainLayout->addWidget(buttonBox);

    connect(createButton, &QPushButton::clicked, this, &RegisterDialog::attemptRegistration);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RegisterDialog::reject);
    connect(m_confirmPasswordEdit, &QLineEdit::returnPressed, this, &RegisterDialog::attemptRegistration);
}

QString RegisterDialog::registeredUsername() const
{
    return m_registeredUsername;
}

QString RegisterDialog::registeredPassword() const
{
    return m_registeredPassword;
}

void RegisterDialog::attemptRegistration()
{
    const QString fullName = m_fullNameEdit->text().trimmed();
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();
    const QString confirmPassword = m_confirmPasswordEdit->text();

    if (fullName.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        m_feedbackLabel->setText("Debes completar todos los campos.");
        return;
    }

    if (password != confirmPassword) {
        m_feedbackLabel->setText("Las contrasenas no coinciden.");
        return;
    }

    QString errorMessage;
    if (!m_authManager->registerUser(fullName, username, password, &errorMessage)) {
        m_feedbackLabel->setText(errorMessage);
        return;
    }

    m_registeredUsername = username;
    m_registeredPassword = password;

    QMessageBox::information(
        this,
        "Usuario creado",
        "El nuevo usuario fue guardado correctamente en el archivo JSON local.");
    accept();
}
