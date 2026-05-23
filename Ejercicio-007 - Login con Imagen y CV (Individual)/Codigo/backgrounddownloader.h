#ifndef BACKGROUNDDOWNLOADER_H
#define BACKGROUNDDOWNLOADER_H

#include "appconfig.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QPixmap>

class QNetworkReply;

class BackgroundDownloader : public QObject
{
    Q_OBJECT

public:
    explicit BackgroundDownloader(QObject *parent = nullptr);

    void applyConfiguration(const AppConfig &config);
    void prepareBackground();

signals:
    void backgroundReady(const QPixmap &pixmap, bool offlineMode);
    void progressMessage(const QString &message);

private slots:
    void onImageReplyFinished(QNetworkReply *reply);

private:
    QString cacheFilePath() const;
    QPixmap placeholderPixmap() const;
    void emitFromCacheOrPlaceholder(const QString &message, bool offlineMode);

    AppConfig m_config;
    QNetworkAccessManager *m_networkManager = nullptr;
    bool m_waitingForDownload = false;
};

#endif
