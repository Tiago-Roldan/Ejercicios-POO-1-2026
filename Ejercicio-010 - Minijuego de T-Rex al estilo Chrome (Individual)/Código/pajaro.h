#ifndef PAJARO_H
#define PAJARO_H

#include <QTimer>
#include <QWidget>

class Pajaro : public QWidget
{
    Q_OBJECT

public:
    explicit Pajaro(QWidget *parent = nullptr);
    ~Pajaro() override;

    void start();
    void stop();
    void setSpeed(int speed);
    QRect collisionRect() const;
    bool wasCounted() const;
    void markCounted();

signals:
    void leftScreen(Pajaro *pajaro);

private slots:
    void moveLeft();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QTimer *m_timer;
    int m_speed;
    bool m_counted;
    int m_wingFrame;
};

#endif
