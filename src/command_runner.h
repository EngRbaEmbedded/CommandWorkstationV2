#pragma once

#include <QObject>
#include <QStringList>

class QProcess;

class CommandRunner : public QObject
{
    Q_OBJECT

public:
    explicit CommandRunner(QObject *parent = nullptr);
    ~CommandRunner() override;

    bool isRunning() const;

    void startShellCommand(const QString &command, const QString &workingDirectory);

public slots:
    void stop();

signals:
    void outputLine(const QString &text, bool isStdErr);
    void started(const QString &executable, const QStringList &arguments);
    void finished(int exitCode);

private slots:
    void forwardStdout();
    void forwardStderr();
    void onFinished(int exitCode);

private:
    QProcess *m_process = nullptr;
};
