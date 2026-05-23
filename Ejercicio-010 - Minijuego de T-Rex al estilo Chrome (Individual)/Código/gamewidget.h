#ifndef GAMEWIDGET_H
#define GAMEWIDGET_H

#include "cactus.h"
#include "pajaro.h"
#include "trex.h"

#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVector>
#include <QWidget>

class GameWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameWidget(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

private slots:
    void updateGame();
    void spawnCactus();
    void spawnBird();
    void removeBird(Pajaro *pajaro);
    void increaseScore();
    void restartGame();

private:
    void startGame();
    void endGame();
    void clearObstacles();
    void updateScoreLabels();
    void addBonus();
    void createGameOverImage();
    int currentSpeed() const;

    TRex *m_trex;
    QVector<Cactus *> m_cactus;
    QVector<Pajaro *> m_birds;

    QTimer *m_gameTimer;
    QTimer *m_cactusTimer;
    QTimer *m_birdTimer;
    QTimer *m_scoreTimer;

    QLabel *m_scoreLabel;
    QLabel *m_bestScoreLabel;
    QLabel *m_gameOverLabel;
    QPushButton *m_restartButton;

    int m_score;
    int m_bestScore;
    int m_elapsedSeconds;
    bool m_gameOver;

    static const int GroundY = 330;
};

#endif
