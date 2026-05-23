#ifndef PINTURAS_H
#define PINTURAS_H

#include <QWidget>
#include <QColor>
#include <QVector>
#include <QPoint>
#include <QSqlDatabase>

struct Stroke {
    int id = -1;
    QVector<QPoint> points;
    QColor color;
    int width;
};

class Pintura : public QWidget
{
    Q_OBJECT

public:
    explicit Pintura(QWidget *parent = nullptr);
    void setUser(const QString &username);
    void loadFromDatabase();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVector<Stroke> strokes;
    QString currentUser;
    Stroke currentStroke;
    QColor currentColor;
    int currentWidth;
    bool drawing;

    void addStroke(const Stroke &stroke);
    bool saveStrokeToDatabase(Stroke &stroke);
    void clearCanvas();
    void clearUserDatabase();
    void updateStatus();
    void undoLastStroke();
    QSqlDatabase database() const;
};

#endif // PINTURAS_H
