#include "clima.h"

#include "logger.h"

#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QVBoxLayout>

Clima::Clima(QWidget *parent)
    : Pantalla(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_clockTimer(new QTimer(this))
{
    buildUi();

    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &Clima::onWeatherReplyFinished);
    connect(m_clockTimer, &QTimer::timeout,
            this, &Clima::updateClock);

    m_clockTimer->start(1000);
}

void Clima::aplicarConfiguracion(const AppConfig &config)
{
    m_config = config;

    if (!m_config.proxyHost.isEmpty() && m_config.proxyPort > 0) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_config.proxyHost, m_config.proxyPort,
                            m_config.proxyUser, m_config.proxyPassword);
        m_networkManager->setProxy(proxy);
    } else {
        m_networkManager->setProxy(QNetworkProxy::NoProxy);
    }

    m_timezone = QTimeZone(m_config.timezoneId.toUtf8());
    if (!m_timezone.isValid()) {
        m_timezone = QTimeZone::utc();
    }
}

void Clima::actualizarContenido()
{
    requestWeather();
    updateClock();
}

WeatherData Clima::currentData() const
{
    return m_currentData;
}

void Clima::onWeatherReplyFinished(QNetworkReply *reply)
{
    if (reply->request().url().toString() != m_config.weatherUrl
        && !reply->request().url().toString().startsWith(m_config.weatherUrl + QLatin1Char('?'))) {
        reply->deleteLater();
        return;
    }

    m_requestInFlight = false;

    const int requestMode = reply->request().attribute(QNetworkRequest::User).toInt();
    const int httpStatus = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    const QByteArray responseBody = reply->readAll();

    if (reply->error() != QNetworkReply::NoError) {
        Logger::write(QStringLiteral("Error de clima. HTTP: %1 | Qt: %2 | URL: %3 | Respuesta: %4")
                          .arg(httpStatus)
                          .arg(reply->errorString())
                          .arg(reply->request().url().toString())
                          .arg(QString::fromUtf8(responseBody)));

        if ((httpStatus == 422 || httpStatus == 400) && requestMode == RequestByPlaceId) {
            Logger::write(QStringLiteral("Reintentando clima con coordenadas por error de validacion."));
            requestWeather(RequestByCoordinates);
            reply->deleteLater();
            return;
        }

        if ((httpStatus == 422 || httpStatus == 400) && requestMode == RequestByCoordinates) {
            Logger::write(QStringLiteral("Reintentando clima con alias de unidad por error de validacion."));
            requestWeather(RequestByCoordinatesWithUnitAlias);
            reply->deleteLater();
            return;
        }

        if (isConnectivityError(reply->error())) {
            applyWeatherData(offlineWeather(QStringLiteral("Sin internet. Mostrando clima simulado.")));
        } else {
            showApiError(QStringLiteral("La API respondio con un error. Revise la configuracion o intente de nuevo."));
        }
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(responseBody, &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        Logger::write(QStringLiteral("Respuesta de clima invalida. HTTP: %1 | Respuesta: %2")
                          .arg(httpStatus)
                          .arg(QString::fromUtf8(responseBody)));
        showApiError(QStringLiteral("La API devolvio un formato inesperado."));
        reply->deleteLater();
        return;
    }

    const QJsonObject root = document.object();
    const QJsonObject current = root.value(QStringLiteral("current")).toObject();
    if (current.isEmpty() || !current.value(QStringLiteral("temperature")).isDouble()) {
        Logger::write(QStringLiteral("Respuesta de clima sin campos esperados. HTTP: %1 | Respuesta: %2")
                          .arg(httpStatus)
                          .arg(QString::fromUtf8(responseBody)));
        showApiError(QStringLiteral("La API no devolvio los datos esperados."));
        reply->deleteLater();
        return;
    }

    WeatherData data;
    data.valid = true;
    data.offline = false;
    data.temperature = current.value(QStringLiteral("temperature")).toDouble();
    data.description = current.value(QStringLiteral("summary")).toString();
    if (data.description.isEmpty()) {
        data.description = current.value(QStringLiteral("weather")).toString();
    }
    if (data.description.isEmpty()) {
        data.description = QStringLiteral("Condiciones estables");
    }
    data.city = m_config.cityName;
    data.timezoneId = root.value(QStringLiteral("timezone")).toString(m_config.timezoneId);
    data.uiMessage = QStringLiteral("Clima actualizado correctamente.");

    Logger::write(QStringLiteral("Consulta de clima exitosa para %1.").arg(data.city));
    applyWeatherData(data);
    reply->deleteLater();
}

void Clima::updateClock()
{
    const QTimeZone activeZone = m_timezone.isValid() ? m_timezone : QTimeZone::utc();
    const QString timeText = QDateTime::currentDateTimeUtc()
                                 .toTimeZone(activeZone)
                                 .toString(QStringLiteral("dd/MM/yyyy hh:mm:ss"));
    m_timeLabel->setText(QStringLiteral("Hora local: %1").arg(timeText));
}

void Clima::buildUi()
{
    auto *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 18);
    layout->setSpacing(6);

    m_cityLabel = new QLabel(QStringLiteral("Cordoba Capital"), this);
    m_temperatureLabel = new QLabel(QStringLiteral("--.- C"), this);
    m_descriptionLabel = new QLabel(QStringLiteral("Consultando servicio meteorologico..."), this);
    m_timeLabel = new QLabel(QStringLiteral("Hora local: --"), this);
    m_statusLabel = new QLabel(QString(), this);

    m_cityLabel->setStyleSheet(QStringLiteral("font-size: 18px; font-weight: 700; color: #17324d;"));
    m_temperatureLabel->setStyleSheet(QStringLiteral("font-size: 30px; font-weight: 700; color: #0a84ff;"));
    m_descriptionLabel->setStyleSheet(QStringLiteral("font-size: 14px; color: #2f455c;"));
    m_timeLabel->setStyleSheet(QStringLiteral("font-size: 13px; color: #4f6478;"));
    m_statusLabel->setStyleSheet(QStringLiteral("font-size: 12px; color: #60758a;"));
    m_descriptionLabel->setWordWrap(true);
    m_statusLabel->setWordWrap(true);

    layout->addWidget(m_cityLabel);
    layout->addWidget(m_temperatureLabel);
    layout->addWidget(m_descriptionLabel);
    layout->addWidget(m_timeLabel);
    layout->addWidget(m_statusLabel);

    setStyleSheet(QStringLiteral(
        "QWidget {"
        "background: rgba(255, 255, 255, 0.86);"
        "border: 1px solid rgba(15, 76, 129, 0.15);"
        "border-radius: 18px;"
        "}"));
}

void Clima::requestWeather()
{
    if (m_requestInFlight) {
        return;
    }

    requestWeather(RequestByPlaceId);
}

void Clima::applyWeatherData(const WeatherData &data)
{
    m_currentData = data;
    m_cityLabel->setText(data.city);
    m_temperatureLabel->setText(QStringLiteral("%1 C").arg(data.temperature, 0, 'f', 1));
    m_descriptionLabel->setText(data.description);
    m_statusLabel->setText(data.uiMessage);

    m_timezone = QTimeZone(data.timezoneId.toUtf8());
    if (!m_timezone.isValid()) {
        m_timezone = QTimeZone(m_config.timezoneId.toUtf8());
    }

    updateClock();
    emit weatherUpdated(m_currentData);
}

void Clima::showApiError(const QString &message)
{
    m_currentData = WeatherData();
    m_cityLabel->setText(m_config.cityName);
    m_temperatureLabel->setText(QStringLiteral("--.- C"));
    m_descriptionLabel->setText(QStringLiteral("No fue posible obtener el clima real."));
    m_statusLabel->setText(message);
}

bool Clima::isConnectivityError(QNetworkReply::NetworkError error) const
{
    switch (error) {
    case QNetworkReply::ConnectionRefusedError:
    case QNetworkReply::RemoteHostClosedError:
    case QNetworkReply::HostNotFoundError:
    case QNetworkReply::TimeoutError:
    case QNetworkReply::OperationCanceledError:
    case QNetworkReply::SslHandshakeFailedError:
    case QNetworkReply::TemporaryNetworkFailureError:
    case QNetworkReply::NetworkSessionFailedError:
    case QNetworkReply::ProxyConnectionRefusedError:
    case QNetworkReply::ProxyConnectionClosedError:
    case QNetworkReply::ProxyNotFoundError:
    case QNetworkReply::ProxyTimeoutError:
    case QNetworkReply::ProxyAuthenticationRequiredError:
    case QNetworkReply::UnknownNetworkError:
    case QNetworkReply::UnknownProxyError:
        return true;
    default:
        return false;
    }
}

QNetworkRequest Clima::createWeatherRequest(RequestMode mode) const
{
    QUrl url(m_config.weatherUrl);
    QUrlQuery query;

    if (mode == RequestByPlaceId && !m_config.cityPlaceId.trimmed().isEmpty()) {
        query.addQueryItem(QStringLiteral("place_id"), m_config.cityPlaceId);
    } else {
        query.addQueryItem(QStringLiteral("lat"), m_config.latitude);
        query.addQueryItem(QStringLiteral("lon"), m_config.longitude);
    }

    query.addQueryItem(QStringLiteral("sections"), QStringLiteral("current"));
    query.addQueryItem(QStringLiteral("timezone"), QStringLiteral("auto"));
    query.addQueryItem(QStringLiteral("language"), m_config.language);

    if (mode == RequestByCoordinatesWithUnitAlias) {
        query.addQueryItem(QStringLiteral("unit"), m_config.units);
    } else {
        query.addQueryItem(QStringLiteral("units"), m_config.units);
    }

    query.addQueryItem(QStringLiteral("key"), m_config.apiKey);
    url.setQuery(query);

    QNetworkRequest request(url);
    request.setAttribute(QNetworkRequest::User, static_cast<int>(mode));
    request.setRawHeader("Accept", "application/json");
    request.setRawHeader("User-Agent", "QtLoginClimaCV/1.0");
    return request;
}

void Clima::requestWeather(RequestMode mode)
{
    const QNetworkRequest request = createWeatherRequest(mode);
    m_requestInFlight = true;
    Logger::write(QStringLiteral("Iniciando consulta de clima. Modo de intento: %1 URL: %2")
                      .arg(static_cast<int>(mode))
                      .arg(request.url().toString()));
    m_networkManager->get(request);
}

WeatherData Clima::offlineWeather(const QString &message) const
{
    WeatherData data;
    data.valid = true;
    data.offline = true;
    data.temperature = 21.5;
    data.description = QStringLiteral("Cielo parcialmente nublado (simulado)");
    data.city = m_config.cityName;
    data.timezoneId = m_config.timezoneId;
    data.uiMessage = message;
    return data;
}
