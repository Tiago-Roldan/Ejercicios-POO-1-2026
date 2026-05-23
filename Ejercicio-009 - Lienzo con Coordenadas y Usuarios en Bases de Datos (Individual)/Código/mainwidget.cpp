#include "mainwidget.h"
#include "loginwidget.h"
#include "registerwidget.h"
#include "pinturas.h"
#include <QStackedLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSqlDatabase>
#include <QCoreApplication>
#include <QSqlQuery>
#include <QSqlError>
#include <QDir>
#include <QMessageBox>
#include <QDateTime>

MainWidget::MainWidget(QWidget *parent)
    : QWidget(parent), loginWidget(nullptr), registerWidget(nullptr), canvasWidget(nullptr), paintingArea(nullptr)
{
    setupDatabase();
    setupScreens();
    if (hasUsers()) {
        showLogin();
    } else {
        showRegister();
    }
}

void MainWidget::setupDatabase()
{
    const QString databasePath = QCoreApplication::applicationDirPath() + "/data.db";
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(databasePath);
    if (!db.open()) {
        QMessageBox::critical(this, "Error de base", "No se pudo abrir la base de datos SQLite.");
        return;
    }
    createTables();
}

void MainWidget::createTables()
{
    QSqlDatabase db = QSqlDatabase::database();
    QSqlQuery query(db);
    query.exec("CREATE TABLE IF NOT EXISTS users ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "first_name TEXT NOT NULL,"
               "last_name TEXT NOT NULL,"
               "username TEXT NOT NULL UNIQUE,"
               "password_hash TEXT NOT NULL"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS strokes ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "username TEXT NOT NULL,"
               "color TEXT NOT NULL,"
               "width INTEGER NOT NULL,"
               "created_at TEXT NOT NULL"
               ");");

    query.exec("CREATE TABLE IF NOT EXISTS points ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT,"
               "stroke_id INTEGER NOT NULL,"
               "seq INTEGER NOT NULL,"
               "x INTEGER NOT NULL,"
               "y INTEGER NOT NULL,"
               "FOREIGN KEY(stroke_id) REFERENCES strokes(id) ON DELETE CASCADE"
               ");");
}

bool MainWidget::hasUsers() const
{
    QSqlDatabase db = QSqlDatabase::database();
    if (!db.isOpen() && !db.open()) {
        return false;
    }
    QSqlQuery query(db);
    query.exec("SELECT COUNT(1) FROM users");
    if (query.next()) {
        return query.value(0).toInt() > 0;
    }
    return false;
}

void MainWidget::setupScreens()
{
    stackedLayout = new QStackedLayout(this);

    loginWidget = new LoginWidget(this);
    registerWidget = new RegisterWidget(this);

    connect(static_cast<LoginWidget *>(loginWidget), &LoginWidget::loginSuccess, this, &MainWidget::onLoginSuccess);
    connect(static_cast<LoginWidget *>(loginWidget), &LoginWidget::requestRegister, this, &MainWidget::showRegister);
    connect(static_cast<RegisterWidget *>(registerWidget), &RegisterWidget::registrationDone, this, &MainWidget::onRegistrationDone);
    connect(static_cast<RegisterWidget *>(registerWidget), &RegisterWidget::requestLogin, this, &MainWidget::showLogin);

    canvasWidget = new QWidget(this);
    QVBoxLayout *canvasLayout = new QVBoxLayout(canvasWidget);
    QLabel *title = new QLabel("Pantalla de dibujo\n\n- Dibuje arrastrando con el mouse.\n- Cambie color con R, G, B.\n- Ajuste el grosor con la rueda del mouse.\n- Escape borra el lienzo.\n- Ctrl+Z deshace la última acción.");
    title->setWordWrap(true);
    title->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    title->setStyleSheet("font-weight: bold; margin: 5px;");
    paintingArea = new Pintura(canvasWidget);
    paintingArea->setMinimumSize(800, 500);

    QLabel *footer = new QLabel("Se guardan las coordenadas de cada trazo en la base de datos y se reconstruye automáticamente al iniciar sesión.");
    footer->setWordWrap(true);
    QPushButton *logoutButton = new QPushButton("Cerrar sesión", canvasWidget);
    connect(logoutButton, &QPushButton::clicked, this, &MainWidget::onLogoutClicked);

    canvasLayout->addWidget(title);
    canvasLayout->addWidget(paintingArea, 1);
    canvasLayout->addWidget(footer);
    canvasLayout->addWidget(logoutButton);
    canvasWidget->setLayout(canvasLayout);

    stackedLayout->addWidget(registerWidget);
    stackedLayout->addWidget(loginWidget);
    stackedLayout->addWidget(canvasWidget);
    setLayout(stackedLayout);
}

void MainWidget::showLogin()
{
    stackedLayout->setCurrentWidget(loginWidget);
}

void MainWidget::showRegister()
{
    stackedLayout->setCurrentWidget(registerWidget);
}

void MainWidget::onLoginSuccess(const QString &username)
{
    currentUsername = username;
    paintingArea->setUser(username);
    paintingArea->loadFromDatabase();
    stackedLayout->setCurrentWidget(canvasWidget);
}

void MainWidget::onRegistrationDone()
{
    showLogin();
}

void MainWidget::onLogoutClicked()
{
    currentUsername.clear();
    paintingArea->setUser(QString());
    paintingArea->loadFromDatabase();
    showLogin();
}
