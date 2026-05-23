#include "login.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPushButton>
#include <QResizeEvent>
#include <QVBoxLayout>

Login::Login(const Configuracion &configuracion, Logger *logger, QWidget *parent)
    : Pantalla(configuracion, logger, parent)
{
    inicializarUI();
    conectarEventos();
    cargarDatos();
}

void Login::inicializarUI()
{
    setWindowTitle("Editor multilenguaje - Login");
    setMinimumSize(460, 320);
    setStyleSheet(
        "QWidget { background: #f4f7fb; color: #1d2733; }"
        "QLineEdit { padding: 10px; border: 1px solid #a9b7c6; border-radius: 8px; background: white; }"
        "QPushButton { padding: 10px; border-radius: 8px; background: #0b5ed7; color: white; font-weight: 600; }"
        "QPushButton:hover { background: #084db2; }"
        "QLabel#titulo { font-size: 26px; font-weight: 700; }"
        "QLabel#subtitulo { color: #4e5d6c; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(48, 40, 48, 40);
    layout->setSpacing(14);

    auto *titulo = new QLabel("Ingreso al editor");
    titulo->setObjectName("titulo");
    auto *subtitulo = new QLabel("Usuario inicial: admin | Password inicial: 1234");
    subtitulo->setObjectName("subtitulo");
    usuarioEdit_ = new QLineEdit;
    usuarioEdit_->setPlaceholderText("Usuario");
    passwordEdit_ = new QLineEdit;
    passwordEdit_->setPlaceholderText("Contrasena");
    passwordEdit_->setEchoMode(QLineEdit::Password);
    ingresarBtn_ = new QPushButton("Entrar");
    estadoLabel_ = new QLabel("Ingresa tus credenciales para continuar.");
    estadoLabel_->setWordWrap(true);

    layout->addWidget(titulo);
    layout->addWidget(subtitulo);
    layout->addSpacing(12);
    layout->addWidget(usuarioEdit_);
    layout->addWidget(passwordEdit_);
    layout->addWidget(ingresarBtn_);
    layout->addWidget(estadoLabel_);
    layout->addStretch();
}

void Login::conectarEventos()
{
    connect(ingresarBtn_, &QPushButton::clicked, this, &Login::intentarLogin);
    connect(passwordEdit_, &QLineEdit::returnPressed, this, &Login::intentarLogin);
}

void Login::cargarDatos()
{
    usuarioEdit_->setText(configuracion_.usuarioInicial);
    passwordEdit_->clear();
    estadoLabel_->setText("Ingresa tus credenciales para continuar.");
}

bool Login::validarEstado() const
{
    return !usuarioEdit_->text().trimmed().isEmpty() && !passwordEdit_->text().isEmpty();
}

void Login::registrarEvento(const QString &descripcion)
{
    if (logger_) {
        logger_->registrar(QString("Login | %1").arg(descripcion));
    }
}

void Login::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        registrarEvento("Atajo Enter usado para intentar iniciar sesion");
        intentarLogin();
        return;
    }
    QWidget::keyPressEvent(event);
}

void Login::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click en login en coordenadas (%1,%2)")
                        .arg(event->position().x())
                        .arg(event->position().y()));
    QWidget::mousePressEvent(event);
}

void Login::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Redimension login a %1x%2")
                        .arg(event->size().width())
                        .arg(event->size().height()));
    QWidget::resizeEvent(event);
}

void Login::closeEvent(QCloseEvent *event)
{
    registrarEvento("Cierre de ventana de login confirmado");
    event->accept();
}

void Login::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Login obtuvo el foco");
    QWidget::focusInEvent(event);
}

void Login::focusOutEvent(QFocusEvent *event)
{
    registrarEvento("Login perdio el foco");
    QWidget::focusOutEvent(event);
}

void Login::intentarLogin()
{
    if (!validarEstado()) {
        estadoLabel_->setText("Completa usuario y contrasena antes de continuar.");
        registrarEvento("Intento invalido por campos incompletos");
        return;
    }

    if (usuarioEdit_->text().trimmed() == configuracion_.usuarioInicial &&
        passwordEdit_->text() == configuracion_.passwordInicial) {
        intentosFallidos_ = 0;
        estadoLabel_->setText("Credenciales validadas. Abriendo editor...");
        registrarEvento("Inicio de sesion exitoso");
        emit solicitarEditor();
        return;
    }

    ++intentosFallidos_;
    const int restantes = qMax(0, 3 - intentosFallidos_);
    estadoLabel_->setText(
        QString("Credenciales incorrectas. Intentos restantes antes del bloqueo: %1")
            .arg(restantes));
    registrarEvento(QString("Intento fallido #%1").arg(intentosFallidos_));

    if (intentosFallidos_ >= 3) {
        intentosFallidos_ = 0;
        estadoLabel_->setText("Se alcanzo el limite de intentos. Sistema bloqueado temporalmente.");
        registrarEvento("Se activa modo bloqueado temporal");
        emit solicitarBloqueo();
    }
}
