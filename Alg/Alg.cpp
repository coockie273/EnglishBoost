#include "Alg.h"
#include <set>
#include <QPainter>
#include <QTimerEvent>



//int CountPeaksWith1Connect(std::vector<Peak*> s) {
//    int c = 0;
//    for (int i = 0; i < s.size(); i++) {
//        if (s[i]->GetConsCount() == 1) {
//            c++;
//        }
//    }
//    return c;
//}

bool ArePeaksWithOneConnection(std::vector<Peak*>v) {
    for (auto it = v.begin(); it != v.end(); it++) {
        if  (((*it)->GetConsCount() == 1)&&(!(*it)->GetDone())) {
            return true;
        }
    }
    return false;
}

void Alg::ClearLastWay(int index, int j) {
    for (int k = 0; k < j + 1; k++) {
        peaks[occupied_cells[index + k]]->returnDone();
    }
    for (int k = 0; k < j + 1; k++) {
        occupied_cells.pop_back();
        if (k != j) {
            lines_p.pop_back();
            lines_p.pop_back();
        }
    }
}

std::vector<Peak*> FormGraph(std::vector<Peak*>s) {
    std::vector<Peak*> graph;
    for (int i = 0; i < s.size(); i++) {
        if (!s[i]->GetDone()) {
            graph.push_back(s[i]);
        }
    }
    return graph;
}

int IsGraphConnectivity(int n,std::vector<Peak*>s) {

    int visitedVertices = 1;
    s[n]->SetNumber();                         // помечаем вершину как пройденную
    for (int i = 0; i < s[n]->GetConsCount(); i ++ ) {                             // проходим по смежным с u вершинам
        if (s[n]->GetCons()[i]->GetNumber() < 1000) {                    // проверяем, не находились ли мы ранее в выбранной вершине
            auto it = std::find(s.begin(), s.end(), s[n]->GetCons()[i]);
            int i = it - s.begin();
            visitedVertices += IsGraphConnectivity(i, s);
        }
    }
    return visitedVertices;
}


Alg::Alg(QWidget* parent)
    : QMainWindow(parent)
{
    QSqlQuery query;
    query.prepare("SELECT word, synonym, word_length, size_field from words JOIN levels on levels.id = words.level_id where words.level_id = :id");
    query.bindValue(":id", level);
    query.exec();
    QString s = query.lastError().text();
    query.next();
    size = query.value(3).toInt();
    do {
        size_words.push_back(query.value(2).toInt());
        words.push_back(query.value(0).toString());
        size = query.value(3).toInt();
    } while (query.next());
    for (int i = 0; i < size * size; i++) {
        peaks.push_back(new Peak(i));
    }

    for (int i = 0; i < size * size - 1; i++) {
        if ((i + 1) % size == 0) {
            peaks[i]->AddCon(peaks[i + size]);
        }
        else {
            if (i + 1 < peaks.size()) {
                peaks[i]->AddCon(peaks[i + 1]);
                if (i + size < peaks.size()) {
                    peaks[i]->AddCon(peaks[i + size]);
                }
            }
        }
    }
}

void Alg::paintEvent(QPaintEvent* e) {
    QPainter paint(this);
    QPen pen;
    int spacing = 0;
    pen.setWidth(1);
    paint.setPen(pen);
    for (int i = 0; i < size; i++) {
        paint.drawRect(101 + spacing, 15, 100, 100 * (size)); // | | |
        spacing += 100;
    }
    for (int i = 0; i < size ; i++) {
        paint.drawRect(101, 15 + i * 100, spacing, 100); /// ----
        pen.setWidth(1);
        paint.setPen(pen);
    }
    if (lines_p.size() != 0) {
        for (int i = 0; i < lines_p.size() - 1; i += 2) {
            paint.drawLine(lines_p[i], lines_p[i + 1]);
        }
    }
}

void Alg::TryAlg() {
    std::vector<std::vector<int>> used_peaks;
    used_peaks.resize(size_words.size());
    int stop_index = 0;
    int continue_index = 0;
    bool Break = false;
    for (int i = 0; i < size_words.size(); i++) {
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
            int position = rand() % (size * size);
            if (used_peaks[i].size() >= size * size - occupied_cells.size()) {
                int index = 0;
                for (int s = 0; s < i - 1; s++) {
                    index += size_words[s];
                }
                ClearLastWay(index, size_words[i - 1] - 1);
                used_peaks[i].clear();
                i--;
                while (ArePeaksWithOneConnection(peaks)) {
                    index = 0;
                    for (int s = 0; s < i - 1; s++) {
                        index += size_words[s];
                    }
                    ClearLastWay(index, size_words[i - 1] - 1);
                    used_peaks[i].clear();
                    i--;
                }
                i --;
                continue;
            }
            while (std::find(occupied_cells.begin(), occupied_cells.end(), position) != occupied_cells.end() || std::find(used_peaks[i].begin(), used_peaks[i].end(), position) != used_peaks[i].end()) {
                position = rand() % (size * size);
            }
            occupied_cells.push_back(position);
            start = &peaks[position];
        }
        if (std::find(used_peaks[i].begin(), used_peaks[i].end(), (*start)->GetNumber()) == used_peaks[i].end()) {
            used_peaks[i].push_back((*start)->GetNumber());
        }
        for (int j = 0; j < size_words[i]-1; j++) {
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
                 if (IsGraphConnectivity(0, help) != help.size()) {  //0->последний, мб не правильно*
                     continue_index = i;
                     for (int i = 0; i < peaks.size(); i++) {
                         if (peaks[i]->GetNumber() >= 1000) {
                             peaks[i]->SetNumberAway();
                         }
                     }
                     int index = 0;
                     for (int s = 0; s < i; s++) {
                         index += size_words[s];
                     }
                     (*next_step)->returnDone();
                     ClearLastWay(index, j);
                     while (ArePeaksWithOneConnection(peaks)) {
                         index = 0;
                         for (int s = 0; s < i-1; s++) {
                             index += size_words[s];
                         }
                         ClearLastWay(index, size_words[i - 1] - 1);
                         i--;
                     }
                     stop_index = i;
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
                 lines_p.push_back(QPoint(151 + 100 * ((*start)->GetNumber() % size), 65 + 100 * ((*start)->GetNumber() / size)));
                 lines_p.push_back(QPoint(151 + 100 * ((*next_step)->GetNumber() % size), 65 + 100 * ((*next_step)->GetNumber() / size)));
                 start = next_step;
        }
        if (Break) {
            Break = false;
        }
    }
    repaint();
}
//AMOGUS