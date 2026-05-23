#include "ventana.h"

#include "logger.h"
#include "ui_ventana.h"

#include <QDesktopServices>
#include <QFileInfo>
#include <QPainter>
#include <QResizeEvent>
#include <QUrl>

Ventana::Ventana(QWidget *parent)
    : Pantalla(parent)
    , ui(new Ui::Ventana)
{
    ui->setupUi(this);

    connect(ui->openCvButton, &QPushButton::clicked, this, &Ventana::openCv);

    ui->profilePhotoLabel->setPixmap(buildAvatar());
}

Ventana::~Ventana()
{
    delete ui;
}

void Ventana::aplicarConfiguracion(const AppConfig &config)
{
    m_config = config;
    buildProfileCard(false);
}

void Ventana::actualizarContenido()
{
    refreshBackground();
}

void Ventana::setBackground(const QPixmap &pixmap, bool offlineMode)
{
    m_background = pixmap;
    buildProfileCard(offlineMode);
    refreshBackground();
}

void Ventana::resizeEvent(QResizeEvent *event)
{
    Pantalla::resizeEvent(event);

    ui->backgroundLabel->setGeometry(rect());
    const int cardWidth = qMax(420, qMin(width() - 120, 780));
    const int cardHeight = qMax(420, qMin(height() - 120, 640));
    ui->overlayCard->setGeometry(60, 60, cardWidth, cardHeight);
    refreshBackground();
}

void Ventana::openCv()
{
    const QFileInfo cvFile(m_config.cvFilePath);
    if (!cvFile.exists()) {
        ui->cvMetaLabel->setText(QStringLiteral("No se encontro el PDF del CV en la carpeta del proyecto."));
        Logger::write(QStringLiteral("No se pudo abrir el CV porque no existe el archivo."));
        return;
    }

    QDesktopServices::openUrl(QUrl::fromLocalFile(cvFile.absoluteFilePath()));
    Logger::write(QStringLiteral("CV abierto desde la ventana principal."));
}

void Ventana::refreshBackground()
{
    if (m_background.isNull()) {
        return;
    }

    ui->backgroundLabel->setGeometry(rect());
    ui->backgroundLabel->setPixmap(m_background.scaled(size(), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
}

void Ventana::buildProfileCard(bool offlineMode)
{
    const QFileInfo cvInfo(m_config.cvFilePath);
    const QString cvState = cvInfo.exists()
                                ? QStringLiteral("CV vinculado: %1 KB").arg(cvInfo.size() / 1024)
                                : QStringLiteral("CV pendiente de ubicacion");

    ui->nameLabel->setText(QStringLiteral("Carlos Ruiz"));
    ui->headlineLabel->setText(QStringLiteral("Estudiante de Ingenieria Informatica | Perfil orientado a desarrollo Qt/C++"));
    ui->summaryLabel->setText(
        QStringLiteral("Presentacion profesional inspirada en LinkedIn. Perfil enfocado en interfaces de escritorio, consumo de APIs, "
                       "arquitectura orientada a objetos y experiencias claras para el usuario. La ventana principal integra el CV en PDF "
                       "disponible en el proyecto y una tarjeta visual elegante para evitar una pantalla vacia."));
    ui->experienceLabel->setText(
        QStringLiteral("Fortalezas destacadas:\n"
                       "- Desarrollo con C++ y Qt Widgets\n"
                       "- Integracion de servicios REST con QNetworkAccessManager\n"
                       "- Modelado orientado a objetos y uso de herencia\n"
                       "- Registro de eventos, validacion de datos y soporte offline"));
    ui->locationLabel->setText(QStringLiteral("Cordoba Capital, Cordoba, Argentina"));
    ui->cvMetaLabel->setText(offlineMode
                                 ? QStringLiteral("%1 | Fondo offline activo").arg(cvState)
                                 : cvState);
}

QPixmap Ventana::buildAvatar() const
{
    QPixmap pixmap(180, 180);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(Qt::NoPen);
    painter.setBrush(QColor("#d8e6f4"));
    painter.drawEllipse(0, 0, 180, 180);

    painter.setBrush(QColor("#ffffff"));
    painter.drawEllipse(45, 28, 90, 90);
    painter.drawRoundedRect(35, 110, 110, 48, 20, 20);

    painter.setPen(QColor("#17324d"));
    QFont font = painter.font();
    font.setPointSize(22);
    font.setBold(true);
    painter.setFont(font);
    painter.drawText(QRect(0, 0, 180, 180), Qt::AlignCenter, QStringLiteral("CR"));

    return pixmap;
}
