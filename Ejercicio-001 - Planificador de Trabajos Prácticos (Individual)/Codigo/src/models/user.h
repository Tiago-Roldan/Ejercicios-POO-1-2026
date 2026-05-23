#ifndef USER_H
#define USER_H

#include <QJsonObject>
#include <QString>

struct User
{
    QString username;
    QString fullName;
    QString passwordHash;

    static User fromJson(const QJsonObject &json)
    {
        User user;
        user.username = json.value("username").toString().trimmed();
        user.fullName = json.value("fullName").toString().trimmed();
        user.passwordHash = json.value("passwordHash").toString().trimmed();
        return user;
    }

    QJsonObject toJson() const
    {
        QJsonObject json;
        json["username"] = username;
        json["fullName"] = fullName;
        json["passwordHash"] = passwordHash;
        return json;
    }
};

#endif // USER_H
