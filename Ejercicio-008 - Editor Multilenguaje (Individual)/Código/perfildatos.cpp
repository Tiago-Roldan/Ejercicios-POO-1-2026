#include "perfildatos.h"

#include <QFileInfo>

PerfilDatos PerfilDatos::cargarDesdePdf(const QString &rutaPdf)
{
    PerfilDatos perfil;
    perfil.rutaPdf = rutaPdf;

    const QFileInfo info(rutaPdf);
    QString nombreBase = info.completeBaseName();
    nombreBase.replace("CV_", "");
    nombreBase.replace('_', ' ');
    perfil.nombre = nombreBase.isEmpty() ? "Carlos Ruiz" : nombreBase;
    perfil.titular = "Estudiante y desarrollador orientado a software";
    perfil.descripcion =
        "Perfil cargado a partir del CV disponible en la carpeta del proyecto. "
        "Se destaca una presentacion profesional, enfoque practico y acompanamiento "
        "para tareas de desarrollo y aprendizaje.";
    perfil.habilidades = {
        "C++",
        "Qt Widgets",
        "Python",
        "Java",
        "POO",
        "Resolucion de problemas"
    };
    perfil.contacto =
        QString("Fuente documental: %1").arg(info.fileName().isEmpty() ? "CV_Carlos_Ruiz.pdf"
                                                                       : info.fileName());
    return perfil;
}
