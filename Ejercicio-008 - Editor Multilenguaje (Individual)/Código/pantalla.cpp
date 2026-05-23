#include "pantalla.h"

Pantalla::Pantalla(const Configuracion &configuracion, Logger *logger, QWidget *parent)
    : QWidget(parent), configuracion_(configuracion), logger_(logger)
{
    setAttribute(Qt::WA_DeleteOnClose, false);
}
