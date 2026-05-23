#ifndef EDITORCODIGO_H
#define EDITORCODIGO_H

#include <QPlainTextEdit>

class EditorCodigo : public QPlainTextEdit
{
    Q_OBJECT

public:
    explicit EditorCodigo(QWidget *parent = nullptr);

signals:
    void lineaAbandonada(int numeroLinea, const QString &contenido);
    void atajoDetectado(const QString &atajo);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private slots:
    void detectarCambioLinea();

private:
    int lineaActual_ = 0;
    void emitirLineaActual();
};

#endif
