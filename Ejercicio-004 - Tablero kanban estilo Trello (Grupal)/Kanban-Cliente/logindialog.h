#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

class KanbanApi;
class QLineEdit;
class QLabel;
class QPushButton;

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(KanbanApi *apiClient, QWidget *parent = nullptr);

private:
    void setupUi();
    void connectSignals();
    void updateStatus(const QString &message, bool isError = false);
    void tryLogin();
    void tryRegister();

    KanbanApi *api = nullptr;
    QLineEdit *usernameEdit = nullptr;
    QLineEdit *passwordEdit = nullptr;
    QPushButton *loginButton = nullptr;
    QPushButton *registerButton = nullptr;
    QLabel *statusLabel = nullptr;
};

#endif // LOGINDIALOG_H
