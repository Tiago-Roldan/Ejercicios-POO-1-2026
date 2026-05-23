#include "kanbanapi.h"

#include <QEventLoop>
#include <QJsonArray>
#include <QJsonParseError>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QUrl>
#include <algorithm>

namespace
{
constexpr int RequestTimeoutMs = 10000;
}

bool KanbanApi::hasToken() const
{
    return !accessToken.isEmpty();
}

bool KanbanApi::sendAuthRequest(const QString &path,
                                const QJsonObject &payload,
                                QJsonDocument &responseDoc,
                                QString &errorMessage)
{
    const QUrl url(authBaseUrl + path);
    if (!url.isValid()) {
        errorMessage = "URL auth invalida.";
        return false;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QNetworkReply *reply = networkManager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(RequestTimeoutMs);
    loop.exec();

    if (!timeoutTimer.isActive()) {
        reply->abort();
        reply->deleteLater();
        errorMessage = "Timeout de red.";
        return false;
    }

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const auto networkError = reply->error();
    const QByteArray body = reply->readAll();
    QJsonParseError parseError{};
    responseDoc = QJsonDocument::fromJson(body, &parseError);
    reply->deleteLater();

    if (networkError != QNetworkReply::NoError || status >= 400) {
        if (responseDoc.isObject() && responseDoc.object().contains("detail")) {
            errorMessage = responseDoc.object().value("detail").toString();
        } else if (responseDoc.isObject() && responseDoc.object().contains("message")) {
            errorMessage = responseDoc.object().value("message").toString();
        } else if (!body.isEmpty()) {
            errorMessage = QString::fromUtf8(body);
        } else {
            errorMessage = "Credenciales invalidas.";
        }
        return false;
    }

    if (parseError.error != QJsonParseError::NoError && !body.isEmpty()) {
        errorMessage = "Respuesta JSON invalida.";
        return false;
    }

    return true;
}

bool KanbanApi::sendKanbanRequest(const QString &path,
                                  const QByteArray &method,
                                  const QJsonObject &payload,
                                  QJsonDocument &responseDoc,
                                  QString &errorMessage)
{
    const QUrl url(kanbanBaseUrl + path);
    if (!url.isValid()) {
        errorMessage = "URL kanban invalida.";
        return false;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    if (!accessToken.isEmpty()) {
        request.setRawHeader("Authorization", ("Bearer " + accessToken).toUtf8());
    }

    QNetworkReply *reply = nullptr;
    if (method == "GET") {
        reply = networkManager.get(request);
    } else if (method == "POST") {
        reply = networkManager.post(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    } else if (method == "PUT") {
        reply = networkManager.put(request, QJsonDocument(payload).toJson(QJsonDocument::Compact));
    } else if (method == "DELETE") {
        const QByteArray body = payload.isEmpty()
            ? QByteArray()
            : QJsonDocument(payload).toJson(QJsonDocument::Compact);
        reply = networkManager.sendCustomRequest(request, "DELETE", body);
    } else {
        errorMessage = "Metodo HTTP no soportado.";
        return false;
    }

    QTimer timeoutTimer;
    timeoutTimer.setSingleShot(true);
    QEventLoop loop;
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeoutTimer, &QTimer::timeout, &loop, &QEventLoop::quit);
    timeoutTimer.start(RequestTimeoutMs);
    loop.exec();

    if (!timeoutTimer.isActive()) {
        reply->abort();
        reply->deleteLater();
        errorMessage = "Timeout de red.";
        return false;
    }

    const int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray body = reply->readAll();
    QJsonParseError parseError{};
    responseDoc = QJsonDocument::fromJson(body, &parseError);

    if (reply->error() != QNetworkReply::NoError || status >= 400) {
        if (responseDoc.isObject() && responseDoc.object().contains("detail")) {
            errorMessage = responseDoc.object().value("detail").toString();
        } else if (!body.isEmpty()) {
            errorMessage = QString::fromUtf8(body);
        } else {
            errorMessage = reply->errorString();
        }
        reply->deleteLater();
        return false;
    }

    if (parseError.error != QJsonParseError::NoError && !body.isEmpty()) {
        errorMessage = "Respuesta JSON invalida.";
        reply->deleteLater();
        return false;
    }

    reply->deleteLater();
    return true;
}

bool KanbanApi::login(const QString &username, const QString &password, QString &errorMessage)
{
    QJsonDocument responseDoc;
    const QJsonObject payload{
        {"username", username.trimmed()},
        {"password", password}
    };

    if (!sendAuthRequest("/login", payload, responseDoc, errorMessage)) {
        return false;
    }

    if (!responseDoc.isObject() || !responseDoc.object().contains("access_token")) {
        errorMessage = "No se recibio token.";
        return false;
    }

    accessToken = responseDoc.object().value("access_token").toString();
    if (accessToken.isEmpty()) {
        errorMessage = "Token vacio.";
        return false;
    }

    return true;
}

bool KanbanApi::registerUser(const QString &username, const QString &password, QString &errorMessage)
{
    QJsonDocument responseDoc;
    const QJsonObject payload{
        {"username", username.trimmed()},
        {"password", password}
    };

    if (!sendAuthRequest("/register", payload, responseDoc, errorMessage)) {
        return false;
    }

    return true;
}

bool KanbanApi::loadBoard(QList<ColumnData> &boardColumns, QString &errorMessage)
{
    QJsonDocument responseDoc;
    if (!sendKanbanRequest("/board", "GET", QJsonObject{}, responseDoc, errorMessage)) {
        return false;
    }

    if (!responseDoc.isObject()) {
        errorMessage = "Formato de /board invalido.";
        return false;
    }

    boardColumns.clear();
    const QJsonArray columnsArray = responseDoc.object().value("columns").toArray();
    for (const QJsonValue &columnValue : columnsArray) {
        const QJsonObject columnObj = columnValue.toObject();
        ColumnData column;
        column.id = columnObj.value("id").toInt();
        column.title = columnObj.value("title").toString();
        column.position = columnObj.value("position").toInt();

        const QJsonArray cardsArray = columnObj.value("cards").toArray();
        for (const QJsonValue &cardValue : cardsArray) {
            const QJsonObject cardObj = cardValue.toObject();
            CardData card;
            card.id = cardObj.value("id").toInt();
            card.title = cardObj.value("title").toString();
            card.description = cardObj.value("description").toString();
            card.position = cardObj.value("position").toInt();
            card.columnId = column.id;
            column.cards.append(card);
        }

        std::sort(column.cards.begin(), column.cards.end(), [](const CardData &a, const CardData &b) {
            return a.position < b.position;
        });

        boardColumns.append(column);
    }

    std::sort(boardColumns.begin(), boardColumns.end(), [](const ColumnData &a, const ColumnData &b) {
        return a.position < b.position;
    });

    return true;
}

bool KanbanApi::createColumn(const QString &title, QString &errorMessage)
{
    QJsonDocument responseDoc;
    return sendKanbanRequest("/columns", "POST", {{"title", title.trimmed()}}, responseDoc, errorMessage);
}

bool KanbanApi::editColumn(int columnId, const QString &newTitle, QString &errorMessage)
{
    QJsonDocument responseDoc;
    return sendKanbanRequest(QString("/columns/%1").arg(columnId), "PUT", {{"title", newTitle.trimmed()}}, responseDoc, errorMessage);
}

bool KanbanApi::deleteColumn(int columnId, QString &errorMessage)
{
    QJsonDocument responseDoc;
    return sendKanbanRequest(QString("/columns/%1").arg(columnId), "DELETE", QJsonObject{}, responseDoc, errorMessage);
}

bool KanbanApi::createCard(const QString &title, const QString &description, int columnId, QString &errorMessage)
{
    QJsonDocument responseDoc;
    const QJsonObject payload{
        {"title", title.trimmed()},
        {"description", description},
        {"column_id", columnId}
    };
    return sendKanbanRequest("/cards", "POST", payload, responseDoc, errorMessage);
}

bool KanbanApi::editCard(int cardId, const QString &title, const QString &description, QString &errorMessage)
{
    QJsonDocument responseDoc;
    const QJsonObject payload{
        {"title", title.trimmed()},
        {"description", description}
    };
    return sendKanbanRequest(QString("/cards/%1").arg(cardId), "PUT", payload, responseDoc, errorMessage);
}

bool KanbanApi::deleteCard(int cardId, QString &errorMessage)
{
    QJsonDocument responseDoc;
    return sendKanbanRequest(QString("/cards/%1").arg(cardId), "DELETE", QJsonObject{}, responseDoc, errorMessage);
}

bool KanbanApi::moveCard(int cardId, int toColumnId, int newPosition, QString &errorMessage)
{
    QJsonDocument responseDoc;
    const QJsonObject payload{
        {"to_column_id", toColumnId},
        {"new_position", newPosition}
    };
    return sendKanbanRequest(QString("/cards/%1/move").arg(cardId), "POST", payload, responseDoc, errorMessage);
}

bool KanbanApi::reorderCard(int cardId, int newPosition, QString &errorMessage)
{
    QJsonDocument responseDoc;
    return sendKanbanRequest(QString("/cards/%1/reorder").arg(cardId),
                             "POST",
                             {{"new_position", newPosition}},
                             responseDoc,
                             errorMessage);
}
