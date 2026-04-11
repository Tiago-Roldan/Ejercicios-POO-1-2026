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
    AuthManager *m_authManager;
    QLineEdit *m_fullNameEdit;
    QLineEdit *m_usernameEdit;
    QLineEdit *m_passwordEdit;
    QLineEdit *m_confirmPasswordEdit;
    QLabel *m_feedbackLabel;
    QString m_registeredUsername;
    QString m_registeredPassword;
};

#endif // REGISTERDIALOG_H
