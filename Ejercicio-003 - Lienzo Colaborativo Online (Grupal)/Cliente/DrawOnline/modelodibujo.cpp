#include "modelodibujo.h"

//Constructor del modelo. Inicializa el objeto base de QObject.
ModeloDibujo::ModeloDibujo(QObject *parent) : QObject(parent) {}

// Acceso de solo lectura a todos los trazos almacenados en el modelo.
const QVector<Trazo> &ModeloDibujo::trazos() const
{
    return m_trazos;
}

// Metodo para agregar un trazo creado localmente y marcarlo como pendiente para sincronizacion con el servidor.
void ModeloDibujo::agregarTrazoLocal(const Trazo &trazo)
{
    // Solo agregamos el trazo si no tenemos uno con el mismo id para evitar duplicados.
    if (m_knownIds.contains(trazo.id)) {
        return;
    }

    // Agregamos el trazo a la lista completa y a la lista de pendientes, y registramos su id.
    m_trazos.append(trazo);
    m_pendientes.append(trazo);
    m_knownIds.insert(trazo.id);

    // Emitimos la señal para indicar que el modelo ha cambiado y se debe repintar el lienzo.
    emit modeloCambiado();
}

// Metodo para fusionar un trazo recibido del servidor sin duplicar. Solo se agrega si no existe un trazo con el mismo id.
void ModeloDibujo::fusionarTrazoRemoto(const Trazo &trazo)
{
    // Solo fusiona si el trazo todavia no existe en el modelo.
    if (m_knownIds.contains(trazo.id)) {
        return;
    }
    m_trazos.append(trazo);
    m_knownIds.insert(trazo.id);
    emit modeloCambiado();
}

// Metodo para consultar rapidamente si ya tenemos un trazo con ese id, usando el conjunto de ids conocidos.
bool ModeloDibujo::tieneTrazo(const QUuid &id) const
{
    return m_knownIds.contains(id);
}

// Metodo para devolver y limpiar la lista de trazos pendientes.
// Esto se llama cuando el gestor de sincronizacion quiere enviar los trazos al servidor.
QVector<Trazo> ModeloDibujo::tomarTrazosPendientes()
{
    // Al devolver la lista, se limpia para no reenviar duplicados.
    QVector<Trazo> out = m_pendientes;
    m_pendientes.clear();
    return out;
}

// Metodo para reencolar trazos pendientes si el envio al servidor falla.
// Se vuelve a agregar a la lista de pendientes y al conjunto de ids conocidos si no existian ya.
void ModeloDibujo::reencolarPendientes(const QVector<Trazo> &trazos)
{
    // Si fallo el envio, volvemos a agregarlos como pendientes.
    for (const Trazo &t : trazos) {
        if (!m_knownIds.contains(t.id)) {
            m_trazos.append(t);
            m_knownIds.insert(t.id);
        }
        m_pendientes.append(t);
    }
    if (!trazos.isEmpty()) {
        emit modeloCambiado();
    }
}
