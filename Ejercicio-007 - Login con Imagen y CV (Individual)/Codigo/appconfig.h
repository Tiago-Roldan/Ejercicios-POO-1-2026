#ifndef APPCONFIG_H
#define APPCONFIG_H

#include <QString>

struct AppConfig
{
    QString apiKey;
    QString cityPlaceId;
    QString cityName;
    QString latitude;
    QString longitude;
    QString timezoneId;
    QString weatherUrl;
    QString backgroundUrl;
    QString cacheDirectory;
    QString logFilePath;
    QString cvFilePath;
    QString proxyHost;
    QString proxyUser;
    QString proxyPassword;
    int proxyPort = 0;
    int failedAttemptsLimit = 3;
    int lockDurationSeconds = 30;
    QString units = QStringLiteral("metric");
    QString language = QStringLiteral("es");
};

#endif
