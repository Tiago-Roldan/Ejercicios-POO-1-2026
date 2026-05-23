#include "monitorclient.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkReply>

MonitorClient::MonitorClient(QObject *parent)
    : QObject(parent)
{
    m_timer.setSingleShot(false);
    connect(&m_timer, &QTimer::timeout, this, &MonitorClient::onTimeout);
}

void MonitorClient::setEndpoint(const QUrl &url)
{
    m_endpoint = url;
}

void MonitorClient::setIntervalSec(int seconds)
{
    if (seconds <= 0) {
        m_timer.stop();
        return;
    }
    m_timer.setInterval(seconds * 1000);
    if (!m_timer.isActive()) {
        m_timer.start();
    }
}

void MonitorClient::setLoadThreshold(double threshold)
{
    m_loadThreshold = threshold;
}

void MonitorClient::start()
{
    if (m_timer.interval() > 0 && !m_timer.isActive()) {
        m_timer.start();
    }
}

void MonitorClient::refresh()
{
    if (!m_endpoint.isValid() || m_endpoint.isEmpty()) {
        emit errorOccurred("Endpoint invalido. Configure una URL valida.");
        return;
    }

    QNetworkRequest request(m_endpoint);
    request.setHeader(QNetworkRequest::UserAgentHeader, "PanelVPS/1.0");
    QNetworkReply *reply = m_nam.get(request);
    connect(reply, &QNetworkReply::finished, this, &MonitorClient::onReplyFinished);
}

void MonitorClient::onTimeout()
{
    refresh();
}

void MonitorClient::onReplyFinished()
{
    auto *reply = qobject_cast<QNetworkReply *>(sender());
    if (!reply) {
        return;
    }

    const auto payload = reply->readAll();
    const auto error = reply->error();
    reply->deleteLater();

    if (error != QNetworkReply::NoError) {
        emit errorOccurred(QString("Error de red: %1").arg(reply->errorString()));
        HealthData down = buildFallbackData("CAIDO", "-");
        down.timestamp = QDateTime::currentDateTime();
        emit dataUpdated(down);
        return;
    }

    processJson(payload);
}

void MonitorClient::processJson(const QByteArray &payload)
{
    QJsonParseError parseError;
    const auto doc = QJsonDocument::fromJson(payload, &parseError);
    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        emit errorOccurred("Respuesta JSON invalida.");
        HealthData invalid = buildFallbackData("ALERTA", "-");
        invalid.timestamp = QDateTime::currentDateTime();
        emit dataUpdated(invalid);
        return;
    }

    const QJsonObject obj = doc.object();
    HealthData data;
    data.uptimeText = obj.value("uptime").toString("-");
    data.load = obj.value("load").toDouble(0.0);
    data.memUsedMB = obj.value("mem_used").toInt(0);
    data.memTotalMB = obj.value("mem_total").toInt(0);
    data.diskUsedGB = obj.value("disk_used").toInt(0);
    data.diskTotalGB = obj.value("disk_total").toInt(0);
    data.timestamp = QDateTime::currentDateTime();

    const QString statusField = obj.value("status").toString().trimmed();
    if (!statusField.isEmpty()) {
        data.statusText = statusField.toUpper();
    } else {
        data.statusText = deriveStatus(data);
    }

    emit dataUpdated(data);
}

HealthData MonitorClient::buildFallbackData(const QString &status, const QString &uptime) const
{
    HealthData data;
    data.statusText = status;
    data.uptimeText = uptime;
    return data;
}

QString MonitorClient::deriveStatus(const HealthData &data) const
{
    if (data.memTotalMB > 0 && data.memUsedMB > data.memTotalMB * 0.9) {
        return "ALERTA";
    }
    if (data.diskTotalGB > 0 && data.diskUsedGB > data.diskTotalGB * 0.9) {
        return "ALERTA";
    }
    if (data.load >= m_loadThreshold) {
        return "ALERTA";
    }
    return "OK";
}
