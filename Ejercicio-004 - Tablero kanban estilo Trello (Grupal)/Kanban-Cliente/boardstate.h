#ifndef BOARDSTATE_H
#define BOARDSTATE_H

#include <QList>

#include "kanbanmodels.h"

class BoardState
{
public:
    QList<ColumnData> columns;
    int selectedColumnId = -1;
    int selectedCardId = -1;

    static bool sameBoard(const QList<ColumnData> &a, const QList<ColumnData> &b)
    {
        if (a.size() != b.size()) {
            return false;
        }

        for (int i = 0; i < a.size(); ++i) {
            const ColumnData &ca = a[i];
            const ColumnData &cb = b[i];
            if (ca.id != cb.id || ca.title != cb.title || ca.position != cb.position || ca.cards.size() != cb.cards.size()) {
                return false;
            }

            for (int j = 0; j < ca.cards.size(); ++j) {
                const CardData &ra = ca.cards[j];
                const CardData &rb = cb.cards[j];
                if (ra.id != rb.id || ra.title != rb.title || ra.description != rb.description || ra.position != rb.position
                    || ra.columnId != rb.columnId) {
                    return false;
                }
            }
        }

        return true;
    }

    bool hasChanges(const QList<ColumnData> &incoming) const
    {
        return !sameBoard(columns, incoming);
    }

    void replaceBoard(const QList<ColumnData> &incoming)
    {
        columns = incoming;
    }

    void syncSelection()
    {
        bool cardFound = false;
        bool columnFound = false;

        for (const ColumnData &column : columns) {
            if (column.id == selectedColumnId) {
                columnFound = true;
            }
            for (const CardData &card : column.cards) {
                if (card.id == selectedCardId) {
                    cardFound = true;
                    selectedColumnId = column.id;
                    break;
                }
            }
            if (cardFound) {
                break;
            }
        }

        if (!cardFound) {
            selectedCardId = -1;
        }

        if (!columnFound && !cardFound) {
            selectedColumnId = columns.isEmpty() ? -1 : columns.first().id;
        }
    }
};

#endif // BOARDSTATE_H
