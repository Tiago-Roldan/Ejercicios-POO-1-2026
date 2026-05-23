#include "modobloqueado.h"

#include <QCloseEvent>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QLabel>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QVBoxLayout>

ModoBloqueado::ModoBloqueado(const Configuracion &configuracion, Logger *logger, QWidget *parent)
    : Pantalla(configuracion, logger, parent)
{
    inicializarUI();
    conectarEventos();
    cargarDatos();
}

void ModoBloqueado::inicializarUI()
{
    setWindowTitle("Editor multilenguaje - Modo bloqueado");
    setMinimumSize(460, 260);
    setStyleSheet(
        "QWidget { background: #fff4f4; color: #5d1616; }"
        "QLabel#titulo { font-size: 28px; font-weight: 700; }"
        "QLabel#contador { font-size: 36px; font-weight: 700; color: #c12f2f; }");

    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(36, 32, 36, 32);
    layout->setSpacing(16);

    auto *titulo = new QLabel("Modo bloqueado");
    titulo->setObjectName("titulo");
    mensajeLabel_ = new QLabel("Demasiados intentos fallidos. Espera para volver a ingresar.");
    mensajeLabel_->setWordWrap(true);
    contadorLabel_ = new QLabel;
    contadorLabel_->setObjectName("contador");

    layout->addWidget(titulo);
    layout->addWidget(mensajeLabel_);
    layout->addWidget(contadorLabel_);
    layout->addStretch();
}

void ModoBloqueado::conectarEventos()
{
    connect(&temporizador_, &QTimer::timeout, this, &ModoBloqueado::actualizarCuentaRegresiva);
}

void ModoBloqueado::cargarDatos()
{
    segundosRestantes_ = configuracion_.tiempoBloqueoSegundos;
    contadorLabel_->setText(QString::number(segundosRestantes_));
    temporizador_.start(1000);
    registrarEvento("Temporizador de bloqueo iniciado");
}

bool ModoBloqueado::validarEstado() const
{
    return segundosRestantes_ >= 0;
}

void ModoBloqueado::registrarEvento(const QString &descripcion)
{
    if (logger_) {
        logger_->registrar(QString("ModoBloqueado | %1").arg(descripcion));
    }
}

void ModoBloqueado::keyPressEvent(QKeyEvent *event)
{
    registrarEvento(QString("Tecla ignorada durante bloqueo: %1").arg(event->key()));
    event->accept();
}

void ModoBloqueado::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click bloqueado en (%1,%2)")
                        .arg(event->position().x())
                        .arg(event->position().y()));
    event->accept();
}

void ModoBloqueado::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Redimension modo bloqueado a %1x%2")
                        .arg(event->size().width())
                        .arg(event->size().height()));
    QWidget::resizeEvent(event);
}

void ModoBloqueado::closeEvent(QCloseEvent *event)
{
    registrarEvento("Intento de cierre bloqueado hasta finalizar temporizador");
    event->ignore();
}

void ModoBloqueado::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Modo bloqueado obtuvo el foco");
    QWidget::focusInEvent(event);
}

void ModoBloqueado::focusOutEvent(QFocusEvent *event)
{
    registrarEvento("Modo bloqueado perdio el foco");
    QWidget::focusOutEvent(event);
}

void ModoBloqueado::actualizarCuentaRegresiva()
{
    --segundosRestantes_;
    contadorLabel_->setText(QString::number(qMax(0, segundosRestantes_)));

    if (segundosRestantes_ <= 0) {
        temporizador_.stop();
        registrarEvento("Bloqueo finalizado, regreso al login");
        emit solicitarLogin();
    }
}
