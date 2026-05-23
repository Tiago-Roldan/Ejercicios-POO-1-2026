#include "backgrounddownloader.h"

#include "logger.h"

#include <QDir>
#include <QFile>
#include <QLinearGradient>
#include <QNetworkProxy>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QPainter>
#include <QPainterPath>

BackgroundDownloader::BackgroundDownloader(QObject *parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
{
    connect(m_networkManager, &QNetworkAccessManager::finished,
            this, &BackgroundDownloader::onImageReplyFinished);
}

void BackgroundDownloader::applyConfiguration(const AppConfig &config)
{
    m_config = config;

    if (!m_config.proxyHost.isEmpty() && m_config.proxyPort > 0) {
        QNetworkProxy proxy(QNetworkProxy::HttpProxy, m_config.proxyHost, m_config.proxyPort,
                            m_config.proxyUser, m_config.proxyPassword);
        m_networkManager->setProxy(proxy);
    } else {
        m_networkManager->setProxy(QNetworkProxy::NoProxy);
    }
}

void BackgroundDownloader::prepareBackground()
{
    const QString cachePath = cacheFilePath();
    if (QFile::exists(cachePath)) {
        QPixmap cached(cachePath);
        if (!cached.isNull()) {
            Logger::write(QStringLiteral("Fondo cargado desde cache local."));
            emit progressMessage(QStringLiteral("Fondo listo desde cache local."));
            emit backgroundReady(cached, false);
            return;
        }
    }

    const QUrl url(m_config.backgroundUrl);
    if (!url.isValid()) {
        Logger::write(QStringLiteral("URL de fondo invalida. Se usa placeholder."));
        emitFromCacheOrPlaceholder(QStringLiteral("No se pudo validar la imagen de fondo. Se usa modo offline."), true);
        return;
    }

    QNetworkRequest request(url);
    request.setRawHeader("Accept", "image/jpeg,image/*");
    request.setRawHeader("User-Agent", "QtLoginClimaCV/1.0");

    Logger::write(QStringLiteral("Descargando imagen principal."));
    emit progressMessage(QStringLiteral("Descargando imagen principal..."));
    m_waitingForDownload = true;
    m_networkManager->get(request);
}

void BackgroundDownloader::onImageReplyFinished(QNetworkReply *reply)
{
    if (!m_waitingForDownload || reply->request().url().toString() != m_config.backgroundUrl) {
        reply->deleteLater();
        return;
    }

    m_waitingForDownload = false;

    if (reply->error() != QNetworkReply::NoError) {
        Logger::write(QStringLiteral("Error al descargar imagen de fondo: %1").arg(reply->errorString()));
        emitFromCacheOrPlaceholder(QStringLiteral("Sin internet para la imagen. Se usa fondo alternativo."), true);
        reply->deleteLater();
        return;
    }

    const QByteArray imageData = reply->readAll();
    QPixmap pixmap;
    if (!pixmap.loadFromData(imageData, "JPG") && !pixmap.loadFromData(imageData)) {
        Logger::write(QStringLiteral("La imagen descargada no pudo interpretarse."));
        emitFromCacheOrPlaceholder(QStringLiteral("La imagen descargada no es valida. Se usa fondo alternativo."), true);
        reply->deleteLater();
        return;
    }

    QFile file(cacheFilePath());
    if (file.open(QIODevice::WriteOnly)) {
        file.write(imageData);
        file.close();
    }

    Logger::write(QStringLiteral("Imagen principal descargada y guardada en cache."));
    emit progressMessage(QStringLiteral("Imagen principal lista."));
    emit backgroundReady(pixmap, false);
    reply->deleteLater();
}

QString BackgroundDownloader::cacheFilePath() const
{
    return QDir(m_config.cacheDirectory).filePath(QStringLiteral("background.jpg"));
}

QPixmap BackgroundDownloader::placeholderPixmap() const
{
    QPixmap pixmap(1600, 900);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QLinearGradient gradient(0, 0, pixmap.width(), pixmap.height());
    gradient.setColorAt(0.0, QColor(0xf6, 0xfb, 0xff));
    gradient.setColorAt(1.0, QColor(0xdc, 0xea, 0xf7));
    painter.fillRect(pixmap.rect(), gradient);

    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor(255, 255, 255, 130));
    painter.drawPolygon(QPolygonF() << QPointF(0, 220) << QPointF(450, 120) << QPointF(620, 330) << QPointF(160, 470));
    painter.setBrush(QColor(194, 219, 240, 150));
    painter.drawPolygon(QPolygonF() << QPointF(520, 120) << QPointF(980, 40) << QPointF(1300, 240) << QPointF(860, 410));
    painter.setBrush(QColor(223, 235, 246, 180));
    painter.drawPolygon(QPolygonF() << QPointF(1080, 380) << QPointF(1490, 220) << QPointF(1600, 520) << QPointF(1360, 780));

    painter.setPen(QColor(0x1f, 0x47, 0x68));
    QFont titleFont = painter.font();
    titleFont.setPointSize(28);
    titleFont.setBold(true);
    painter.setFont(titleFont);
    painter.drawText(QRect(90, 120, 720, 60), Qt::AlignLeft | Qt::AlignVCenter, QStringLiteral("Perfil profesional"));

    QFont bodyFont = painter.font();
    bodyFont.setPointSize(16);
    bodyFont.setBold(false);
    painter.setFont(bodyFont);
    painter.drawText(QRect(90, 190, 860, 120), Qt::TextWordWrap,
                     QStringLiteral("Modo offline activo. El contenido principal sigue disponible con un fondo generado localmente."));

    return pixmap;
}

void BackgroundDownloader::emitFromCacheOrPlaceholder(const QString &message, bool offlineMode)
{
    const QString cachePath = cacheFilePath();
    if (QFile::exists(cachePath)) {
        QPixmap cached(cachePath);
        if (!cached.isNull()) {
            emit progressMessage(QStringLiteral("Se uso la imagen guardada en cache."));
            emit backgroundReady(cached, offlineMode);
            return;
        }
    }

    emit progressMessage(message);
    emit backgroundReady(placeholderPixmap(), offlineMode);
}
