#ifndef KANBANWIDGET_H
#define KANBANWIDGET_H

#include <QWidget>
#include <QMap>

#include "boardstate.h"
#include "kanbanapi.h"

class QLabel;
class QPushButton;
class QScrollArea;
class QHBoxLayout;
class QListWidget;
class QGroupBox;
class QTimer;

class KanbanWidget : public QWidget
{
    Q_OBJECT

public:
    explicit KanbanWidget(KanbanApi *apiClient, QWidget *parent = nullptr);

private:
    void setupUi();
    void connectSignals();
    void refreshBoard();
    void rebuildBoardUi();
    void captureSelectionFromUi();
    void applySelectionToUi();
    void updateColumnSelectionVisual();
    void updateStatus(const QString &message, bool isError = false);

    bool createColumn();
    bool editColumn();
    bool deleteColumn();
    bool createCard();
    bool editCard();
    bool deleteCard();
    bool moveCardToNeighborColumn(int direction);
    bool reorderCard(int direction);

    int findColumnIndexById(int columnId) const;
    int selectedCardId() const;
    int selectedColumnId() const;
    QList<int> cardOrderForColumn(int columnId) const;

    QLabel *statusLabel = nullptr;

    QPushButton *addColumnButton = nullptr;
    QPushButton *editColumnButton = nullptr;
    QPushButton *deleteColumnButton = nullptr;
    QPushButton *addCardButton = nullptr;
    QPushButton *editCardButton = nullptr;
    QPushButton *deleteCardButton = nullptr;
    QPushButton *moveLeftButton = nullptr;
    QPushButton *moveRightButton = nullptr;
    QPushButton *moveUpButton = nullptr;
    QPushButton *moveDownButton = nullptr;
    QPushButton *refreshButton = nullptr;

    QWidget *columnsContainer = nullptr;
    QHBoxLayout *columnsLayout = nullptr;
    QScrollArea *scrollArea = nullptr;

    QTimer *pollTimer = nullptr;
    KanbanApi *api = nullptr;

    BoardState state;
    QMap<int, QListWidget *> columnLists;
    QMap<int, QGroupBox *> columnGroups;
};

#endif // KANBANWIDGET_H
