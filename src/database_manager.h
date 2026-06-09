#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

class DatabaseManager : public QObject
{
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);

    bool open();
    void close();

    QString loadSetting(const QString &key, const QString &defaultValue = {}) const;
    void saveSetting(const QString &key, const QString &value);

    QVariantList fetchPresetsGrouped() const;
    void seedDefaultsIfEmpty();

    void insertRunLog(const QString &cwd,
                      const QString &command,
                      int exitCode,
                      qint64 durationMs,
                      const QString &outputTail);

    QVariantList recentRuns(int limit = 50) const;

    QString databasePath() const { return m_dbPath; }

    QString presetCommand(int presetId) const;
    QString presetCwd(int presetId) const;

    bool addPreset(const QString &category,
                   const QString &label,
                   const QString &command,
                   const QString &cwdOpt,
                   const QString &buttonColorHex = {});

    bool deleteCustomPreset(int id);
    bool updateCustomPreset(int id,
                            const QString &label,
                            const QString &command,
                            const QString &cwdOpt,
                            const QString &buttonColorHex = {});

    QString presetCategory(int presetId) const;

private:
    bool createSchema();
    void migratePresetsAddCwd();
    void migratePresetsAddButtonColor();
    int nextCategorySortOrder(const QString &category) const;
    QString m_dbPath;
};
