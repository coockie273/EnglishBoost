#pragma once
#include "OtherFunc.h"

using namespace std;

std::vector<Peak*> FormGraph(std::vector<Peak*>s) {
    std::vector<Peak*> graph;
    for (int i = 0; i < s.size(); i++) {
        if (!s[i]->GetDone()) {
            graph.push_back(s[i]);
        }
    }
    return graph;
}

int IsGraphConnectivity(int n, std::vector<Peak*>s) {

    int visitedVertices = 1;
    s[n]->SetNumber();                         
    for (int i = 0; i < s[n]->GetConsCount(); i++) {                             
        if (s[n]->GetCons()[i]->GetNumber() < 1000) {                    
            auto it = std::find(s.begin(), s.end(), s[n]->GetCons()[i]);
            int i = it - s.begin();
            visitedVertices += IsGraphConnectivity(i, s);
        }
    }
    return visitedVertices;
}

bool ArePeaksWithOneConnection(std::vector<Peak*>v) {
    for (auto it = v.begin(); it != v.end(); it++) {
        if (((*it)->GetConsCount() == 1) && (!(*it)->GetDone())) {
            return true;
        }
    }
    return false;
}

QString GetStringFromCnfg(QString s) {
    QFile file("Config.txt");
    file.open(QIODevice::ReadWrite);
    QString cnfg;
    QString newStr = " ";
    while (newStr != nullptr) {
        cnfg = file.readLine();
        newStr = cnfg.left(cnfg.indexOf(' '));
        if (newStr == s) {
            break;
        }
    }
    file.close();
    return cnfg;
}

void SetStringToCnfg(QString data, QString index_data) {
    QFile file("Config.txt");
    file.open(QIODevice::ReadWrite);
    list<QString> data_f;
    QString s = file.readLine();
    int number = 0;
    int i = 0;
    while (s != nullptr) {
        data_f.push_back(s);
        if (s.contains(index_data)) number = i;
        s = file.readLine();
        i++;
    }
    file.remove();
    QFile file2("Config.txt");
    file2.open(QIODevice::ReadWrite);
    i = 0;
    for (auto it = data_f.begin(); it != data_f.end(); it++,i++) {
        if (i == number) {
            file2.write((data + '\n').toLocal8Bit());
        }
        else {
            file2.write(it->toLocal8Bit());
        }
    }
    file.close();
}


int ConvertNumberOfTail(int x, int size_field) {
    int div = x / size_field;
    int mod = x % size_field;
    return mod * size_field + div;
}
