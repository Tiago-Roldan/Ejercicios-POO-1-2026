#include "cactus.h"

#include <QPainter>

Cactus::Cactus(QWidget *parent)
    : QWidget(parent),
      m_counted(false)
{
    setFixedSize(34, 58);
}

QRect Cactus::collisionRect() const
{
    return geometry().adjusted(5, 4, -5, -2);
}

bool Cactus::wasCounted() const
{
    return m_counted;
}

void Cactus::markCounted()
{
    m_counted = true;
}

void Cactus::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(28, 132, 76));

    painter.drawRoundedRect(13, 5, 11, 50, 5, 5);
    painter.drawRoundedRect(3, 25, 11, 8, 4, 4);
    painter.drawRoundedRect(3, 17, 7, 16, 3, 3);
    painter.drawRoundedRect(23, 32, 9, 8, 4, 4);
    painter.drawRoundedRect(27, 24, 6, 16, 3, 3);
}
