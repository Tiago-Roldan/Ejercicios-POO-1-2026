#ifndef PLANNEROPTIONS_H
#define PLANNEROPTIONS_H

#include <QString>
#include <QStringList>

namespace PlannerOptions {

inline QStringList statusOptions()
{
    return QStringList{"Pendiente", "En progreso", "Entregado"};
}

inline QStringList priorityOptions()
{
    return QStringList{"Alta", "Media", "Baja"};
}

inline QStringList filterOptions(const QStringList &options)
{
    QStringList filters{"Todos"};
    filters.append(options);
    return filters;
}

inline int priorityRank(const QString &priority)
{
    if (priority == "Alta") {
        return 0;
    }
    if (priority == "Media") {
        return 1;
    }
    return 2;
}

} // namespace PlannerOptions

#endif // PLANNEROPTIONS_H
