#ifndef REGISTERDIALOG_H
#define REGISTERDIALOG_H

#include <QDialog>

class AuthManager;
class QLabel;
class QLineEdit;

class RegisterDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RegisterDialog(AuthManager *authManager, QWidget *parent = nullptr);

    QString registeredUsername() const;
    QString registeredPassword() const;

private slots:
    void attemptRegistration();

private:
    AuthManager *authManager;
    QLineEdit *fullNameEdit;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *confirmPasswordEdit;
    QLabel *feedbackLabel;
    QString registeredUsernameValue;
    QString registeredPasswordValue;
};

#endif // REGISTERDIALOG_H
