#include "pinturas.h"
#include <QPainter>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QSqlQuery>
#include <QSqlError>
#include <QDateTime>
#include <QSqlDatabase>
#include <QDebug>

Pintura::Pintura(QWidget *parent)
    : QWidget(parent), currentColor(Qt::black), currentWidth(4), drawing(false)
{
    setAttribute(Qt::WA_StaticContents);
    setFocusPolicy(Qt::StrongFocus);
    setAutoFillBackground(true);
    QPalette pal = palette();
    pal.setColor(QPalette::Window, Qt::white);
    setPalette(pal);
}

void Pintura::setUser(const QString &username)
{
    currentUser = username;
}

void Pintura::loadFromDatabase()
{
    strokes.clear();
    if (currentUser.isEmpty()) {
        update();
        return;
    }

    QSqlDatabase db = database();
    if (!db.isOpen() && !db.open()) {
        update();
        return;
    }

    QSqlQuery query(db);
    query.prepare("SELECT s.id, s.color, s.width, p.seq, p.x, p.y "
                  "FROM strokes s JOIN points p ON p.stroke_id = s.id "
                  "WHERE s.username = :username ORDER BY s.id, p.seq");
    query.bindValue(":username", currentUser);
    if (!query.exec()) {
        update();
        return;
    }

    int currentStrokeId = -1;
    Stroke stroke;
    while (query.next()) {
        const int strokeId = query.value(0).toInt();
        const QString colorName = query.value(1).toString();
        const int width = query.value(2).toInt();
        const int x = query.value(4).toInt();
        const int y = query.value(5).toInt();

        if (strokeId != currentStrokeId) {
            if (currentStrokeId != -1) {
                strokes.append(stroke);
            }
            currentStrokeId = strokeId;
            stroke.id = strokeId;
            stroke.points.clear();
            stroke.color = QColor(colorName);
            stroke.width = width;
        }
        stroke.points.append(QPoint(x, y));
    }

    if (currentStrokeId != -1) {
        strokes.append(stroke);
    }

    update();
}

void Pintura::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.fillRect(rect(), Qt::white);

    auto drawStroke = [&painter](const Stroke &stroke) {
        if (stroke.points.isEmpty()) {
            return;
        }
        QPen pen(stroke.color, stroke.width, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
        painter.setPen(pen);
        if (stroke.points.size() == 1) {
            painter.drawPoint(stroke.points.first());
        } else {
            for (int i = 1; i < stroke.points.size(); ++i) {
                painter.drawLine(stroke.points.at(i - 1), stroke.points.at(i));
            }
        }
    };

    for (const Stroke &stroke : strokes) {
        drawStroke(stroke);
    }

    if (drawing) {
        drawStroke(currentStroke);
    }
}

void Pintura::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !currentUser.isEmpty()) {
        currentStroke.id = -1;
        currentStroke.points.clear();
        currentStroke.color = currentColor;
        currentStroke.width = currentWidth;
        currentStroke.points.append(event->pos());
        drawing = true;
    }
}

void Pintura::mouseMoveEvent(QMouseEvent *event)
{
    if (drawing && (event->buttons() & Qt::LeftButton)) {
        currentStroke.points.append(event->pos());
        update();
    }
}

void Pintura::mouseReleaseEvent(QMouseEvent *event)
{
    if (drawing && event->button() == Qt::LeftButton) {
        if (currentStroke.points.size() > 1) {
            if (saveStrokeToDatabase(currentStroke)) {
                addStroke(currentStroke);
            }
        }
        drawing = false;
        currentStroke.points.clear();
    }
}

void Pintura::wheelEvent(QWheelEvent *event)
{
    const int steps = event->angleDelta().y() / 120;
    if (steps != 0) {
        currentWidth = qBound(1, currentWidth + steps, 40);
        update();
    }
}

void Pintura::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_Z) {
        undoLastStroke();
        return;
    }

    switch (event->key()) {
    case Qt::Key_R:
        currentColor = Qt::red;
        break;
    case Qt::Key_G:
        currentColor = Qt::green;
        break;
    case Qt::Key_B:
        currentColor = Qt::blue;
        break;
    case Qt::Key_Escape:
        clearCanvas();
        break;
    default:
        QWidget::keyPressEvent(event);
        return;
    }
    update();
}

void Pintura::addStroke(const Stroke &stroke)
{
    strokes.append(stroke);
}

bool Pintura::saveStrokeToDatabase(Stroke &stroke)
{
    QSqlDatabase db = database();
    if (!db.isOpen() && !db.open()) {
        return false;
    }

    if (!db.transaction()) {
        return false;
    }

    QSqlQuery strokeQuery(db);
    strokeQuery.prepare("INSERT INTO strokes (username, color, width, created_at) VALUES (:username, :color, :width, :created_at)");
    strokeQuery.bindValue(":username", currentUser);
    strokeQuery.bindValue(":color", stroke.color.name());
    strokeQuery.bindValue(":width", stroke.width);
    strokeQuery.bindValue(":created_at", QDateTime::currentDateTime().toString(Qt::ISODate));
    if (!strokeQuery.exec()) {
        db.rollback();
        return false;
    }

    const qint64 strokeId = strokeQuery.lastInsertId().toLongLong();
    QSqlQuery pointQuery(db);
    pointQuery.prepare("INSERT INTO points (stroke_id, seq, x, y) VALUES (:stroke_id, :seq, :x, :y)");
    for (int i = 0; i < stroke.points.size(); ++i) {
        pointQuery.bindValue(":stroke_id", strokeId);
        pointQuery.bindValue(":seq", i);
        pointQuery.bindValue(":x", stroke.points.at(i).x());
        pointQuery.bindValue(":y", stroke.points.at(i).y());
        if (!pointQuery.exec()) {
            db.rollback();
            return false;
        }
    }

    if (!db.commit()) {
        db.rollback();
        return false;
    }

    stroke.id = static_cast<int>(strokeId);
    return true;
}

void Pintura::clearCanvas()
{
    strokes.clear();
    clearUserDatabase();
    update();
}

void Pintura::clearUserDatabase()
{
    if (currentUser.isEmpty()) {
        return;
    }

    QSqlDatabase db = database();
    if (!db.isOpen() && !db.open()) {
        return;
    }

    if (!db.transaction()) {
        return;
    }

    QSqlQuery query(db);
    query.prepare("DELETE FROM points WHERE stroke_id IN (SELECT id FROM strokes WHERE username = :username)");
    query.bindValue(":username", currentUser);
    query.exec();

    query.prepare("DELETE FROM strokes WHERE username = :username");
    query.bindValue(":username", currentUser);
    query.exec();
    db.commit();
}

void Pintura::undoLastStroke()
{
    if (strokes.isEmpty()) {
        return;
    }

    Stroke lastStroke = strokes.takeLast();
    if (lastStroke.id == -1) {
        update();
        return;
    }

    QSqlDatabase db = database();
    if (!db.isOpen() && !db.open()) {
        update();
        return;
    }

    QSqlQuery removeQuery(db);
    removeQuery.prepare("DELETE FROM points WHERE stroke_id = :stroke_id");
    removeQuery.bindValue(":stroke_id", lastStroke.id);
    removeQuery.exec();
    removeQuery.prepare("DELETE FROM strokes WHERE id = :stroke_id");
    removeQuery.bindValue(":stroke_id", lastStroke.id);
    removeQuery.exec();
    update();
}

QSqlDatabase Pintura::database() const
{
    return QSqlDatabase::database();
}
