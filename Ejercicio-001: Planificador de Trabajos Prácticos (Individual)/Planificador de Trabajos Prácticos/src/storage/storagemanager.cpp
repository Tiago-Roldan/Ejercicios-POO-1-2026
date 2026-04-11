#include "storagemanager.h"

#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSaveFile>
#include <QStandardPaths>
#include <QTextStream>
#include <QtGlobal>
#include <QUrl>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <QStringConverter>
#endif

namespace {

QString resolveProjectDataDirectory()
{
    QDir currentDirectory(QCoreApplication::applicationDirPath());

    while (currentDirectory.exists()) {
        const QStringList projectFiles = currentDirectory.entryList(
            QStringList() << "*.pro", QDir::Files | QDir::Readable);
        if (!projectFiles.isEmpty()) {
            return currentDirectory.filePath("data");
        }

        if (!currentDirectory.cdUp()) {
            break;
        }
    }

    return QString();
}

} // namespace

StorageManager::StorageManager()
{
    QString basePath = resolveProjectDataDirectory();
    if (basePath.isEmpty()) {
        basePath = QDir(QCoreApplication::applicationDirPath()).filePath("data");
    }
    if (basePath.isEmpty()) {
        basePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    }

    m_dataDirectoryPath = basePath;
}

bool StorageManager::initialize() const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    return ensureUsersFile()
        && ensurePracticalWorksFile()
        && ensureHistoryFile()
        && ensureSessionFile();
}

QList<User> StorageManager::loadUsers() const
{
    return readUsersTextFile(usersFilePath());
}

bool StorageManager::saveUsers(const QList<User> &users) const
{
    return writeUsersTextFile(usersFilePath(), users);
}

QList<PracticalWork> StorageManager::loadPracticalWorks() const
{
    return readPracticalWorksTextFile(practicalWorksFilePath());
}

bool StorageManager::savePracticalWorks(const QList<PracticalWork> &works) const
{
    return writePracticalWorksTextFile(practicalWorksFilePath(), works);
}

QList<HistoryEntry> StorageManager::loadHistory() const
{
    return readHistoryTextFile(historyFilePath());
}

bool StorageManager::saveHistory(const QList<HistoryEntry> &historyEntries) const
{
    return writeHistoryTextFile(historyFilePath(), historyEntries);
}

bool StorageManager::appendHistory(const HistoryEntry &entry) const
{
    QList<HistoryEntry> historyEntries = loadHistory();
    historyEntries.append(entry);
    return saveHistory(historyEntries);
}

bool StorageManager::saveSession(const QString &username, const QDateTime &expiresAtUtc) const
{
    return writeSessionTextFile(sessionFilePath(), username, expiresAtUtc);
}

bool StorageManager::loadSession(QString *username, QDateTime *expiresAtUtc) const
{
    return readSessionTextFile(sessionFilePath(), username, expiresAtUtc);
}

bool StorageManager::clearSession() const
{
    QFile sessionFile(sessionFilePath());
    if (!sessionFile.exists()) {
        return true;
    }
    return sessionFile.remove();
}

QString StorageManager::dataDirectoryPath() const
{
    return m_dataDirectoryPath;
}

QString StorageManager::usersStorageFilePath() const
{
    return usersFilePath();
}

QString StorageManager::practicalWorksStorageFilePath() const
{
    return practicalWorksFilePath();
}

QString StorageManager::historyStorageFilePath() const
{
    return historyFilePath();
}

QString StorageManager::sessionStorageFilePath() const
{
    return sessionFilePath();
}

QString StorageManager::executableDataDirectoryPath() const
{
    return QDir(QCoreApplication::applicationDirPath()).filePath("data");
}

QString StorageManager::usersFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("users.txt");
}

QString StorageManager::practicalWorksFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("practicalworks.txt");
}

QString StorageManager::legacyPracticalWorksJsonFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("practicalworks.json");
}

QString StorageManager::legacyUsersJsonFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("users.json");
}

QString StorageManager::historyFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("history.txt");
}

QString StorageManager::legacyHistoryJsonFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("history.json");
}

QString StorageManager::sessionFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("session.txt");
}

QString StorageManager::legacySessionJsonFilePath() const
{
    return QDir(m_dataDirectoryPath).filePath("session.json");
}

bool StorageManager::ensureDataDirectory() const
{
    QDir directory;
    return directory.mkpath(m_dataDirectoryPath);
}

bool StorageManager::ensureSeedFile(const QString &resourcePath, const QString &targetPath) const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    if (QFile::exists(targetPath)) {
        return true;
    }

    if (!QFile::copy(resourcePath, targetPath)) {
        return false;
    }

    return ensureFileWritable(targetPath);
}

bool StorageManager::migrateExistingTextFile(const QString &legacyPath, const QString &targetPath) const
{
    if (legacyPath == targetPath || !QFile::exists(legacyPath) || QFile::exists(targetPath)) {
        return QFile::exists(targetPath);
    }

    if (!ensureDataDirectory()) {
        return false;
    }

    if (!QFile::copy(legacyPath, targetPath)) {
        return false;
    }

    return ensureFileWritable(targetPath);
}

bool StorageManager::ensureFileWritable(const QString &path) const
{
    QFile file(path);
    if (!file.exists()) {
        return true;
    }

    QFileDevice::Permissions permissions = file.permissions();
    permissions |= QFileDevice::WriteOwner;
    permissions |= QFileDevice::WriteUser;
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    permissions |= QFileDevice::WriteGroup;
    permissions |= QFileDevice::WriteOther;
#endif

    return file.setPermissions(permissions);
}

bool StorageManager::ensureUsersFile() const
{
    const QString targetPath = usersFilePath();
    if (QFile::exists(targetPath)) {
        return true;
    }

    const QString legacyTextPath = QDir(executableDataDirectoryPath()).filePath("users.txt");
    if (migrateExistingTextFile(legacyTextPath, targetPath)) {
        return true;
    }

    const QString legacyJsonPath = legacyUsersJsonFilePath();
    if (QFile::exists(legacyJsonPath)) {
        const QList<User> legacyUsers = readLegacyUsersJsonFile(legacyJsonPath);
        const QFileInfo legacyFileInfo(legacyJsonPath);
        if (!legacyUsers.isEmpty() || legacyFileInfo.size() == 0) {
            return writeUsersTextFile(targetPath, legacyUsers);
        }
    }

    return ensureSeedFile(":/seed/users.txt", targetPath);
}

bool StorageManager::ensurePracticalWorksFile() const
{
    const QString targetPath = practicalWorksFilePath();
    if (QFile::exists(targetPath)) {
        return true;
    }

    const QString legacyTextPath = QDir(executableDataDirectoryPath()).filePath("practicalworks.txt");
    if (migrateExistingTextFile(legacyTextPath, targetPath)) {
        return true;
    }

    const QString legacyJsonPath = legacyPracticalWorksJsonFilePath();
    if (QFile::exists(legacyJsonPath)) {
        const QList<PracticalWork> legacyWorks = readLegacyPracticalWorksJsonFile(legacyJsonPath);
        const QFileInfo legacyFileInfo(legacyJsonPath);
        if (!legacyWorks.isEmpty() || legacyFileInfo.size() == 0) {
            return writePracticalWorksTextFile(targetPath, legacyWorks);
        }
    }

    return ensureSeedFile(":/seed/practicalworks.txt", targetPath);
}

bool StorageManager::ensureHistoryFile() const
{
    const QString targetPath = historyFilePath();
    if (QFile::exists(targetPath)) {
        return true;
    }

    const QString legacyTextPath = QDir(executableDataDirectoryPath()).filePath("history.txt");
    if (migrateExistingTextFile(legacyTextPath, targetPath)) {
        return true;
    }

    const QString legacyJsonPath = legacyHistoryJsonFilePath();
    if (QFile::exists(legacyJsonPath)) {
        const QList<HistoryEntry> legacyHistory = readLegacyHistoryJsonFile(legacyJsonPath);
        const QFileInfo legacyFileInfo(legacyJsonPath);
        if (!legacyHistory.isEmpty() || legacyFileInfo.size() == 0) {
            return writeHistoryTextFile(targetPath, legacyHistory);
        }
    }

    return writeHistoryTextFile(targetPath, QList<HistoryEntry>());
}

bool StorageManager::ensureSessionFile() const
{
    const QString targetPath = sessionFilePath();
    if (QFile::exists(targetPath)) {
        return true;
    }

    const QString legacyTextPath = QDir(executableDataDirectoryPath()).filePath("session.txt");
    if (migrateExistingTextFile(legacyTextPath, targetPath)) {
        return true;
    }

    const QString legacyJsonPath = legacySessionJsonFilePath();
    if (QFile::exists(legacyJsonPath)) {
        QString username;
        QDateTime expiresAtUtc;
        if (readLegacySessionJsonFile(legacyJsonPath, &username, &expiresAtUtc)) {
            return writeSessionTextFile(targetPath, username, expiresAtUtc);
        }
    }

    return writeSessionTextFile(targetPath, QString(), QDateTime());
}

QList<User> StorageManager::readUsersTextFile(const QString &path) const
{
    QList<User> users;
    QFile file(path);
    if (!file.exists()) {
        return users;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return users;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    User currentUser;
    bool inBlock = false;

    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line == "USER_BEGIN") {
            currentUser = User();
            inBlock = true;
            continue;
        }

        if (line == "USER_END") {
            if (inBlock && !currentUser.username.isEmpty()) {
                users.append(currentUser);
            }
            currentUser = User();
            inBlock = false;
            continue;
        }

        if (!inBlock) {
            continue;
        }

        const int separatorIndex = line.indexOf('=');
        if (separatorIndex <= 0) {
            continue;
        }

        const QString key = line.left(separatorIndex).trimmed();
        const QString value = decodeTextValue(line.mid(separatorIndex + 1));

        if (key == "username") {
            currentUser.username = value.trimmed();
        } else if (key == "fullName") {
            currentUser.fullName = value.trimmed();
        } else if (key == "passwordHash") {
            currentUser.passwordHash = value.trimmed();
        }
    }

    return users;
}

QList<HistoryEntry> StorageManager::readHistoryTextFile(const QString &path) const
{
    QList<HistoryEntry> historyEntries;
    QFile file(path);
    if (!file.exists()) {
        return historyEntries;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return historyEntries;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    HistoryEntry currentEntry;
    bool inBlock = false;

    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line == "HISTORY_BEGIN") {
            currentEntry = HistoryEntry();
            inBlock = true;
            continue;
        }

        if (line == "HISTORY_END") {
            if (inBlock && currentEntry.timestamp.isValid() && !currentEntry.action.isEmpty()) {
                historyEntries.append(currentEntry);
            }
            currentEntry = HistoryEntry();
            inBlock = false;
            continue;
        }

        if (!inBlock) {
            continue;
        }

        const int separatorIndex = line.indexOf('=');
        if (separatorIndex <= 0) {
            continue;
        }

        const QString key = line.left(separatorIndex).trimmed();
        const QString value = decodeTextValue(line.mid(separatorIndex + 1));

        if (key == "timestamp") {
            currentEntry.timestamp = QDateTime::fromString(value.trimmed(), Qt::ISODate);
        } else if (key == "username") {
            currentEntry.username = value.trimmed();
        } else if (key == "action") {
            currentEntry.action = value.trimmed();
        }
    }

    return historyEntries;
}

bool StorageManager::readSessionTextFile(
    const QString &path,
    QString *username,
    QDateTime *expiresAtUtc) const
{
    QFile file(path);
    if (!file.exists()) {
        return false;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    QString storedUsername;
    QDateTime storedExpiry;
    bool inBlock = false;

    while (!stream.atEnd()) {
        const QString line = stream.readLine().trimmed();
        if (line.isEmpty()) {
            continue;
        }

        if (line == "SESSION_BEGIN") {
            inBlock = true;
            continue;
        }

        if (line == "SESSION_END") {
            break;
        }

        if (!inBlock) {
            continue;
        }

        const int separatorIndex = line.indexOf('=');
        if (separatorIndex <= 0) {
            continue;
        }

        const QString key = line.left(separatorIndex).trimmed();
        const QString value = decodeTextValue(line.mid(separatorIndex + 1));

        if (key == "username") {
            storedUsername = value.trimmed();
        } else if (key == "expiresAtUtc") {
            storedExpiry = QDateTime::fromString(value.trimmed(), Qt::ISODate);
        }
    }

    if (storedUsername.isEmpty() || !storedExpiry.isValid()) {
        return false;
    }

    if (username != nullptr) {
        *username = storedUsername;
    }

    if (expiresAtUtc != nullptr) {
        *expiresAtUtc = storedExpiry.toUTC();
    }

    return true;
}

bool StorageManager::writeUsersTextFile(const QString &path, const QList<User> &users) const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    if (!ensureFileWritable(path)) {
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    for (const User &user : users) {
        stream << "USER_BEGIN\n";
        stream << "username=" << encodeTextValue(user.username) << "\n";
        stream << "fullName=" << encodeTextValue(user.fullName) << "\n";
        stream << "passwordHash=" << encodeTextValue(user.passwordHash) << "\n";
        stream << "USER_END\n\n";
    }

    stream.flush();
    return file.commit();
}

QList<PracticalWork> StorageManager::readPracticalWorksTextFile(const QString &path) const
{
    QList<PracticalWork> works;
    QFile file(path);
    if (!file.exists()) {
        return works;
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return works;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    PracticalWork currentWork;
    bool inBlock = false;

    while (!stream.atEnd()) {
        const QString rawLine = stream.readLine();
        const QString line = rawLine.trimmed();

        if (line.isEmpty()) {
            continue;
        }

        if (line == "TASK_BEGIN") {
            currentWork = PracticalWork();
            inBlock = true;
            continue;
        }

        if (line == "TASK_END") {
            if (inBlock && !currentWork.id.isEmpty()) {
                if (!currentWork.dueDate.isValid()) {
                    currentWork.dueDate = QDate::currentDate();
                }
                works.append(currentWork);
            }
            currentWork = PracticalWork();
            inBlock = false;
            continue;
        }

        if (!inBlock) {
            continue;
        }

        const int separatorIndex = line.indexOf('=');
        if (separatorIndex <= 0) {
            continue;
        }

        const QString key = line.left(separatorIndex).trimmed();
        const QString encodedValue = line.mid(separatorIndex + 1);
        const QString value = decodeTextValue(encodedValue);

        if (key == "id") {
            currentWork.id = value.trimmed();
        } else if (key == "title") {
            currentWork.title = value;
        } else if (key == "subject") {
            currentWork.subject = value;
        } else if (key == "dueDate") {
            currentWork.dueDate = QDate::fromString(value.trimmed(), Qt::ISODate);
        } else if (key == "status") {
            currentWork.status = value;
        } else if (key == "priority") {
            currentWork.priority = value;
        } else if (key == "notes") {
            currentWork.notes = value;
        }
    }

    return works;
}

QList<PracticalWork> StorageManager::readLegacyPracticalWorksJsonFile(const QString &path) const
{
    QList<PracticalWork> works;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return works;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
        return works;
    }

    const QJsonArray array = document.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            works.append(PracticalWork::fromJson(value.toObject()));
        }
    }
    return works;
}

QList<User> StorageManager::readLegacyUsersJsonFile(const QString &path) const
{
    QList<User> users;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return users;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
        return users;
    }

    const QJsonArray array = document.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            users.append(User::fromJson(value.toObject()));
        }
    }

    return users;
}

QList<HistoryEntry> StorageManager::readLegacyHistoryJsonFile(const QString &path) const
{
    QList<HistoryEntry> historyEntries;
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return historyEntries;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isArray()) {
        return historyEntries;
    }

    const QJsonArray array = document.array();
    for (const QJsonValue &value : array) {
        if (value.isObject()) {
            historyEntries.append(HistoryEntry::fromJson(value.toObject()));
        }
    }

    return historyEntries;
}

bool StorageManager::readLegacySessionJsonFile(
    const QString &path,
    QString *username,
    QDateTime *expiresAtUtc) const
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return false;
    }

    const QJsonObject object = document.object();
    const QString storedUsername = object.value("username").toString().trimmed();
    const QDateTime storedExpiry = QDateTime::fromString(
        object.value("expiresAtUtc").toString().trimmed(), Qt::ISODate);

    if (storedUsername.isEmpty() || !storedExpiry.isValid()) {
        return false;
    }

    if (username != nullptr) {
        *username = storedUsername;
    }

    if (expiresAtUtc != nullptr) {
        *expiresAtUtc = storedExpiry.toUTC();
    }

    return true;
}

bool StorageManager::writePracticalWorksTextFile(
    const QString &path,
    const QList<PracticalWork> &works) const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    if (!ensureFileWritable(path)) {
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    for (const PracticalWork &work : works) {
        stream << "TASK_BEGIN\n";
        stream << "id=" << encodeTextValue(work.id) << "\n";
        stream << "title=" << encodeTextValue(work.title) << "\n";
        stream << "subject=" << encodeTextValue(work.subject) << "\n";
        stream << "dueDate=" << work.dueDate.toString(Qt::ISODate) << "\n";
        stream << "status=" << encodeTextValue(work.status) << "\n";
        stream << "priority=" << encodeTextValue(work.priority) << "\n";
        stream << "notes=" << encodeTextValue(work.notes) << "\n";
        stream << "TASK_END\n\n";
    }

    stream.flush();
    return file.commit();
}

bool StorageManager::writeHistoryTextFile(
    const QString &path,
    const QList<HistoryEntry> &historyEntries) const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    if (!ensureFileWritable(path)) {
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    for (const HistoryEntry &entry : historyEntries) {
        stream << "HISTORY_BEGIN\n";
        stream << "timestamp=" << encodeTextValue(entry.timestamp.toUTC().toString(Qt::ISODate)) << "\n";
        stream << "username=" << encodeTextValue(entry.username) << "\n";
        stream << "action=" << encodeTextValue(entry.action) << "\n";
        stream << "HISTORY_END\n\n";
    }

    stream.flush();
    return file.commit();
}

bool StorageManager::writeSessionTextFile(
    const QString &path,
    const QString &username,
    const QDateTime &expiresAtUtc) const
{
    if (!ensureDataDirectory()) {
        return false;
    }

    if (!ensureFileWritable(path)) {
        return false;
    }

    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    stream.setEncoding(QStringConverter::Utf8);
#else
    stream.setCodec("UTF-8");
#endif

    stream << "SESSION_BEGIN\n";
    stream << "username=" << encodeTextValue(username.trimmed()) << "\n";
    stream << "expiresAtUtc=" << encodeTextValue(expiresAtUtc.toUTC().toString(Qt::ISODate)) << "\n";
    stream << "SESSION_END\n";

    stream.flush();
    return file.commit();
}

QString StorageManager::encodeTextValue(const QString &value)
{
    return QString::fromLatin1(QUrl::toPercentEncoding(value));
}

QString StorageManager::decodeTextValue(const QString &value)
{
    return QUrl::fromPercentEncoding(value.toLatin1());
}
