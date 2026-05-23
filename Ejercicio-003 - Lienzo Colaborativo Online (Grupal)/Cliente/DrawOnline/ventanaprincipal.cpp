#include "ventanaprincipal.h"

#include <QPushButton>
#include <QToolBar>
#include <QUrl>
#include <QtGlobal>

//Creo un espacio de nombres para guardar la url del VPS
namespace
{
    // URL fija por defecto. Es la de nuestro VPS
    const char *kUrlBasePorDefecto = "http://147.93.189.45:8080";
}


//Constructor de la ventana principal. Configura la interfaz y las conexiones entre componentes.
VentanaPrincipal::VentanaPrincipal(QWidget *parent) : QMainWindow(parent), m_gestor_sinc(&m_modelo)
{
    setWindowTitle("DrawOnline");

    // Creamos el lienzo central.
    m_lienzo = new VistaLienzo(&m_modelo, this);
    setCentralWidget(m_lienzo);

    // Barra superior con el boton Guardar.
    QToolBar *barra = new QToolBar("Barra", this);
    barra->setMovable(false);
    barra->setIconSize(QSize(16, 16));
    addToolBar(Qt::TopToolBarArea, barra);

    // Boton Guardar con estilo Metro (azul, blanco, sin bordes).
    QPushButton *botonGuardar = new QPushButton("Guardar", this);
    botonGuardar->setFixedHeight(30);
    // Estilo simple tipo Metro.
    botonGuardar->setStyleSheet(
        "QPushButton {"
        "  background-color: #0078D7;"
        "  color: white;"
        "  border: none;"
        "  padding: 4px 14px;"
        "  font-weight: 600;"
        "}"
        "QPushButton:hover { background-color: #1683DE; }"
        "QPushButton:pressed { background-color: #005A9E; }");
    barra->addWidget(botonGuardar);

    // SEÑALES Y CONEXIONES
    // Conexion vista -> modelo (cuando se termina un trazo).
    connect(m_lienzo, &VistaLienzo::trazoCompletado, &m_modelo, &ModeloDibujo::agregarTrazoLocal);
    // Conexion boton -> sincronizacion (guardar pendientes).
    connect(botonGuardar, &QPushButton::clicked, &m_gestor_sinc, &GestorSincronizacion::guardarPendientes);

    // Uso la variable de la url de mi VPS.
    const QUrl urlBase = QUrl(kUrlBasePorDefecto);
    m_gestor_sinc.setUrlBase(urlBase);
    m_gestor_sinc.cargarInicial();
}
