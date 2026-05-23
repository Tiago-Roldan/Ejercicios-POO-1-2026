#include "gestoraplicacion.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    GestorAplicacion gestor;
    gestor.iniciar();
    return app.exec();
}
