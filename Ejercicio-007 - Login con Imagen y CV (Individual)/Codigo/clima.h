#ifndef CLIMA_H
#define CLIMA_H

#include "pantalla.h"

#include <QDateTime>
#include <QLabel>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimeZone>
#include <QTimer>

struct WeatherData
{
    bool valid = false;
    bool offline = false;
    double temperature = 0.0;
    QString description;
    QString city;
    QString timezoneId;
    QString uiMessage;
};

class Clima : public Pantalla
{
    Q_OBJECT

public:
    explicit Clima(QWidget *parent = nullptr);

    void aplicarConfiguracion(const AppConfig &config) override;
    void actualizarContenido() override;
    WeatherData currentData() const;

signals:
    void weatherUpdated(const WeatherData &data);

private slots:
    void onWeatherReplyFinished(QNetworkReply *reply);
    void updateClock();

private:
    enum RequestMode
    {
        RequestByPlaceId = 0,
        RequestByCoordinates = 1,
        RequestByCoordinatesWithUnitAlias = 2
    };

    void buildUi();
    void requestWeather();
    void requestWeather(RequestMode mode);
    void applyWeatherData(const WeatherData &data);
    void showApiError(const QString &message);
    bool isConnectivityError(QNetworkReply::NetworkError error) const;
    QNetworkRequest createWeatherRequest(RequestMode mode) const;
    WeatherData offlineWeather(const QString &message) const;

    AppConfig m_config;
    QNetworkAccessManager *m_networkManager = nullptr;
    QLabel *m_cityLabel = nullptr;
    QLabel *m_temperatureLabel = nullptr;
    QLabel *m_descriptionLabel = nullptr;
    QLabel *m_timeLabel = nullptr;
    QLabel *m_statusLabel = nullptr;
    QTimer *m_clockTimer = nullptr;
    WeatherData m_currentData;
    QTimeZone m_timezone;
    bool m_requestInFlight = false;
};

#endif
