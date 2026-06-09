#pragma once

#include <QObject>
#include <QString>
#include <QVariantList>

#include <memory>

class CommandRunner;
class DatabaseManager;
class QUrl;

class WorkstationBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString workingDirectory READ workingDirectory WRITE setWorkingDirectory NOTIFY
                   workingDirectoryChanged)
    Q_PROPERTY(QString gitBranch READ gitBranch NOTIFY gitInfoChanged)
    Q_PROPERTY(QString gitStatus READ gitStatus NOTIFY gitInfoChanged)
    Q_PROPERTY(QString gitRemote READ gitRemote NOTIFY gitInfoChanged)
    Q_PROPERTY(bool gitRepo READ gitRepo NOTIFY gitInfoChanged)
    Q_PROPERTY(bool gitDirty READ gitDirty NOTIFY gitInfoChanged)
    Q_PROPERTY(bool busy READ busy NOTIFY busyChanged)
    Q_PROPERTY(bool databaseOpen READ databaseOpen NOTIFY databaseOpenChanged)
    Q_PROPERTY(QString databasePath READ databasePath NOTIFY databaseOpenChanged)
    Q_PROPERTY(bool useSystemTerminal READ useSystemTerminal WRITE setUseSystemTerminal NOTIFY
                   useSystemTerminalChanged)

public:
    explicit WorkstationBridge(QObject *parent = nullptr);
    ~WorkstationBridge() override;

    QString workingDirectory() const;
    void setWorkingDirectory(const QString &path);

    QString gitBranch() const;
    QString gitStatus() const;
    QString gitRemote() const;
    bool gitRepo() const;
    bool gitDirty() const;
    bool busy() const;
    bool databaseOpen() const;
    QString databasePath() const;
    bool useSystemTerminal() const;
    void setUseSystemTerminal(bool v);

    Q_INVOKABLE bool initDatabase();
    Q_INVOKABLE QVariantList presetGroups();
    Q_INVOKABLE QVariantList recentRuns(int limit = 40);
    Q_INVOKABLE void runPreset(int presetId);
    Q_INVOKABLE void runShellCommand(const QString &command);
    Q_INVOKABLE void stopCommand();
    Q_INVOKABLE void refreshGit();
    Q_INVOKABLE void gitPull();
    Q_INVOKABLE void gitPush();
    Q_INVOKABLE void gitCommitAll(const QString &message);
    Q_INVOKABLE void gitDiscardChanges();
    Q_INVOKABLE QString pickFolderUrl(const QUrl &url);
    Q_INVOKABLE QString urlToLocalPath(const QUrl &url) const;
    Q_INVOKABLE bool saveCustomPreset(const QString &name,
                                      const QString &command,
                                      const QString &cwd,
                                      const QString &buttonColor = {});
    Q_INVOKABLE bool deleteCustomPreset(int id);
    Q_INVOKABLE bool updateCustomPreset(int id,
                                        const QString &name,
                                        const QString &command,
                                        const QString &cwd,
                                        const QString &buttonColor = {});

signals:
    void workingDirectoryChanged();
    void gitInfoChanged();
    void busyChanged();
    void databaseOpenChanged();
    void useSystemTerminalChanged();
    void terminalCleared();
    void outputLine(const QString &text, bool isStdErr);

private slots:
    void onRunnerOutput(const QString &text, bool isStdErr);
    void onRunnerFinished(int exitCode);

private:
    QString resolveCwd() const;
    QString resolvePresetWorkingDirectory(const QString &category, const QString &storedCwd) const;
    void setBusy(bool v);
    void beginRun(const QString &command, const QString &cwdForRun);
    void appendOutput(const QString &text, bool isStdErr);
    int runGitCapture(const QStringList &args, QString *combinedOutput) const;

    bool tryLaunchInSystemTerminal(const QString &command, const QString &cwdForRun);

    QString m_workingDirectory;
    QString m_gitBranch;
    QString m_gitStatus;
    QString m_gitRemote;
    bool m_gitRepo = false;
    bool m_gitDirty = false;
    bool m_busy = false;
    bool m_databaseOpen = false;
    bool m_useSystemTerminal = false;

    QString m_currentCommand;
    QString m_outputBuffer;
    QString m_runCwd;
    qint64 m_runStartMs = 0;

    std::unique_ptr<DatabaseManager> m_db;
    std::unique_ptr<CommandRunner> m_runner;
};
