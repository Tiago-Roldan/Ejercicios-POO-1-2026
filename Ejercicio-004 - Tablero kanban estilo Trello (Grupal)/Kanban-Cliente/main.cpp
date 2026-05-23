#include <QApplication>
#include <QDialog>
#include <QFont>
#include "kanbanwidget.h"
#include "logindialog.h"
#include "kanbanapi.h"

static void applyModernTheme(QApplication &app)
{
    app.setStyle("Fusion");
    app.setFont(QFont("Segoe UI", 10));
    app.setStyleSheet(
        "QWidget { background: #f4f7fb; color: #1f2937; }"
        "QLabel#AppTitle { font-size: 22px; font-weight: 700; color: #111827; }"
        "QLabel#SectionTitle { font-size: 14px; font-weight: 600; color: #111827; }"
        "QWidget#TopBar { background: #ffffff; border: 1px solid #dbe4ee; border-radius: 12px; }"
        "QWidget#TopBar QWidget, QWidget#TopBar QLabel { background: #ffffff; }"
        "QWidget#TopBar QPushButton { background: #e2e8f0; color: #0f172a; }"
        "QWidget#TopBar QPushButton:hover { background: #d4deea; }"
        "QWidget#TopBar QPushButton:pressed { background: #c3d0e0; }"
        "QWidget#TopBar QPushButton#PrimaryButton { background: #2563eb; color: white; font-weight: 600; }"
        "QWidget#TopBar QPushButton#PrimaryButton:hover { background: #1d4ed8; }"
        "QWidget#TopBar QPushButton#DangerButton { background: #dc2626; color: white; font-weight: 600; }"
        "QWidget#TopBar QPushButton#DangerButton:hover { background: #b91c1c; }"
        "QFrame#SectionDivider { background: #dbe4ee; min-width: 1px; max-width: 1px; margin: 6px 2px; }"
        "QGroupBox { border: 1px solid #dbe4ee; border-radius: 10px; margin-top: 8px; padding-top: 12px; background: #ffffff; }"
        "QGroupBox[selected=\"true\"] { border: 2px solid #3b82f6; background: #eff6ff; }"
        "QGroupBox::title { subcontrol-origin: margin; left: 12px; padding: 0 6px; color: #334155; }"
        "QLineEdit, QListWidget { border: 1px solid #cbd5e1; border-radius: 8px; padding: 8px; background: #ffffff; color: #111827; }"
        "QLineEdit { min-height: 32px; }"
        "QLineEdit::placeholder { color: #6b7280; }"
        "QLineEdit:focus, QListWidget:focus { border: 1px solid #3b82f6; }"
        "QPushButton { border: 0; border-radius: 8px; padding: 8px 12px; background: #e2e8f0; color: #0f172a; }"
        "QPushButton:hover { background: #d4deea; }"
        "QPushButton:pressed { background: #c3d0e0; }"
        "QPushButton:disabled { background: #e5e7eb; color: #9ca3af; }"
        "QPushButton#PrimaryButton { background: #2563eb; color: white; font-weight: 600; }"
        "QPushButton#PrimaryButton:hover { background: #1d4ed8; }"
        "QPushButton#DangerButton { background: #dc2626; color: white; font-weight: 600; }"
        "QPushButton#DangerButton:hover { background: #b91c1c; }"
        "QScrollArea { border: 0; background: transparent; }"
        "QListWidget::item { border-radius: 6px; padding: 6px; margin: 2px; }"
        "QListWidget::item:selected { background: #dbeafe; color: #1e3a8a; }"
        "QListWidget::item:selected:!active { background: #dbeafe; color: #1e3a8a; }"
    );
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    applyModernTheme(app);

    KanbanApi api;
    LoginDialog loginDialog(&api);
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    KanbanWidget widget(&api);
    widget.resize(1100, 700);
    widget.show();
    return app.exec();
}
