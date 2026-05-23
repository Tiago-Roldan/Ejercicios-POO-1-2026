#include "gamewidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    GameWidget game;
    game.setWindowTitle("Ejercicio 10 - T-Rex Extremo");
    game.show();

    return app.exec();
}
