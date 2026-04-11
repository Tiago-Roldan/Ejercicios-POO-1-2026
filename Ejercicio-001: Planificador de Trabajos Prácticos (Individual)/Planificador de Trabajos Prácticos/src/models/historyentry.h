#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include <QDateTime>
#include <QJsonObject>
#include <QString>

struct HistoryEntry
{
    QDateTime timestamp;
    QString username;
    QString action;

    static HistoryEntry fromJson(const QJsonObject &json)
    {
        HistoryEntry entry;
        entry.timestamp = QDateTime::fromString(json.value("timestamp").toString().trimmed(), Qt::ISODate);
        entry.username = json.value("username").toString().trimmed();
        entry.action = json.value("action").toString().trimmed();

        if (!entry.timestamp.isValid()) {
            entry.timestamp = QDateTime::currentDateTime();
        }

        return entry;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json["timestamp"] = timestamp.toString(Qt::ISODate);
        json["username"] = username;
        json["action"] = action;
        return json;
    }

    QString displayText() const
    {
        return QString("[%1] %2 - %3")
            .arg(timestamp.toLocalTime().toString("dd/MM/yyyy HH:mm:ss"), username, action);
    }

    static HistoryEntry create(const QString &username, const QString &action)
    {
        HistoryEntry entry;
        entry.timestamp = QDateTime::currentDateTimeUtc();
        entry.username = username;
        entry.action = action;
        return entry;
    }
};

#endif // HISTORYENTRY_H
