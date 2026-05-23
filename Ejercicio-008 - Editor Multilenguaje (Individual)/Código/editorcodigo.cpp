#include "editorcodigo.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QTextBlock>

EditorCodigo::EditorCodigo(QWidget *parent)
    : QPlainTextEdit(parent)
{
    connect(this, &QPlainTextEdit::cursorPositionChanged,
            this, &EditorCodigo::detectarCambioLinea);
}

void EditorCodigo::keyPressEvent(QKeyEvent *event)
{
    if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_S) {
        emit atajoDetectado("Ctrl+S");
    } else if (event->modifiers() == Qt::ControlModifier && event->key() == Qt::Key_E) {
        emit atajoDetectado("Ctrl+E");
    }
    QPlainTextEdit::keyPressEvent(event);
}

void EditorCodigo::focusOutEvent(QFocusEvent *event)
{
    emitirLineaActual();
    QPlainTextEdit::focusOutEvent(event);
}

void EditorCodigo::mousePressEvent(QMouseEvent *event)
{
    QPlainTextEdit::mousePressEvent(event);
}

void EditorCodigo::detectarCambioLinea()
{
    const int nuevaLinea = textCursor().blockNumber();
    if (nuevaLinea != lineaActual_) {
        const QTextBlock bloqueAnterior = document()->findBlockByNumber(lineaActual_);
        emit lineaAbandonada(lineaActual_, bloqueAnterior.text());
        lineaActual_ = nuevaLinea;
    }
}

void EditorCodigo::emitirLineaActual()
{
    const QTextBlock bloque = document()->findBlockByNumber(textCursor().blockNumber());
    emit lineaAbandonada(textCursor().blockNumber(), bloque.text());
}
