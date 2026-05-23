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
    , authManager(authManager)
    , fullNameEdit(new QLineEdit(this))
    , usernameEdit(new QLineEdit(this))
    , passwordEdit(new QLineEdit(this))
    , confirmPasswordEdit(new QLineEdit(this))
    , feedbackLabel(new QLabel(this))
{
    setWindowTitle("Registro de usuario");
    setModal(true);
    resize(430, 280);

    auto *mainLayout = new QVBoxLayout(this);

    auto *subtitleLabel = new QLabel(
        "Crea un usuario nuevo para que quede guardado en el archivo JSON local.", this);
    subtitleLabel->setWordWrap(true);

    auto *formLayout = new QFormLayout();
    fullNameEdit->setPlaceholderText("Nombre y apellido");
    usernameEdit->setPlaceholderText("Usuario");
    passwordEdit->setPlaceholderText("Contrasena");
    confirmPasswordEdit->setPlaceholderText("Repetir contrasena");
    passwordEdit->setEchoMode(QLineEdit::Password);
    confirmPasswordEdit->setEchoMode(QLineEdit::Password);

    formLayout->addRow("Nombre completo:", fullNameEdit);
    formLayout->addRow("Usuario:", usernameEdit);
    formLayout->addRow("Contrasena:", passwordEdit);
    formLayout->addRow("Confirmar:", confirmPasswordEdit);

    feedbackLabel->setStyleSheet("color: #b42318;");
    feedbackLabel->setWordWrap(true);

    auto *buttonBox = new QDialogButtonBox(this);
    QPushButton *createButton = buttonBox->addButton("Crear usuario", QDialogButtonBox::AcceptRole);
    buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);

    mainLayout->addWidget(subtitleLabel);
    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(feedbackLabel);
    mainLayout->addWidget(buttonBox);

    connect(createButton, &QPushButton::clicked, this, &RegisterDialog::attemptRegistration);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &RegisterDialog::reject);
    connect(confirmPasswordEdit, &QLineEdit::returnPressed, this, &RegisterDialog::attemptRegistration);
}

QString RegisterDialog::registeredUsername() const
{
    return registeredUsernameValue;
}

QString RegisterDialog::registeredPassword() const
{
    return registeredPasswordValue;
}

void RegisterDialog::attemptRegistration()
{
    const QString fullName = fullNameEdit->text().trimmed();
    const QString username = usernameEdit->text().trimmed();
    const QString password = passwordEdit->text();
    const QString confirmPassword = confirmPasswordEdit->text();

    if (fullName.isEmpty() || username.isEmpty() || password.isEmpty() || confirmPassword.isEmpty()) {
        feedbackLabel->setText("Debes completar todos los campos.");
        return;
    }

    if (password != confirmPassword) {
        feedbackLabel->setText("Las contrasenas no coinciden.");
        return;
    }

    QString errorMessage;
    if (!authManager->registerUser(fullName, username, password, &errorMessage)) {
        feedbackLabel->setText(errorMessage);
        return;
    }

    registeredUsernameValue = username;
    registeredPasswordValue = password;

    QMessageBox::information(
        this,
        "Usuario creado",
        "El nuevo usuario fue guardado correctamente en el archivo JSON local.");
    accept();
}
