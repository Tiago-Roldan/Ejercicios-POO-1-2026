#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    explicit Logger(const QString &rutaArchivo);
    void registrar(const QString &accion);

private:
    QString rutaArchivo_;
};

#endif
