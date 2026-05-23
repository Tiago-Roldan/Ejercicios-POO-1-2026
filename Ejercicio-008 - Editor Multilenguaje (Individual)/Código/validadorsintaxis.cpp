#include "validadorsintaxis.h"

#include <QRegularExpression>

namespace {
ResultadoValidacion lineaVaciaOComentario(const QString &linea, const QString &prefijoComentario)
{
    const QString simplificada = linea.trimmed();
    if (simplificada.isEmpty() || simplificada.startsWith(prefijoComentario)) {
        return {true, QString()};
    }
    return {false, QString()};
}

bool parentesisBalanceados(const QString &texto)
{
    int balance = 0;
    for (const QChar c : texto) {
        if (c == '(') {
            ++balance;
        } else if (c == ')') {
            --balance;
            if (balance < 0) {
                return false;
            }
        }
    }
    return balance == 0;
}
}

QString ValidadorCpp::lenguaje() const
{
    return "C++";
}

ResultadoValidacion ValidadorCpp::validarLinea(const QString &linea) const
{
    const QString texto = linea.trimmed();
    if (texto.isEmpty() || texto.startsWith("//")) {
        return {true, QString()};
    }
    if (!parentesisBalanceados(texto)) {
        return {false, "Revisa los parentesis: parece que la linea no cierra bien."};
    }
    if (texto.endsWith("{") || texto.endsWith("}") || texto.startsWith("#")) {
        return {true, QString()};
    }
    if (texto.contains(QRegularExpression("^\\s*(if|for|while|switch)\\s*\\(.*\\)$"))) {
        return {false, "A esta estructura le falta abrir bloque o terminar la instruccion correctamente."};
    }
    if (!texto.endsWith(";")) {
        return {false, "En C++ la mayoria de las instrucciones terminan con ';'."};
    }
    return {true, QString()};
}

QString ValidadorPython::lenguaje() const
{
    return "Python";
}

ResultadoValidacion ValidadorPython::validarLinea(const QString &linea) const
{
    const QString texto = linea;
    const QString simplificado = texto.trimmed();
    if (simplificado.isEmpty() || simplificado.startsWith("#")) {
        return {true, QString()};
    }
    if (!parentesisBalanceados(simplificado)) {
        return {false, "La linea tiene parentesis desbalanceados."};
    }
    if (simplificado.endsWith(";")) {
        return {false, "En Python no hace falta terminar la linea con ';'."};
    }
    if (simplificado.contains(QRegularExpression("^\\s*(if|elif|for|while|def|class)\\b")) &&
        !simplificado.endsWith(':')) {
        return {false, "Esta linea necesita ':' al final para abrir el bloque."};
    }
    return {true, QString()};
}

QString ValidadorJava::lenguaje() const
{
    return "Java";
}

ResultadoValidacion ValidadorJava::validarLinea(const QString &linea) const
{
    const QString texto = linea.trimmed();
    if (texto.isEmpty() || texto.startsWith("//")) {
        return {true, QString()};
    }
    if (!parentesisBalanceados(texto)) {
        return {false, "La sentencia tiene parentesis desbalanceados."};
    }
    if (texto.endsWith("{") || texto.endsWith("}") ||
        texto.startsWith("package ") || texto.startsWith("import ")) {
        return {true, QString()};
    }
    if (texto.contains(QRegularExpression("^\\s*(if|for|while|switch)\\s*\\(.*\\)$"))) {
        return {false, "La estructura necesita abrir bloque con '{' o completar la instruccion."};
    }
    if (!texto.endsWith(";")) {
        return {false, "En Java esta linea deberia terminar con ';'."};
    }
    return {true, QString()};
}
