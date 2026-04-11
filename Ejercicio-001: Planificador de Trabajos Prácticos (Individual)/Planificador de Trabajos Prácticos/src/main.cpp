#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>

#include "auth/authmanager.h"
#include "models/historyentry.h"
#include "storage/storagemanager.h"
#include "ui/logindialog.h"
#include "ui/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("POORoldan");
    QCoreApplication::setApplicationName("PracticalPlannerQt");

    StorageManager storageManager;
    if (!storageManager.initialize()) {
        QMessageBox::critical(
            nullptr,
            "Error al iniciar",
            "No fue posible preparar los archivos locales de la aplicacion.");
        return 1;
    }

    AuthManager authManager(&storageManager);
    const bool restoredSession = authManager.resumeSession();

    if (!restoredSession) {
        LoginDialog loginDialog(&authManager);
        if (loginDialog.exec() != QDialog::Accepted) {
            return 0;
        }

        storageManager.appendHistory(
            HistoryEntry::create(authManager.currentUsername(), "Inicio sesion manual."));
    } else {
        storageManager.appendHistory(
            HistoryEntry::create(authManager.currentUsername(), "Restauro la sesion local."));
    }

    MainWindow window(&authManager, &storageManager);
    window.show();

    return app.exec();
}
