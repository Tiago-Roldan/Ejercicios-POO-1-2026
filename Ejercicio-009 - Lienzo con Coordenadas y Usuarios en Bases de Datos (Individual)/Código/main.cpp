#include <QApplication>
#include "mainwidget.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWidget w;
    w.setWindowTitle("Aplicación de Dibujo con Login");
    w.resize(900, 650);
    w.show();
    return app.exec();
}
