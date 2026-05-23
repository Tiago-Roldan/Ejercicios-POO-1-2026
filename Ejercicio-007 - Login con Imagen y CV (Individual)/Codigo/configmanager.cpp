#include "configmanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QSettings>

QString ConfigManager::projectRootPath()
{
    QStringList candidatePaths;
    candidatePaths << QDir::currentPath()
                   << QCoreApplication::applicationDirPath();

    for (const QString &candidate : candidatePaths) {
        QDir dir(candidate);
        for (int i = 0; i < 6; ++i) {
            if (dir.exists(QStringLiteral("config.ini"))
                || dir.exists(QStringLiteral("CV_Carlos_Ruiz.pdf"))
                || dir.exists(QStringLiteral("LoginClimaCV.pro"))) {
                return dir.absolutePath();
            }

            if (!dir.cdUp()) {
                break;
            }
        }
    }

    return QDir::currentPath();
}

AppConfig ConfigManager::load()
{
    const QString rootPath = projectRootPath();
    const QString configPath = QDir(rootPath).filePath(QStringLiteral("config.ini"));
    const QString defaultCacheDir = QDir(rootPath).filePath(QStringLiteral("cache"));
    const QString defaultLogFile = QDir(rootPath).filePath(QStringLiteral("eventos.log"));
    const QString defaultCvPath = QDir(rootPath).filePath(QStringLiteral("CV_Carlos_Ruiz.pdf"));

    QSettings settings(configPath, QSettings::IniFormat);

    AppConfig config;
    config.apiKey = settings.value(QStringLiteral("weather/apiKey"),
                                   QStringLiteral("54omft5ilfukxkp6fimfxedf9k7gzwrbv9qz63zl")).toString();
    config.cityPlaceId = settings.value(QStringLiteral("weather/placeId"),
                                        QStringLiteral("cordoba")).toString();
    config.cityName = settings.value(QStringLiteral("weather/cityName"),
                                     QStringLiteral("Cordoba Capital, Cordoba")).toString();
    config.latitude = settings.value(QStringLiteral("weather/latitude"),
                                     QStringLiteral("-31.4135")).toString();
    config.longitude = settings.value(QStringLiteral("weather/longitude"),
                                      QStringLiteral("-64.18105")).toString();
    config.timezoneId = settings.value(QStringLiteral("weather/timezone"),
                                       QStringLiteral("America/Argentina/Cordoba")).toString();
    config.weatherUrl = settings.value(QStringLiteral("weather/url"),
                                       QStringLiteral("https://www.meteosource.com/api/v1/free/point")).toString();
    config.backgroundUrl = settings.value(QStringLiteral("background/url"),
                                          QStringLiteral("https://www.fiftysounds.com/images/graphics/white-low-poly-background-338.jpg")).toString();
    config.cacheDirectory = settings.value(QStringLiteral("paths/cacheDirectory"), defaultCacheDir).toString();
    config.logFilePath = settings.value(QStringLiteral("paths/logFile"), defaultLogFile).toString();
    config.cvFilePath = settings.value(QStringLiteral("paths/cvFile"), defaultCvPath).toString();
    config.proxyHost = settings.value(QStringLiteral("proxy/host")).toString();
    config.proxyPort = settings.value(QStringLiteral("proxy/port"), 0).toInt();
    config.proxyUser = settings.value(QStringLiteral("proxy/user")).toString();
    config.proxyPassword = settings.value(QStringLiteral("proxy/password")).toString();
    config.failedAttemptsLimit = settings.value(QStringLiteral("security/failedAttemptsLimit"), 3).toInt();
    config.lockDurationSeconds = settings.value(QStringLiteral("security/lockDurationSeconds"), 30).toInt();
    config.units = settings.value(QStringLiteral("weather/units"), QStringLiteral("metric")).toString();
    config.language = settings.value(QStringLiteral("weather/language"), QStringLiteral("es")).toString();

    if (config.cacheDirectory.trimmed().isEmpty()) {
        config.cacheDirectory = defaultCacheDir;
    }
    if (config.logFilePath.trimmed().isEmpty()) {
        config.logFilePath = defaultLogFile;
    }
    if (config.cvFilePath.trimmed().isEmpty()) {
        config.cvFilePath = defaultCvPath;
    }

    QDir().mkpath(config.cacheDirectory);

    return config;
}
