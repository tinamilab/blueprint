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
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
        &app, [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
