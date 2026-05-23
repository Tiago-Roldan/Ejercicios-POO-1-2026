#ifndef MAINWIDGET_H
#define MAINWIDGET_H

#include <QWidget>

class LoginWidget;
class RegisterWidget;
class Pintura;
class QStackedLayout;

class MainWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MainWidget(QWidget *parent = nullptr);

private slots:
    void showLogin();
    void showRegister();
    void onLoginSuccess(const QString &username);
    void onRegistrationDone();
    void onLogoutClicked();

private:
    QWidget *loginWidget;
    QWidget *registerWidget;
    QWidget *canvasWidget;
    Pintura *paintingArea;
    QStackedLayout *stackedLayout;
    QString currentUsername;

    void setupDatabase();
    bool hasUsers() const;
    void createTables();
    void setupScreens();
};

#endif // MAINWIDGET_H
