#ifndef MODObLOQUEADO_H
#define MODObLOQUEADO_H

#include "pantalla.h"

#include <QTimer>

class QLabel;

class ModoBloqueado : public Pantalla
{
    Q_OBJECT

public:
    explicit ModoBloqueado(const Configuracion &configuracion, Logger *logger, QWidget *parent = nullptr);

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
    void actualizarCuentaRegresiva();

private:
    QLabel *mensajeLabel_ = nullptr;
    QLabel *contadorLabel_ = nullptr;
    QTimer temporizador_;
    int segundosRestantes_ = 0;
};

#endif
