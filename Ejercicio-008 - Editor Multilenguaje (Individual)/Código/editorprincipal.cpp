#include "editorprincipal.h"

#include "editorcodigo.h"

#include <QCloseEvent>
#include <QComboBox>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QFocusEvent>
#include <QHBoxLayout>
#include <QImage>
#include <QKeyEvent>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QResizeEvent>
#include <QTextBlock>
#include <QTextDocument>
#include <QTextEdit>
#include <QVBoxLayout>

EditorPrincipal::EditorPrincipal(const Configuracion &configuracion, Logger *logger, QWidget *parent)
    : Pantalla(configuracion, logger, parent)
{
    construirValidadores();
    inicializarUI();
    conectarEventos();
    cargarDatos();
}

void EditorPrincipal::inicializarUI()
{
    setWindowTitle("Editor multilenguaje");
    setStyleSheet(
        "QWidget { background: #edf2f7; color: #17212b; }"
        "QLabel#panelTitulo { font-size: 22px; font-weight: 700; }"
        "QLabel#avatar { background: #0f766e; color: white; border-radius: 42px; font-size: 26px; font-weight: 700; }"
        "QComboBox, QPushButton { padding: 8px; border: 1px solid #9fb3c8; border-radius: 8px; background: white; }"
        "QPushButton { background: #0f766e; color: white; font-weight: 600; }"
        "QPushButton:hover { background: #0b5f59; }"
        "QListWidget { background: white; border: 1px solid #c7d3df; border-radius: 10px; }"
        "QPlainTextEdit { background: #10151c; color: #f4f7fb; border-radius: 12px; padding: 10px; font-family: Consolas; font-size: 12pt; }");

    auto *principal = new QHBoxLayout(this);
    principal->setContentsMargins(18, 18, 18, 18);
    principal->setSpacing(16);

    auto *panelEditor = new QVBoxLayout;
    panelEditor->setSpacing(10);

    auto *barra = new QHBoxLayout;
    auto *titulo = new QLabel("Editor de codigo");
    titulo->setObjectName("panelTitulo");
    lenguajeCombo_ = new QComboBox;
    lenguajeCombo_->addItems({"C++", "Python", "Java"});
    exportarBtn_ = new QPushButton("Exportar a JPG");
    barra->addWidget(titulo);
    barra->addStretch();
    barra->addWidget(new QLabel("Lenguaje:"));
    barra->addWidget(lenguajeCombo_);
    barra->addWidget(exportarBtn_);

    editor_ = new EditorCodigo;
    estadoLabel_ = new QLabel("Selecciona un lenguaje y comienza a escribir.");
    diagnosticoLabel_ = new QLabel("Sin errores en la linea actual.");
    diagnosticoLabel_->setStyleSheet("QLabel { color: #0f766e; font-weight: 600; }");

    panelEditor->addLayout(barra);
    panelEditor->addWidget(editor_, 1);
    panelEditor->addWidget(estadoLabel_);
    panelEditor->addWidget(diagnosticoLabel_);

    auto *panelPerfil = new QVBoxLayout;
    panelPerfil->setSpacing(10);

    auto *avatar = new QLabel("CR");
    avatar->setObjectName("avatar");
    avatar->setFixedSize(84, 84);
    avatar->setAlignment(Qt::AlignCenter);

    nombrePerfilLabel_ = new QLabel;
    nombrePerfilLabel_->setObjectName("panelTitulo");
    titularPerfilLabel_ = new QLabel;
    titularPerfilLabel_->setWordWrap(true);
    descripcionPerfilLabel_ = new QLabel;
    descripcionPerfilLabel_->setWordWrap(true);
    habilidadesList_ = new QListWidget;
    contactoPerfilLabel_ = new QLabel;
    contactoPerfilLabel_->setWordWrap(true);

    panelPerfil->addWidget(avatar, 0, Qt::AlignLeft);
    panelPerfil->addWidget(nombrePerfilLabel_);
    panelPerfil->addWidget(titularPerfilLabel_);
    panelPerfil->addWidget(descripcionPerfilLabel_);
    panelPerfil->addWidget(new QLabel("Habilidades"));
    panelPerfil->addWidget(habilidadesList_, 1);
    panelPerfil->addWidget(new QLabel("Contacto / Fuente"));
    panelPerfil->addWidget(contactoPerfilLabel_);

    QWidget *panelPerfilWidget = new QWidget;
    panelPerfilWidget->setLayout(panelPerfil);
    panelPerfilWidget->setFixedWidth(300);
    panelPerfilWidget->setStyleSheet(
        "QWidget { background: white; border-radius: 16px; }"
        "QLabel { background: transparent; }");

    principal->addLayout(panelEditor, 1);
    principal->addWidget(panelPerfilWidget);
}

void EditorPrincipal::conectarEventos()
{
    connect(lenguajeCombo_, &QComboBox::currentTextChanged,
            this, &EditorPrincipal::cambiarLenguaje);
    connect(editor_, &EditorCodigo::lineaAbandonada,
            this, &EditorPrincipal::validarLinea);
    connect(editor_, &EditorCodigo::atajoDetectado,
            this, &EditorPrincipal::procesarAtajo);
    connect(exportarBtn_, &QPushButton::clicked, this, &EditorPrincipal::exportarJpg);
    connect(editor_->document(), &QTextDocument::contentsChanged, this, [this]() {
        hayCambiosSinExportar_ = true;
    });
}

void EditorPrincipal::cargarDatos()
{
    perfil_ = PerfilDatos::cargarDesdePdf(configuracion_.rutaCv);
    nombrePerfilLabel_->setText(perfil_.nombre);
    titularPerfilLabel_->setText(perfil_.titular);
    descripcionPerfilLabel_->setText(perfil_.descripcion);
    contactoPerfilLabel_->setText(perfil_.contacto);
    habilidadesList_->clear();
    habilidadesList_->addItems(perfil_.habilidades);

    const int index = lenguajeCombo_->findText(configuracion_.lenguajePorDefecto);
    lenguajeCombo_->setCurrentIndex(index >= 0 ? index : 1);
    cambiarLenguaje(lenguajeCombo_->currentText());
    registrarEvento("Editor principal cargado");
}

bool EditorPrincipal::validarEstado() const
{
    return validadorActual_ != nullptr;
}

void EditorPrincipal::registrarEvento(const QString &descripcion)
{
    if (logger_) {
        logger_->registrar(QString("EditorPrincipal | %1").arg(descripcion));
    }
}

void EditorPrincipal::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_E) {
        registrarEvento("Atajo global Ctrl+E detectado en pantalla principal");
        exportarJpg();
        return;
    }
    QWidget::keyPressEvent(event);
}

void EditorPrincipal::mousePressEvent(QMouseEvent *event)
{
    registrarEvento(QString("Click en editor principal (%1,%2)")
                        .arg(event->position().x())
                        .arg(event->position().y()));
    QWidget::mousePressEvent(event);
}

void EditorPrincipal::resizeEvent(QResizeEvent *event)
{
    registrarEvento(QString("Editor redimensionado a %1x%2")
                        .arg(event->size().width())
                        .arg(event->size().height()));
    QWidget::resizeEvent(event);
}

void EditorPrincipal::closeEvent(QCloseEvent *event)
{
    registrarEvento("Solicitud de cierre del editor");
    if (hayCambiosSinExportar_) {
        const auto respuesta = QMessageBox::question(
            this,
            "Confirmar salida",
            "Hay cambios sin exportar. Deseas exportar el codigo antes de salir?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
            QMessageBox::Yes);

        if (respuesta == QMessageBox::Cancel) {
            event->ignore();
            return;
        }
        if (respuesta == QMessageBox::Yes) {
            exportarJpg();
        }
    }
    event->accept();
}

void EditorPrincipal::focusInEvent(QFocusEvent *event)
{
    registrarEvento("Editor principal obtuvo el foco");
    QWidget::focusInEvent(event);
}

void EditorPrincipal::focusOutEvent(QFocusEvent *event)
{
    registrarEvento("Editor principal perdio el foco y valida la linea activa");
    validarLinea(editor_->textCursor().blockNumber(), editor_->textCursor().block().text());
    QWidget::focusOutEvent(event);
}

void EditorPrincipal::cambiarLenguaje(const QString &lenguaje)
{
    validadorActual_ = crearValidador(lenguaje);
    erroresPorLinea_.clear();

    const QString contenidoActual = editor_->toPlainText().trimmed();
    if (contenidoActual.isEmpty() || contenidoActual.startsWith("# Escribe tu codigo aqui") ||
        contenidoActual.startsWith("// Escribe tu codigo aqui")) {
        if (lenguaje == "C++") {
            editor_->setPlainText(
                "#include <iostream>\n\n"
                "int main() {\n"
                "    std::cout << \"Hola Qt\" << std::endl;\n"
                "    return 0;\n"
                "}\n");
        } else if (lenguaje == "Java") {
            editor_->setPlainText(
                "public class Main {\n"
                "    public static void main(String[] args) {\n"
                "        System.out.println(\"Hola Qt\");\n"
                "    }\n"
                "}\n");
        } else {
            editor_->setPlainText(
                "# Escribe tu codigo aqui\n"
                "def main():\n"
                "    print(\"Hola Qt\")\n\n"
                "main()\n");
        }
    }

    actualizarResaltado();
    estadoLabel_->setText(QString("Lenguaje activo: %1").arg(lenguaje));
    diagnosticoLabel_->setText("Validador cambiado. La siguiente validacion usara el lenguaje seleccionado.");
    diagnosticoLabel_->setStyleSheet("QLabel { color: #0f766e; font-weight: 600; }");
    registrarEvento(QString("Lenguaje seleccionado: %1").arg(lenguaje));
}

void EditorPrincipal::validarLinea(int numeroLinea, const QString &contenido)
{
    if (!validadorActual_) {
        return;
    }

    const ResultadoValidacion resultado = validadorActual_->validarLinea(contenido);
    if (resultado.valido) {
        erroresPorLinea_.remove(numeroLinea);
        diagnosticoLabel_->setText(
            QString("Linea %1 validada correctamente.").arg(numeroLinea + 1));
        diagnosticoLabel_->setStyleSheet("QLabel { color: #0f766e; font-weight: 600; }");
    } else {
        erroresPorLinea_[numeroLinea] = resultado.mensaje;
        diagnosticoLabel_->setText(
            QString("Linea %1: %2").arg(numeroLinea + 1).arg(resultado.mensaje));
        diagnosticoLabel_->setStyleSheet("QLabel { color: #c53030; font-weight: 700; }");
    }

    actualizarResaltado();
    registrarEvento(QString("Validacion de linea %1 en %2: %3")
                        .arg(numeroLinea + 1)
                        .arg(validadorActual_->lenguaje())
                        .arg(resultado.valido ? "ok" : resultado.mensaje));
}

void EditorPrincipal::procesarAtajo(const QString &atajo)
{
    registrarEvento(QString("Atajo del editor detectado: %1").arg(atajo));
    if (atajo == "Ctrl+S" || atajo == "Ctrl+E") {
        exportarJpg();
    }
}

void EditorPrincipal::exportarJpg()
{
    const QStringList lineas = editor_->toPlainText().split('\n');
    const QFont fuente("Consolas", 14);
    QFontMetrics metricas(fuente);

    int anchoMaximo = 1200;
    for (const QString &linea : lineas) {
        anchoMaximo = qMax(anchoMaximo, metricas.horizontalAdvance(linea) + 80);
    }

    const int altoLinea = metricas.lineSpacing() + 4;
    const int alto = qMax(700, 120 + lineas.size() * altoLinea);
    QImage imagen(anchoMaximo, alto, QImage::Format_RGB32);
    imagen.fill(QColor("#f8fafc"));

    QPainter painter(&imagen);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(QRect(20, 20, anchoMaximo - 40, alto - 40), QColor("#ffffff"));
    painter.setPen(QColor("#102030"));
    painter.setFont(QFont("Segoe UI", 16, QFont::Bold));
    painter.drawText(QRect(40, 35, anchoMaximo - 80, 30),
                     QString("Exportacion de codigo - %1").arg(lenguajeCombo_->currentText()));

    painter.setFont(fuente);
    int y = 90;
    for (int i = 0; i < lineas.size(); ++i) {
        const bool tieneError = erroresPorLinea_.contains(i);
        if (tieneError) {
            painter.fillRect(QRect(35, y - metricas.ascent(), anchoMaximo - 70, altoLinea),
                             QColor(255, 230, 230));
        }
        painter.setPen(QColor("#64748b"));
        painter.drawText(40, y, QString("%1").arg(i + 1, 3));
        painter.setPen(tieneError ? QColor("#b91c1c") : QColor("#0f172a"));
        painter.drawText(90, y, lineas.at(i));
        y += altoLinea;
    }
    painter.end();

    QDir().mkpath(configuracion_.rutaExportacion);
    const QString rutaArchivo = QDir(configuracion_.rutaExportacion).filePath(
        QString("codigo_%1_%2.%3.jpg")
            .arg(lenguajeCombo_->currentText().toLower().remove('+'))
            .arg(QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss"))
            .arg(extensionLenguaje()));

    if (imagen.save(rutaArchivo, "JPG", 95)) {
        hayCambiosSinExportar_ = false;
        estadoLabel_->setText(QString("Codigo exportado a: %1").arg(rutaArchivo));
        registrarEvento(QString("Exportacion JPG completada en %1").arg(rutaArchivo));
    } else {
        estadoLabel_->setText("No se pudo exportar la imagen JPG.");
        registrarEvento("Fallo la exportacion JPG");
    }
}

void EditorPrincipal::construirValidadores()
{
    validadorActual_ = crearValidador(configuracion_.lenguajePorDefecto);
}

void EditorPrincipal::actualizarResaltado()
{
    QList<QTextEdit::ExtraSelection> selecciones;
    for (auto it = erroresPorLinea_.cbegin(); it != erroresPorLinea_.cend(); ++it) {
        QTextEdit::ExtraSelection seleccion;
        QTextCursor cursor(editor_->document()->findBlockByNumber(it.key()));
        cursor.select(QTextCursor::LineUnderCursor);
        seleccion.cursor = cursor;
        seleccion.format.setBackground(QColor(255, 212, 212));
        seleccion.format.setProperty(QTextFormat::FullWidthSelection, true);
        selecciones.append(seleccion);
    }
    editor_->setExtraSelections(selecciones);
}

QString EditorPrincipal::extensionLenguaje() const
{
    const QString lenguaje = lenguajeCombo_->currentText();
    if (lenguaje == "C++") {
        return "cpp";
    }
    if (lenguaje == "Java") {
        return "java";
    }
    return "py";
}

std::unique_ptr<ValidadorSintaxis> EditorPrincipal::crearValidador(const QString &lenguaje) const
{
    if (lenguaje == "C++") {
        return std::make_unique<ValidadorCpp>();
    }
    if (lenguaje == "Java") {
        return std::make_unique<ValidadorJava>();
    }
    return std::make_unique<ValidadorPython>();
}
