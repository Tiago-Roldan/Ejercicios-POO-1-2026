#ifndef AUTHMANAGER_H
#define AUTHMANAGER_H

#include <QString>

class StorageManager;

class AuthManager
{
public:
    explicit AuthManager(StorageManager *storageManager);

    bool login(const QString &username, const QString &password);
    bool registerUser(
        const QString &fullName,
        const QString &username,
        const QString &password,
        QString *errorMessage = nullptr);
    bool resumeSession();
    void logout();

    QString currentUsername() const;
    QString currentDisplayName() const;

private:
    bool setCurrentUser(const QString &username);
    static QString hashPassword(const QString &password);

    StorageManager *storageManager;
    QString currentUsernameValue;
    QString currentDisplayNameValue;
};

#endif // AUTHMANAGER_H
