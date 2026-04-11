#include "trazo.h"

#include <QtMath>

// Constructor que acepta un id para fusionar trazos recibidos del servidor sin crear nuevos ids.
Trazo::Trazo(const QUuid &idTrazo) : id(idTrazo) {}

// Metodo para agregar puntos con interpolacion para evitar huecos al mover rapido el mouse.
void Trazo::agregarPuntoInterpolado(const QPointF &p, qreal paso)
{
    // Interpolamos entre el ultimo punto y el nuevo para mantener continuidad.
    if (puntos.isEmpty()) {
        puntos.append(p);
        return;
    }

    // Calculamos la distancia entre el ultimo punto y el nuevo.
    const QPointF ultimo = puntos.last();
    const qreal dx = p.x() - ultimo.x();
    const qreal dy = p.y() - ultimo.y();
    const qreal dist = std::hypot(dx, dy); //Pitagoras simple.

    // Si la distancia es pequena, solo agregamos el punto final.
    if (dist <= paso) {
        puntos.append(p);
        return;
    }

    // Si la distancia es mayor, insertamos puntos intermedios separados por "paso".
    const int segmentos = static_cast<int>(dist / paso);
    for (int i = 1; i <= segmentos; ++i) {
        const qreal t = static_cast<qreal>(i) / static_cast<qreal>(segmentos + 1);
        puntos.append(QPointF(ultimo.x() + dx * t, ultimo.y() + dy * t));
    }
    puntos.append(p);
}

// Metodo para construir una ruta suavizada a partir de los puntos almacenados.
// Se usa en el paintEvent para dibujar curvas suaves.
QPainterPath Trazo::construirRutaSuavizada() const
{
    // Si no hay puntos, devolvemos una ruta vacia.
    QPainterPath path;
    if (puntos.isEmpty()) {
        return path;
    }

    // Si hay un solo punto, dibujamos un punto circular con el grosor.
    if (puntos.size() == 1) {
        path.addEllipse(puntos.first(), grosor * 0.5, grosor * 0.5);
        return path;
    }

    // Curvas: usamos quadTo para suavizar entre puntos.
    // La curva va del punto actual al medio entre el siguiente y el siguiente-siguiente, usando el punto siguiente como control.
    path.moveTo(puntos.first());
    for (int i = 1; i < puntos.size() - 1; ++i) {
        const QPointF &p1 = puntos.at(i);
        const QPointF &p2 = puntos.at(i + 1);
        const QPointF medio((p1.x() + p2.x()) * 0.5, (p1.y() + p2.y()) * 0.5);
        path.quadTo(p1, medio);
    }
    path.lineTo(puntos.last());
    return path;
}

// Metodo para serializar el trazo a JSON para enviarlo al servidor.
QJsonObject Trazo::aJson() const
{
    // Mantenemos claves en ingles para el protocolo del backend.
    //Armamos el json con los campos del trazo.
    QJsonObject obj;
    obj["id"] = id.toString(QUuid::WithoutBraces);
    obj["width"] = grosor;
    obj["eraser"] = esGoma;
    QJsonObject c; //El color se guarda como un objeto con r,g,b,a.
    c["r"] = color.red();
    c["g"] = color.green();
    c["b"] = color.blue();
    c["a"] = color.alpha();
    obj["color"] = c;
    QJsonArray pts; //Los puntos se guardan como un array de objetos con x,y.
    for (const QPointF &p : puntos) {
        QJsonObject pj;
        pj["x"] = p.x();
        pj["y"] = p.y();
        pts.append(pj);
    }
    obj["points"] = pts;
    return obj;
}

// Metodo para reconstruir un trazo desde JSON recibido del servidor.
Trazo Trazo::desdeJson(const QJsonObject &obj)
{
    // Obtenemos los campos del json y los asignamos al trazo.
    // Si falta algun campo, usamos valores por defecto.
    Trazo s(QUuid(obj.value("id").toString()));
    s.grosor = obj.value("width").toDouble(4.0);
    s.esGoma = obj.value("eraser").toBool(false);
    const QJsonObject c = obj.value("color").toObject();
    s.color = QColor(c.value("r").toInt(0), c.value("g").toInt(0), c.value("b").toInt(0), c.value("a").toInt(255));

    // Los puntos se reconstruyen a partir del array de objetos con x,y.
    const QJsonArray pts = obj.value("points").toArray();
    s.puntos.reserve(pts.size());
    for (const QJsonValue &v : pts) {
        const QJsonObject pj = v.toObject();
        s.puntos.append(QPointF(pj.value("x").toDouble(), pj.value("y").toDouble()));
    }

    // Devolvemos el trazo reconstruido.
    return s;
}
