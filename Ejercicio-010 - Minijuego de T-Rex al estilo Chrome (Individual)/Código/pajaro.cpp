#include "pajaro.h"

#include <QPainter>

Pajaro::Pajaro(QWidget *parent)
    : QWidget(parent),
      m_timer(new QTimer(this)),
      m_speed(7),
      m_counted(false),
      m_wingFrame(0)
{
    setFixedSize(52, 34);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(moveLeft()));
}

Pajaro::~Pajaro()
{
    stop();
}

void Pajaro::start()
{
    m_timer->start(30);
}

void Pajaro::stop()
{
    m_timer->stop();
}

void Pajaro::setSpeed(int speed)
{
    m_speed = speed;
}

QRect Pajaro::collisionRect() const
{
    return geometry().adjusted(5, 6, -5, -6);
}

bool Pajaro::wasCounted() const
{
    return m_counted;
}

void Pajaro::markCounted()
{
    m_counted = true;
}

void Pajaro::moveLeft()
{
    move(x() - m_speed, y());
    m_wingFrame = 1 - m_wingFrame;
    update();

    if (x() + width() < 0) {
        emit leftScreen(this);
    }
}

void Pajaro::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(72, 72, 72));

    painter.drawEllipse(18, 12, 22, 13);
    painter.drawEllipse(35, 10, 10, 10);

    QPolygon upperWing;
    if (m_wingFrame == 0) {
        upperWing << QPoint(23, 14) << QPoint(5, 2) << QPoint(28, 12);
    } else {
        upperWing << QPoint(23, 19) << QPoint(5, 31) << QPoint(29, 22);
    }
    painter.drawPolygon(upperWing);

    painter.setBrush(Qt::white);
    painter.drawEllipse(41, 13, 2, 2);
}
