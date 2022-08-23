#pragma once
#include "Peak.h"
#include <QString>
#include <QFile>
#include <list>


std::vector<Peak*> FormGraph(std::vector<Peak*>s);
int IsGraphConnectivity(int n, std::vector<Peak*>s);
bool ArePeaksWithOneConnection(std::vector<Peak*>v);
QString GetStringFromCnfg(QString s);
void SetStringToCnfg(QString data,QString index_data);
int ConvertNumberOfTail(int x, int size_field);