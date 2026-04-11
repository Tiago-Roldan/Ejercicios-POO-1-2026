#include "mainwindow.h"

#include <algorithm>

#include <QAbstractButton>
#include <QCloseEvent>
#include <QComboBox>
#include <QDate>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidget>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSignalBlocker>
#include <QSplitter>
#include <QStatusBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "../auth/authmanager.h"
#include "../planneroptions.h"
#include "../storage/storagemanager.h"
#include "taskdialog.h"

namespace {

void deleteLayoutItems(QLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            delete widget;
        }

        if (QLayout *childLayout = item->layout()) {
            deleteLayoutItems(childLayout);
            delete childLayout;
        }

        delete item;
    }
}

QLabel *makeHeaderLabel(const QString &text)
{
    auto *label = new QLabel(text);
    QFont font = label->font();
    font.setBold(true);
    label->setFont(font);
    label->setStyleSheet("color: #101828;");
    return label;
}

QLabel *makeCellLabel(const QString &text)
{
    auto *label = new QLabel(text);
    label->setWordWrap(true);
    label->setStyleSheet("padding: 6px 0;");
    return label;
}

} // namespace

MainWindow::MainWindow(AuthManager *authManager, StorageManager *storageManager, QWidget *parent)
    : QMainWindow(parent)
    , m_authManager(authManager)
    , m_storageManager(storageManager)
    , m_notesDirty(false)
    , m_welcomeLabel(nullptr)
    , m_boardSummaryLabel(nullptr)
    , m_notesTargetLabel(nullptr)
    , m_storagePathLabel(nullptr)
    , m_statusFilterCombo(nullptr)
    , m_priorityFilterCombo(nullptr)
    , m_notesEdit(nullptr)
    , m_saveNotesButton(nullptr)
    , m_historyList(nullptr)
    , m_boardLayout(nullptr)
{
    buildUi();
    loadTasks();
    loadHistory();
    renderBoard();
    clearCurrentTask();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (!ensureNotesHandled("cerrar la aplicacion")) {
        event->ignore();
        return;
    }

    QMainWindow::closeEvent(event);
}

void MainWindow::openAddTaskDialog()
{
    if (!ensureNotesHandled("crear un nuevo trabajo practico")) {
        return;
    }

    TaskDialog dialog(this);
    dialog.setWindowTitle("Nuevo trabajo practico");

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QList<PracticalWork> updatedTasks = m_tasks;
    const PracticalWork newTask = dialog.task();
    updatedTasks.append(newTask);
    sortTasks(updatedTasks);

    if (!saveTasksAndReload(updatedTasks, "No se pudo guardar el nuevo trabajo practico.")) {
        return;
    }

    logAction(QString("Creo el trabajo practico \"%1\".").arg(newTask.title));
    showStatusMessage("Trabajo practico creado.");
}

void MainWindow::saveSelectedNotes()
{
    saveNotesForCurrentTask(true);
}

void MainWindow::handleLogout()
{
    if (!ensureNotesHandled("cerrar la sesion")) {
        return;
    }

    if (QMessageBox::question(
            this,
            "Cerrar sesion",
            "Se eliminara la sesion local guardada. Deseas cerrar sesion ahora?")
        != QMessageBox::Yes) {
        return;
    }

    logAction("Cerro la sesion.");
    m_authManager->logout();
    close();
}

void MainWindow::markNotesDirty()
{
    if (m_currentTaskId.isEmpty()) {
        return;
    }

    m_notesDirty = true;
    refreshNotesPanel();
}

void MainWindow::buildUi()
{
    setWindowTitle("Practical Planner Qt");
    resize(1280, 780);

    auto *centralWidget = new QWidget(this);
    auto *rootLayout = new QHBoxLayout(centralWidget);

    auto *mainSplitter = new QSplitter(Qt::Horizontal, centralWidget);
    rootLayout->addWidget(mainSplitter);

    auto *leftPanel = new QWidget(mainSplitter);
    auto *leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setSpacing(14);

    auto *topBarLayout = new QHBoxLayout();
    m_welcomeLabel = new QLabel(
        QString("Usuario activo: %1 (%2)")
            .arg(m_authManager->currentDisplayName(), m_authManager->currentUsername()),
        leftPanel);
    QPushButton *logoutButton = new QPushButton("Cerrar sesion", leftPanel);
    topBarLayout->addWidget(m_welcomeLabel);
    topBarLayout->addStretch();
    topBarLayout->addWidget(logoutButton);

    auto *filtersBox = new QGroupBox("Filtros y acciones", leftPanel);
    auto *filtersLayout = new QHBoxLayout(filtersBox);
    m_statusFilterCombo = new QComboBox(filtersBox);
    m_priorityFilterCombo = new QComboBox(filtersBox);
    QPushButton *addTaskButton = new QPushButton("Nuevo trabajo practico", filtersBox);

    m_statusFilterCombo->addItems(PlannerOptions::filterOptions(PlannerOptions::statusOptions()));
    m_priorityFilterCombo->addItems(PlannerOptions::filterOptions(PlannerOptions::priorityOptions()));

    filtersLayout->addWidget(new QLabel("Estado:", filtersBox));
    filtersLayout->addWidget(m_statusFilterCombo);
    filtersLayout->addSpacing(10);
    filtersLayout->addWidget(new QLabel("Prioridad:", filtersBox));
    filtersLayout->addWidget(m_priorityFilterCombo);
    filtersLayout->addStretch();
    filtersLayout->addWidget(addTaskButton);

    auto *boardBox = new QGroupBox("Tablero de trabajos practicos", leftPanel);
    auto *boardBoxLayout = new QVBoxLayout(boardBox);
    m_boardSummaryLabel = new QLabel(boardBox);
    auto *boardScrollArea = new QScrollArea(boardBox);
    boardScrollArea->setWidgetResizable(true);
    auto *boardContainer = new QWidget(boardScrollArea);
    m_boardLayout = new QGridLayout(boardContainer);
    m_boardLayout->setContentsMargins(8, 8, 8, 8);
    m_boardLayout->setHorizontalSpacing(18);
    m_boardLayout->setVerticalSpacing(8);
    m_boardLayout->setColumnStretch(0, 2);
    m_boardLayout->setColumnStretch(1, 2);
    m_boardLayout->setColumnStretch(2, 1);
    m_boardLayout->setColumnStretch(3, 1);
    m_boardLayout->setColumnStretch(4, 1);
    m_boardLayout->setColumnStretch(5, 2);
    boardScrollArea->setWidget(boardContainer);
    boardBoxLayout->addWidget(m_boardSummaryLabel);
    boardBoxLayout->addWidget(boardScrollArea);

    auto *notesBox = new QGroupBox("Editor de notas", leftPanel);
    auto *notesLayout = new QVBoxLayout(notesBox);
    m_notesTargetLabel = new QLabel("Selecciona un trabajo practico desde el tablero.", notesBox);
    m_notesTargetLabel->setWordWrap(true);
    m_notesEdit = new QTextEdit(notesBox);
    m_notesEdit->setPlaceholderText("Escribe observaciones, pendientes o detalles del trabajo practico.");
    m_saveNotesButton = new QPushButton("Guardar notas", notesBox);
    notesLayout->addWidget(m_notesTargetLabel);
    notesLayout->addWidget(m_notesEdit);
    notesLayout->addWidget(m_saveNotesButton, 0, Qt::AlignRight);

    m_storagePathLabel = new QLabel(
        QString("Archivo de trabajos: %1\nArchivo de usuarios: %2")
            .arg(m_storageManager->practicalWorksStorageFilePath(),
                 m_storageManager->usersStorageFilePath()),
        leftPanel);
    m_storagePathLabel->setWordWrap(true);
    m_storagePathLabel->setStyleSheet("color: #475467;");

    leftLayout->addLayout(topBarLayout);
    leftLayout->addWidget(filtersBox);
    leftLayout->addWidget(boardBox, 3);
    leftLayout->addWidget(notesBox, 2);
    leftLayout->addWidget(m_storagePathLabel);

    auto *historyBox = new QGroupBox("Historial de acciones", mainSplitter);
    auto *historyLayout = new QVBoxLayout(historyBox);
    m_historyList = new QListWidget(historyBox);
    m_historyList->setAlternatingRowColors(true);
    historyLayout->addWidget(m_historyList);

    mainSplitter->addWidget(leftPanel);
    mainSplitter->addWidget(historyBox);
    mainSplitter->setStretchFactor(0, 3);
    mainSplitter->setStretchFactor(1, 2);

    setCentralWidget(centralWidget);

    connect(addTaskButton, &QPushButton::clicked, this, &MainWindow::openAddTaskDialog);
    connect(logoutButton, &QPushButton::clicked, this, &MainWindow::handleLogout);
    connect(m_saveNotesButton, &QPushButton::clicked, this, &MainWindow::saveSelectedNotes);
    connect(m_notesEdit, &QTextEdit::textChanged, this, &MainWindow::markNotesDirty);
    connect(m_statusFilterCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        renderBoard();
    });
    connect(m_priorityFilterCombo, qOverload<int>(&QComboBox::currentIndexChanged), this, [this]() {
        renderBoard();
    });
}

void MainWindow::loadTasks()
{
    m_tasks = m_storageManager->loadPracticalWorks();
    sortTasks(m_tasks);
}

void MainWindow::loadHistory()
{
    m_historyEntries = m_storageManager->loadHistory();
    m_historyList->clear();

    for (int index = m_historyEntries.size() - 1; index >= 0; --index) {
        m_historyList->addItem(m_historyEntries.at(index).displayText());
    }
}

void MainWindow::renderBoard()
{
    clearBoard();

    const QStringList headers{"Titulo", "Materia", "Entrega", "Estado", "Prioridad", "Acciones"};
    for (int column = 0; column < headers.size(); ++column) {
        m_boardLayout->addWidget(makeHeaderLabel(headers.at(column)), 0, column);
    }

    const QList<PracticalWork> visibleTasks = filteredTasks();
    m_boardSummaryLabel->setText(
        QString("Mostrando %1 de %2 trabajos practicos.")
            .arg(visibleTasks.size())
            .arg(m_tasks.size()));

    if (visibleTasks.isEmpty()) {
        auto *emptyLabel = new QLabel(
            "No hay trabajos practicos que coincidan con los filtros seleccionados.", this);
        emptyLabel->setWordWrap(true);
        emptyLabel->setAlignment(Qt::AlignCenter);
        emptyLabel->setStyleSheet("padding: 24px; color: #475467;");
        m_boardLayout->addWidget(emptyLabel, 1, 0, 1, headers.size());
        return;
    }

    for (int row = 0; row < visibleTasks.size(); ++row) {
        const PracticalWork &task = visibleTasks.at(row);
        const int boardRow = row + 1;

        QLabel *titleLabel = makeCellLabel(task.title);
        QLabel *subjectLabel = makeCellLabel(task.subject);
        QLabel *dueDateLabel = makeCellLabel(task.dueDate.toString("dd/MM/yyyy"));
        QLabel *statusLabel = makeCellLabel(task.status);
        QLabel *priorityLabel = makeCellLabel(task.priority);

        if (task.dueDate < QDate::currentDate() && task.status != "Entregado") {
            dueDateLabel->setStyleSheet("padding: 6px 0; color: #b42318; font-weight: 600;");
        }

        if (task.priority == "Alta") {
            priorityLabel->setStyleSheet("padding: 6px 0; color: #b42318; font-weight: 600;");
        }

        auto *actionsWidget = new QWidget(this);
        auto *actionsLayout = new QHBoxLayout(actionsWidget);
        actionsLayout->setContentsMargins(0, 0, 0, 0);
        actionsLayout->setSpacing(6);

        QPushButton *notesButton = new QPushButton("Notas", actionsWidget);
        QPushButton *editButton = new QPushButton("Editar", actionsWidget);
        QPushButton *deleteButton = new QPushButton("Eliminar", actionsWidget);

        actionsLayout->addWidget(notesButton);
        actionsLayout->addWidget(editButton);
        actionsLayout->addWidget(deleteButton);

        connect(notesButton, &QPushButton::clicked, this, [this, taskId = task.id]() {
            openNotesForTask(taskId);
        });
        connect(editButton, &QPushButton::clicked, this, [this, taskId = task.id]() {
            openEditTaskDialog(taskId);
        });
        connect(deleteButton, &QPushButton::clicked, this, [this, taskId = task.id]() {
            deleteTask(taskId);
        });

        m_boardLayout->addWidget(titleLabel, boardRow, 0);
        m_boardLayout->addWidget(subjectLabel, boardRow, 1);
        m_boardLayout->addWidget(dueDateLabel, boardRow, 2);
        m_boardLayout->addWidget(statusLabel, boardRow, 3);
        m_boardLayout->addWidget(priorityLabel, boardRow, 4);
        m_boardLayout->addWidget(actionsWidget, boardRow, 5);
    }

    m_boardLayout->setRowStretch(visibleTasks.size() + 1, 1);
}

QList<PracticalWork> MainWindow::filteredTasks() const
{
    QList<PracticalWork> filtered;
    const QString selectedStatus = m_statusFilterCombo->currentText();
    const QString selectedPriority = m_priorityFilterCombo->currentText();

    for (const PracticalWork &task : m_tasks) {
        const bool matchesStatus = selectedStatus == "Todos" || task.status == selectedStatus;
        const bool matchesPriority = selectedPriority == "Todos" || task.priority == selectedPriority;

        if (matchesStatus && matchesPriority) {
            filtered.append(task);
        }
    }

    sortTasks(filtered);
    return filtered;
}

void MainWindow::clearBoard()
{
    deleteLayoutItems(m_boardLayout);
}

int MainWindow::findTaskIndex(const QString &taskId) const
{
    for (int index = 0; index < m_tasks.size(); ++index) {
        if (m_tasks.at(index).id == taskId) {
            return index;
        }
    }

    return -1;
}

void MainWindow::openEditTaskDialog(const QString &taskId)
{
    if (!ensureNotesHandled("editar un trabajo practico")) {
        return;
    }

    const int taskIndex = findTaskIndex(taskId);
    if (taskIndex < 0) {
        return;
    }

    TaskDialog dialog(this);
    dialog.setWindowTitle("Editar trabajo practico");
    dialog.setTask(m_tasks.at(taskIndex));

    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    QList<PracticalWork> updatedTasks = m_tasks;
    const PracticalWork updatedTask = dialog.task();
    updatedTasks[taskIndex] = updatedTask;
    sortTasks(updatedTasks);

    if (!saveTasksAndReload(updatedTasks, "No se pudo actualizar el trabajo practico.")) {
        return;
    }

    refreshNotesPanel();
    logAction(QString("Edito el trabajo practico \"%1\".").arg(updatedTask.title));
    showStatusMessage("Trabajo practico actualizado.");
}

void MainWindow::deleteTask(const QString &taskId)
{
    if (!ensureNotesHandled("eliminar un trabajo practico")) {
        return;
    }

    const int taskIndex = findTaskIndex(taskId);
    if (taskIndex < 0) {
        return;
    }

    const PracticalWork task = m_tasks.at(taskIndex);
    QString confirmationText =
        QString("Deseas eliminar el trabajo practico \"%1\"? Esta accion no se puede deshacer.")
            .arg(task.title);

    if (m_currentTaskId == taskId && m_notesDirty) {
        confirmationText.append("\n\nTambien se perderan las notas sin guardar.");
    }

    if (QMessageBox::question(this, "Eliminar trabajo practico", confirmationText) != QMessageBox::Yes) {
        return;
    }

    QList<PracticalWork> updatedTasks = m_tasks;
    updatedTasks.removeAt(taskIndex);

    if (!saveTasksAndReload(updatedTasks, "No se pudo eliminar el trabajo practico.")) {
        return;
    }

    if (m_currentTaskId == taskId) {
        clearCurrentTask();
    }

    logAction(QString("Elimino el trabajo practico \"%1\".").arg(task.title));
    showStatusMessage("Trabajo practico eliminado.");
}

void MainWindow::openNotesForTask(const QString &taskId)
{
    if (!ensureNotesHandled("cambiar de trabajo practico")) {
        return;
    }

    const int taskIndex = findTaskIndex(taskId);
    if (taskIndex < 0) {
        return;
    }

    m_currentTaskId = taskId;
    {
        QSignalBlocker blocker(m_notesEdit);
        m_notesEdit->setPlainText(m_tasks.at(taskIndex).notes);
    }
    m_notesDirty = false;
    refreshNotesPanel();
    showStatusMessage(QString("Notas cargadas para \"%1\".").arg(m_tasks.at(taskIndex).title));
}

void MainWindow::clearCurrentTask()
{
    m_currentTaskId.clear();
    m_notesDirty = false;
    {
        QSignalBlocker blocker(m_notesEdit);
        m_notesEdit->clear();
    }
    refreshNotesPanel();
}

void MainWindow::refreshNotesPanel()
{
    if (m_currentTaskId.isEmpty()) {
        m_notesTargetLabel->setText("Selecciona un trabajo practico desde el tablero.");
        m_notesEdit->setEnabled(false);
        m_saveNotesButton->setEnabled(false);
        return;
    }

    const int taskIndex = findTaskIndex(m_currentTaskId);
    if (taskIndex < 0) {
        clearCurrentTask();
        return;
    }

    QString targetText =
        QString("Notas de \"%1\" (%2)").arg(m_tasks.at(taskIndex).title, m_tasks.at(taskIndex).subject);
    if (m_notesDirty) {
        targetText.append(" - cambios sin guardar");
    }

    m_notesTargetLabel->setText(targetText);
    m_notesEdit->setEnabled(true);
    m_saveNotesButton->setEnabled(true);
}

bool MainWindow::saveNotesForCurrentTask(bool showFeedback)
{
    const int taskIndex = findTaskIndex(m_currentTaskId);
    if (taskIndex < 0) {
        if (showFeedback) {
            QMessageBox::information(this, "Sin seleccion", "Primero selecciona un trabajo practico.");
        }
        return false;
    }

    QList<PracticalWork> updatedTasks = m_tasks;
    const QString taskTitle = updatedTasks[taskIndex].title;
    updatedTasks[taskIndex].notes = m_notesEdit->toPlainText();

    if (!saveTasksAndReload(updatedTasks, "No se pudieron guardar las notas del trabajo practico.")) {
        return false;
    }

    m_notesDirty = false;
    refreshNotesPanel();
    logAction(QString("Guardo las notas de \"%1\".").arg(taskTitle));

    if (showFeedback) {
        showStatusMessage("Notas guardadas.");
    }

    return true;
}

bool MainWindow::saveTasksAndReload(const QList<PracticalWork> &tasks, const QString &errorText)
{
    if (!m_storageManager->savePracticalWorks(tasks)) {
        QMessageBox::critical(
            this,
            "Error al guardar",
            QString("%1\n\nArchivo: %2")
                .arg(errorText, m_storageManager->practicalWorksStorageFilePath()));
        return false;
    }

    const QString previousTaskId = m_currentTaskId;
    loadTasks();
    renderBoard();

    if (!previousTaskId.isEmpty() && findTaskIndex(previousTaskId) >= 0) {
        const int currentIndex = findTaskIndex(previousTaskId);
        m_currentTaskId = previousTaskId;
        {
            QSignalBlocker blocker(m_notesEdit);
            m_notesEdit->setPlainText(m_tasks.at(currentIndex).notes);
        }
        m_notesDirty = false;
        refreshNotesPanel();
    } else if (!previousTaskId.isEmpty()) {
        clearCurrentTask();
    }

    return true;
}

bool MainWindow::ensureNotesHandled(const QString &nextActionDescription)
{
    if (!m_notesDirty || m_currentTaskId.isEmpty()) {
        return true;
    }

    QMessageBox messageBox(this);
    messageBox.setWindowTitle("Notas sin guardar");
    messageBox.setText("Hay cambios sin guardar en el editor de notas.");
    messageBox.setInformativeText(
        QString("Que deseas hacer antes de %1?").arg(nextActionDescription));

    QAbstractButton *saveButton = messageBox.addButton("Guardar", QMessageBox::AcceptRole);
    QAbstractButton *discardButton =
        messageBox.addButton("Descartar cambios", QMessageBox::DestructiveRole);
    messageBox.addButton("Cancelar", QMessageBox::RejectRole);

    messageBox.exec();

    if (messageBox.clickedButton() == saveButton) {
        return saveNotesForCurrentTask(true);
    }

    if (messageBox.clickedButton() == discardButton) {
        const int taskIndex = findTaskIndex(m_currentTaskId);
        if (taskIndex >= 0) {
            QSignalBlocker blocker(m_notesEdit);
            m_notesEdit->setPlainText(m_tasks.at(taskIndex).notes);
        }
        m_notesDirty = false;
        refreshNotesPanel();
        return true;
    }

    return false;
}

void MainWindow::logAction(const QString &action)
{
    const HistoryEntry entry = HistoryEntry::create(m_authManager->currentUsername(), action);
    if (!m_storageManager->appendHistory(entry)) {
        showStatusMessage("No se pudo actualizar el historial.");
        return;
    }

    m_historyEntries.append(entry);
    m_historyList->insertItem(0, entry.displayText());
}

void MainWindow::showStatusMessage(const QString &message)
{
    statusBar()->showMessage(message, 4000);
}

void MainWindow::sortTasks(QList<PracticalWork> &tasks)
{
    std::sort(tasks.begin(), tasks.end(), [](const PracticalWork &left, const PracticalWork &right) {
        if (left.dueDate != right.dueDate) {
            return left.dueDate < right.dueDate;
        }

        if (left.priority != right.priority) {
            return PlannerOptions::priorityRank(left.priority)
                < PlannerOptions::priorityRank(right.priority);
        }

        return left.title.toLower() < right.title.toLower();
    });
}
