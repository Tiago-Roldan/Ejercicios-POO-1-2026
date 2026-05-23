#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QTimer>

#include "modelodibujo.h"

// Clase que Encapsula la comunicacion con el backend para sincronizar trazos.
// Estrategia:
// - Al iniciar: GET /drawing y fusionar.
// - Al guardar: POST /drawing con solo los trazos pendientes.
// - En segundo plano: polling (GET) para recibir trazos de otros.
class GestorSincronizacion : public QObject
{
    Q_OBJECT
public:

    // Constructor que recibe un puntero al modelo para acceder a los trazos y pendientes.
    // No es dueño del modelo, solo lo referencia.
    explicit GestorSincronizacion(ModeloDibujo *modelo, QObject *parent = nullptr);

    // Configura la URL base del servidor (la de nuestro VPS)
    void setUrlBase(const QUrl &url);
    QUrl urlBase() const;

public slots:
    // Slots para cargar el dibujo inicial del servidor y para guardar los trazos pendientes.
    void cargarInicial();
    void guardarPendientes();

private slots:
    // Slot para consultar actualizaciones del servidor en segundo plano usando polling.
    void consultarActualizaciones();

private:
    // Metodo para fusionar trazos recibidos del servidor a partir de la respuesta JSON.
    void fusionarDesdeRespuesta(const QByteArray &data);

    // Atributos para gestionar la comunicacion con el servidor y el modelo.
    ModeloDibujo *m_modelo = nullptr;
    QNetworkAccessManager m_net;
    QTimer m_pollTimer;
    QUrl m_urlBase;
};
