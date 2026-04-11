#include <QApplication>
#include <QDateTime>
#include <QDoubleSpinBox>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

#include "monitorclient.h"

static QLabel *makeMetricLabel(const QString &title)
{
    auto *label = new QLabel(title);
    label->setObjectName("metricTitle");
    return label;
}

static QLabel *makeMetricValue(const QString &value)
{
    auto *label = new QLabel(value);
    label->setObjectName("metricValue");
    return label;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWidget window;
    window.setWindowTitle("Panel de Monitoreo VPS");
    window.resize(980, 620);

    auto *rootLayout = new QVBoxLayout(&window);
    rootLayout->setContentsMargins(20, 20, 20, 20);
    rootLayout->setSpacing(16);

    auto *header = new QHBoxLayout();
    auto *title = new QLabel("VPS Health Console");
    title->setObjectName("title");
    auto *statusBadge = new QLabel("SIN DATOS");
    statusBadge->setObjectName("statusBadge");
    statusBadge->setMinimumWidth(140);
    statusBadge->setAlignment(Qt::AlignCenter);
    header->addWidget(title, 1);
    header->addWidget(statusBadge, 0);
    rootLayout->addLayout(header);

    auto *mainLayout = new QHBoxLayout();
    mainLayout->setSpacing(16);
    rootLayout->addLayout(mainLayout, 1);

    auto *leftPanel = new QFrame();
    leftPanel->setObjectName("panel");
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(16, 16, 16, 16);
    leftLayout->setSpacing(12);

    auto *metricsTitle = new QLabel("Metricas principales");
    metricsTitle->setObjectName("sectionTitle");
    leftLayout->addWidget(metricsTitle);

    auto *grid = new QGridLayout();
    grid->setHorizontalSpacing(18);
    grid->setVerticalSpacing(10);

    auto *uptimeValue = makeMetricValue("-");
    auto *loadValue = makeMetricValue("-");
    auto *memoryValue = makeMetricValue("-");
    auto *diskValue = makeMetricValue("-");
    auto *lastCheckValue = makeMetricValue("-");

    grid->addWidget(makeMetricLabel("Uptime"), 0, 0);
    grid->addWidget(uptimeValue, 0, 1);
    grid->addWidget(makeMetricLabel("Carga"), 1, 0);
    grid->addWidget(loadValue, 1, 1);
    grid->addWidget(makeMetricLabel("Memoria"), 2, 0);
    grid->addWidget(memoryValue, 2, 1);
    grid->addWidget(makeMetricLabel("Disco"), 3, 0);
    grid->addWidget(diskValue, 3, 1);
    grid->addWidget(makeMetricLabel("Ultimo chequeo"), 4, 0);
    grid->addWidget(lastCheckValue, 4, 1);

    leftLayout->addLayout(grid);

    auto *historyTitle = new QLabel("Historial reciente");
    historyTitle->setObjectName("sectionTitle");
    leftLayout->addWidget(historyTitle);

    auto *historyList = new QListWidget();
    historyList->setObjectName("history");
    historyList->setMinimumHeight(200);
    leftLayout->addWidget(historyList, 1);

    auto *rightPanel = new QFrame();
    rightPanel->setObjectName("panel");
    auto *rightLayout = new QVBoxLayout(rightPanel);
    rightLayout->setContentsMargins(16, 16, 16, 16);
    rightLayout->setSpacing(12);

    auto *controlsTitle = new QLabel("Controles");
    controlsTitle->setObjectName("sectionTitle");
    rightLayout->addWidget(controlsTitle);

    // QLineEdit: permite configurar el endpoint del VPS desde la app.
    auto *endpointLabel = new QLabel("Endpoint de salud");
    endpointLabel->setObjectName("controlLabel");
    auto *endpointEdit = new QLineEdit();
    endpointEdit->setPlaceholderText("https://tu-vps.com/health");
    endpointEdit->setToolTip("URL del endpoint que devuelve el JSON de salud.");
    rightLayout->addWidget(endpointLabel);
    rightLayout->addWidget(endpointEdit);

    // QSpinBox: define el intervalo de chequeo automatico.
    auto *intervalLabel = new QLabel("Intervalo de chequeo (seg)");
    intervalLabel->setObjectName("controlLabel");
    auto *intervalSpin = new QSpinBox();
    intervalSpin->setRange(5, 3600);
    intervalSpin->setValue(30);
    intervalSpin->setToolTip("Cada cuantos segundos se consulta el VPS.");
    rightLayout->addWidget(intervalLabel);
    rightLayout->addWidget(intervalSpin);

    // QDoubleSpinBox: umbral para alertar por carga alta.
    auto *thresholdLabel = new QLabel("Umbral de carga (alerta)");
    thresholdLabel->setObjectName("controlLabel");
    auto *thresholdSpin = new QDoubleSpinBox();
    thresholdSpin->setRange(0.1, 16.0);
    thresholdSpin->setSingleStep(0.1);
    thresholdSpin->setValue(1.0);
    thresholdSpin->setToolTip("Si la carga supera este valor, se marca alerta.");
    rightLayout->addWidget(thresholdLabel);
    rightLayout->addWidget(thresholdSpin);

    // QPushButton: refresco manual inmediato del estado.
    auto *refreshButton = new QPushButton("Refrescar ahora");
    refreshButton->setToolTip("Ejecuta un chequeo manual al instante.");
    rightLayout->addWidget(refreshButton);

    rightLayout->addStretch(1);

    mainLayout->addWidget(leftPanel, 2);
    mainLayout->addWidget(rightPanel, 1);

    QString style = R"(
        QWidget {
            background: #0f172a;
            color: #e2e8f0;
            font-family: "Segoe UI";
            font-size: 13px;
        }
        #title {
            font-size: 26px;
            font-weight: 700;
            color: #f8fafc;
        }
        #statusBadge {
            border-radius: 14px;
            padding: 6px 14px;
            font-weight: 700;
            background: #334155;
            color: #e2e8f0;
        }
        #panel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                        stop:0 #111827, stop:1 #1e293b);
            border: 1px solid #1f2937;
            border-radius: 14px;
        }
        #sectionTitle {
            font-size: 16px;
            font-weight: 600;
            color: #f1f5f9;
        }
        #metricTitle {
            color: #94a3b8;
        }
        #metricValue {
            font-weight: 600;
            color: #e2e8f0;
        }
        #controlLabel {
            color: #cbd5f5;
        }
        QLineEdit, QSpinBox, QDoubleSpinBox, QListWidget {
            background: #0b1220;
            border: 1px solid #1f2937;
            border-radius: 8px;
            padding: 6px;
        }
        QPushButton {
            background: #2563eb;
            border: none;
            border-radius: 10px;
            padding: 10px;
            font-weight: 600;
        }
        QPushButton:hover {
            background: #1d4ed8;
        }
        QListWidget {
            outline: none;
        }
    )";
    window.setStyleSheet(style);

    MonitorClient client;
    client.setIntervalSec(intervalSpin->value());
    client.setLoadThreshold(thresholdSpin->value());

    auto updateStatusBadge = [&](const QString &status) {
        statusBadge->setText(status);
        if (status == "OK") {
            statusBadge->setStyleSheet("background:#16a34a; color:#f8fafc;");
        } else if (status == "ALERTA") {
            statusBadge->setStyleSheet("background:#f59e0b; color:#0f172a;");
        } else if (status == "CAIDO") {
            statusBadge->setStyleSheet("background:#ef4444; color:#f8fafc;");
        } else {
            statusBadge->setStyleSheet("background:#334155; color:#e2e8f0;");
        }
    };

    QObject::connect(endpointEdit, &QLineEdit::editingFinished, [&]() {
        client.setEndpoint(QUrl(endpointEdit->text().trimmed()));
    });

    QObject::connect(intervalSpin, qOverload<int>(&QSpinBox::valueChanged), [&](int value) {
        client.setIntervalSec(value);
    });

    QObject::connect(thresholdSpin, qOverload<double>(&QDoubleSpinBox::valueChanged), [&](double value) {
        client.setLoadThreshold(value);
    });

    QObject::connect(refreshButton, &QPushButton::clicked, [&]() {
        client.setEndpoint(QUrl(endpointEdit->text().trimmed()));
        client.refresh();
    });

    QObject::connect(&client, &MonitorClient::dataUpdated, [&](const HealthData &data) {
        uptimeValue->setText(data.uptimeText);
        loadValue->setText(QString::number(data.load, 'f', 2));

        if (data.memTotalMB > 0) {
            memoryValue->setText(QString("%1 / %2 MB").arg(data.memUsedMB).arg(data.memTotalMB));
        } else {
            memoryValue->setText("-");
        }

        if (data.diskTotalGB > 0) {
            diskValue->setText(QString("%1 / %2 GB").arg(data.diskUsedGB).arg(data.diskTotalGB));
        } else {
            diskValue->setText("-");
        }

        const QString timeText = data.timestamp.toString("dd/MM/yyyy HH:mm:ss");
        lastCheckValue->setText(timeText);
        updateStatusBadge(data.statusText);

        const QString history = QString("[%1] %2 | carga %3")
                                    .arg(timeText)
                                    .arg(data.statusText)
                                    .arg(QString::number(data.load, 'f', 2));
        historyList->insertItem(0, history);
        while (historyList->count() > 6) {
            delete historyList->takeItem(historyList->count() - 1);
        }
    });

    QObject::connect(&client, &MonitorClient::errorOccurred, [&](const QString &message) {
        historyList->insertItem(0, QString("Alerta: %1").arg(message));
    });

    client.start();
    window.show();

    return app.exec();
}
