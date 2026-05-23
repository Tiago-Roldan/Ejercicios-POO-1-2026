#include "kanbanwidget.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QFrame>
#include <QScrollArea>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>

namespace
{
constexpr int PollIntervalMs = 4000;
}

KanbanWidget::KanbanWidget(KanbanApi *apiClient, QWidget *parent) : QWidget(parent), api(apiClient)
{
    setupUi();
    connectSignals();
    refreshBoard();
    pollTimer->start();
}

void KanbanWidget::setupUi()
{
    setWindowTitle("Kanban Colaborativo");

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(16, 16, 16, 16);
    mainLayout->setSpacing(12);

    auto *titleLabel = new QLabel("Tablero Kanban");
    titleLabel->setObjectName("AppTitle");
    mainLayout->addWidget(titleLabel);

    auto *topBar = new QWidget;
    topBar->setObjectName("TopBar");
    auto *controlsLayout = new QHBoxLayout(topBar);
    controlsLayout->setContentsMargins(14, 12, 14, 12);
    controlsLayout->setSpacing(18);
    addColumnButton = new QPushButton("Crear columna");
    editColumnButton = new QPushButton("Editar columna");
    deleteColumnButton = new QPushButton("Eliminar columna");
    addCardButton = new QPushButton("Crear tarjeta");
    editCardButton = new QPushButton("Editar tarjeta");
    deleteCardButton = new QPushButton("Eliminar tarjeta");
    moveLeftButton = new QPushButton("Mover izq");
    moveRightButton = new QPushButton("Mover der");
    moveUpButton = new QPushButton("Subir");
    moveDownButton = new QPushButton("Bajar");
    refreshButton = new QPushButton("Refrescar");

    addColumnButton->setObjectName("PrimaryButton");
    addCardButton->setObjectName("PrimaryButton");
    deleteColumnButton->setObjectName("DangerButton");
    deleteCardButton->setObjectName("DangerButton");

    auto *columnsSection = new QWidget;
    auto *columnsSectionLayout = new QVBoxLayout(columnsSection);
    columnsSectionLayout->setContentsMargins(0, 0, 0, 0);
    columnsSectionLayout->setSpacing(6);
    auto *columnsTitle = new QLabel("Columnas");
    columnsTitle->setObjectName("SectionTitle");
    auto *columnsButtons = new QHBoxLayout;
    columnsButtons->setContentsMargins(0, 0, 0, 0);
    columnsButtons->setSpacing(8);
    columnsButtons->addWidget(addColumnButton);
    columnsButtons->addWidget(editColumnButton);
    columnsButtons->addWidget(deleteColumnButton);
    columnsSectionLayout->addWidget(columnsTitle);
    columnsSectionLayout->addLayout(columnsButtons);

    auto *cardsSection = new QWidget;
    auto *cardsSectionLayout = new QVBoxLayout(cardsSection);
    cardsSectionLayout->setContentsMargins(0, 0, 0, 0);
    cardsSectionLayout->setSpacing(6);
    auto *cardsTitle = new QLabel("Tarjetas");
    cardsTitle->setObjectName("SectionTitle");
    auto *cardsButtonsTop = new QHBoxLayout;
    cardsButtonsTop->setContentsMargins(0, 0, 0, 0);
    cardsButtonsTop->setSpacing(8);
    cardsButtonsTop->addWidget(addCardButton);
    cardsButtonsTop->addWidget(editCardButton);
    cardsButtonsTop->addWidget(deleteCardButton);
    auto *cardsButtonsBottom = new QHBoxLayout;
    cardsButtonsBottom->setContentsMargins(0, 0, 0, 0);
    cardsButtonsBottom->setSpacing(8);
    cardsButtonsBottom->addWidget(moveLeftButton);
    cardsButtonsBottom->addWidget(moveRightButton);
    cardsButtonsBottom->addWidget(moveUpButton);
    cardsButtonsBottom->addWidget(moveDownButton);
    cardsSectionLayout->addWidget(cardsTitle);
    cardsSectionLayout->addLayout(cardsButtonsTop);
    cardsSectionLayout->addLayout(cardsButtonsBottom);

    auto *refreshSection = new QWidget;
    auto *refreshSectionLayout = new QVBoxLayout(refreshSection);
    refreshSectionLayout->setContentsMargins(0, 0, 0, 0);
    refreshSectionLayout->setSpacing(6);
    auto *refreshTitle = new QLabel("Actualizacion");
    refreshTitle->setObjectName("SectionTitle");
    refreshSectionLayout->addWidget(refreshTitle);
    refreshSectionLayout->addWidget(refreshButton);

    auto *separator1 = new QFrame;
    separator1->setFrameShape(QFrame::VLine);
    separator1->setObjectName("SectionDivider");
    auto *separator2 = new QFrame;
    separator2->setFrameShape(QFrame::VLine);
    separator2->setObjectName("SectionDivider");

    controlsLayout->addWidget(columnsSection);
    controlsLayout->addWidget(separator1);
    controlsLayout->addWidget(cardsSection, 1);
    controlsLayout->addWidget(separator2);
    controlsLayout->addWidget(refreshSection);
    mainLayout->addWidget(topBar);

    scrollArea = new QScrollArea;
    scrollArea->setWidgetResizable(true);
    columnsContainer = new QWidget;
    columnsLayout = new QHBoxLayout(columnsContainer);
    columnsLayout->setSpacing(12);
    columnsLayout->setContentsMargins(8, 8, 8, 8);
    columnsLayout->addStretch(1);
    scrollArea->setWidget(columnsContainer);
    mainLayout->addWidget(scrollArea, 1);

    statusLabel = new QLabel("Cargando tablero...");
    mainLayout->addWidget(statusLabel);

    pollTimer = new QTimer(this);
    pollTimer->setInterval(PollIntervalMs);
}

void KanbanWidget::connectSignals()
{
    connect(addColumnButton, &QPushButton::clicked, this, &KanbanWidget::createColumn);
    connect(editColumnButton, &QPushButton::clicked, this, &KanbanWidget::editColumn);
    connect(deleteColumnButton, &QPushButton::clicked, this, &KanbanWidget::deleteColumn);
    connect(addCardButton, &QPushButton::clicked, this, &KanbanWidget::createCard);
    connect(editCardButton, &QPushButton::clicked, this, &KanbanWidget::editCard);
    connect(deleteCardButton, &QPushButton::clicked, this, &KanbanWidget::deleteCard);
    connect(moveLeftButton, &QPushButton::clicked, this, [this]() { moveCardToNeighborColumn(-1); });
    connect(moveRightButton, &QPushButton::clicked, this, [this]() { moveCardToNeighborColumn(1); });
    connect(moveUpButton, &QPushButton::clicked, this, [this]() { reorderCard(-1); });
    connect(moveDownButton, &QPushButton::clicked, this, [this]() { reorderCard(1); });
    connect(refreshButton, &QPushButton::clicked, this, &KanbanWidget::refreshBoard);
    connect(pollTimer, &QTimer::timeout, this, &KanbanWidget::refreshBoard);
}

void KanbanWidget::updateStatus(const QString &message, bool isError)
{
    statusLabel->setText(message);
    statusLabel->setStyleSheet(isError ? "color: #a00000;" : "color: #1d3557;");
}

void KanbanWidget::captureSelectionFromUi()
{
    for (auto it = columnLists.constBegin(); it != columnLists.constEnd(); ++it) {
        const QList<QListWidgetItem *> selectedItems = it.value()->selectedItems();
        if (!selectedItems.isEmpty()) {
            QListWidgetItem *item = selectedItems.first();
            state.selectedColumnId = it.key();
            state.selectedCardId = item->data(Qt::UserRole).toInt();
            return;
        }
    }
}

void KanbanWidget::applySelectionToUi()
{
    for (QListWidget *list : columnLists) {
        list->clearSelection();
    }

    if (state.selectedCardId < 0 || !columnLists.contains(state.selectedColumnId)) {
        return;
    }

    QListWidget *list = columnLists.value(state.selectedColumnId);
    for (int i = 0; i < list->count(); ++i) {
        QListWidgetItem *item = list->item(i);
        if (item->data(Qt::UserRole).toInt() == state.selectedCardId) {
            list->setCurrentItem(item);
            break;
        }
    }
}

void KanbanWidget::refreshBoard()
{
    if (api == nullptr || !api->hasToken()) {
        return;
    }

    captureSelectionFromUi();

    QString errorMessage;
    QList<ColumnData> incomingColumns;
    if (!api->loadBoard(incomingColumns, errorMessage)) {
        updateStatus("Error al cargar tablero: " + errorMessage, true);
        return;
    }

    if (!state.hasChanges(incomingColumns)) {
        return;
    }

    state.replaceBoard(incomingColumns);
    state.syncSelection();

    rebuildBoardUi();
    applySelectionToUi();
    updateColumnSelectionVisual();
    updateStatus("Tablero actualizado.");
}

void KanbanWidget::rebuildBoardUi()
{
    columnLists.clear();
    columnGroups.clear();

    QLayoutItem *item = nullptr;
    while ((item = columnsLayout->takeAt(0)) != nullptr) {
        if (item->widget() != nullptr) {
            item->widget()->deleteLater();
        }
        delete item;
    }

    for (const ColumnData &column : state.columns) {
        auto *group = new QGroupBox(column.title);
        group->setMinimumWidth(250);
        group->setProperty("selected", column.id == state.selectedColumnId);
        auto *layout = new QVBoxLayout(group);

        auto *selectColumnButton = new QPushButton("Seleccionar columna");
        selectColumnButton->setObjectName("PrimaryButton");
        layout->addWidget(selectColumnButton);
        connect(selectColumnButton, &QPushButton::clicked, this, [this, column]() {
            state.selectedColumnId = column.id;
            state.selectedCardId = -1;
            for (QListWidget *list : columnLists) {
                list->clearSelection();
            }
            updateColumnSelectionVisual();
            updateStatus("Columna seleccionada: " + column.title);
        });

        auto *list = new QListWidget;
        list->setSelectionMode(QAbstractItemView::SingleSelection);
        for (const CardData &card : column.cards) {
            auto *cardItem = new QListWidgetItem(card.title);
            cardItem->setToolTip(card.description);
            cardItem->setData(Qt::UserRole, card.id);
            cardItem->setData(Qt::UserRole + 1, column.id);
            list->addItem(cardItem);
        }
        layout->addWidget(list);
        columnsLayout->addWidget(group);
        columnLists.insert(column.id, list);
        columnGroups.insert(column.id, group);

        connect(list, &QListWidget::itemClicked, this, [this, list](QListWidgetItem *clicked) {
            for (QListWidget *otherList : columnLists) {
                if (otherList != list) {
                    otherList->clearSelection();
                }
            }
            state.selectedColumnId = clicked->data(Qt::UserRole + 1).toInt();
            state.selectedCardId = clicked->data(Qt::UserRole).toInt();
            updateColumnSelectionVisual();
            list->setCurrentItem(clicked);
        });
    }

    columnsLayout->addStretch(1);
}

void KanbanWidget::updateColumnSelectionVisual()
{
    for (auto it = columnGroups.begin(); it != columnGroups.end(); ++it) {
        QGroupBox *group = it.value();
        if (group == nullptr) {
            continue;
        }
        const bool selected = (it.key() == state.selectedColumnId);
        group->setProperty("selected", selected);
        group->style()->unpolish(group);
        group->style()->polish(group);
        group->update();
    }
}

int KanbanWidget::selectedCardId() const
{
    return state.selectedCardId;
}

int KanbanWidget::selectedColumnId() const
{
    return state.selectedColumnId;
}

int KanbanWidget::findColumnIndexById(int columnId) const
{
    for (int i = 0; i < state.columns.size(); ++i) {
        if (state.columns[i].id == columnId) {
            return i;
        }
    }
    return -1;
}

QList<int> KanbanWidget::cardOrderForColumn(int columnId) const
{
    for (const ColumnData &column : state.columns) {
        if (column.id == columnId) {
            QList<int> order;
            for (const CardData &card : column.cards) {
                order.append(card.id);
            }
            return order;
        }
    }
    return {};
}

bool KanbanWidget::createColumn()
{
    const QString title = QInputDialog::getText(this, "Crear columna", "Titulo:");
    if (title.trimmed().isEmpty()) {
        return false;
    }

    QString errorMessage;
    if (!api->createColumn(title, errorMessage)) {
        updateStatus("No se pudo crear columna: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::editColumn()
{
    const int columnId = selectedColumnId();
    if (columnId < 0) {
        updateStatus("Selecciona una columna para editar.", true);
        return false;
    }

    const int index = findColumnIndexById(columnId);
    if (index < 0) {
        return false;
    }

    const QString newTitle = QInputDialog::getText(this,
                                                   "Editar columna",
                                                   "Nuevo titulo:",
                                                   QLineEdit::Normal,
                                                   state.columns[index].title);
    if (newTitle.trimmed().isEmpty()) {
        return false;
    }

    QString errorMessage;
    if (!api->editColumn(columnId, newTitle, errorMessage)) {
        updateStatus("No se pudo editar columna: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::deleteColumn()
{
    const int columnId = selectedColumnId();
    if (columnId < 0) {
        updateStatus("Selecciona una columna para eliminar.", true);
        return false;
    }

    if (QMessageBox::question(this, "Eliminar columna", "Se eliminara la columna y sus tarjetas. Continuar?")
        != QMessageBox::Yes) {
        return false;
    }

    QString errorMessage;
    if (!api->deleteColumn(columnId, errorMessage)) {
        updateStatus("No se pudo eliminar columna: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::createCard()
{
    const int columnId = selectedColumnId();
    if (columnId < 0) {
        updateStatus("Selecciona primero una columna.", true);
        return false;
    }

    const QString title = QInputDialog::getText(this, "Crear tarjeta", "Titulo:");
    if (title.trimmed().isEmpty()) {
        return false;
    }
    const QString description = QInputDialog::getMultiLineText(this, "Crear tarjeta", "Descripcion:");

    QString errorMessage;
    if (!api->createCard(title, description, columnId, errorMessage)) {
        updateStatus("No se pudo crear tarjeta: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::editCard()
{
    const int cardId = selectedCardId();
    const int columnId = selectedColumnId();
    if (cardId < 0 || columnId < 0) {
        updateStatus("Selecciona una tarjeta para editar.", true);
        return false;
    }

    const int columnIndex = findColumnIndexById(columnId);
    if (columnIndex < 0) {
        return false;
    }

    CardData currentCard;
    bool found = false;
    for (const CardData &card : state.columns[columnIndex].cards) {
        if (card.id == cardId) {
            currentCard = card;
            found = true;
            break;
        }
    }
    if (!found) {
        return false;
    }

    const QString newTitle = QInputDialog::getText(this, "Editar tarjeta", "Nuevo titulo:", QLineEdit::Normal, currentCard.title);
    if (newTitle.trimmed().isEmpty()) {
        return false;
    }
    const QString newDescription = QInputDialog::getMultiLineText(this, "Editar tarjeta", "Nueva descripcion:", currentCard.description);

    QString errorMessage;
    if (!api->editCard(cardId, newTitle, newDescription, errorMessage)) {
        updateStatus("No se pudo editar tarjeta: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::deleteCard()
{
    const int cardId = selectedCardId();
    if (cardId < 0) {
        updateStatus("Selecciona una tarjeta para eliminar.", true);
        return false;
    }

    if (QMessageBox::question(this, "Eliminar tarjeta", "Eliminar la tarjeta seleccionada?")
        != QMessageBox::Yes) {
        return false;
    }

    QString errorMessage;
    if (!api->deleteCard(cardId, errorMessage)) {
        updateStatus("No se pudo eliminar tarjeta: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::moveCardToNeighborColumn(int direction)
{
    const int cardId = selectedCardId();
    const int currentColumnIdValue = selectedColumnId();
    if (cardId < 0 || currentColumnIdValue < 0) {
        updateStatus("Selecciona una tarjeta para mover.", true);
        return false;
    }

    const int currentIndex = findColumnIndexById(currentColumnIdValue);
    const int targetIndex = currentIndex + direction;
    if (currentIndex < 0 || targetIndex < 0 || targetIndex >= state.columns.size()) {
        updateStatus("No existe columna destino para ese movimiento.", true);
        return false;
    }

    const ColumnData &targetColumn = state.columns[targetIndex];
    const int newPosition = targetColumn.cards.size();

    QString errorMessage;
    if (!api->moveCard(cardId, targetColumn.id, newPosition, errorMessage)) {
        updateStatus("No se pudo mover tarjeta: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}

bool KanbanWidget::reorderCard(int direction)
{
    const int cardId = selectedCardId();
    const int columnId = selectedColumnId();
    if (cardId < 0 || columnId < 0) {
        updateStatus("Selecciona una tarjeta para reordenar.", true);
        return false;
    }

    const QList<int> order = cardOrderForColumn(columnId);
    const int currentIndex = order.indexOf(cardId);
    const int targetIndex = currentIndex + direction;
    if (currentIndex < 0 || targetIndex < 0 || targetIndex >= order.size()) {
        return false;
    }

    QString errorMessage;
    if (!api->reorderCard(cardId, targetIndex, errorMessage)) {
        updateStatus("No se pudo reordenar tarjeta: " + errorMessage, true);
        return false;
    }

    refreshBoard();
    return true;
}
