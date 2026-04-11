#include "vistalienzo.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QWheelEvent>

// Creo un espacio de nombres para guardar constantes relacionadas con el grosor de los trazos y la interpolacion.
namespace {
constexpr qreal kGrosorMin = 1.0;
constexpr qreal kGrosorMax = 64.0;
constexpr qreal kPasoInterpolacion = 2.0;
}

// Metodo para construir la vista del lienzo.
// Recibe un puntero al modelo para acceder a los trazos almacenados, pero no es dueño del modelo, solo lo referencia.
VistaLienzo::VistaLienzo(ModeloDibujo *modelo, QWidget *parent) : QWidget(parent), m_modelo(modelo)
{
    // Evita parpadeos y permite repintar todo el lienzo.
    setAttribute(Qt::WA_OpaquePaintEvent);
    // Para poder recibir teclas (1..9) debemos tener foco.
    setFocusPolicy(Qt::StrongFocus);

    // Color inicial (tecla 1).
    m_color = colorParaTecla(1);

    // Cada cambio en el modelo debe repintar la vista.
    connect(m_modelo, &ModeloDibujo::modeloCambiado, this, QOverload<>::of(&VistaLienzo::update));
}

// Tamaño minimo recomendado para el widget, para que el lienzo tenga espacio suficiente.
QSize VistaLienzo::minimumSizeHint() const
{
    return QSize(640, 480);
}

// Tamaño recomendado para el widget, puede ser mas grande pero no mas pequeño que el minimo.
QSize VistaLienzo::sizeHint() const
{
    return QSize(1024, 768);
}

// Metodo para dibujar el lienzo. Se llama cada vez que se repinta la vista (update()).
void VistaLienzo::paintEvent(QPaintEvent *event)
{
    // El evento no se usa directamente, pero se incluye como parametro porque es parte de la firma del metodo que sobrescribimos.
    Q_UNUSED(event);
    QPainter painter(this);

    // Fondo blanco del lienzo.
    painter.fillRect(rect(), Qt::white);
    painter.setRenderHint(QPainter::Antialiasing, true);

    //  Funcion generica que dibuja todos los trazos del modelo y el trazo actual en vivo.
    auto dibujarTrazo = [&painter](const Trazo &t) {
        QPen pen(t.esGoma ? QColor(Qt::white) : t.color, t.grosor, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        painter.drawPath(t.construirRutaSuavizada());
    };

    for (const Trazo &t : m_modelo->trazos()) {
        dibujarTrazo(t);
    }

    // Si estamos dibujando un trazo en vivo, lo dibujamos encima de los trazos del modelo para que se vea mientras se construye.
    if (m_dibujando) {
        dibujarTrazo(m_trazoActual);
    }
}

// Eventos de mouse para iniciar, actualizar y finalizar un trazo. El boton izquierdo es lapiz y el derecho es goma.
void VistaLienzo::mousePressEvent(QMouseEvent *event)
{
    // Tomamos foco para que las teclas 1..9 funcionen.
    setFocus();
    if (event->button() == Qt::LeftButton) {
        iniciarTrazo(false, event->position());
    } else if (event->button() == Qt::RightButton) {
        iniciarTrazo(true, event->position());
    }
}

// Solo agregamos puntos al trazo actual si estamos en medio de un trazo (mouse presionado).
void VistaLienzo::mouseMoveEvent(QMouseEvent *event)
{
    if (!m_dibujando) {
        return;
    }
    agregarPunto(event->position());
}

// Al soltar el mouse, finalizamos el trazo y emitimos la senal para que el modelo lo guarde.
void VistaLienzo::mouseReleaseEvent(QMouseEvent *event)
{
    if (!m_dibujando) {
        return;
    }
    if (event->button() == Qt::LeftButton || event->button() == Qt::RightButton) {
        finalizarTrazo();
    }
}

// La rueda del mouse ajusta el grosor del lapiz o goma.
// Cada "paso" suele ser 15 grados, lo que corresponde a un cambio de 1 en el grosor.
void VistaLienzo::wheelEvent(QWheelEvent *event)
{
    // La rueda ajusta el grosor. Cada "paso" suele ser 15 grados.
    const QPoint numDegrees = event->angleDelta() / 8;
    if (numDegrees.isNull()) {
        return;
    }
    const int steps = numDegrees.y() / 15;
    // Ajusta grosor de lapiz y goma con la rueda.
    m_grosor = qBound(kGrosorMin, m_grosor + steps, kGrosorMax);
    event->accept();
}

// La tecla 1..9 selecciona el color del lapiz usando un gradiente entre dos colores fijos. La tecla 0 no se usa.
void VistaLienzo::keyPressEvent(QKeyEvent *event)
{
    // Seleccion de color con teclas 1..9.
    const int key = event->key();
    if (key >= Qt::Key_1 && key <= Qt::Key_9) {
        m_color = colorParaTecla(key - Qt::Key_0);
        event->accept();
        return;
    }
    // Para otras teclas, dejamos que el evento se propague normalmente (no lo aceptamos)
    // para que no interfiera con otras funcionalidades del sistema o de Qt.
    QWidget::keyPressEvent(event);
}

// Metodo para calcular el color asociado a las teclas 1..9 usando un gradiente entre dos colores fijos.
// La tecla 0 no se usa y devuelve el color del lapiz actual.
QColor VistaLienzo::colorParaTecla(int digito) const
{
    // Gradiente lineal entre dos colores fijos.
    const QColor start(192, 19, 76);
    const QColor end(24, 233, 199);
    const qreal t = qBound<qreal>(0.0, (digito - 1) / 8.0, 1.0);
    const int r = static_cast<int>(start.red() + (end.red() - start.red()) * t);
    const int g = static_cast<int>(start.green() + (end.green() - start.green()) * t);
    const int b = static_cast<int>(start.blue() + (end.blue() - start.blue()) * t);
    return QColor(r, g, b);
}

// Metodo para iniciar un nuevo trazo (lapiz o goma) con un UUID unico para fusionar trazos. Se llama al presionar el mouse.
void VistaLienzo::iniciarTrazo(bool esGoma, const QPointF &pos)
{
    // Arranca un trazo nuevo con UUID para fusion.
    m_dibujando = true;
    m_trazoActual = Trazo(QUuid::createUuid()); // El constructor del trazo acepta un id para fusionar trazos.
    m_trazoActual.grosor = m_grosor;
    m_trazoActual.esGoma = esGoma;
    m_trazoActual.color = m_color;
    m_trazoActual.agregarPuntoInterpolado(pos, kPasoInterpolacion);
    update();
}

// Metodo para agregar puntos al trazo actual. Se llama al mover el mouse mientras se dibuja.
void VistaLienzo::agregarPunto(const QPointF &pos)
{
    m_trazoActual.agregarPuntoInterpolado(pos, kPasoInterpolacion);
    update();
}

// Metodo para finalizar el trazo y emitir la senal para que el modelo lo guarde. Se llama al soltar el mouse.
void VistaLienzo::finalizarTrazo()
{
    // Emitimos el trazo completo para que el modelo lo guarde.
    m_dibujando = false;
    emit trazoCompletado(m_trazoActual);
    m_trazoActual = Trazo();
    update();
}