#include "workstation_bridge.h"

#include "command_runner.h"
#include "database_manager.h"
#include "terminal_launcher.h"

#include <QtGlobal>
#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QProcess>
#include <QUrl>

namespace {

QString localPathFromUrl(const QUrl &url)
{
    if (url.isLocalFile())
        return url.toLocalFile();
    return url.toString();
}

} // namespace

WorkstationBridge::WorkstationBridge(QObject *parent)
    : QObject(parent)
    , m_db(std::make_unique<DatabaseManager>())
    , m_runner(std::make_unique<CommandRunner>(this))
{
    connect(m_runner.get(),
            &CommandRunner::outputLine,
            this,
            &WorkstationBridge::onRunnerOutput);
    connect(m_runner.get(), &CommandRunner::finished, this, &WorkstationBridge::onRunnerFinished);

    m_workingDirectory = QDir::homePath();
}

WorkstationBridge::~WorkstationBridge() = default;

QString WorkstationBridge::workingDirectory() const
{
    return m_workingDirectory;
}

void WorkstationBridge::setWorkingDirectory(const QString &path)
{
    const QFileInfo info(path);
    const QString canon = info.isDir() ? QDir(path).canonicalPath() : path;
    const QString use = canon.isEmpty() ? path : canon;
    if (m_workingDirectory == use)
        return;
    m_workingDirectory = use;
    if (m_databaseOpen)
        m_db->saveSetting(QStringLiteral("working_directory"), m_workingDirectory);
    emit workingDirectoryChanged();
    refreshGit();
}

QString WorkstationBridge::gitBranch() const
{
    return m_gitBranch;
}

QString WorkstationBridge::gitStatus() const
{
    return m_gitStatus;
}

QString WorkstationBridge::gitRemote() const
{
    return m_gitRemote;
}

bool WorkstationBridge::gitRepo() const
{
    return m_gitRepo;
}

bool WorkstationBridge::gitDirty() const
{
    return m_gitDirty;
}

bool WorkstationBridge::busy() const
{
    return m_busy;
}

bool WorkstationBridge::databaseOpen() const
{
    return m_databaseOpen;
}

QString WorkstationBridge::databasePath() const
{
    return m_db->databasePath();
}

bool WorkstationBridge::useSystemTerminal() const
{
    return m_useSystemTerminal;
}

void WorkstationBridge::setUseSystemTerminal(bool v)
{
    if (m_useSystemTerminal == v)
        return;
    m_useSystemTerminal = v;
    if (m_databaseOpen)
        m_db->saveSetting(QStringLiteral("use_system_terminal"),
                           v ? QStringLiteral("1") : QStringLiteral("0"));
    emit useSystemTerminalChanged();
}

bool WorkstationBridge::initDatabase()
{
    const bool ok = m_db->open();
    if (m_databaseOpen != ok) {
        m_databaseOpen = ok;
        emit databaseOpenChanged();
    }

    if (!ok)
        return false;

    const QString stored = m_db->loadSetting(QStringLiteral("working_directory"));
    if (!stored.isEmpty()) {
        const QFileInfo info(stored);
        if (info.isDir()) {
            if (m_workingDirectory != stored) {
                m_workingDirectory = QDir(stored).canonicalPath();
                emit workingDirectoryChanged();
            }
        }
    } else {
        m_db->saveSetting(QStringLiteral("working_directory"), m_workingDirectory);
    }

    const QString u = m_db->loadSetting(QStringLiteral("use_system_terminal"));
    if (u.isEmpty()) {
#ifdef Q_OS_LINUX
        m_useSystemTerminal = true;
#else
        m_useSystemTerminal = false;
#endif
    } else {
        m_useSystemTerminal = (u == QLatin1String("1"));
    }
    emit useSystemTerminalChanged();

    refreshGit();
    return true;
}

QVariantList WorkstationBridge::presetGroups()
{
    if (!m_databaseOpen)
        return {};
    return m_db->fetchPresetsGrouped();
}

QVariantList WorkstationBridge::recentRuns(int limit)
{
    if (!m_databaseOpen)
        return {};
    return m_db->recentRuns(limit);
}

void WorkstationBridge::runPreset(int presetId)
{
    if (!m_databaseOpen || presetId <= 0)
        return;

    const QString cmd = m_db->presetCommand(presetId);
    if (cmd.isEmpty())
        return;

    const QString cat = m_db->presetCategory(presetId);
    const QString storedCwd = m_db->presetCwd(presetId);
    const QString wd = resolvePresetWorkingDirectory(cat, storedCwd);

    if (m_useSystemTerminal) {
        tryLaunchInSystemTerminal(cmd, wd);
        return;
    }

    if (m_busy)
        return;

    beginRun(cmd, wd);
    m_runner->startShellCommand(cmd, wd);
}

void WorkstationBridge::runShellCommand(const QString &command)
{
    const QString trimmed = command.trimmed();
    if (trimmed.isEmpty())
        return;

    const QString wd = resolveCwd();

    if (m_useSystemTerminal) {
        tryLaunchInSystemTerminal(trimmed, wd);
        return;
    }

    if (m_busy)
        return;

    beginRun(trimmed, wd);
    m_runner->startShellCommand(trimmed, wd);
}

void WorkstationBridge::stopCommand()
{
    m_runner->stop();
}

QString WorkstationBridge::pickFolderUrl(const QUrl &url)
{
    const QString path = localPathFromUrl(url);
    if (!path.isEmpty())
        setWorkingDirectory(path);
    return m_workingDirectory;
}

QString WorkstationBridge::urlToLocalPath(const QUrl &url) const
{
    return localPathFromUrl(url);
}

bool WorkstationBridge::saveCustomPreset(const QString &name,
                                        const QString &command,
                                        const QString &cwd,
                                        const QString &buttonColor)
{
    if (!m_databaseOpen)
        return false;

    const QString label = name.trimmed();
    const QString cmd = command.trimmed();
    if (label.isEmpty() || cmd.isEmpty())
        return false;

    return m_db->addPreset(QStringLiteral("Personalizados"), label, cmd, cwd.trimmed(), buttonColor);
}

bool WorkstationBridge::deleteCustomPreset(int id)
{
    if (!m_databaseOpen)
        return false;
    return m_db->deleteCustomPreset(id);
}

bool WorkstationBridge::updateCustomPreset(int id,
                                          const QString &name,
                                          const QString &command,
                                          const QString &cwd,
                                          const QString &buttonColor)
{
    if (!m_databaseOpen)
        return false;
    return m_db->updateCustomPreset(id, name, command, cwd.trimmed(), buttonColor);
}

void WorkstationBridge::refreshGit()
{
    QString combined;
    m_gitRepo = (runGitCapture({QStringLiteral("rev-parse"), QStringLiteral("--is-inside-work-tree")}, &combined)
                 == 0)
        && (combined.trimmed() == QLatin1String("true"));

    combined.clear();
    if (m_gitRepo) {
        m_gitDirty = (runGitCapture({QStringLiteral("status"), QStringLiteral("--porcelain")}, &combined) == 0)
            && !combined.trimmed().isEmpty();
    } else {
        m_gitDirty = false;
    }

    if (runGitCapture({QStringLiteral("branch"), QStringLiteral("--show-current")}, &combined)
        == 0) {
        m_gitBranch = combined.trimmed();
    } else {
        m_gitBranch.clear();
    }

    combined.clear();
    if (runGitCapture({QStringLiteral("status"), QStringLiteral("-sb")}, &combined) == 0) {
        m_gitStatus = combined.trimmed();
    } else {
        m_gitStatus = tr("(not a git repository or git not available)");
    }

    combined.clear();
    if (runGitCapture({QStringLiteral("remote"), QStringLiteral("-v")}, &combined) == 0) {
        m_gitRemote = combined.trimmed();
    } else {
        m_gitRemote.clear();
    }

    emit gitInfoChanged();
}

void WorkstationBridge::gitPull()
{
    if (m_busy)
        return;
    runShellCommand(QStringLiteral("git pull"));
}

void WorkstationBridge::gitPush()
{
    if (m_busy)
        return;
    runShellCommand(QStringLiteral("git push"));
}

namespace {

QString shellSingleQuoteForSh(const QString &s)
{
    QString out = QStringLiteral("'");
    for (QChar c : s) {
        if (c == QLatin1Char('\''))
            out += QLatin1String("'\\''");
        else
            out += c;
    }
    out += QLatin1Char('\'');
    return out;
}

} // namespace

void WorkstationBridge::gitCommitAll(const QString &message)
{
    const QString msg = message.trimmed();
    if (msg.isEmpty() || !m_gitRepo || m_busy)
        return;

    const QString cmd = QStringLiteral("git add -A && git commit -m ") + shellSingleQuoteForSh(msg);
    runShellCommand(cmd);
}

void WorkstationBridge::gitDiscardChanges()
{
    if (!m_gitRepo || m_busy)
        return;
    runShellCommand(QStringLiteral("git reset --hard HEAD"));
}

void WorkstationBridge::onRunnerOutput(const QString &text, bool isStdErr)
{
    appendOutput(text, isStdErr);
}

void WorkstationBridge::onRunnerFinished(int exitCode)
{
    setBusy(false);
    const QString finishedCommand = m_currentCommand;

    if (m_databaseOpen && !finishedCommand.isEmpty()) {
        const qint64 duration = QDateTime::currentMSecsSinceEpoch() - m_runStartMs;
        m_db->insertRunLog(m_runCwd, finishedCommand, exitCode, duration, m_outputBuffer);
    }

    appendOutput(tr("\n— Finished with exit code %1 —\n").arg(exitCode), false);
    m_currentCommand.clear();

    if (finishedCommand.startsWith(QStringLiteral("git"), Qt::CaseInsensitive))
        refreshGit();
}

QString WorkstationBridge::resolveCwd() const
{
    const QDir dir(m_workingDirectory);
    return dir.exists() ? dir.absolutePath() : QDir::homePath();
}

QString WorkstationBridge::resolvePresetWorkingDirectory(const QString &category,
                                                         const QString &storedCwd) const
{
    const QString trimmed = storedCwd.trimmed();
    if (trimmed.isEmpty()) {
        if (category == QStringLiteral("Personalizados"))
            return QDir::homePath();
        return resolveCwd();
    }
    const QFileInfo info(trimmed);
    if (info.isDir())
        return QDir(trimmed).canonicalPath();
    if (category == QStringLiteral("Personalizados"))
        return QDir::homePath();
    return resolveCwd();
}

void WorkstationBridge::setBusy(bool v)
{
    if (m_busy == v)
        return;
    m_busy = v;
    emit busyChanged();
}

void WorkstationBridge::beginRun(const QString &command, const QString &cwdForRun)
{
    m_currentCommand = command;
    m_runCwd = cwdForRun;
    m_outputBuffer.clear();
    m_runStartMs = QDateTime::currentMSecsSinceEpoch();
    setBusy(true);
    appendOutput(tr("▶ %1\n").arg(command), false);
    appendOutput(tr("   cwd: %1\n").arg(cwdForRun), false);
}

void WorkstationBridge::appendOutput(const QString &text, bool isStdErr)
{
    emit outputLine(text, isStdErr);
    m_outputBuffer.append(text);
    if (m_outputBuffer.size() > 48000)
        m_outputBuffer = m_outputBuffer.right(40000);
}

bool WorkstationBridge::tryLaunchInSystemTerminal(const QString &command, const QString &cwdForRun)
{
    appendOutput(tr("▶ (system terminal) %1\n").arg(command), false);
    appendOutput(tr("   cwd: %1\n").arg(cwdForRun), false);
    if (TerminalLauncher::launchCommand(cwdForRun, command)) {
        appendOutput(tr("Opened a new terminal window (interactive / sudo ready).\n"), false);
        return true;
    }
    appendOutput(
        tr("Could not open a system terminal. Install gnome-terminal, konsole, or xterm — "
           "or turn off \"System terminal\" in the bar below.\n"),
        true);
    return false;
}

int WorkstationBridge::runGitCapture(const QStringList &args, QString *combinedOutput) const
{
    QProcess p;
    p.setProgram(QStringLiteral("git"));
    p.setArguments(args);
    p.setWorkingDirectory(resolveCwd());
    p.setProcessChannelMode(QProcess::MergedChannels);
    p.start();
    if (!p.waitForFinished(8000)) {
        if (combinedOutput)
            combinedOutput->clear();
        return -1;
    }
    if (combinedOutput)
        *combinedOutput = QString::fromUtf8(p.readAllStandardOutput());
    return p.exitCode();
}
