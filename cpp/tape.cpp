#ifndef tape_cpp
#define tape_cpp

#include "tape.h"

using namespace std;

tape::tape(vector<int>* word, int blank, int reserve_right, int reserve_left) : 
	head(0), blank(blank), positives(word), negatives(new vector<int>()) {
	if (word->empty()) {
		positives->push_back(blank);
	}
	positives->reserve(reserve_right);
	negatives->reserve(reserve_left);
}

void tape::move(bool right) {
	if (right) {
		head++;
		if (head == positives->size()) {
			positives->push_back(blank);
		}
	} else {
		if (head == -negatives->size()) {
			negatives->push_back(blank);
		}
		head--;
	}
}

int tape::read() {
	return (head >= 0) ? positives->at(head) : negatives->at(-(head+1));
}

void tape::write(int x) {
	((head >= 0) ? positives->at(head) : negatives->at(-(head+1))) = x;
}

#endif