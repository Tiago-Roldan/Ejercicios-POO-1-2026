#include "authmanager.h"

#include <QCryptographicHash>
#include <QDateTime>

#include "../storage/storagemanager.h"

AuthManager::AuthManager(StorageManager *storageManager)
    : m_storageManager(storageManager)
{
}

bool AuthManager::login(const QString &username, const QString &password)
{
    const QString normalizedUsername = username.trimmed();
    if (normalizedUsername.isEmpty() || password.isEmpty()) {
        return false;
    }

    const QString passwordHash = hashPassword(password);
    const QList<User> users = m_storageManager->loadUsers();

    for (const User &user : users) {
        if (user.username.compare(normalizedUsername, Qt::CaseInsensitive) == 0
            && user.passwordHash == passwordHash) {
            m_currentUsername = user.username;
            m_currentDisplayName = user.fullName;
            const bool saved = m_storageManager->saveSession(
                user.username, QDateTime::currentDateTimeUtc().addSecs(5 * 60));
            if (!saved) {
                m_currentUsername.clear();
                m_currentDisplayName.clear();
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

    QList<User> users = m_storageManager->loadUsers();
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

    if (!m_storageManager->saveUsers(users)) {
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

    if (!m_storageManager->loadSession(&username, &expiresAtUtc)) {
        return false;
    }

    if (expiresAtUtc < QDateTime::currentDateTimeUtc()) {
        m_storageManager->clearSession();
        return false;
    }

    return setCurrentUser(username);
}

void AuthManager::logout()
{
    m_storageManager->clearSession();
    m_currentUsername.clear();
    m_currentDisplayName.clear();
}

QString AuthManager::currentUsername() const
{
    return m_currentUsername;
}

QString AuthManager::currentDisplayName() const
{
    return m_currentDisplayName;
}

bool AuthManager::setCurrentUser(const QString &username)
{
    const QList<User> users = m_storageManager->loadUsers();
    for (const User &user : users) {
        if (user.username.compare(username, Qt::CaseInsensitive) == 0) {
            m_currentUsername = user.username;
            m_currentDisplayName = user.fullName;
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
