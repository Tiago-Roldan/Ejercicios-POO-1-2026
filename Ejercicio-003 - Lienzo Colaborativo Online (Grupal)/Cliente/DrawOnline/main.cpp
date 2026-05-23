#include <QApplication>
#include "ventanaprincipal.h"


int main(int argc, char *argv[])
{
    // Punto de entrada de la aplicacion Qt.
    // QApplication maneja el loop de eventos y el sistema de widgets.
    QApplication app(argc, argv);

    // Creamos y mostramos la ventana principal.
    VentanaPrincipal w;
    w.show();

    // Ejecuta el loop principal hasta que se cierre la app.
    return app.exec();
}
