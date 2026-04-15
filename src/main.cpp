#include <QApplication>
#include <QFile>
#include <QTextStream>
#include "ui/MainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QApplication::setApplicationName(QStringLiteral("Postman App"));
    QApplication::setApplicationVersion(QStringLiteral("0.1.0"));
    QApplication::setOrganizationName(QStringLiteral("PostmanApp"));

    QFile styleFile(QStringLiteral(":/styles/postman.qss"));
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QTextStream stream(&styleFile);
        app.setStyleSheet(stream.readAll());
    }

    MainWindow window;
    window.show();

    return QApplication::exec();
}
