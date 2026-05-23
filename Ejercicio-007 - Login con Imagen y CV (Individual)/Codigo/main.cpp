#include "appcontroller.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("Login con Clima y CV"));
    app.setOrganizationName(QStringLiteral("Facultad"));

    AppController controller;
    controller.start();

    return app.exec();
}
