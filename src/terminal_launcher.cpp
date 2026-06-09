#include "terminal_launcher.h"

#include <QDir>
#include <QFile>
#include <QFileDevice>
#include <QProcess>
#include <QStandardPaths>
#include <QTemporaryFile>
#include <QTextStream>

namespace {

QString bashSingleQuoted(const QString &s)
{
    QString r;
    r += QLatin1Char('\'');
    for (QChar c : s) {
        if (c == u'\'')
            r += QStringLiteral("'\\''");
        else
            r += c;
    }
    r += QLatin1Char('\'');
    return r;
}

bool startDetached(const QString &program, const QStringList &args)
{
    return QProcess::startDetached(program, args);
}

#ifdef Q_OS_LINUX

bool writeBashScript(const QString &workDir, const QString &command, QString *outPath)
{
    QTemporaryFile f;
    f.setFileTemplate(QDir::tempPath() + QStringLiteral("/cw_run_XXXXXX.sh"));
    f.setAutoRemove(false);
    if (!f.open())
        return false;

    f.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);

    QTextStream out(&f);
    out << QStringLiteral("#!/usr/bin/env bash\n");
    out << QStringLiteral("set +e\n");
    out << QStringLiteral("cd ") << bashSingleQuoted(QDir(workDir).absolutePath()) << QLatin1Char('\n');
    out << command << QLatin1Char('\n');
    out << QStringLiteral("cw_status=$?\n");
    out << QStringLiteral("echo\n");
    out << QStringLiteral("echo \"[Command Workstation] Exit code: $cw_status\"\n");
    out << QStringLiteral("read -n1 -p \"Press any key to close…\"\n");
    out << QStringLiteral("exit $cw_status\n");
    f.close();
    *outPath = f.fileName();
    return true;
}

bool tryLinuxTerminal(const QString &workDir, const QString &scriptPath)
{
    const QString wd = QDir(workDir).absolutePath();
    const QString bashExec =
        QStringLiteral("bash ") + bashSingleQuoted(QDir::cleanPath(scriptPath));

    const QString gnome = QStandardPaths::findExecutable(QStringLiteral("gnome-terminal"));
    if (!gnome.isEmpty()) {
        if (startDetached(gnome, {QStringLiteral("--working-directory"), wd, QStringLiteral("--"),
                                   QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString konsole = QStandardPaths::findExecutable(QStringLiteral("konsole"));
    if (!konsole.isEmpty()) {
        if (startDetached(konsole, {QStringLiteral("--workdir"), wd, QStringLiteral("-e"),
                                    QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString xfce = QStandardPaths::findExecutable(QStringLiteral("xfce4-terminal"));
    if (!xfce.isEmpty()) {
        if (startDetached(xfce, {QStringLiteral("--working-directory"), wd, QStringLiteral("-e"),
                                 bashExec}))
            return true;
    }

    const QString kitty = QStandardPaths::findExecutable(QStringLiteral("kitty"));
    if (!kitty.isEmpty()) {
        if (startDetached(kitty,
                            {QStringLiteral("--directory"), wd, QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString alacritty = QStandardPaths::findExecutable(QStringLiteral("alacritty"));
    if (!alacritty.isEmpty()) {
        if (startDetached(alacritty, {QStringLiteral("--working-directory"), wd,
                                      QStringLiteral("-e"), QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString terminator = QStandardPaths::findExecutable(QStringLiteral("terminator"));
    if (!terminator.isEmpty()) {
        if (startDetached(terminator, {QStringLiteral("--working-directory"), wd,
                                        QStringLiteral("-e"), bashExec}))
            return true;
    }

    const QString foot = QStandardPaths::findExecutable(QStringLiteral("foot"));
    if (!foot.isEmpty()) {
        if (startDetached(foot, {QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString xte = QStandardPaths::findExecutable(QStringLiteral("x-terminal-emulator"));
    if (!xte.isEmpty()) {
        if (startDetached(xte, {QStringLiteral("-e"), QStringLiteral("bash"), scriptPath}))
            return true;
    }

    const QString xterm = QStandardPaths::findExecutable(QStringLiteral("xterm"));
    if (!xterm.isEmpty()) {
        if (startDetached(xterm, {QStringLiteral("-title"), QStringLiteral("Command Workstation"),
                                  QStringLiteral("-e"), QStringLiteral("bash"), scriptPath}))
            return true;
    }

    QFile::remove(scriptPath);
    return false;
}

#endif // Q_OS_LINUX

#ifdef Q_OS_WIN

bool writeBatchFile(const QString &workDir, const QString &command, QString *outPath)
{
    QTemporaryFile f;
    f.setFileTemplate(QDir::tempPath() + QStringLiteral("/cw_run_XXXXXX.bat"));
    f.setAutoRemove(false);
    if (!f.open())
        return false;

    QTextStream out(&f);
    out << QStringLiteral("@echo off\r\n");
    out << QStringLiteral("cd /d ") << QChar('"') << QDir::toNativeSeparators(QDir(workDir).absolutePath())
        << QChar('"') << QStringLiteral("\r\n");
    out << command << QStringLiteral("\r\n");
    out << QStringLiteral("echo.\r\n");
    out << QStringLiteral("pause\r\n");
    f.close();
    *outPath = f.fileName();
    return true;
}

bool tryWindowsTerminal(const QString &workDir, const QString &batPath)
{
    const QString wt = QStandardPaths::findExecutable(QStringLiteral("wt.exe"));
    if (!wt.isEmpty()) {
        QString wdNative = QDir::toNativeSeparators(QDir(workDir).absolutePath());
        if (startDetached(wt, {QStringLiteral("-d"), wdNative, QStringLiteral("cmd"), QStringLiteral("/k"),
                              QDir::toNativeSeparators(batPath)}))
            return true;
    }

    if (startDetached(QStringLiteral("cmd.exe"),
                      {QStringLiteral("/c"), QStringLiteral("start"), QStringLiteral("Command Workstation"),
                       QStringLiteral("cmd"), QStringLiteral("/k"),
                       QDir::toNativeSeparators(batPath)}))
        return true;

    QFile::remove(batPath);
    return false;
}

#endif // Q_OS_WIN

#ifdef Q_OS_MACOS

bool writeMacScript(const QString &workDir, const QString &command, QString *outPath)
{
    QTemporaryFile f;
    f.setFileTemplate(QDir::tempPath() + QStringLiteral("/cw_run_XXXXXX.command"));
    f.setAutoRemove(false);
    if (!f.open())
        return false;

    f.setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner);

    QTextStream out(&f);
    out << QStringLiteral("#!/usr/bin/env bash\n");
    out << QStringLiteral("set +e\n");
    out << QStringLiteral("cd ") << bashSingleQuoted(QDir(workDir).absolutePath()) << QLatin1Char('\n');
    out << command << QLatin1Char('\n');
    out << QStringLiteral("echo\n");
    out << QStringLiteral("read -n1 -p \"Press any key to close…\"\n");
    f.close();
    *outPath = f.fileName();
    return true;
}

#endif // Q_OS_MACOS

} // namespace

bool TerminalLauncher::launchCommand(const QString &workingDirectory, const QString &command)
{
    if (workingDirectory.trimmed().isEmpty() || command.trimmed().isEmpty())
        return false;

#ifdef Q_OS_LINUX
    QString scriptPath;
    if (!writeBashScript(workingDirectory, command, &scriptPath))
        return false;
    return tryLinuxTerminal(workingDirectory, scriptPath);
#elif defined(Q_OS_WIN)
    QString batPath;
    if (!writeBatchFile(workingDirectory, command, &batPath))
        return false;
    return tryWindowsTerminal(workingDirectory, batPath);
#elif defined(Q_OS_MACOS)
    QString scriptPath;
    if (!writeMacScript(workingDirectory, command, &scriptPath))
        return false;
    return startDetached(QStringLiteral("/usr/bin/open"), {scriptPath});
#else
    Q_UNUSED(workingDirectory);
    Q_UNUSED(command);
    return false;
#endif
}
