#ifndef LOGGER_H
#define LOGGER_H

#include <QString>

class Logger
{
public:
    static void setLogFilePath(const QString &filePath);
    static void write(const QString &description);

private:
    static QString &logFilePath();
};

#endif
