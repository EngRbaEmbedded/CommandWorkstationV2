#include "workstation_bridge.h"

#include <QCoreApplication>
#include <QFont>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("CommandWorkstation"));
    QCoreApplication::setApplicationName(QStringLiteral("Command Workstation"));

    QFont appFont;
    appFont.setFamilies({QStringLiteral("JetBrains Sans"), QStringLiteral("JetBrains Mono"),
                         QStringLiteral("Noto Sans"), QStringLiteral("sans-serif")});
    app.setFont(appFont);

    WorkstationBridge bridge;
    if (!bridge.initDatabase())
        qWarning("SQLite database could not be opened. Presets and history will be unavailable.");

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("App"), &bridge);
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("Command_Workstation", "Main");

    return QCoreApplication::exec();
}
