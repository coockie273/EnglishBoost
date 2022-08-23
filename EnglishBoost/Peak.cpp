#include "Peak.h"


Peak::Peak(int x) {
	number = x;
}
Peak::Peak() {
	number = -1;
}
void Peak::AddCon(Peak* p) {
	connects.push_back(p);
	p->GetCons().push_back(this);
}
int Peak::GetConsCount() {
	return connects.size();
}
std::vector<Peak*>& Peak::GetCons() {
	return connects;
}
bool Peak::operator==(Peak p) {
	return number == p.GetNumber();
}
int Peak::GetNumber() {
	return number;
}
void Peak::SetNumber() {
	number += 1000;
}
void Peak::SetNumberAway() {
	number -= 1000;
}
void Peak::Done() {
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
void Peak::returnDone() {
	done = false;
	for (int i = 0; i < connects.size(); i++) {
		if (std::find((connects)[i]->GetCons().begin(), (connects)[i]->GetCons().end(), this) == (connects)[i]->GetCons().end()) {
			(connects)[i]->GetCons().push_back(this);
		}
	}
}
bool Peak::GetDone() {
	return done;
}