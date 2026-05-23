#include "trex.h"

#include <QPainter>

TRex::TRex(QWidget *parent)
    : QWidget(parent),
      m_velocityY(0),
      m_jumping(false),
      m_crouching(false)
{
    setFixedSize(54, 62);
}

void TRex::reset(int groundY)
{
    m_velocityY = 0;
    m_jumping = false;
    m_crouching = false;
    setFixedSize(54, 62);
    move(80, groundY - height());
    update();
}

void TRex::jump()
{
    if (!m_jumping) {
        m_velocityY = -19;
        m_jumping = true;
        m_crouching = false;
        setFixedSize(54, 62);
    }
}

void TRex::crouch(bool enabled)
{
    if (m_jumping) {
        return;
    }

    if (m_crouching == enabled) {
        return;
    }

    const int bottom = y() + height();
    m_crouching = enabled;
    setFixedSize(enabled ? 70 : 54, enabled ? 36 : 62);
    move(x(), bottom - height());
    update();
}

void TRex::moveForward(int limitRight)
{
    const int nextX = qMin(x() + 18, limitRight - width());
    move(nextX, y());
}

void TRex::brake(int limitLeft)
{
    const int nextX = qMax(x() - 18, limitLeft);
    move(nextX, y());
}

void TRex::updatePhysics(int groundY)
{
    if (!m_jumping) {
        return;
    }

    move(x(), y() + m_velocityY);
    m_velocityY += 1;

    if (y() + height() >= groundY) {
        move(x(), groundY - height());
        m_velocityY = 0;
        m_jumping = false;
    }
}

QRect TRex::collisionRect() const
{
    return geometry().adjusted(7, 6, -7, -4);
}

bool TRex::isCrouching() const
{
    return m_crouching;
}

void TRex::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(130, 64, 190));

    if (m_crouching) {
        painter.drawRoundedRect(6, 10, 45, 20, 4, 4);
        painter.drawRoundedRect(41, 4, 22, 18, 4, 4);
        painter.drawRect(16, 28, 8, 8);
        painter.drawRect(43, 28, 8, 8);
    } else {
        painter.drawRoundedRect(10, 20, 28, 34, 5, 5);
        painter.drawRoundedRect(31, 7, 20, 22, 4, 4);
        painter.drawRect(18, 52, 8, 10);
        painter.drawRect(34, 52, 8, 10);
        painter.drawRect(4, 31, 12, 6);
    }

    painter.setBrush(Qt::white);
    painter.drawEllipse(m_crouching ? QPoint(55, 10) : QPoint(45, 14), 2, 2);
}
