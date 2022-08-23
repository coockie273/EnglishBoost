#include "EnglishBoost.h"

#define RAND_CLR rand() % 100 + 101

using namespace std;

int Extract(QString& data)
{
	int i = data.size() - 1;
	while (data[i] != ' ') i--;
	data.remove(0,i);
	return data.toInt();
}

EnglishBoost::EnglishBoost()
{
	ConnectBD();                           //������������ � ��
	QWidget* w = new QWidget;
	this->setObjectName("SecondLay");
	SF = 1; R = 0; G = 0; B = 0;

	QString cnfg = GetStringFromCnfg("scale");

	scale = Extract(cnfg);

	words_w = new QWidget(this);
	words_w->setObjectName("words");
	words_w->hide();

	Settings_w = new SettingsWindow(this);
	//Settings_w->setGeometry();
	Settings_w->hide();

	TimerID = 0;
	TimColorID = 0;
	right = new QMediaPlayer;
	right->setMedia(QUrl::fromLocalFile("sounds/right2.wav"));
	right->setVolume(100);
	hint = new QMediaPlayer;
	hint->setMedia(QUrl::fromLocalFile("sounds/hint.wav"));
	hint->setVolume(20);
	click = new QMediaPlayer;
	click->setMedia(QUrl::fromLocalFile("sounds/click.wav"));
	click->setVolume(100);
	win  = new QMediaPlayer;
	win->setMedia(QUrl::fromLocalFile("sounds/win.wav"));
	win->setVolume(35);

	mistake = new QMediaPlayer;
	mistake->setMedia(QUrl::fromLocalFile("sounds/mistake.wav"));
	mistake->setVolume(100);

	c.setSource(QUrl::fromLocalFile("sounds/choose.wav"));
	//c.setVolume();
	d.setSource(QUrl::fromLocalFile("sounds/d.wav"));
	d.setVolume(0.55);

	blocked.setSource(QUrl::fromLocalFile("sounds/blocked2.wav"));
	/*QFile file("Config.txt");
	file.open(QIODevice::ReadWrite);
	QString cnfg = file.readLine();
	file.close();*/
	

	cnfg = GetStringFromCnfg("currLevel");
	max_opened_level = Extract(cnfg);
	cnfg = GetStringFromCnfg("maxWords");
	max_words = Extract(cnfg);
	this->resize(1.5*scale,1.1*scale); 
	Board = new Table(scale,1);
	QString cnfg_max_l = GetStringFromCnfg("maxLevel");
	max_level = Extract(cnfg_max_l);
	//QPB = new QPushButton(this);
	//QPB->setGeometry(0.8 * scale, 0.5 * scale, scale * 0.5, scale * 0.2);
	//QPB->setText(QString("B E G I N"));
	//QPB->close();
	//QObject::connect(QPB, SIGNAL(clicked()), this, SLOT(Generate()));
	
	settings = new QPushButton("", this);
	settings->setGeometry(1.3 * scale, 0.04*scale, scale * 0.1, scale * 0.1);//
	//settings->setIcon(QIcon("Gear.jpg"));
	settings->setObjectName("Settings");
	//settings->hide();
	//settings->setStyleSheet("border-radius: 5px;");
	QObject::connect(settings, SIGNAL(clicked()), this, SLOT(OpenSettings()));
	
	Hint_one_letter = new QPushButton("", this);
	Hint_one_letter->setGeometry(0.5 * scale, 0.019 * scale, scale * 0.11, scale * 0.11);
	Hint_one_letter->setObjectName("Hint");
	Hint_one_letter->hide();
	QObject::connect(Hint_one_letter, SIGNAL(clicked()), this, SLOT(ShowOneLetter()));

	Hint_full_word = new QPushButton("Show one word", this);
	Hint_full_word->setGeometry(1.1 * scale, 1.05 * scale, scale * 0.2, scale * 0.05);
	Hint_full_word->hide();
	QObject::connect(Hint_full_word, SIGNAL(clicked()), this, SLOT(ShowWord()));
	//Generate();
	for (int i = 0; i < max_words; i++) {
		number_for_words.push_back(new QLabel(QString::number(i + 1), this));
		number_for_words[number_for_words.size() - 1]->setObjectName("number");
		number_for_words[number_for_words.size() - 1]->hide();
	}

	for (int i = 0; i < max_level; i++) {
		levels.push_back(new QPushButton(QString::number(i + 1),this));
		levels[levels.size() - 1]->setGeometry(0.1 * scale + i % 6 * 0.15 *scale + i%6*0.025*scale, 0.2 * scale + i / 6 * 0.15 * scale + i / 6 * 0.025 * scale, 0.15 * scale, 0.15 * scale);
		if (i + 1 > max_opened_level) {
			levels[levels.size() - 1]->setObjectName("blocked");
			QObject::connect(levels[levels.size() - 1], SIGNAL(clicked()), this, SLOT(Shake()));
		}
		else {
			levels[levels.size() - 1]->setObjectName("levels");
			QObject::connect(levels[levels.size() - 1], SIGNAL(clicked()), this, SLOT(Generate()));
		}
		levels[levels.size() - 1]->show();
	}
	message_window = new QWidget();
	QLabel* try_to = new QLabel("Try to assemble the word", message_window);
	try_to->setGeometry(0.05 * scale, 0.05 * scale, 0.6 * scale, 0.18 * scale);
	try_to->setObjectName("LabelInWindow");
	QLabel* way = new QLabel("in a different way", message_window);
	way->setGeometry(0.05 * scale, 0.22 * scale, 0.6 * scale, 0.18 * scale);
	way->setObjectName("LabelInWindow");
	message_window->hide();
	message_window->setObjectName("messageWindow");
	message_window->setWindowFlags(Qt::FramelessWindowHint);
	message_window->setGeometry(this->x() + 1.1 * scale, this->y() + 0.4 * scale, 0.7 * scale, 0.5 * scale);
}

void EnglishBoost::timerEvent(QTimerEvent* event)
{
	if (TimerID)											// МБ слишком много всего напихано в таймер, выполняться будет досточно долго
	{
		int x = this->mapFromGlobal(QCursor::pos()).x();
		int y = this->mapFromGlobal(QCursor::pos()).y();
		Tile** tbl = Board->tbl;
		int size_field = Board->size_field;
		int tileW = Board->tile_width;
		auto drawing_tiles = Board->drawing_tiles;
		//for (int i = 0; i < size_field; i++) {     // Этот цикл можно будет заменить формулой a = f(x,y); b = g(x, y); -> tbl[a][b];
		//	for (int j = 0; j < size_field; j++) {
		//		if (x > tbl[i][j].corn_x && x < tbl[i][j].corn_x+ tileW && y > tbl[i][j].corn_y && y < tbl[i][j].corn_y + tileW) {			//Проверяем, тыкнул ли пользователь в ячеку.
		int tailX = (x - scale * 0.5) / Board->tile_width;
		int tailY = (y - scale * 0.15) / Board->tile_width;
		if (tailX>=0 && tailX<size_field && tailY>=0 &&tailY < size_field) {
			int i = tailX;
			int j = tailY;
			int current_tile = drawing_tiles.back();
			auto it = std::find(drawing_tiles.begin(), drawing_tiles.end(), i + j * size_field);
			if (it == drawing_tiles.end()) {																							//Если ячейка не является активной (закрашенной)
				if ((i + j * Board->size_field == current_tile - 1 && i!=size_field-1)|| (i + j * size_field == current_tile + 1 && i!=0)  
					|| (i + j * size_field == current_tile - size_field && j!=size_field-1)  || (i + j * size_field == current_tile + size_field && j!=0)) {  // Проверяем, тыкнули ли мы на соседнюю ячейку, относительно последней закрашенной
					Board->drawing_tiles.push_back(i + j * size_field);																	//Если все хорошо, закрашиваем и эту ячейку
					entered_word.push_back(tbl[i][j].letter);																			//Добавляем в текущее введненное слово символ
					c.play();
				}
			}
			else {																														//Если ячейка является активной (закрашенной)
				if (drawing_tiles.back() != i + j * size_field) {																		//Если это не последняя закрашенная ячейка
					auto iter = drawing_tiles.end();
					advance(iter, -2);
					int size=0;
					for (auto it = Board->found_words.begin(); it != Board->found_words.end(); it++) {
						size += it->size();
					}
					if ((it == iter)&& (drawing_tiles.size()!=size+1)){																	//Если ячейка, на которой у нас курсор, является предпоследней закрашенной(активной),
						Board->drawing_tiles.pop_back();				//Ячейка перестаёт быть активной,
						d.play();
						entered_word.chop(1);																							//Удаляем последнюю букву из текущего слова
					}
				}
			}
		}
			//}
		//}
		repaint();
	}

	if (TimColorID)
	{
		if (R != nR || G != nG || B != nB)
		{
			const int dd = 5;
			int delta = 1;
			if (abs(R - nR) > dd) delta = dd;
			if (R < nR) R+= delta; else if (R != nR) R-= delta;
			delta = 1;
			if (abs(G - nG) > dd) delta = dd;
			if (G < nG) G+= delta; else if (G != nG) G-= delta;
			delta = 1;
			if (abs(B - nB) > dd) delta = dd;
			if (B < nB) B+= delta; else if (B != nB) B-= delta;
			update();
		}
		else
		{
			killTimer(TimColorID);
			TimColorID = 0;
			//if (inActive) //QPB->show();

		}
	}
	if (ShakeTimerId) {
		delete levels[shakingButton - 1];       
		levels.removeAt(shakingButton - 1);
		levels.insert(shakingButton - 1,new QPushButton(QString::number(shakingButton), this));
		levels[shakingButton-1]->setGeometry(0.1 * scale + (shakingButton-1) % 6 * 0.2 * scale + (shakingButton - 1) % 6 * 0.025 * scale, 0.2 * scale + (shakingButton - 1) / 6 * 0.2 * scale + (shakingButton - 1) / 6 * 0.025 * scale, 0.2 * scale, 0.2 * scale);
		if (countShakes == 0)
			levels[shakingButton - 1]->setObjectName("blockedLeft");
		else if (countShakes == 1)
			levels[shakingButton - 1]->setObjectName("blockedRight");
		else if (countShakes == 2)
			levels[shakingButton - 1]->setObjectName("blocked");
		levels[shakingButton - 1]->show();
		QObject::connect(levels[shakingButton - 1], SIGNAL(clicked()), this, SLOT(Shake()));
		if (countShakes == 2) {
			killTimer(ShakeTimerId);
			ShakeTimerId = 0;
			countShakes = 0;
		}
		else { countShakes++; }
	}
	if (TimHintId) {
		if (start_t != end_t) {
			Board->drawing_tiles.push_back(ConvertNumberOfTail(*start_t, Board->size_field));
			if (std::find(Board->hint_letters.begin(), Board->hint_letters.end(), ConvertNumberOfTail(*start_t, Board->size_field)) != Board->hint_letters.end()) {
				Board->hint_letters.clear();
				SetStringToCnfg("hintLetter - ", "hintLetter");
				Board->showed_letters = 0;
				Board->index_showed_letter = -1;
			}
			start_t++;
			c.play();
		}
		else {
			killTimer(TimHintId);
			TimHintId = 0;
			WordWasFounded();
			auto number = Board->words.find(*(--Board->found_words.end()));
			int k = 0;
			while (number != Board->words.begin()) {
				k++;
				number--;
			}
			Board->found_synonyms.push_back(k);
		}
		repaint(); 
	}
}

void EnglishBoost::mousePressEvent(QMouseEvent* event) 
{
	if (Settings_w)
	if (Settings_w->isVisible()) {
		delete Settings_w;
		return;
	}
	if (message_window->isVisible()) {
		message_window->close();
		return;
	}
	if (!TimHintId) {
		if (event->button() == Qt::LeftButton) {
			if (inActive) {
				if (!clicked) {															//Если нажали первый раз
					double x = event->pos().x();
					double y = event->pos().y();
					Tile** tbl = Board->tbl;
					int titleW = Board->tile_width;
					int size_field = Board->size_field;
					for (int i = 0; i < size_field; i++) {
						for (int j = 0; j < size_field; j++) {
							if (x > tbl[i][j].corn_x && x < tbl[i][j].corn_x + titleW && y > tbl[i][j].corn_y && y < tbl[i][j].corn_y + titleW) {
								if (std::find(Board->drawing_tiles.begin(), Board->drawing_tiles.end(), i + j * size_field) == Board->drawing_tiles.end()) {	//Ячейка не является активной(то есть пользователь нажал не на найденное слово)
									c.play();
									clicked = true;
									entered_word.push_back(tbl[i][j].letter);													//Начинаем следить за его курсором
									Board->drawing_tiles.push_back(i + j * size_field);
									TimerID = startTimer(20);
								}
							}
						}
					}
				}
				else {											//Нажали во второй раз
					killTimer(TimerID);							//Перестаем следить за курсором
					TimerID = 0;
					clicked = false;
					if (!CheckWord(entered_word)) {				//Если такое слово не было загадано
						int start = Board->drawing_tiles.size() - entered_word.size();
						auto iter = Board->drawing_tiles.begin();
						advance(iter, start);
						mistake->play();
						Board->drawing_tiles.erase(iter, Board->drawing_tiles.end());		//очищаем ячейки
					}
					else {
						auto number = Board->words.find(entered_word);
						int k = 0;
						while (number != Board->words.begin()) {
							k++;
							number--;
						}
						Board->found_synonyms.push_back(k);
						auto it = Board->drawing_tiles.end();
						advance(it, -entered_word.size());
						if (std::find(Board->hint_letters.begin(), Board->hint_letters.end(), *it) != Board->hint_letters.end()) {
							Board->hint_letters.clear();
							SetStringToCnfg("hintLetter - ", "hintLetter");
							Board->showed_letters = 0;
							Board->index_showed_letter = -1;
						}
						Board->found_words.push_back(entered_word);			//Если слово угадано верно, помещаем его в контейнер с найденными словами
						WordWasFounded();
					}
					entered_word.clear();			//Очищаем текущее слово, потому что текущий поиск слова закончен.
					repaint();
				}
			}
		}
	}
}

bool EnglishBoost::CheckWord(QString s) {
	auto words = Board->words;
	auto it = words.find(s);
	if (it == words.end()) {
		return false;
	}
	int size1 = 0;
	for (auto iter = words.begin(); iter != it; iter++) {
		size1 += iter->first.size();
	}
	int size2 = 0;
	for (auto it = Board->found_words.begin(); it != Board->found_words.end(); it++) {
		size2 += it->size();
	}
	auto it_t = Board->drawing_tiles.begin();
	advance(it_t, size2);
	auto it_w = Board->occupied_cells.begin();
	advance(it_w, size1);
	for (int i = 0; i < s.size(); i++, it_w++, it_t++) {
		if (*it_w % Board->size_field == *it_w / Board->size_field) {
			if (*it_w != *it_t) {
				message_window->show();
				return false;
			}
		}
		else {
			if (ConvertNumberOfTail(*it_w,Board->size_field) != *it_t) {
				message_window->show();
				return false;
			}
		}
	}
	if (std::find(Board->found_words.begin(), Board->found_words.end(), s) == Board->found_words.end()) {
		return true;
	}
	return false;
}

void EnglishBoost::resizeEvent(QResizeEvent* event)
{
	delete Settings_w;
	int tmp = this->width() / 1.5;
	int tmpp = this->height() / 1.1;
	if (tmpp < tmp) tmp = tmpp;
	if (SF) {
		scale = tmp;
		if (scale > 1400) scale = 1400;
		if (scale < 200) scale = 200;
		Board->NewSize(scale);
		settings->setGeometry(1.3 * scale, 0.04 * scale, scale * 0.1, scale * 0.1);
		Hint_one_letter->setGeometry(0.5 * scale, 0.019 * scale, scale * 0.11, scale * 0.11);
		Hint_full_word->setGeometry(1.1 * scale, 1.05 * scale, scale * 0.2, scale * 0.05);
		for (int i = 0; i < max_level; i++) {
			double scl = 0.2;
			levels[i]->setGeometry(0.1 * scale + i % 6 * scl * scale + i % 6 * 0.025 * scale, 0.2 * scale + i / 6 * scl * scale + i / 6 * 0.025 * scale, scl * scale, scl * scale);
		}
		message_window->setGeometry(this->x() + 1.1 * scale, this->y() + 0.4 * scale, 0.7 * scale, 0.5 * scale);
		//QPB->setGeometry(0.8 * scale, 0.5 * scale, scale * 0.5, scale * 0.2);
		SF = 0;
		this->resize(1.5 * scale, 1.1 * scale);
		SF = 1;
		update();
		repaint();
	}
	
}

void Table::NewSize(int scale)
{
	double coef = 0.1;
	tile_width = (scale * (double(1 - coef))) / size_field;

	int x_offset = scale * (0.4 + coef);
	int y_offset = scale * ((0.1 + coef) - 0.05);

	for (int i = 0; i < size_field; i++)
		for (int j = 0; j < size_field; j++)
		{
			tbl[i][j].corn_x = x_offset + i * tile_width;
			tbl[i][j].corn_y = y_offset + j * tile_width;
		}
}

void EnglishBoost::ChangeColor(int r, int g, int b, int period)
{
	nG = g;
	nB = b;
	nR = r;
	killTimer(TimColorID);
	TimColorID = startTimer(period);
}


void EnglishBoost::Generate()
{
	Hint_one_letter->show();
	ChangeColor(RAND_CLR, RAND_CLR, RAND_CLR, 20);
	scale = this->width() * 2 / 3;
	this->resize(1.5 * scale, 1.1 * scale);
	if (Next_level) {
		int c = Board->current_level;
		delete Board;
		c++;
		if (c > max_opened_level) {
			max_opened_level++;
			SetStringToCnfg(QString("currLevel - ") + QString::number(max_opened_level), "currLevel");
			if (max_opened_level > max_level) {
				max_opened_level--;
				c = 1;
			}
		}
		for (int i = 0; i < max_words; i++) {
			number_for_words[i]->hide();
		}
		Board = new Table(scale, c);
	}
	else {
		click->play();
		QPushButton* sender = qobject_cast<QPushButton*>(QObject::sender());
		delete Board;
		Board = new Table(scale, sender->text().toInt());
		for (int i = 0; i < max_level; i++) {
			levels[i]->hide();
		}
	}
	inActive = true;
	update();
}

Table::~Table()
{
	for (int i = 0; i < size_field; i++)
		delete[] tbl[i];
}

Tile::Tile(QChar l, int x, int y)
{
	letter = l;
	corn_x = x;
	corn_y = y;
	tile_color = QColor(0, 0, 0);
}

void EnglishBoost::paintEvent(QPaintEvent* event)
{
	
	QPainter QP(this);
	QPen pen;
	pen.setWidth(4);
	QP.setPen(pen);
	QP.setBrush(QBrush(QColor(0, 150, 150)));
	QFont QF(QString("Ink free"), 0.05*scale);
	QF.setBold(1);
	QP.setFont(QF);
	if (!inActive) {
		QP.save();
		pen.setColor("White");
		QFont QF(QString("Ink free"), 0.05*scale);
		QP.setFont(QF);
		QP.setPen(pen);
		QP.drawText(0.5*scale, scale * 0.04, scale * 0.5, scale * 0.1, 100, "Select level");
		QP.restore();
	}
	else {
		QP.setBrush(QBrush(QColor((R < 100) ? 0 : (R - 100), (G < 100) ? 0 : (G - 100), (B < 100) ? 0 : (B - 100))));
		QP.drawRect(-10, -10, 2 * scale, 2 * scale);
	}
		//QP.drawRect(0, 0, 0.4 * scale, 1.1 * scale);
		//QP.drawRect(1.4 * scale, 0, 0.1 * scale, 1.1 * scale);
	int size_word = (scale - scale * 0.02 - scale * 0.17) / max_words;

	if (inActive) {
		QFont QF(QString("Ink free"), 0.03 * scale);
		QF.setBold(1);
		QP.setFont(QF);
		words_w->setGeometry(scale * 0.05, 0.2 * scale, 0.4 * scale, size_word * Board->synonyms.size());
		words_w->show();
		QP.drawImage(QRect(scale * 0.05, scale * 0.02, scale * 0.33, scale * 0.15), QImage("lvl_window.png"));
		QP.drawText(scale * 0.1, scale * 0.02, scale * 0.3, scale * 0.07, 0, "Level " + QString::number(Board->current_level));
		QP.drawText(scale * 0.08, scale * 0.09, scale * 0.3, scale * 0.07, 0, "out of " + QString::number(max_level));
		QP.drawText(scale * 0.105, scale * 0.02, scale * 0.3, scale * 0.07, 0, "Level " + QString::number(Board->current_level));
		QP.drawText(scale * 0.085, scale * 0.09, scale * 0.3, scale * 0.07, 0, "out of " + QString::number(max_level));
		QP.setBrush(QBrush(QColor(R, G, B)));
		QP.drawImage(QRect(0.75 * scale, 0.03 * scale, 0.45 * scale, 0.1 * scale), QImage("text3.png"));

		Board->Draw(&QP);
		pen.setColor("White");
		QP.setPen(pen);
		int max_len = 0;
		auto it = Board->synonyms.begin();
		for (int i = 0; i < Board->synonyms.size(); i++)
		{
			if (it->length() + 3 > max_len) max_len = it->length() + 3;
			it++;
		}
		it = Board->synonyms.begin();
		QFont QF2(QString("Ink free"), scale * 0.45 / max_len);
		QF2.setBold(1);
		QP.setFont(QF2);
		for (int i = 0; i < Board->synonyms.size(); i++)
		{
			QP.save();
			if (i == Board->index_showed_letter) {			//Помечаем, что подсказки относятся к данному синониму
				pen.setColor("red");
				QP.setPen(pen);
			}
			if (std::find(Board->found_synonyms.begin(), Board->found_synonyms.end(), i) != Board->found_synonyms.end()) {
				QF2.setStrikeOut(true);
				QP.setFont(QF2);
			}
			QP.drawText(scale * 0.07, 0.2 * scale + i * size_word, 0.45 * scale, size_word, 0, QString::number(i + 1) + ". " + *it);
			QP.restore();
			it++;
		}
	}
}

Table::Table(int scale,int c)
{

	current_level = c;
	GetDataFromBD(current_level);			//�������� ������ �� ��


	for (int i = 0; i < size_field * size_field; i++) {
		peaks.push_back(new Peak(i));
	}


	for (int i = 0; i < size_field * size_field - 1; i++) {			// ��������� ���� Peaks
		if ((i + 1) % size_field == 0) {
			peaks[i]->AddCon(peaks[i + size_field]);
		}
		else {
			if (i + 1 < peaks.size()) {
				peaks[i]->AddCon(peaks[i + 1]);
				if (i + size_field < peaks.size()) {
					peaks[i]->AddCon(peaks[i + size_field]);
				}
			}
		}
	}
	GeneratePositions();								//���������� ������������ ����
	double coef = 0.1;
	tile_width = (scale*(double(1-coef))) / size_field;
	tbl = new Tile * [size_field];
	for (int i = 0; i < size_field; i++) tbl[i] = new Tile[size_field];
	tbl = new Tile * [size_field];
	for (int i = 0; i < size_field; i++) tbl[i] = new Tile[size_field];
	int x_offset = scale*(0.4+coef);
	int y_offset = scale*((0.1+coef)-0.05);

	for (int i = 0; i < size_field; i++)
		for (int j = 0; j < size_field; j++)
		{
			tbl[i][j].corn_x = x_offset + i * tile_width;
			tbl[i][j].corn_y = y_offset + j * tile_width;
		}
	auto iterator = words.begin();
	auto iter = iterator->first.begin();
	for (auto it = occupied_cells.begin(); it != occupied_cells.end();it++) {
		if (iter == iterator->first.end()) {
			iterator++;
			iter = iterator->first.begin();
		}
		tbl[*it/size_field][*it%size_field].corn_x = x_offset + *it / size_field * tile_width;
		tbl[*it / size_field][*it % size_field].corn_y = y_offset + *it % size_field * tile_width;
		tbl[*it / size_field][*it % size_field].letter = *iter;
		iter++;
	}



}

void Table::Draw(QPainter* QP)
{
	QFont QF(QString("Calibri"), tile_width*0.5);
	QF.setBold(1);
	QP->setFont(QF);
	for (int i = 0; i < size_field; i++)
		for (int j = 0; j < size_field; j++)
		{
			QP->save();  
			if (std::find(drawing_tiles.begin(), drawing_tiles.end(), i + size_field * j) != drawing_tiles.end()) {
				QP->setBrush(QBrush(QP->brush().color().lighter(140)));
			}
			QP->drawRect(tbl[i][j].corn_x, tbl[i][j].corn_y, tile_width, tile_width);
			QP->restore();
			int text_offset = 0; //Troublesome
			if (tbl[i][j].letter == 'i' || tbl[i][j].letter == 'l') text_offset = tile_width * 0.1;
			if (tbl[i][j].letter == 'w' || tbl[i][j].letter == 'm') text_offset = -tile_width * 0.1;                           //BIG LETTERS??//
			QP->save();
			if (std::find(hint_letters.begin(), hint_letters.end(), i + size_field * j) != hint_letters.end()) {
				QP->setPen(QPen(QColor("red")));
			}
			QP->drawText(tbl[i][j].corn_x + tile_width/4 + text_offset, tbl[i][j].corn_y + 3*tile_width/4,QString(tbl[i][j].letter/*.toUpper()*/));
			QP->restore();
		}
}

bool EnglishBoost::ConnectBD() {
	QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
	db.setDatabaseName("EnglishBoost.dat"); 
	db.setHostName("localhost");
	db.setUserName("root");
	db.setPassword("root");
	if (!db.open()) {
		return false;
	}
	return true;
}

void Table::GetDataFromBD(int lvl) {
	QSqlQuery query;
	query.prepare("SELECT word, synonym, word_length, size_field from words JOIN levels on levels.id = words.level_id where words.level_id = :id");
	query.bindValue(":id", lvl);
	query.exec();
	while (query.next()) {
		words[query.value(0).toString()] = query.value(2).toInt();
		auto it = words.begin();
		int p;
		for (int i = 0; i < words.size(); i++, it++) {
			if (it->first == query.value(0).toString()) {
				p = i;
			}
		}
		auto iter = synonyms.begin();
		advance(iter, p);
		synonyms.insert(iter,query.value(1).toString());
		size_field = query.value(3).toInt();
	}
}

void Table::ClearLastWay(int index, int j) {
	for (int k = 0; k < j + 1; k++) {
		auto iter = occupied_cells.begin();
		advance(iter, index + k);
		peaks[*iter]->returnDone();
	}
	for (int k = 0; k < j + 1; k++) {
		occupied_cells.pop_back();
	}
}

void EnglishBoost::WordWasFounded() {
	ChangeColor(RAND_CLR, RAND_CLR, RAND_CLR, 20);
	right->play();
	if (Board->found_words.size() == Board->words.size()) {
		win->play();
		Next_level = true;
		Generate();
		SetStringToCnfg("foundedWords - ", "foundedWords");
		return;
	}
	else {
		QString s;
		for (auto it = Board->found_words.begin(); it != Board->found_words.end(); it++) {
			if (it == Board->found_words.begin()) {
				s += *it;
			}
			else {
				s += "," + *it;
			}
		}
		SetStringToCnfg("foundedWords - " + s, "foundedWords");
	}
	auto it = Board->words.begin();
	int number = 0;
	auto res = Board->found_words.end();
	res--;
	for (int i = 0; i < Board->words.size(); i++, it++) {
		if (it->first == *res) {
			number = i;
			break;
		}
	}
	auto cur_tail = Board->drawing_tiles.begin();
	advance(cur_tail, Board->drawing_tiles.size() - res->size());
	int tail = *cur_tail;
	number_for_words[number]->setGeometry(0.46 * scale + Board->tile_width * (tail % Board->size_field) + Board->tile_width, 0.13 * scale + Board->tile_width * (tail / Board->size_field),0.1*scale,0.1*scale);
	number_for_words[number]->show();
}

void EnglishBoost::RestoreWords() {
	QString string = GetStringFromCnfg("foundedWords");
	int k = 0;
	QChar qc = ' ';
	while (qc != '-') {
		qc = string[k];
		k++;
	}
	string.remove(0, k+1);
	string.chop(1);
	if (string.contains('\r')) {
		string.remove('\r');
	}
	QString ready_str;
	for (int i = 0; i < string.size(); i++) {
		if (string[i] != ',' && i!=string.size()-1) {
			ready_str.push_back(string[i]);
		}
		else {

			if (i== string.size() - 1) {
				ready_str.push_back(string[i]);
			}
			Board->found_words.push_back(ready_str);
			auto number = Board->words.find(*(--Board->found_words.end()));
			int k = 0;
			while (number != Board->words.begin()) {
				k++;
				number--;
			}
			Board->found_synonyms.push_back(k);
			auto end = Board->words.find(ready_str);
			int pos = 0;
			for (auto it = Board->words.begin(); it != end; it++) {
				pos += it->second;
			}
			auto start = Board->occupied_cells.begin();
			advance(start, pos);
			for (int i = 0; i < end->second; i++,start++) {
				Board->drawing_tiles.push_back(ConvertNumberOfTail(*start,Board->size_field));
			}
			ready_str.clear();
		}
	}
	repaint();
}

void EnglishBoost::RestotrHints() {
	QString string = GetStringFromCnfg("hintLetter");
	int k = 0;
	QChar qc = ' ';
	while (qc != '-') {
		qc = string[k];
		k++;
	}
	string.remove(0, k + 1);
	string.chop(1);
	if (string.contains('\r')) {
		string.remove('\r');
	}
	QString ready_str;
	int count_data = 0;
	for (int i = 0; i < string.size(); i++) {
		if (string[i] != ',' && i != string.size() - 1) {
			ready_str.push_back(string[i]);
		}
		else {
			if (i == string.size() - 1) {
				ready_str.push_back(string[i]);
			}
			if (!count_data) {
				Board->index_showed_letter = ready_str.toInt();
			}
			else {
				auto iter = Board->occupied_cells.begin();
				advance(iter, ready_str.toInt());
				Board->hint_letters.push_back(ConvertNumberOfTail(*iter, Board->size_field));
				Board->showed_letters++;
			}
			ready_str.clear();
			count_data++;
		}
	}
}

void EnglishBoost::ShowOneLetter() {
	if (hint->state() != QMediaPlayer::PlayingState) {
		hint->play();
		map<QString, int>::iterator iter;
		int word_index;
		if (Board->showed_letters) {
			word_index = Board->index_showed_letter;
		}
		else {
			do {
				iter = Board->words.begin();
				word_index = rand() % (Board->words.size());
				advance(iter, word_index);
			} while (std::find(Board->found_words.begin(), Board->found_words.end(), iter->first) != Board->found_words.end());
			Board->index_showed_letter = word_index;

		}
		auto end = Board->words.begin();
		advance(end, Board->index_showed_letter);
		int pos = 0;
		for (auto it = Board->words.begin(); it != end; it++) {
			pos += it->second;
		}
		pos += Board->showed_letters;
		auto tail = Board->occupied_cells.begin();
		advance(tail, pos);
		Board->hint_letters.push_back(ConvertNumberOfTail(*tail, Board->size_field));
		QString s2 = QString::number(word_index) + ",";
		for (auto it = Board->hint_letters.begin(); it != Board->hint_letters.end(); it++) {
			int n = ConvertNumberOfTail(*it,Board->size_field);
			auto iter = std::find(Board->occupied_cells.begin(), Board->occupied_cells.end(), n);
			int k = 0;
			auto a = Board->occupied_cells.begin();
			while (a != iter) {
				k++;
				a++;
			}
			s2 += QString::number(k);
			auto g = Board->hint_letters.end();
			advance(g, -1);
			if (it != g) {
				s2 += ",";
			}
		}
		SetStringToCnfg("hintLetter - " + s2, "hintLetter");
		Board->showed_letters++;
		iter = Board->words.begin();
		advance(iter, Board->index_showed_letter);
		if (Board->showed_letters == iter->second) {
			Board->showed_letters = 0;
			Board->index_showed_letter = -1;
			Board->found_words.push_back(iter->first);
			for (auto it = Board->hint_letters.begin(); it != Board->hint_letters.end(); it++) {
				Board->drawing_tiles.push_back(*it);
			}
			auto number = Board->words.find(iter->first);
			int k = 0;
			while (number != Board->words.begin()) {
				k++;
				number--;
			}
			Board->found_synonyms.push_back(k);
			SetStringToCnfg("hintLetter - ", "hintLetter");
			Board->hint_letters.clear();
			WordWasFounded();
		}
		repaint();
	}
}

void EnglishBoost::ShowWord() {
		map<QString, int>::iterator iter;
		int word_index;
		do {
			iter = Board->words.begin();
			word_index = rand() % (Board->words.size());
			advance(iter, word_index);
		} while (std::find(Board->found_words.begin(), Board->found_words.end(), iter->first) != Board->found_words.end());
		int pos = 0;
		auto end = Board->words.begin();
		advance(end, word_index);
		for (auto it = Board->words.begin(); it != end; it++) {
			pos += it->second;
		}
		auto start = Board->occupied_cells.begin();
		advance(start, pos);
		start_t = start;
		auto c_start = start;
		advance(c_start, end->second);
		end_t = c_start;
		//auto c_start = start;
		//killTimer(TimHintId);
		killTimer(TimColorID);
		killTimer(TimerID);
		TimerID = 0;
		TimColorID = 0;
		clicked = false;
		int start3 = Board->drawing_tiles.size() - entered_word.size();
		if (start3 != Board->drawing_tiles.size()) {
			auto itert = Board->drawing_tiles.begin();
			advance(itert, start3);
			Board->drawing_tiles.erase(itert, Board->drawing_tiles.end());		//очищаем ячейки
		}
		entered_word.clear();
		TimHintId = startTimer(200);
		Board->found_words.push_back(end->first);
		/*for (int i = 0; i < end->first.size(); i++, c_start++) {
			Board->drawing_tiles.push_back(ConvertNumberOfTail(*c_start, Board->size_field));
		}
		if (std::find(Board->hint_letters.begin(), Board->hint_letters.end(), ConvertNumberOfTail(*start,Board->size_field)) != Board->hint_letters.end()) {
			Board->hint_letters.clear();
			Board->showed_letters = 0;
			Board->index_showed_letter = -1;
		}
		QSound::play("sounds/right2.wav");
		ChangeColor(RAND_CLR, RAND_CLR, RAND_CLR, 170);
		Board->found_words.push_back(end->first);
		if (Board->found_words.size() == Board->words.size()) {
			QSound::play("sounds/win.wav");
			Generate();
		}
		repaint();*/
}

SettingsWindow::SettingsWindow(EnglishBoost* p) {

	parent = p;
	QString cfg = GetStringFromCnfg("scale");
	scale = parent->scale;
	QFont QF(QString("Ink free"), 0.02 * scale);
	this->setFont(QF);
	close = new QPushButton(this);
	close->setGeometry(0.62 * scale, 0.01 * scale, scale * 0.07, scale * 0.07);
	close->setObjectName("close");

	quit = new QPushButton("Quit",this);
	quit->setGeometry(0.05 * scale, 0.3 * scale, 0.29 * scale, 0.15 * scale);
	quit->setObjectName("ButtonSettings");

	QObject::connect(quit, SIGNAL(clicked()), this, SLOT(closeParent()));

	r = new QPushButton("Back to menu", this);
	r->setGeometry(0.36 * scale, 0.3 * scale, 0.29 * scale, 0.15 * scale);
	r->setObjectName("ButtonSettings");
	QObject::connect(r, SIGNAL(clicked()), this, SLOT(OpenMenu()));

	field = new QWidget(this);
	field->setGeometry(0.05 * scale, 0.13 * scale, 0.6 * scale, 0.15 * scale);
	field->setObjectName("words");

	QObject::connect(close, SIGNAL(clicked()), this, SLOT(closeMe()));


	volume = new QSlider(Qt::Horizontal,this);
	volume->setValue(parent->volume);
	volume->setGeometry(0.2*scale,0.155*scale,0.40*scale,0.1*scale);

	QObject::connect(volume, SIGNAL(sliderReleased()), this, SLOT(checkSound()));
	repaint();
	
}

SettingsWindow::~SettingsWindow()
{
	parent->Settings_w = nullptr;
}

void SettingsWindow::checkSound() {
	double volume_local = QAudio::convertVolume(volume->value()/100., QAudio::LogarithmicVolumeScale,
		QAudio::LinearVolumeScale);
	parent->volume = volume_local;
	parent->right->setVolume(volume_local*100);
	parent->click->setVolume(volume_local*100);
	parent->mistake->setVolume(volume_local*100);
	parent->hint->setVolume(volume_local*25);
	parent->c.setVolume(volume_local);
	parent->d.setVolume(volume_local * 0.55);
	parent->win->setVolume(volume_local*35);
	parent->blocked.setVolume(volume_local);
	if (!parent->c.isPlaying())
	parent->c.play();
}

void SettingsWindow::closeParent() {
	parent->close();
	delete this;
}

void SettingsWindow::closeMe() {
	delete this;
}

void SettingsWindow::timerEvent(QTimerEvent* event) {


}
void SettingsWindow::OpenMenu() {
	QFont QF(QString("Ink free"),  0.05* scale);
	this->setFont(QF);
	scale = parent->scale;
	parent->click->play();
	parent->inActive = false;
	parent->update();
	parent->Hint_one_letter->hide();
	for (int i = 0; i < parent->max_level; i++) {
		delete[] parent->levels[i];
	}
	parent->levels.clear();
	for (int i = 0; i < parent->max_level; i++) {
		QPushButton* QPB = new QPushButton(QString::number(i + 1), parent);
		parent->levels.push_back(QPB);
		parent->levels[parent->levels.size() - 1]->setGeometry(0.1 * scale + i % 6 * 0.2 * scale + i % 6 * 0.025 * scale, 0.2 * scale + i / 6 * 0.2 * scale + i / 6 * 0.025 * scale, 0.2 * scale, 0.2 * scale);
		if (i + 1 > parent->max_opened_level) {
			parent->levels[parent->levels.size() - 1]->setObjectName("blocked");
			QObject::connect(parent->levels[parent->levels.size() - 1], SIGNAL(clicked()), parent, SLOT(Shake()));
		}
		else {
			parent->levels[parent->levels.size() - 1]->setObjectName("levels");
			QObject::connect(parent->levels[parent->levels.size() - 1], SIGNAL(clicked()), parent, SLOT(Generate()));
		}
		parent->levels[parent->levels.size() - 1]->show();
	}
	for (int i = 0; i < parent->max_words; i++) {
		parent->number_for_words[i]->hide();
	}
	parent->words_w->hide();
	parent->Next_level=false;
	delete this;

}

void EnglishBoost::Shake() {
	blocked.play();
	ShakeTimerId = startTimer(100);
	QPushButton* sender = qobject_cast<QPushButton*>(QObject::sender());
	shakingButton = sender->text().toInt();
}

void SettingsWindow::paintEvent(QPaintEvent* event) {
	close->setGeometry(0.62 * scale, 0.01 * scale, scale * 0.07, scale * 0.07);
	quit->setGeometry(0.05 * scale, 0.3 * scale, 0.29 * scale, 0.15 * scale);
	r->setGeometry(0.36 * scale, 0.3 * scale, 0.29 * scale, 0.15 * scale);
	field->setGeometry(0.05 * scale, 0.13 * scale, 0.6 * scale, 0.15 * scale);
	volume->setGeometry(0.2 * scale, 0.155 * scale, 0.40 * scale, 0.1 * scale);
	QPainter c(this);
	QPen pen;
	pen.setWidth(4);
	c.setPen(pen);
	QFont QF(QString("Ink free"), scale*0.05);
	QF.setBold(1);
	c.setFont(QF);
	c.drawImage(QRect(0.1*scale, 0.18 * scale, 0.05 * scale, 0.05 * scale), QImage("volume.png"));
	c.drawText(QRect(0.21 * scale, 0.01* scale, 0.4 * scale, 0.15 * scale), 0,"Settings");
}


void EnglishBoost::OpenSettings(){
	click->play();
	if (Settings_w == nullptr) Settings_w = new SettingsWindow(this);
	Settings_w->setWindowFlags(Qt::FramelessWindowHint);
	Settings_w->setGeometry(this->x() + 0.5 * scale, this->y() + 0.2 * scale, 0.7 * scale, 0.5*scale);
	Settings_w->setObjectName("SettingsWindow");
	Settings_w->show();
	killTimer(TimerID);							//Перестаем следить за курсором
	TimerID = 0;
	clicked = false;
	int start = Board->drawing_tiles.size() - entered_word.size();
	auto iter = Board->drawing_tiles.begin();
	advance(iter, start);
	Board->drawing_tiles.erase(iter, Board->drawing_tiles.end());		//очищаем ячейки
	entered_word.clear();			//Очищаем текущее слово, потому что текущий поиск слова закончен.
	repaint();
}

void Table::GeneratePositions() {
	std::vector<std::vector<int>> used_peaks;
	used_peaks.resize(words.size());
	bool Break = false;
	bool decr_it = false;
	int i = -1;
	for (auto it = words.begin(); it != words.end(); it++) {
		i++;
		if (decr_it) {
			it--;
			decr_it = false;
		}
		bool yes = true;
		Peak** start;
		for (int j = 0; j < peaks.size(); j++) {
			if ((peaks[j]->GetConsCount() == 1) && (!peaks[j]->GetDone())) {
				start = &peaks[j];
				occupied_cells.push_back(j);
				yes = false;
				break;
			}
		}
		if (yes) {
			int position = rand() % (size_field * size_field);
			if (used_peaks[i].size() >= size_field * size_field - occupied_cells.size()) {
				int index = 0;
				for (int s = 0; s < i - 1; s++) {
					auto iter = words.begin();
					advance(iter, s);
					index += iter->second;
				}
				ClearLastWay(index, (--it)->second - 1);
				used_peaks[i].clear();
				for (int j = i; j < used_peaks.size(); j++) {
					used_peaks[j].clear();
				}
				i--;
				while (ArePeaksWithOneConnection(peaks)) {
					index = 0;
					for (int s = 0; s < i - 1; s++) {
						auto iter = words.begin();
						advance(iter, s);
						index += iter->second;
					}
					ClearLastWay(index, (--it)->second - 1);
					for (int j = i; j < used_peaks.size(); j++) {
						used_peaks[j].clear();
					}
					i--;
				}
				it--;
				i--;
				continue;
			}
			while (std::find(occupied_cells.begin(), occupied_cells.end(), position) != occupied_cells.end() || std::find(used_peaks[i].begin(), used_peaks[i].end(), position) != used_peaks[i].end()) {
				position = rand() % (size_field * size_field);
			}
			occupied_cells.push_back(position);
			start = &peaks[position];
		}
		if (std::find(used_peaks[i].begin(), used_peaks[i].end(), (*start)->GetNumber()) == used_peaks[i].end()) {
			used_peaks[i].push_back((*start)->GetNumber());
		}
		for (int j = 0; j < it->second - 1; j++) {
			(*start)->Done();
			Peak** next_step = &(*start)->GetCons()[0];
			for (int k = 1; k < (*start)->GetConsCount(); k++) {
				if ((*start)->GetCons()[k]->GetConsCount() < (*next_step)->GetConsCount()) {
					next_step = &(*start)->GetCons()[k];
				}
			}
			(*next_step)->Done();
			std::vector<Peak*> help = FormGraph(peaks);
			if (help.size() != 0) {
				if (IsGraphConnectivity(0, help) != help.size()) {  //0->���������, �� �� ���������*
					for (int i = 0; i < peaks.size(); i++) {
						if (peaks[i]->GetNumber() >= 1000) {
							peaks[i]->SetNumberAway();
						}
					}
					int index = 0;
					for (int s = 0; s < i; s++) {
						auto iter = words.begin();
						advance(iter, s);
						index += iter->second;
					}
					(*next_step)->returnDone();
					ClearLastWay(index, j);
					while (ArePeaksWithOneConnection(peaks)) {
						index = 0;
						for (int s = 0; s < i - 1; s++) {
							auto iter = words.begin();
							advance(iter, s);
							index += iter->second;
						}
						ClearLastWay(index, (--it)->second - 1);
						i--;
					}
					if (it != words.begin()) {
						it--;
					}
					else {
						decr_it = true;
					}
					i--;
					Break = true;
					break;
				}
				for (int i = 0; i < peaks.size(); i++) {
					if (peaks[i]->GetNumber() >= 1000) {
						peaks[i]->SetNumberAway();
					}
				}
			}
			occupied_cells.push_back((*next_step)->GetNumber());
			//lines_p.push_back(QPoint(151 + 100 * ((*start)->GetNumber() % size), 65 + 100 * ((*start)->GetNumber() / size)));
			//positions.push_back((*start)->GetNumber());

			//lines_p.push_back(QPoint(151 + 100 * ((*next_step)->GetNumber() % size), 65 + 100 * ((*next_step)->GetNumber() / size)));
			start = next_step;
		}
		if (Break) {
			Break = false;
		}
	}
}