#pragma once
#include <QWidget>
#include <QPaintEvent>
#include <QHideEvent>
#include <QPainter>
#include <QSlider>
#include <QPushButton>
#include <QTimer>
#include <QResizeEvent>
#include <QSqlTableModel>
#include <QtSql>              
#include <QSqlDatabase>       
#include <QSqlQuery>
#include <map>
#include <algorithm>
#include <ctime>
#include "Peak.h"           
#include "OtherFunc.h"
#include <QLabel>
#include <QSound>
#include <QSoundEffect>
#include <QMediaPlayer>
#include <QSlider>


using namespace std;

class EnglishBoost;

class Tile
{
  friend class Table;
  friend class EnglishBoost;
  QChar letter;
  int corn_x;
  int corn_y;
  QColor tile_color;
public:
  Tile(QChar l='\0', int x=0, int y=0);
};

class Table
{
  friend class EnglishBoost;
  int tile_width;
  int current_level=1;              //Переменная, в которой будет лежать id текущего уровня.
  map<QString, int> words;        //map со словами и их длинной.
  list<QString> synonyms;         //Синонимы  
  int size_field;                 //Размер сетки (size_field*size_field)
  vector<Peak*> peaks;       //Представление сетки в виде графа.
  list<int> occupied_cells;// Поле для алгоритма размещения.
  list<int> drawing_tiles;   //Номера ячеек, которые красим
  list<int> found_synonyms;
  list<int> hint_letters;                                               //Подкрашиваем подскази!!
  list<QString> found_words; //Все найденные слова
  Tile** tbl;
  int showed_letters = 0;
  int index_showed_letter=-1;
public:

  Table(int scale, int c);
  void NewSize(int scale);
  void Draw(QPainter* QP);
  void GetDataFromBD(int lvl);                // И эту
  void ClearLastWay(int index, int j);        //Вспомогательная функция
  void GeneratePositions();                   //Сам алгоритм размещения слов
  ~Table();
};

class SettingsWindow :public QWidget
{
    Q_OBJECT

    friend class EnglishBoost;
    int scale;
    EnglishBoost* parent;
    QSlider* volume;
    QPushButton* close;
    QPushButton* quit;
    QPushButton* r;
    QWidget* field;

public:
    SettingsWindow(EnglishBoost* parent);
    ~SettingsWindow();
public:
    void paintEvent(QPaintEvent* event);
    void timerEvent(QTimerEvent* event);
public slots:
    void OpenMenu();
    void closeParent();
    void closeMe();
    void checkSound();
};

class EnglishBoost:public QWidget
{
  Q_OBJECT

  friend class Table;
  friend class SettingsWindow;

  SettingsWindow* Settings_w;
  QWidget* words_w;
  QWidget* message_window;
  int max_opened_level;
  QPushButton* QPB;
  QPushButton* settings;
  QPushButton* Hint_one_letter;
  QPushButton* Hint_full_word;
  QLabel* level;
  Table* Board;
  QSoundEffect c;
  QSoundEffect d;
  QSoundEffect blocked;
  QMediaPlayer* right;
  QMediaPlayer* click;
  QMediaPlayer* win;
  QMediaPlayer* mistake;
  QMediaPlayer* hint;
  QString entered_word;       // Слово, которое натыкали
  list<int>::iterator start_t;
  list<int>::iterator end_t;
  QList<QPushButton*> levels;
  QList<QLabel*> number_for_words;
  bool clicked = false;
  int TimerID=0;
  int TimColorID=0;
  int TimHintId = 0;
  int ShakeTimerId = 0;
  int countShakes = 0;
  int shakingButton;
  int scale;
  int SF;
  int max_level;
  int max_words;
  int volume = 100;
  int R, G, B, nR, nG, nB;
  bool inActive = false;
  //bool menu = true;
  bool Next_level;
  QPixmap* pixmap;

public:
    EnglishBoost();
    void RestoreWords();
    void RestotrHints();
    void ChangeColor(int r, int g, int b, int period = 100);
    bool CheckWord(QString s);
    void WordWasFounded();
    void paintEvent(QPaintEvent* event);
    void timerEvent(QTimerEvent* event);
    void resizeEvent(QResizeEvent* event);
    void mousePressEvent(QMouseEvent* event);
    bool ConnectBD();
public slots:
  void Shake();
  void OpenSettings();
  void Generate();
  void ShowOneLetter();
  void ShowWord();
private:
};



