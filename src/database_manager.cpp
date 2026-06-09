#include "database_manager.h"

#include <QDir>
#include <QFileInfo>
#include <QtGlobal>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStandardPaths>
#include <QVariantMap>

namespace {

QString connectionName()
{
    return QStringLiteral("cw_sqlite");
}

QString normalizeButtonColorHex(const QString &raw)
{
    QString s = raw.trimmed();
    if (s.isEmpty())
        return {};

    if (!s.startsWith(QLatin1Char('#')))
        return {};

    if (s.size() == 4) {
        const QString r = s.mid(1, 1);
        const QString g = s.mid(2, 1);
        const QString b = s.mid(3, 1);
        s = QLatin1Char('#') + r + r + g + g + b + b;
    }

    if (s.size() == 7) {
        for (int i = 1; i < 7; ++i) {
            const QChar c = s.at(i);
            const bool hex = (c >= QLatin1Char('0') && c <= QLatin1Char('9'))
                || (c >= QLatin1Char('a') && c <= QLatin1Char('f'))
                || (c >= QLatin1Char('A') && c <= QLatin1Char('F'));
            if (!hex)
                return {};
        }
        return s.toLower();
    }

    if (s.size() == 9) {
        QString rgb = QStringLiteral("#");
        for (int i = 3; i < 9; ++i) {
            const QChar c = s.at(i);
            const bool hex = (c >= QLatin1Char('0') && c <= QLatin1Char('9'))
                || (c >= QLatin1Char('a') && c <= QLatin1Char('f'))
                || (c >= QLatin1Char('A') && c <= QLatin1Char('F'));
            if (!hex)
                return {};
            rgb += c;
        }
        return rgb.toLower();
    }

    return {};
}

} // namespace

DatabaseManager::DatabaseManager(QObject *parent)
    : QObject(parent)
{
    const QString root = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(root);
    m_dbPath = root + QStringLiteral("/command_workstation.sqlite");
}

bool DatabaseManager::open()
{
    const QString conn = connectionName();
    if (QSqlDatabase::contains(conn))
        QSqlDatabase::removeDatabase(conn);

    QSqlDatabase db = QSqlDatabase::addDatabase(QStringLiteral("QSQLITE"), conn);
    db.setDatabaseName(m_dbPath);
    if (!db.open()) {
        return false;
    }

    return createSchema();
}

void DatabaseManager::close()
{
    const QString conn = connectionName();
    {
        QSqlDatabase db = QSqlDatabase::database(conn, false);
        if (db.isOpen())
            db.close();
    }
    QSqlDatabase::removeDatabase(conn);
}

QString DatabaseManager::loadSetting(const QString &key, const QString &defaultValue) const
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral("SELECT value FROM settings WHERE key = ?"));
    q.addBindValue(key);
    if (!q.exec() || !q.next())
        return defaultValue;
    return q.value(0).toString();
}

void DatabaseManager::saveSetting(const QString &key, const QString &value)
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral(
        "INSERT INTO settings(key, value) VALUES(?, ?) "
        "ON CONFLICT(key) DO UPDATE SET value = excluded.value"));
    q.addBindValue(key);
    q.addBindValue(value);
    q.exec();
}

bool DatabaseManager::createSchema()
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    const QStringList statements = {
        QStringLiteral("CREATE TABLE IF NOT EXISTS settings ("
                       "key TEXT PRIMARY KEY NOT NULL,"
                       "value TEXT NOT NULL)"),
        QStringLiteral("CREATE TABLE IF NOT EXISTS presets ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "category TEXT NOT NULL,"
                       "label TEXT NOT NULL,"
                       "command TEXT NOT NULL,"
                       "sort_order INTEGER NOT NULL DEFAULT 0)"),
        QStringLiteral("CREATE TABLE IF NOT EXISTS run_log ("
                       "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                       "started_at TEXT NOT NULL,"
                       "cwd TEXT NOT NULL,"
                       "command TEXT NOT NULL,"
                       "exit_code INTEGER NOT NULL,"
                       "duration_ms INTEGER NOT NULL,"
                       "output_tail TEXT NOT NULL)"),
    };

    for (const QString &sql : statements) {
        if (!q.exec(sql))
            return false;
    }
    migratePresetsAddCwd();
    migratePresetsAddButtonColor();
    seedDefaultsIfEmpty();
    return true;
}

void DatabaseManager::migratePresetsAddCwd()
{
    QSqlQuery pragma(QSqlDatabase::database(connectionName()));
    if (!pragma.exec(QStringLiteral("PRAGMA table_info(presets)")))
        return;
    while (pragma.next()) {
        if (pragma.value(1).toString() == QLatin1String("cwd"))
            return;
    }
    QSqlQuery alter(QSqlDatabase::database(connectionName()));
    alter.exec(QStringLiteral("ALTER TABLE presets ADD COLUMN cwd TEXT"));
}

void DatabaseManager::migratePresetsAddButtonColor()
{
    QSqlQuery pragma(QSqlDatabase::database(connectionName()));
    if (!pragma.exec(QStringLiteral("PRAGMA table_info(presets)")))
        return;
    while (pragma.next()) {
        if (pragma.value(1).toString() == QLatin1String("button_color"))
            return;
    }
    QSqlQuery alter(QSqlDatabase::database(connectionName()));
    alter.exec(QStringLiteral("ALTER TABLE presets ADD COLUMN button_color TEXT"));
}

int DatabaseManager::nextCategorySortOrder(const QString &category) const
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral(
        "SELECT COALESCE(MAX(sort_order), -1) + 1 FROM presets WHERE category = ?"));
    q.addBindValue(category);
    if (!q.exec() || !q.next())
        return 0;
    return q.value(0).toInt();
}

bool DatabaseManager::addPreset(const QString &category,
                                const QString &label,
                                const QString &command,
                                const QString &cwdOpt,
                                const QString &buttonColorHex)
{
    QSqlQuery ins(QSqlDatabase::database(connectionName()));
    ins.prepare(QStringLiteral("INSERT INTO presets(category, label, command, sort_order, cwd, "
                               "button_color) VALUES(?,?,?,?,?,?)"));
    ins.addBindValue(category);
    ins.addBindValue(label);
    ins.addBindValue(command);
    ins.addBindValue(nextCategorySortOrder(category));

    const QString trimmedCwd = cwdOpt.trimmed();
    if (trimmedCwd.isEmpty()) {
        ins.addBindValue(QVariant());
    } else {
        const QFileInfo info(trimmedCwd);
        if (!info.isDir())
            return false;
        ins.addBindValue(QDir(trimmedCwd).canonicalPath());
    }

    const QString col = normalizeButtonColorHex(buttonColorHex);
    if (col.isEmpty())
        ins.addBindValue(QVariant());
    else
        ins.addBindValue(col);

    return ins.exec();
}

void DatabaseManager::seedDefaultsIfEmpty()
{
    QSqlQuery count(QSqlDatabase::database(connectionName()));
    if (!count.exec(QStringLiteral("SELECT COUNT(*) FROM presets")))
        return;
    if (!count.next() || count.value(0).toInt() > 0)
        return;

    struct Row
    {
        QString category;
        QString label;
        QString command;
        int order;
    };

    const QList<Row> defaults = {
        {QStringLiteral("General"), QStringLiteral("Working directory (print)"),
#ifdef Q_OS_WIN
         QStringLiteral("cd"), 0},
#else
         QStringLiteral("pwd"), 0},
#endif
        {QStringLiteral("General"), QStringLiteral("List files"),
#ifdef Q_OS_WIN
         QStringLiteral("dir"), 1},
#else
         QStringLiteral("ls -la"), 1},
#endif
        {QStringLiteral("Build / check"), QStringLiteral("CMake configure (Debug)"),
         QStringLiteral("cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug"), 0},
        {QStringLiteral("Build / check"), QStringLiteral("CMake build"),
         QStringLiteral("cmake --build build"), 1},
        {QStringLiteral("Quality"), QStringLiteral("Git short status"),
         QStringLiteral("git status -sb"), 0},
    };

    QSqlQuery ins(QSqlDatabase::database(connectionName()));
    ins.prepare(QStringLiteral(
        "INSERT INTO presets(category, label, command, sort_order) VALUES(?,?,?,?)"));
    for (const Row &r : defaults) {
        ins.addBindValue(r.category);
        ins.addBindValue(r.label);
        ins.addBindValue(r.command);
        ins.addBindValue(r.order);
        ins.exec();
    }
}

QVariantList DatabaseManager::fetchPresetsGrouped() const
{
    QVariantList categories;
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.setForwardOnly(true);
    if (!q.exec(QStringLiteral(
            "SELECT DISTINCT category FROM presets ORDER BY category COLLATE NOCASE")))
        return categories;

    QStringList catNames;
    while (q.next())
        catNames.append(q.value(0).toString());

    for (const QString &cat : catNames) {
        QVariantMap group;
        group.insert(QStringLiteral("name"), cat);
        QVariantList items;
        QSqlQuery pq(QSqlDatabase::database(connectionName()));
        pq.prepare(QStringLiteral(
            "SELECT id, label, command, cwd, button_color FROM presets WHERE category = ? ORDER BY "
            "sort_order, id"));
        pq.addBindValue(cat);
        if (pq.exec()) {
            while (pq.next()) {
                QVariantMap row;
                row.insert(QStringLiteral("id"), pq.value(0));
                row.insert(QStringLiteral("label"), pq.value(1));
                row.insert(QStringLiteral("command"), pq.value(2));
                row.insert(QStringLiteral("cwd"), pq.value(3));
                row.insert(QStringLiteral("buttonColor"), pq.value(4));
                row.insert(QStringLiteral("category"), cat);
                items.append(row);
            }
        }
        group.insert(QStringLiteral("items"), items);
        categories.append(group);
    }

    return categories;
}

QString DatabaseManager::presetCommand(int presetId) const
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral("SELECT command FROM presets WHERE id = ?"));
    q.addBindValue(presetId);
    if (!q.exec() || !q.next())
        return {};
    return q.value(0).toString();
}

QString DatabaseManager::presetCategory(int presetId) const
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral("SELECT category FROM presets WHERE id = ?"));
    q.addBindValue(presetId);
    if (!q.exec() || !q.next())
        return {};
    return q.value(0).toString();
}

bool DatabaseManager::deleteCustomPreset(int id)
{
    if (id <= 0)
        return false;
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral("DELETE FROM presets WHERE id = ? AND category = ?"));
    q.addBindValue(id);
    q.addBindValue(QStringLiteral("Personalizados"));
    if (!q.exec())
        return false;
    return q.numRowsAffected() > 0;
}

bool DatabaseManager::updateCustomPreset(int id,
                                         const QString &label,
                                         const QString &command,
                                         const QString &cwdOpt,
                                         const QString &buttonColorHex)
{
    if (id <= 0)
        return false;

    QSqlQuery check(QSqlDatabase::database(connectionName()));
    check.prepare(QStringLiteral("SELECT 1 FROM presets WHERE id = ? AND category = ?"));
    check.addBindValue(id);
    check.addBindValue(QStringLiteral("Personalizados"));
    if (!check.exec() || !check.next())
        return false;

    const QString lab = label.trimmed();
    const QString cmd = command.trimmed();
    if (lab.isEmpty() || cmd.isEmpty())
        return false;

    QVariant cwdBind;
    const QString trimmedCwd = cwdOpt.trimmed();
    if (trimmedCwd.isEmpty()) {
        cwdBind = QVariant();
    } else {
        const QFileInfo info(trimmedCwd);
        if (!info.isDir())
            return false;
        cwdBind = QDir(trimmedCwd).canonicalPath();
    }

    const QString col = normalizeButtonColorHex(buttonColorHex);
    QVariant colorBind;
    if (col.isEmpty())
        colorBind = QVariant();
    else
        colorBind = col;

    QSqlQuery u(QSqlDatabase::database(connectionName()));
    u.prepare(QStringLiteral(
        "UPDATE presets SET label = ?, command = ?, cwd = ?, button_color = ? WHERE id = ? AND "
        "category = ?"));
    u.addBindValue(lab);
    u.addBindValue(cmd);
    u.addBindValue(cwdBind);
    u.addBindValue(colorBind);
    u.addBindValue(id);
    u.addBindValue(QStringLiteral("Personalizados"));
    return u.exec();
}

QString DatabaseManager::presetCwd(int presetId) const
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral("SELECT cwd FROM presets WHERE id = ?"));
    q.addBindValue(presetId);
    if (!q.exec() || !q.next())
        return {};
    return q.value(0).toString();
}

void DatabaseManager::insertRunLog(const QString &cwd,
                                   const QString &command,
                                   int exitCode,
                                   qint64 durationMs,
                                   const QString &outputTail)
{
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.prepare(QStringLiteral(
        "INSERT INTO run_log(started_at, cwd, command, exit_code, duration_ms, output_tail) "
        "VALUES(datetime('now'),?,?,?,?,?)"));
    q.addBindValue(cwd);
    q.addBindValue(command);
    q.addBindValue(exitCode);
    q.addBindValue(static_cast<qint64>(durationMs));
    q.addBindValue(outputTail.left(8000));
    q.exec();
}

QVariantList DatabaseManager::recentRuns(int limit) const
{
    QVariantList out;
    QSqlQuery q(QSqlDatabase::database(connectionName()));
    q.setForwardOnly(true);
    q.prepare(QStringLiteral(
        "SELECT started_at, cwd, command, exit_code, duration_ms "
        "FROM run_log ORDER BY id DESC LIMIT ?"));
    q.addBindValue(limit);
    if (!q.exec())
        return out;

    while (q.next()) {
        QVariantMap row;
        row.insert(QStringLiteral("startedAt"), q.value(0));
        row.insert(QStringLiteral("cwd"), q.value(1));
        row.insert(QStringLiteral("command"), q.value(2));
        row.insert(QStringLiteral("exitCode"), q.value(3));
        row.insert(QStringLiteral("durationMs"), q.value(4));
        out.append(row);
    }
    return out;
}
