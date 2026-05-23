#ifndef PRACTICALWORK_H
#define PRACTICALWORK_H

#include <QDate>
#include <QJsonObject>
#include <QString>

struct PracticalWork
{
    QString id;
    QString title;
    QString subject;
    QDate dueDate;
    QString status;
    QString priority;
    QString notes;

    static PracticalWork fromJson(const QJsonObject &json)
    {
        PracticalWork work;
        work.id = json.value("id").toString().trimmed();
        work.title = json.value("title").toString().trimmed();
        work.subject = json.value("subject").toString().trimmed();
        work.dueDate = QDate::fromString(json.value("dueDate").toString().trimmed(), Qt::ISODate);
        work.status = json.value("status").toString().trimmed();
        work.priority = json.value("priority").toString().trimmed();
        work.notes = json.value("notes").toString();

        if (!work.dueDate.isValid()) {
            work.dueDate = QDate::currentDate();
        }

        return work;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json["id"] = id;
        json["title"] = title;
        json["subject"] = subject;
        json["dueDate"] = dueDate.toString(Qt::ISODate);
        json["status"] = status;
        json["priority"] = priority;
        json["notes"] = notes;
        return json;
    }
};

#endif // PRACTICALWORK_H
