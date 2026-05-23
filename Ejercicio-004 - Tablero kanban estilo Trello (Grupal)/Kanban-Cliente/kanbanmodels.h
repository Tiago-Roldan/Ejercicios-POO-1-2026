#ifndef KANBANMODELS_H
#define KANBANMODELS_H

#include <QList>
#include <QString>

struct CardData
{
    int id = -1;
    QString title;
    QString description;
    int position = 0;
    int columnId = -1;
};

struct ColumnData
{
    int id = -1;
    QString title;
    int position = 0;
    QList<CardData> cards;
};

#endif // KANBANMODELS_H
