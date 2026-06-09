#pragma once

#include <QString>

/** Opens a real TTY via the user's terminal emulator (Linux / macOS / Windows). */
class TerminalLauncher
{
public:
    static bool launchCommand(const QString &workingDirectory, const QString &command);
};
