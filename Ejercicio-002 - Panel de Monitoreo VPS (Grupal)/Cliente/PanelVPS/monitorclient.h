#pragma once

#include <QObject>
#include <QDateTime>
#include <QNetworkAccessManager>
#include <QTimer>
#include <QUrl>

struct HealthData
{
    QString statusText;
    QString uptimeText;
    double load = 0.0;
    int memUsedMB = 0;
    int memTotalMB = 0;
    int diskUsedGB = 0;
    int diskTotalGB = 0;
    QDateTime timestamp;
};

class MonitorClient : public QObject
{
    Q_OBJECT
public:
    explicit MonitorClient(QObject *parent = nullptr);

    void setEndpoint(const QUrl &url);
    void setIntervalSec(int seconds);
    void setLoadThreshold(double threshold);
    void start();
    void refresh();

signals:
    void dataUpdated(const HealthData &data);
    void errorOccurred(const QString &message);

private slots:
    void onTimeout();
    void onReplyFinished();

private:
    void processJson(const QByteArray &payload);
    HealthData buildFallbackData(const QString &status, const QString &uptime) const;
    QString deriveStatus(const HealthData &data) const;

    QNetworkAccessManager m_nam;
    QTimer m_timer;
    QUrl m_endpoint;
    double m_loadThreshold = 1.0;
};
