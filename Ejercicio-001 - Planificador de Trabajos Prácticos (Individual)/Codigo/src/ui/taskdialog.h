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
    QLineEdit *titleEdit;
    QLineEdit *subjectEdit;
    QDateEdit *dueDateEdit;
    QComboBox *statusCombo;
    QComboBox *priorityCombo;
    QString taskIdValue;
    QString taskNotesValue;
};

#endif // TASKDIALOG_H
