#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class AuthManager;
class QLabel;
class QLineEdit;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(AuthManager *authManager, QWidget *parent = nullptr);

private slots:
    void attemptLogin();
    void openRegisterDialog();

private:
    AuthManager *authManager;
    QLineEdit *usernameEdit;
    QLineEdit *passwordEdit;
    QLabel *feedbackLabel;
};

#endif // LOGINDIALOG_H
