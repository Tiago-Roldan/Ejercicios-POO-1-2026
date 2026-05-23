#ifndef GESTORAPLICACION_H
#define GESTORAPLICACION_H

#include "configuracion.h"

#include <QObject>

class Logger;
class Pantalla;

class GestorAplicacion : public QObject
{
    Q_OBJECT

public:
    explicit GestorAplicacion(QObject *parent = nullptr);
    ~GestorAplicacion() override;

    void iniciar();

private:
    void mostrarPantalla(Pantalla *nuevaPantalla, bool fullScreen = false);
    void conectarPantalla(Pantalla *pantalla);

    Configuracion configuracion_;
    Logger *logger_ = nullptr;
    Pantalla *pantallaActual_ = nullptr;
};

#endif
