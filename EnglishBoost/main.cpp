#include "EnglishBoost.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    //привет Кирилл, я твой единственный подписчик 
    //Amogus sus sussy baka
    QApplication a(argc, argv);
    QFile file("StyleSheet.css"); //css file.
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);
    file.close();
    EnglishBoost w;
    //w.setWindowFlags(Qt::FramelessWindowHint);
    w.show();
    return a.exec();
}
