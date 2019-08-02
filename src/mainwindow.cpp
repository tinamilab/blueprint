#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QDebug>

#include "backend.h"
//#include "control_midi.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    //qmlRegisterType<Control_midi>("com.tinami.control_midi", 1, 0, "Control_midi");
    qmlRegisterType<BackEnd>("com.tinami.backend", 1, 0, "BackEnd");

    QQmlApplicationEngine engine;

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
