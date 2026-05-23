#ifndef LOGINWIDGET_H
#define LOGINWIDGET_H

#include <QWidget>

namespace Ui {
class LoginWidget;
}

class LoginWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWidget(QWidget *parent = nullptr);
    ~LoginWidget();

signals:
    void loginSuccess(const QString &username);
    void requestRegister();

private slots:
    void onLoginClicked();
    void onRegisterClicked();

private:
    Ui::LoginWidget *ui;
    void logAttempt(const QString &username, bool success);
    QString hashPassword(const QString &password) const;
};

#endif // LOGINWIDGET_H
