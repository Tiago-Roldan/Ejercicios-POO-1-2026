#pragma once

#include <QObject>
#include <QSet>
#include <QVector>

#include "trazo.h"

// Clase del Modelo que almacena trazos y gestiona pendientes de sincronizacion.
// La idea clave es llevar dos listas:
// - m_trazos: todo lo dibujado en el lienzo.
// - m_pendientes: trazos locales que aun no se enviaron al servidor.
class ModeloDibujo : public QObject
{
    Q_OBJECT
public:
    explicit ModeloDibujo(QObject *parent = nullptr);

    // Acceso de solo lectura a todos los trazos.
    const QVector<Trazo> &trazos() const;

    // Agrega un trazo creado localmente y lo marca como pendiente.
    void agregarTrazoLocal(const Trazo &trazo);
    // Fusiona un trazo recibido del servidor sin duplicar.
    void fusionarTrazoRemoto(const Trazo &trazo);
    // Consulta rapida para saber si ya tenemos un trazo con ese id.
    bool tieneTrazo(const QUuid &id) const;

    // Devuelve y limpia la lista de pendientes.
    QVector<Trazo> tomarTrazosPendientes();
    // Si el envio falla, se reencolan para intentar luego.
    void reencolarPendientes(const QVector<Trazo> &trazos);

signals:
    // Emite cuando el contenido del modelo cambia y hay que repintar.
    void modeloCambiado();

private:
    QVector<Trazo> m_trazos; // Lista completa de trazos en el lienzo.
    QVector<Trazo> m_pendientes; // Trazos locales aun no sincronizados con el servidor.
    QSet<QUuid> m_knownIds; // Conjunto de ids de trazos ya conocidos para evitar duplicados.
};
