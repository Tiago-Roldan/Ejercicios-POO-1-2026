#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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

class MainWindow : public QMainWindow
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
    bool saveTasksAndReload(const QList<PracticalWork> &tasks, const QString &errorText);
    bool ensureNotesHandled(const QString &nextActionDescription);
    void logAction(const QString &action);
    void showStatusMessage(const QString &message);
    static void sortTasks(QList<PracticalWork> &tasks);

    AuthManager *m_authManager;
    StorageManager *m_storageManager;
    QList<PracticalWork> m_tasks;
    QList<HistoryEntry> m_historyEntries;
    QString m_currentTaskId;
    bool m_notesDirty;

    QLabel *m_welcomeLabel;
    QLabel *m_boardSummaryLabel;
    QLabel *m_notesTargetLabel;
    QLabel *m_storagePathLabel;
    QComboBox *m_statusFilterCombo;
    QComboBox *m_priorityFilterCombo;
    QTextEdit *m_notesEdit;
    QPushButton *m_saveNotesButton;
    QListWidget *m_historyList;
    QGridLayout *m_boardLayout;
};

#endif // MAINWINDOW_H
