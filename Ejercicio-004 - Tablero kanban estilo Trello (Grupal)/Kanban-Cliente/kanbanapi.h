#ifndef KANBANAPI_H
#define KANBANAPI_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QNetworkAccessManager>
#include <QString>

#include "kanbanmodels.h"

class KanbanApi
{
public:
    bool registerUser(const QString &username, const QString &password, QString &errorMessage);
    bool login(const QString &username, const QString &password, QString &errorMessage);
    bool loadBoard(QList<ColumnData> &boardColumns, QString &errorMessage);

    bool createColumn(const QString &title, QString &errorMessage);
    bool editColumn(int columnId, const QString &newTitle, QString &errorMessage);
    bool deleteColumn(int columnId, QString &errorMessage);

    bool createCard(const QString &title, const QString &description, int columnId, QString &errorMessage);
    bool editCard(int cardId, const QString &title, const QString &description, QString &errorMessage);
    bool deleteCard(int cardId, QString &errorMessage);

    bool moveCard(int cardId, int toColumnId, int newPosition, QString &errorMessage);
    bool reorderCard(int cardId, int newPosition, QString &errorMessage);

    bool hasToken() const;

private:
    bool sendKanbanRequest(const QString &path,
                           const QByteArray &method,
                           const QJsonObject &payload,
                           QJsonDocument &responseDoc,
                           QString &errorMessage);

    bool sendAuthRequest(const QString &path,
                         const QJsonObject &payload,
                         QJsonDocument &responseDoc,
                         QString &errorMessage);

    const QString authBaseUrl = "http://147.93.189.45:8000";
    const QString kanbanBaseUrl = "http://147.93.189.45:8001";
    QString accessToken;
    QNetworkAccessManager networkManager;
};

#endif // KANBANAPI_H
