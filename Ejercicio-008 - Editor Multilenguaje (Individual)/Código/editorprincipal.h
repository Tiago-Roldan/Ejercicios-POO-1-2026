#ifndef EDITORPRINCIPAL_H
#define EDITORPRINCIPAL_H

#include "pantalla.h"
#include "perfildatos.h"
#include "validadorsintaxis.h"

#include <QMap>
#include <memory>

class QCloseEvent;
class QComboBox;
class QLabel;
class QListWidget;
class QPushButton;
class QTextEdit;
class EditorCodigo;

class EditorPrincipal : public Pantalla
{
    Q_OBJECT

public:
    explicit EditorPrincipal(const Configuracion &configuracion, Logger *logger, QWidget *parent = nullptr);
    ~EditorPrincipal() override = default;

    void inicializarUI() override;
    void conectarEventos() override;
    void cargarDatos() override;
    bool validarEstado() const override;
    void registrarEvento(const QString &descripcion) override;

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private slots:
    void cambiarLenguaje(const QString &lenguaje);
    void validarLinea(int numeroLinea, const QString &contenido);
    void procesarAtajo(const QString &atajo);
    void exportarJpg();

private:
    void construirValidadores();
    void actualizarResaltado();
    QString extensionLenguaje() const;
    std::unique_ptr<ValidadorSintaxis> crearValidador(const QString &lenguaje) const;

    EditorCodigo *editor_ = nullptr;
    QComboBox *lenguajeCombo_ = nullptr;
    QLabel *estadoLabel_ = nullptr;
    QLabel *diagnosticoLabel_ = nullptr;
    QListWidget *habilidadesList_ = nullptr;
    QLabel *nombrePerfilLabel_ = nullptr;
    QLabel *titularPerfilLabel_ = nullptr;
    QLabel *descripcionPerfilLabel_ = nullptr;
    QLabel *contactoPerfilLabel_ = nullptr;
    QPushButton *exportarBtn_ = nullptr;

    PerfilDatos perfil_;
    std::unique_ptr<ValidadorSintaxis> validadorActual_;
    QMap<int, QString> erroresPorLinea_;
    bool hayCambiosSinExportar_ = false;
};

#endif
