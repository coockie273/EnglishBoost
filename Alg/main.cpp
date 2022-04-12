#include "Alg.h"
#include <QtWidgets/QApplication>
#include <QSqlTableModel>
#include <QtSql>              // Если в этой строке ошибка жмём на верхней панели "Проект"->"Свойства"->"Qt Project Setting"-> в Qt Modules дописываем sql.
#include <QSqlDatabase>       //Итоговая строка в Qt Modules - core;gui;widgets;sql
#include <QSqlQuery>

int main(int argc, char *argv[])
{
    srand(time(0));
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("EnglishBoost.dat"); // Название файла, который лежит в папке с проектом, в нём будет вся заполненная БД.
    db.setHostName("localhost");
    db.setUserName("root");
    db.setPassword("root");
    if (!db.open()) {
        return false;
    }
    QApplication a(argc, argv);
    Alg w;
    w.TryAlg();
    w.showMaximized();


    return a.exec();
}
