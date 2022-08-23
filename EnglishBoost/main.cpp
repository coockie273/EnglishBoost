#include "EnglishBoost.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile file("StyleSheet.css"); //css file.
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
    EnglishBoost w;
    w.show();
    return a.exec();
}
