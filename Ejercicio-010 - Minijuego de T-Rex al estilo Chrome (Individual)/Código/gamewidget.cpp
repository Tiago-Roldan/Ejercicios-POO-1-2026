#include "gamewidget.h"

#include <QKeyEvent>
#include <QPainter>
#include <QPixmap>
#include <QRandomGenerator>

const int GameWidget::GroundY;

GameWidget::GameWidget(QWidget *parent)
    : QWidget(parent),
      m_trex(new TRex(this)),
      m_gameTimer(new QTimer(this)),
      m_cactusTimer(new QTimer(this)),
      m_birdTimer(new QTimer(this)),
      m_scoreTimer(new QTimer(this)),
      m_scoreLabel(new QLabel(this)),
      m_bestScoreLabel(new QLabel(this)),
      m_gameOverLabel(new QLabel(this)),
      m_restartButton(new QPushButton("Reiniciar", this)),
      m_score(0),
      m_bestScore(0),
      m_elapsedSeconds(0),
      m_gameOver(false)
{
    setFixedSize(860, 420);
    setFocusPolicy(Qt::StrongFocus);

    m_scoreLabel->setGeometry(620, 18, 110, 28);
    m_bestScoreLabel->setGeometry(735, 18, 110, 28);
    m_scoreLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    m_bestScoreLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    m_gameOverLabel->setGeometry(280, 105, 300, 120);
    m_gameOverLabel->setAlignment(Qt::AlignCenter);
    m_gameOverLabel->hide();

    m_restartButton->setGeometry(380, 238, 100, 34);
    m_restartButton->setFocusPolicy(Qt::NoFocus);
    m_restartButton->hide();

    connect(m_gameTimer, SIGNAL(timeout()), this, SLOT(updateGame()));
    connect(m_cactusTimer, SIGNAL(timeout()), this, SLOT(spawnCactus()));
    connect(m_birdTimer, SIGNAL(timeout()), this, SLOT(spawnBird()));
    connect(m_scoreTimer, SIGNAL(timeout()), this, SLOT(increaseScore()));
    connect(m_restartButton, SIGNAL(clicked()), this, SLOT(restartGame()));

    createGameOverImage();
    startGame();
}

void GameWidget::keyPressEvent(QKeyEvent *event)
{
    if (m_gameOver) {
        if (event->key() == Qt::Key_R || event->key() == Qt::Key_Return) {
            restartGame();
        }
        return;
    }

    switch (event->key()) {
    case Qt::Key_Space:
        m_trex->jump();
        break;
    case Qt::Key_Down:
        m_trex->crouch(true);
        break;
    case Qt::Key_Right:
        m_trex->moveForward(width() / 2);
        break;
    case Qt::Key_Left:
        m_trex->brake(20);
        break;
    default:
        QWidget::keyPressEvent(event);
        break;
    }
}

void GameWidget::keyReleaseEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Down) {
        m_trex->crouch(false);
        return;
    }

    QWidget::keyReleaseEvent(event);
}

void GameWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.fillRect(rect(), QColor(247, 247, 247));

    painter.setPen(QPen(QColor(92, 92, 92), 2));
    painter.drawLine(0, GroundY + 1, width(), GroundY + 1);

    painter.setPen(QColor(190, 190, 190));
    for (int x = 15; x < width(); x += 76) {
        painter.drawLine(x, GroundY + 15, x + 24, GroundY + 15);
    }
}

void GameWidget::updateGame()
{
    m_trex->updatePhysics(GroundY);

    const int speed = currentSpeed();
    for (int i = m_cactus.size() - 1; i >= 0; --i) {
        Cactus *cactus = m_cactus.at(i);
        cactus->move(cactus->x() - speed, cactus->y());

        if (!cactus->wasCounted() && cactus->x() + cactus->width() < m_trex->x()) {
            cactus->markCounted();
            addBonus();
        }

        if (m_trex->collisionRect().intersects(cactus->collisionRect())) {
            endGame();
            return;
        }

        if (cactus->x() + cactus->width() < 0) {
            m_cactus.removeAt(i);
            cactus->deleteLater();
        }
    }

    for (Pajaro *bird : m_birds) {
        bird->setSpeed(speed);

        if (!bird->wasCounted() && bird->x() + bird->width() < m_trex->x()) {
            bird->markCounted();
            addBonus();
        }

        if (m_trex->collisionRect().intersects(bird->collisionRect())) {
            endGame();
            return;
        }
    }
}

void GameWidget::spawnCactus()
{
    if (m_gameOver) {
        return;
    }

    Cactus *cactus = new Cactus(this);
    cactus->move(width(), GroundY - cactus->height());
    cactus->show();
    m_cactus.append(cactus);
}

void GameWidget::spawnBird()
{
    if (m_gameOver) {
        return;
    }

    Pajaro *bird = new Pajaro(this);
    const int altitude = QRandomGenerator::global()->bounded(85, 205);
    bird->move(width(), GroundY - altitude);
    bird->setSpeed(currentSpeed());
    connect(bird, SIGNAL(leftScreen(Pajaro*)), this, SLOT(removeBird(Pajaro*)));
    bird->show();
    bird->start();
    m_birds.append(bird);
}

void GameWidget::removeBird(Pajaro *pajaro)
{
    const int index = m_birds.indexOf(pajaro);
    if (index >= 0) {
        m_birds.removeAt(index);
    }
    pajaro->stop();
    pajaro->deleteLater();
}

void GameWidget::increaseScore()
{
    if (m_gameOver) {
        return;
    }

    ++m_elapsedSeconds;
    ++m_score;
    updateScoreLabels();
}

void GameWidget::restartGame()
{
    startGame();
}

void GameWidget::startGame()
{
    clearObstacles();

    m_score = 0;
    m_elapsedSeconds = 0;
    m_gameOver = false;
    m_trex->reset(GroundY);

    m_gameOverLabel->hide();
    m_restartButton->hide();
    updateScoreLabels();

    m_gameTimer->start(30);
    m_cactusTimer->start(1450);
    m_birdTimer->start(5000);
    m_scoreTimer->start(1000);

    QTimer::singleShot(2200, this, SLOT(spawnCactus()));
    setFocus();
}

void GameWidget::endGame()
{
    m_gameOver = true;

    m_gameTimer->stop();
    m_cactusTimer->stop();
    m_birdTimer->stop();
    m_scoreTimer->stop();

    for (Pajaro *bird : m_birds) {
        bird->stop();
    }

    if (m_score > m_bestScore) {
        m_bestScore = m_score;
    }

    updateScoreLabels();
    m_gameOverLabel->show();
    m_restartButton->show();
    m_restartButton->setFocus();
}

void GameWidget::clearObstacles()
{
    for (Cactus *cactus : m_cactus) {
        cactus->deleteLater();
    }
    m_cactus.clear();

    for (Pajaro *bird : m_birds) {
        bird->stop();
        bird->deleteLater();
    }
    m_birds.clear();
}

void GameWidget::updateScoreLabels()
{
    m_scoreLabel->setText(QString("Puntos: %1").arg(m_score));
    m_bestScoreLabel->setText(QString("Max: %1").arg(m_bestScore));
}

void GameWidget::addBonus()
{
    m_score += 10;
    updateScoreLabels();
}

void GameWidget::createGameOverImage()
{
    QPixmap image(m_gameOverLabel->size());
    image.fill(Qt::transparent);

    QPainter painter(&image);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 235));
    painter.drawRoundedRect(image.rect().adjusted(2, 2, -2, -2), 8, 8);

    painter.setPen(QPen(QColor(45, 45, 45), 3));
    QFont titleFont = painter.font();
    titleFont.setPointSize(24);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(0, 24, image.width(), 42), Qt::AlignCenter, "GAME OVER");

    QFont textFont = painter.font();
    textFont.setPointSize(10);
    textFont.setBold(false);
    painter.setFont(textFont);
    painter.drawText(QRect(0, 76, image.width(), 24), Qt::AlignCenter, "Presiona R o el boton Reiniciar");

    m_gameOverLabel->setPixmap(image);
}

int GameWidget::currentSpeed() const
{
    return 7 + (m_elapsedSeconds / 8);
}
