#pragma once

#include <QColor>
#include <QWidget>

#include "modelodibujo.h"
#include "trazo.h"

// Clase para la Vista del lienzo que dibuja con paintEvent y captura la interaccion.
// Es responsable de:
// - Recibir eventos de mouse/teclado/rueda.
// - Construir el trazo actual (en vivo).
// - Pintar el modelo y el trazo en curso.
class VistaLienzo : public QWidget
{
    Q_OBJECT
public:

    // Constructor que recibe un puntero al modelo para acceder a los trazos almacenados.
    // El lienzo no es dueño del modelo, solo lo referencia.
    explicit VistaLienzo(ModeloDibujo *modelo, QWidget *parent = nullptr);

    // Tamaño minimo recomendado para el widget, para que el lienzo tenga espacio suficiente.
    QSize minimumSizeHint() const override;
    QSize sizeHint() const override;

signals:
    // Se emite cuando el usuario termina un trazo (mouse suelta).
    void trazoCompletado(const Trazo &trazo);

protected:
    // Sobrescribimos los eventos para manejar la interaccion del usuario.
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    ModeloDibujo *m_modelo = nullptr;   // No es dueño del modelo, solo lo referencia para acceder a los trazos.
    Trazo m_trazoActual;                // Trazo en construccion mientras el usuario dibuja.
    bool m_dibujando = false;           // Indica si estamos en medio de un trazo (mouse presionado).
    qreal m_grosor = 6.0;               // Grosor actual del lapiz o goma, ajustable con la rueda del mouse.
    QColor m_color;                     // Color actual del lapiz, seleccionable con las teclas 1..9.

    // Calcula el color asociado a las teclas 1..9 usando un gradiente.
    QColor colorParaTecla(int digito) const;
    // Inicia un nuevo trazo (lapiz o goma).
    void iniciarTrazo(bool esGoma, const QPointF &pos);
    // Agrega puntos al trazo actual.
    void agregarPunto(const QPointF &pos);
    // Finaliza el trazo y emite la senal.
    void finalizarTrazo();
};
