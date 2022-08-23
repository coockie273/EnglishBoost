#pragma once
#include <vector>


class Peak {							//Вершина графа
	std::vector<Peak*> connects;
	int number;
	bool done = false;

public:
	Peak(int x);
	Peak();
	void AddCon(Peak* p);			//Связь с другими вершинами
	int GetConsCount();				
	std::vector<Peak*>& GetCons();	
	bool operator==(Peak p);
	int GetNumber();
	void SetNumber();
	void SetNumberAway();
	void Done();					//Вершина была использована, очищаем все связи
	void returnDone();				// Возвращаем все связи после метода Done, если необходимо
	bool GetDone();
};