#ifndef VENTANA_H
#define VENTANA_H

#include "appconfig.h"
#include "pantalla.h"

#include <QPixmap>

QT_BEGIN_NAMESPACE
namespace Ui {
class Ventana;
}
QT_END_NAMESPACE

class Ventana : public Pantalla
{
    Q_OBJECT

public:
    explicit Ventana(QWidget *parent = nullptr);
    ~Ventana() override;

    void aplicarConfiguracion(const AppConfig &config) override;
    void actualizarContenido() override;
    void setBackground(const QPixmap &pixmap, bool offlineMode);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void openCv();

private:
    void refreshBackground();
    void buildProfileCard(bool offlineMode);
    QPixmap buildAvatar() const;

    Ui::Ventana *ui = nullptr;
    AppConfig m_config;
    QPixmap m_background;
};

#endif
