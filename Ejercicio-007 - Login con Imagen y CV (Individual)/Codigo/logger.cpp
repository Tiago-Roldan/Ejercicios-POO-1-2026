#include "logger.h"

#include <QDateTime>
#include <QFile>
#include <QTextStream>

QString &Logger::logFilePath()
{
    static QString path;
    return path;
}

void Logger::setLogFilePath(const QString &filePath)
{
    logFilePath() = filePath;
}

void Logger::write(const QString &description)
{
    if (logFilePath().isEmpty()) {
        return;
    }

    QFile file(logFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        return;
    }

    QTextStream stream(&file);
    stream << QDateTime::currentDateTime().toString(QStringLiteral("yyyy-MM-dd hh:mm:ss"))
           << " | " << description << '\n';
}
