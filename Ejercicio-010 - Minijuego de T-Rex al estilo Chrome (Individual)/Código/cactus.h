#ifndef CACTUS_H
#define CACTUS_H

#include <QWidget>

class Cactus : public QWidget
{
    Q_OBJECT

public:
    explicit Cactus(QWidget *parent = nullptr);

    QRect collisionRect() const;
    bool wasCounted() const;
    void markCounted();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    bool m_counted;
};

#endif
