#ifndef REGISTERWIDGET_H
#define REGISTERWIDGET_H

#include <QWidget>

namespace Ui {
class RegisterWidget;
}

class RegisterWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterWidget(QWidget *parent = nullptr);
    ~RegisterWidget();

signals:
    void registrationDone();
    void requestLogin();

private slots:
    void onRegisterClicked();
    void onBackClicked();

private:
    Ui::RegisterWidget *ui;
    QString hashPassword(const QString &password) const;
    bool usernameExists(const QString &username) const;
};

#endif // REGISTERWIDGET_H
