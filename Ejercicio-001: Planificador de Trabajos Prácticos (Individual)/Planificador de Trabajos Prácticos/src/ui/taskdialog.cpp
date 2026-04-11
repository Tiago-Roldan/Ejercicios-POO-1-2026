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
    , m_titleEdit(new QLineEdit(this))
    , m_subjectEdit(new QLineEdit(this))
    , m_dueDateEdit(new QDateEdit(this))
    , m_statusCombo(new QComboBox(this))
    , m_priorityCombo(new QComboBox(this))
{
    setWindowTitle("Trabajo practico");
    resize(420, 220);

    auto *mainLayout = new QVBoxLayout(this);
    auto *formLayout = new QFormLayout();

    m_dueDateEdit->setCalendarPopup(true);
    m_dueDateEdit->setDate(QDate::currentDate().addDays(7));
    m_statusCombo->addItems(PlannerOptions::statusOptions());
    m_priorityCombo->addItems(PlannerOptions::priorityOptions());

    formLayout->addRow("Titulo:", m_titleEdit);
    formLayout->addRow("Materia:", m_subjectEdit);
    formLayout->addRow("Fecha de entrega:", m_dueDateEdit);
    formLayout->addRow("Estado:", m_statusCombo);
    formLayout->addRow("Prioridad:", m_priorityCombo);

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
    m_taskId = task.id;
    m_taskNotes = task.notes;
    m_titleEdit->setText(task.title);
    m_subjectEdit->setText(task.subject);
    m_dueDateEdit->setDate(task.dueDate);
    m_statusCombo->setCurrentText(task.status);
    m_priorityCombo->setCurrentText(task.priority);
}

PracticalWork TaskDialog::task() const
{
    PracticalWork task;
    task.id = m_taskId.isEmpty() ? QUuid::createUuid().toString(QUuid::WithoutBraces) : m_taskId;
    task.title = m_titleEdit->text().trimmed();
    task.subject = m_subjectEdit->text().trimmed();
    task.dueDate = m_dueDateEdit->date();
    task.status = m_statusCombo->currentText();
    task.priority = m_priorityCombo->currentText();
    task.notes = m_taskNotes;
    return task;
}

void TaskDialog::accept()
{
    if (m_titleEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Dato faltante", "Debes ingresar un titulo.");
        return;
    }

    if (m_subjectEdit->text().trimmed().isEmpty()) {
        QMessageBox::warning(this, "Dato faltante", "Debes ingresar una materia.");
        return;
    }

    QDialog::accept();
}
