#pragma once
#include <vector>


class Peak {							//������� �����
	std::vector<Peak*> connects;
	int number;
	bool done = false;

public:
	Peak(int x);
	Peak();
	void AddCon(Peak* p);			//����� � ������� ���������
	int GetConsCount();				
	std::vector<Peak*>& GetCons();	
	bool operator==(Peak p);
	int GetNumber();
	void SetNumber();
	void SetNumberAway();
	void Done();					//������� ���� ������������, ������� ��� �����
	void returnDone();				// ���������� ��� ����� ����� ������ Done, ���� ����������
	bool GetDone();
};