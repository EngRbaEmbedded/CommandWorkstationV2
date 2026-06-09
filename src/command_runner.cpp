#include "command_runner.h"

#include <QProcess>

namespace {

void splitShellCommand(const QString &command, QString *outProgram, QStringList *outArgs)
{
#ifdef Q_OS_WIN
    *outProgram = QStringLiteral("cmd.exe");
    *outArgs = QStringList{QStringLiteral("/d"), QStringLiteral("/s"), QStringLiteral("/c"), command};
#else
    *outProgram = QStringLiteral("/bin/sh");
    *outArgs = QStringList{QStringLiteral("-c"), command};
#endif
}

} // namespace

CommandRunner::CommandRunner(QObject *parent)
    : QObject(parent)
    , m_process(new QProcess(this))
{
    m_process->setProcessChannelMode(QProcess::SeparateChannels);
    connect(m_process, &QProcess::readyReadStandardOutput, this, &CommandRunner::forwardStdout);
    connect(m_process, &QProcess::readyReadStandardError, this, &CommandRunner::forwardStderr);
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    connect(m_process,
            &QProcess::finished,
            this,
            [this](int exitCode, QProcess::ExitStatus status) {
                Q_UNUSED(status);
                onFinished(exitCode);
            });
#else
    connect(m_process,
            QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished),
            this,
            [this](int exitCode, QProcess::ExitStatus status) {
                Q_UNUSED(status);
                onFinished(exitCode);
            });
#endif
}

CommandRunner::~CommandRunner()
{
    stop();
}

bool CommandRunner::isRunning() const
{
    return m_process->state() != QProcess::NotRunning;
}

void CommandRunner::startShellCommand(const QString &command, const QString &workingDirectory)
{
    stop();

    QString program;
    QStringList arguments;
    splitShellCommand(command, &program, &arguments);

    m_process->setProgram(program);
    m_process->setArguments(arguments);
    m_process->setWorkingDirectory(workingDirectory);
    m_process->start();
    if (!m_process->waitForStarted(3000)) {
        emit outputLine(tr("Failed to start shell: %1").arg(m_process->errorString()), true);
        emit finished(-1);
        return;
    }
    emit started(program, arguments);
}

void CommandRunner::stop()
{
    if (!m_process || m_process->state() == QProcess::NotRunning)
        return;

#ifdef Q_OS_WIN
    m_process->kill();
#else
    m_process->terminate();
    if (!m_process->waitForFinished(2000))
        m_process->kill();
#endif
    m_process->waitForFinished(5000);
}

void CommandRunner::forwardStdout()
{
    const QByteArray chunk = m_process->readAllStandardOutput();
    emit outputLine(QString::fromUtf8(chunk), false);
}

void CommandRunner::forwardStderr()
{
    const QByteArray chunk = m_process->readAllStandardError();
    emit outputLine(QString::fromUtf8(chunk), true);
}

void CommandRunner::onFinished(int exitCode)
{
    emit finished(exitCode);
}
