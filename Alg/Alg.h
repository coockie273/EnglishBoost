#pragma once

#include <QtWidgets/QMainWindow>
#include "Peak.h"
#include <QSqlTableModel>
#include <QtSql>              // Если в этой строке ошибка жмём на верхней панели "Проект"->"Свойства"->"Qt Project Setting"-> в Qt Modules дописываем sql.
#include <QSqlDatabase>       //Итоговая строка в Qt Modules - core;gui;widgets;sql
#include <QSqlQuery>


class Alg : public QMainWindow
{
    Q_OBJECT

    int level = 2;
    int size;
    std::vector<Peak*> peaks;
    std::vector<int> size_words;
    std::vector<QString> words;
    std::vector<QPoint> lines_p;
    std::vector<int> occupied_cells;
public:
    Alg(QWidget *parent = Q_NULLPTR);
    //void timerEvent(QTimerEvent* event);
    void paintEvent(QPaintEvent* e);
    void TryAlg();
    void ClearLastWay(int i, int j);

};
