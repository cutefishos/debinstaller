#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QCommandLineParser>
#include <QTranslator>
#include <QFile>

#include "debinstaller.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(true);

    qmlRegisterUncreatableType<DebInstaller>("Cutefish.DebInstaller", 1, 0,
                                             "DebInstaller", "error");

    // Translations
    QLocale locale;
    QString qmFilePath = QString("%1/%2.qm").arg("/usr/share/cutefish-debinstaller/translations/").arg(locale.name());
    if (QFile::exists(qmFilePath)) {
        QTranslator *translator = new QTranslator(app.instance());
        if (translator->load(qmFilePath)) {
            app.installTranslator(translator);
        } else {
            translator->deleteLater();
        }
    }

    qmlRegisterUncreatableType<DebInstaller>("Cutefish.DebInstaller", 1, 0, "DebInstaller", "error");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("file", ".deb file");
    parser.process(app);

    QString fileName;
    if (!parser.positionalArguments().isEmpty()) {
        fileName = parser.positionalArguments().first();
    }

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);

    DebInstaller *debInstaller = new DebInstaller;
    engine.rootContext()->setContextProperty("Installer", debInstaller);
    engine.load(url);
    debInstaller->setFileName(fileName);

    return app.exec();
}
