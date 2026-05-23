#ifndef PANTALLA_H
#define PANTALLA_H

#include "appconfig.h"

#include <QWidget>

class Pantalla : public QWidget
{
    Q_OBJECT

public:
    explicit Pantalla(QWidget *parent = nullptr);
    ~Pantalla() override;

    virtual void aplicarConfiguracion(const AppConfig &config) = 0;
    virtual void actualizarContenido() = 0;
};

#endif
