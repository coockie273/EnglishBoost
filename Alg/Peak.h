#pragma once
#include <vector>


class Peak {
	std::vector<Peak*> connects;
	int number;
	bool done = false;

public:
	Peak(int x) {
		number = x;
	}
	Peak() {
		number = -1;
	}
	void AddCon(Peak* p) {
		connects.push_back(p);
		p->GetCons().push_back(this);
	}
	int GetConsCount() {
		return connects.size();
	}
	std::vector<Peak*>& GetCons() {
		return connects;
	}
	bool operator==(Peak p) {
		return number == p.GetNumber();
	}
	int GetNumber() {
		return number;
	}
	void SetNumber() {
		number +=1000;
	}
	void SetNumberAway() {
		number -= 1000;
	}
	void Done() {
		done = true;
		for (int i = 0; i < connects.size(); i++) {
			auto it = this->connects[i]->GetCons().begin();
			for (; it != connects[i]->GetCons().end(); it++) {
				if (**it == *this) {
					it = connects[i]->GetCons().erase(it);
					if (it == connects[i]->GetCons().end()) {
						break;
					}
				}
			}
		}
	}
	void returnDone() {
		done = false;
		for (int i = 0; i < connects.size(); i++) {
			if (std::find((connects)[i]->GetCons().begin(), (connects)[i]->GetCons().end(), this) == (connects)[i]->GetCons().end()) {
				(connects)[i]->GetCons().push_back(this);
			}
		}
	}
	bool GetDone() {
		return done;
	}
};
