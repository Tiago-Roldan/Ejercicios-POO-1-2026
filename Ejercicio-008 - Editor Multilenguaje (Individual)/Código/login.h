#ifndef LOGIN_H
#define LOGIN_H

#include "pantalla.h"

#include <QDateTime>

class QLabel;
class QLineEdit;
class QPushButton;

class Login : public Pantalla
{
    Q_OBJECT

public:
    explicit Login(const Configuracion &configuracion, Logger *logger, QWidget *parent = nullptr);

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
    void intentarLogin();

private:
    QLineEdit *usuarioEdit_ = nullptr;
    QLineEdit *passwordEdit_ = nullptr;
    QLabel *estadoLabel_ = nullptr;
    QPushButton *ingresarBtn_ = nullptr;
    int intentosFallidos_ = 0;
};

#endif
