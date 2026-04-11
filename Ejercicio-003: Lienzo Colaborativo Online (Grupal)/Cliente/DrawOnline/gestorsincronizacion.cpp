#include "gestorsincronizacion.h"

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

// Creo un espacio de nombres para guardar la constante del intervalo de consulta en milisegundos.
namespace {
constexpr int kIntervaloConsultaMs = 2000;
}

// Constructor del gestor de sincronizacion.
// Recibe un puntero al modelo para acceder a los trazos y pendientes, pero no es dueño del modelo, solo lo referencia.
GestorSincronizacion::GestorSincronizacion(ModeloDibujo *modelo, QObject *parent) : QObject(parent), m_modelo(modelo)
{
    // Configura polling (técnica de consultas constantes al servidor) cada 2 segundos.
    m_pollTimer.setInterval(kIntervaloConsultaMs);

    // Conecta el timer al slot que consulta actualizaciones del servidor.
    connect(&m_pollTimer, &QTimer::timeout, this, &GestorSincronizacion::consultarActualizaciones);
}

// Metodo para configurar la URL base del servidor (la de nuestro VPS).
void GestorSincronizacion::setUrlBase(const QUrl &url)
{
    m_urlBase = url;
}

// Metodo para obtener la URL base configurada.
QUrl GestorSincronizacion::urlBase() const
{
    return m_urlBase;
}

// Metodo para cargar el dibujo inicial del servidor al iniciar la aplicación.
// Hace un GET /drawing y fusiona los trazos recibidos.
void GestorSincronizacion::cargarInicial()
{
    // Recupera el estado inicial desde el servidor.
    if (!m_urlBase.isValid()) {
        return;
    }

    // Hace un GET a /drawing para obtener el dibujo actual del servidor.
    QNetworkRequest request(m_urlBase.resolved(QUrl("/drawing")));
    QNetworkReply *reply = m_net.get(request);

    // Conecta la respuesta para fusionar los trazos recibidos y luego iniciar el polling.
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        reply->deleteLater();
        fusionarDesdeRespuesta(data);
        // Empezamos a consultar solo luego de la carga inicial.
        m_pollTimer.start();
    });
}

// Metodo para guardar los trazos pendientes en el servidor.
void GestorSincronizacion::guardarPendientes()
{
    // Envia solo los trazos pendientes para evitar sobrescritura.
    if (!m_urlBase.isValid()) {
        return;
    }
    const QVector<Trazo> pendientes = m_modelo->tomarTrazosPendientes();
    if (pendientes.isEmpty()) {
        return;
    }

    // Armamos el payload JSON con el arreglo de trazos.
    QJsonArray arr;
    for (const Trazo &t : pendientes) {
        arr.append(t.aJson());
    }
    QJsonObject payload;
    payload["strokes"] = arr;
    const QJsonDocument doc(payload);

    // Hacemos un POST a /drawing con el payload JSON de los trazos pendientes.
    QNetworkRequest request(m_urlBase.resolved(QUrl("/drawing")));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = m_net.post(request, doc.toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply, pendientes]() {
        // Si falla, reencolamos para no perder el trabajo.
        if (reply->error() != QNetworkReply::NoError) {
            m_modelo->reencolarPendientes(pendientes);
        }
        reply->deleteLater();
    });
}

// Metodo para consultar actualizaciones del servidor en segundo plano usando polling.
void GestorSincronizacion::consultarActualizaciones()
{
    // Consulta periodica para recibir nuevos trazos de otros usuarios.
    if (!m_urlBase.isValid()) {
        return;
    }

    // Hace un GET a /drawing para obtener el dibujo actual del servidor y fusionar los trazos recibidos.
    QNetworkRequest request(m_urlBase.resolved(QUrl("/drawing")));
    QNetworkReply *reply = m_net.get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        const QByteArray data = reply->readAll();
        reply->deleteLater();
        fusionarDesdeRespuesta(data);
    });
}

// Metodo para fusionar trazos recibidos del servidor a partir de la respuesta JSON.
void GestorSincronizacion::fusionarDesdeRespuesta(const QByteArray &data)
{
    // Fusiona los trazos recibidos sin perder lo local.
    if (data.isEmpty()) {
        return;
    }
    // Parseamos el JSON y verificamos que sea un objeto con un arreglo de trazos.
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return;
    }
    // Iteramos sobre el arreglo de trazos y los fusionamos al modelo sin duplicar.
    const QJsonArray arr = doc.object().value("strokes").toArray();
    for (const QJsonValue &v : arr) {
        const Trazo t = Trazo::desdeJson(v.toObject());
        m_modelo->fusionarTrazoRemoto(t);
    }
}
