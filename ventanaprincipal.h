#pragma once

#include <QMainWindow>

#include "vistalienzo.h"
#include "modelodibujo.h"
#include "gestorsincronizacion.h"

// Clase para la Ventana principal con barra superior y lienzo.
// Coordina la conexion entre vista, modelo y sincronizacion.
class VentanaPrincipal : public QMainWindow
{
    Q_OBJECT
public:
    explicit VentanaPrincipal(QWidget *parent = nullptr);

private:
    // Modelo con los trazos y la cola de pendientes.
    ModeloDibujo m_modelo;
    // Vista del lienzo donde se dibuja.
    VistaLienzo *m_lienzo = nullptr;
    // Gestor de sincronizacion con el servidor.
    GestorSincronizacion m_gestor_sinc;
};
