#include "authmanager.h"

#include <QCryptographicHash>
#include <QDateTime>

#include "../storage/storagemanager.h"

AuthManager::AuthManager(StorageManager *storageManager)
    : storageManager(storageManager)
{
}

bool AuthManager::login(const QString &username, const QString &password)
{
    const QString normalizedUsername = username.trimmed();
    if (normalizedUsername.isEmpty() || password.isEmpty()) {
        return false;
    }

    const QString passwordHash = hashPassword(password);
    const QList<User> users = storageManager->loadUsers();

    for (const User &user : users) {
        if (user.username.compare(normalizedUsername, Qt::CaseInsensitive) == 0
            && user.passwordHash == passwordHash) {
            currentUsernameValue = user.username;
            currentDisplayNameValue = user.fullName;
            const bool saved = storageManager->saveSession(
                user.username, QDateTime::currentDateTimeUtc().addSecs(5 * 60));
            if (!saved) {
                currentUsernameValue.clear();
                currentDisplayNameValue.clear();
            }
            return saved;
        }
    }

    return false;
}

bool AuthManager::registerUser(
    const QString &fullName,
    const QString &username,
    const QString &password,
    QString *errorMessage)
{
    const QString normalizedFullName = fullName.trimmed();
    const QString normalizedUsername = username.trimmed();

    if (normalizedFullName.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Debes ingresar el nombre completo.";
        }
        return false;
    }

    if (normalizedUsername.isEmpty()) {
        if (errorMessage != nullptr) {
            *errorMessage = "Debes ingresar un nombre de usuario.";
        }
        return false;
    }

    if (password.trimmed().size() < 4) {
        if (errorMessage != nullptr) {
            *errorMessage = "La contrasena debe tener al menos 4 caracteres.";
        }
        return false;
    }

    QList<User> users = storageManager->loadUsers();
    for (const User &existingUser : users) {
        if (existingUser.username.compare(normalizedUsername, Qt::CaseInsensitive) == 0) {
            if (errorMessage != nullptr) {
                *errorMessage = "Ese nombre de usuario ya existe.";
            }
            return false;
        }
    }

    User newUser;
    newUser.fullName = normalizedFullName;
    newUser.username = normalizedUsername;
    newUser.passwordHash = hashPassword(password);
    users.append(newUser);

    if (!storageManager->saveUsers(users)) {
        if (errorMessage != nullptr) {
            *errorMessage = "No se pudo guardar el nuevo usuario en el archivo de texto.";
        }
        return false;
    }

    return true;
}

bool AuthManager::resumeSession()
{
    QString username;
    QDateTime expiresAtUtc;

    if (!storageManager->loadSession(&username, &expiresAtUtc)) {
        return false;
    }

    if (expiresAtUtc < QDateTime::currentDateTimeUtc()) {
        storageManager->clearSession();
        return false;
    }

    return setCurrentUser(username);
}

void AuthManager::logout()
{
    storageManager->clearSession();
    currentUsernameValue.clear();
    currentDisplayNameValue.clear();
}

QString AuthManager::currentUsername() const
{
    return currentUsernameValue;
}

QString AuthManager::currentDisplayName() const
{
    return currentDisplayNameValue;
}

bool AuthManager::setCurrentUser(const QString &username)
{
    const QList<User> users = storageManager->loadUsers();
    for (const User &user : users) {
        if (user.username.compare(username, Qt::CaseInsensitive) == 0) {
            currentUsernameValue = user.username;
            currentDisplayNameValue = user.fullName;
            return true;
        }
    }

    return false;
}

QString AuthManager::hashPassword(const QString &password)
{
    return QString(
        QCryptographicHash::hash(password.toUtf8(), QCryptographicHash::Sha256).toHex());
}
