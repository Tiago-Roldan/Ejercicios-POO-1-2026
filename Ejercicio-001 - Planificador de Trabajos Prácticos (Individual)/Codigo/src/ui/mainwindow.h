#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#include "../models/historyentry.h"
#include "../models/practicalwork.h"

class AuthManager;
class StorageManager;
class QLabel;
class QListWidget;
class QComboBox;
class QTextEdit;
class QPushButton;
class QGridLayout;
class QCloseEvent;
class QTimer;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(AuthManager *authManager, StorageManager *storageManager, QWidget *parent = nullptr);

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void openAddTaskDialog();
    void saveSelectedNotes();
    void handleLogout();
    void markNotesDirty();

private:
    void buildUi();
    void loadTasks();
    void loadHistory();
    void renderBoard();
    QList<PracticalWork> filteredTasks() const;
    void clearBoard();
    int findTaskIndex(const QString &taskId) const;
    void openEditTaskDialog(const QString &taskId);
    void deleteTask(const QString &taskId);
    void openNotesForTask(const QString &taskId);
    void clearCurrentTask();
    void refreshNotesPanel();
    bool saveNotesForCurrentTask(bool showFeedback);
    bool saveTasksAndReload(const QList<PracticalWork> &updatedTasks, const QString &errorText);
    bool ensureNotesHandled(const QString &nextActionDescription);
    void logAction(const QString &action);
    void showStatusMessage(const QString &message);
    static void sortTasks(QList<PracticalWork> &tasks);

    AuthManager *authManager;
    StorageManager *storageManager;
    QList<PracticalWork> tasks;
    QList<HistoryEntry> historyEntries;
    QString currentTaskId;
    bool notesDirty;

    QLabel *welcomeLabel;
    QLabel *boardSummaryLabel;
    QLabel *notesTargetLabel;
    QLabel *storagePathLabel;
    QLabel *statusMessageLabel;
    QComboBox *statusFilterCombo;
    QComboBox *priorityFilterCombo;
    QTextEdit *notesEdit;
    QPushButton *saveNotesButton;
    QListWidget *historyList;
    QGridLayout *boardLayout;
    QTimer *statusMessageTimer;
};

#endif // MAINWINDOW_H
