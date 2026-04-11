#ifndef TASKDIALOG_H
#define TASKDIALOG_H

#include <QDialog>

#include "../models/practicalwork.h"

class QComboBox;
class QDateEdit;
class QLineEdit;

class TaskDialog : public QDialog
{
    Q_OBJECT

public:
    explicit TaskDialog(QWidget *parent = nullptr);

    void setTask(const PracticalWork &task);
    PracticalWork task() const;

protected:
    void accept() override;

private:
    QLineEdit *m_titleEdit;
    QLineEdit *m_subjectEdit;
    QDateEdit *m_dueDateEdit;
    QComboBox *m_statusCombo;
    QComboBox *m_priorityCombo;
    QString m_taskId;
    QString m_taskNotes;
};

#endif // TASKDIALOG_H
