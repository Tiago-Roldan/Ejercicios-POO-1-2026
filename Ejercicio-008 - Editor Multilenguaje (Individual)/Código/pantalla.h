#ifndef PANTALLA_H
#define PANTALLA_H

#include "configuracion.h"
#include "logger.h"

#include <QWidget>

class Pantalla : public QWidget
{
    Q_OBJECT

public:
    explicit Pantalla(const Configuracion &configuracion, Logger *logger, QWidget *parent = nullptr);
    virtual ~Pantalla() = default;

    virtual void inicializarUI() = 0;
    virtual void conectarEventos() = 0;
    virtual void cargarDatos() = 0;
    virtual bool validarEstado() const = 0;
    virtual void registrarEvento(const QString &descripcion) = 0;

signals:
    void solicitarLogin();
    void solicitarEditor();
    void solicitarBloqueo();

protected:
    Configuracion configuracion_;
    Logger *logger_;
};

#endif
