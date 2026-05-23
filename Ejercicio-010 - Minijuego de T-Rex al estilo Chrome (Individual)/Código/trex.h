#ifndef TREX_H
#define TREX_H

#include <QWidget>

class TRex : public QWidget
{
    Q_OBJECT

public:
    explicit TRex(QWidget *parent = nullptr);

    void reset(int groundY);
    void jump();
    void crouch(bool enabled);
    void moveForward(int limitRight);
    void brake(int limitLeft);
    void updatePhysics(int groundY);
    QRect collisionRect() const;
    bool isCrouching() const;

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    int m_velocityY;
    bool m_jumping;
    bool m_crouching;
};

#endif
