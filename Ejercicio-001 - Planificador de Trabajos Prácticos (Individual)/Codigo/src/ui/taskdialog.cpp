#include "taskdialog.h"

#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QUuid>
#include <QVBoxLayout>

#include "../planneroptions.h"

TaskDialog::TaskDialog(QWidget *parent)
    : QDialog(parent)
    , titleEdit(new QLineEdit(this))
    , subjectEdit(new QLineEdit(this))
    , dueDateEdit(new QDateEdit(this))
    , statusCombo(new QComboBox(this))
    , priorityCombo(new QComboBox(this))
{
    setWindowTitle("Trabajo practico");
    resize(420, 220);

    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();

    dueDateEdit->setCalendarPopup(true);
    dueDateEdit->setDate(QDate::currentDate().addDays(7));
    statusCombo->addItems(PlannerOptions::statusOptions());
    priorityCombo->addItems(PlannerOptions::priorityOptions());

    formLayout->addRow("Titulo:", titleEdit);
    formLayout->addRow("Materia:", subjectEdit);
    formLayout->addRow("Fecha de entrega:", dueDateEdit);
    formLayout->addRow("Estado:", statusCombo);
    formLayout->addRow("Prioridad:", priorityCombo);

    auto *buttonBox = new QDialogButtonBox(this);
    buttonBox->addButton("Guardar", QDialogButtonBox::AcceptRole);
    buttonBox->addButton("Cancelar", QDialogButtonBox::RejectRole);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &TaskDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &TaskDialog::reject);
}

void TaskDialog::setTask(const PracticalWork &task)
{
    taskIdValue = task.id;
    taskNotesValue = task.notes;
    titleEdit->setText(task.title);
    subjectEdit->setText(task.subject);
    dueDateEdit->setDate(task.dueDate);
    statusCombo->setCurrentText(task.status);
    priorityCombo->setCurrentText(task.priority);
}

PracticalWork TaskDialog::task() const
{
    PracticalWork task;
    task.id = taskIdValue.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : taskIdValue;
    task.title = titleEdit->text().trimmed();
    task.subject = subjectEdit->text().trimmed();
    task.dueDate = dueDateEdit->date();
    task.status = statusCombo->currentText();
    task.priority = priorityCombo->currentText();
    task.notes = taskNotesValue;
    return task;
}

void TaskDialog::accept()
{
    if (titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Dato faltante", "Debes ingresar un titulo.");
        return;
    }

    if (subjectEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Dato faltante", "Debes ingresar una materia.");
        return;
    }

    QDialog::accept();
}
