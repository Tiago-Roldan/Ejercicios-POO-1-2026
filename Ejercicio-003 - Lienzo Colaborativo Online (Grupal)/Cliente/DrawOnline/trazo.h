#pragma once

#include <QColor>
#include <QJsonObject>
#include <QJsonArray>
#include <QPainterPath>
#include <QPointF>
#include <QVector>
#include <QUuid>

// Clase que Representa un trazo de lapiz o goma.
// - Guarda el conjunto de puntos en el orden en que se dibujaron.
// - Guarda el color, el grosor y si el trazo es de tipo goma.
// - Incluye funciones para suavizar y serializar el trazo.
class Trazo
{
public:
    // Identificador unico para evitar duplicados al fusionar trazos.
    QUuid id;
    // Color del trazo cuando NO es goma.
    QColor color;
    // Grosor del trazo en pixeles.
    qreal grosor = 4.0;
    // Indica si este trazo es una goma (borrado).
    bool esGoma = false;
    // Puntos crudos capturados durante el movimiento del mouse.
    QVector<QPointF> puntos;

    // Constructores. Uno por defecto y otro que acepta un id para fusionar trazos.
    Trazo() = default;
    explicit Trazo(const QUuid &idTrazo);

    // Agrega puntos interpolados para evitar huecos al mover rapido el mouse.
    // "paso" es la distancia aproximada entre puntos generados.
    void agregarPuntoInterpolado(const QPointF &p, qreal paso);
    // Construye una ruta suave a partir de los puntos almacenados.
    // Se usa en el paintEvent para dibujar curvas suaves.
    QPainterPath construirRutaSuavizada() const;

    // Serializa el trazo a JSON para enviarlo al servidor.
    QJsonObject aJson() const;
    // Reconstruye un trazo desde JSON recibido del servidor.
    static Trazo desdeJson(const QJsonObject &obj);
};
