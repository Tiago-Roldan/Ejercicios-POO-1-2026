#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QDateTime>
#include <QStringList>
#include <QString>

#include "../models/historyentry.h"
#include "../models/practicalwork.h"
#include "../models/user.h"

class StorageManager
{
public:
    StorageManager();

    bool initialize() const;

    QList<User> loadUsers() const;
    bool saveUsers(const QList<User> &users) const;
    QList<PracticalWork> loadPracticalWorks() const;
    bool savePracticalWorks(const QList<PracticalWork> &works) const;

    QList<HistoryEntry> loadHistory() const;
    bool saveHistory(const QList<HistoryEntry> &historyEntries) const;
    bool appendHistory(const HistoryEntry &entry) const;

    bool saveSession(const QString &username, const QDateTime &expiresAtUtc) const;
    bool loadSession(QString *username, QDateTime *expiresAtUtc) const;
    bool clearSession() const;

    QString dataDirectoryPath() const;
    QString usersStorageFilePath() const;
    QString practicalWorksStorageFilePath() const;
    QString historyStorageFilePath() const;
    QString sessionStorageFilePath() const;

private:
    QString executableDataDirectoryPath() const;
    QString usersFilePath() const;
    QString practicalWorksFilePath() const;
    QString legacyPracticalWorksJsonFilePath() const;
    QString legacyUsersJsonFilePath() const;
    QString historyFilePath() const;
    QString legacyHistoryJsonFilePath() const;
    QString sessionFilePath() const;
    QString legacySessionJsonFilePath() const;

    bool ensureDataDirectory() const;
    bool ensureSeedFile(const QString &resourcePath, const QString &targetPath) const;
    bool migrateExistingTextFile(const QString &legacyPath, const QString &targetPath) const;
    bool ensureFileWritable(const QString &path) const;
    bool ensureUsersFile() const;
    bool ensurePracticalWorksFile() const;
    bool ensureHistoryFile() const;
    bool ensureSessionFile() const;
    QList<User> readUsersTextFile(const QString &path) const;
    QList<PracticalWork> readPracticalWorksTextFile(const QString &path) const;
    QList<HistoryEntry> readHistoryTextFile(const QString &path) const;
    bool readSessionTextFile(const QString &path, QString *username, QDateTime *expiresAtUtc) const;
    bool writeUsersTextFile(const QString &path, const QList<User> &users) const;
    QList<PracticalWork> readLegacyPracticalWorksJsonFile(const QString &path) const;
    QList<User> readLegacyUsersJsonFile(const QString &path) const;
    QList<HistoryEntry> readLegacyHistoryJsonFile(const QString &path) const;
    bool readLegacySessionJsonFile(
        const QString &path,
        QString *username,
        QDateTime *expiresAtUtc) const;
    bool writePracticalWorksTextFile(const QString &path, const QList<PracticalWork> &works) const;
    bool writeHistoryTextFile(const QString &path, const QList<HistoryEntry> &historyEntries) const;
    bool writeSessionTextFile(
        const QString &path,
        const QString &username,
        const QDateTime &expiresAtUtc) const;
    static QString encodeTextValue(const QString &value);
    static QString decodeTextValue(const QString &value);

    QString m_dataDirectoryPath;
};

#endif // STORAGEMANAGER_H
