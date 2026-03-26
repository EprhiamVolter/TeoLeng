#ifndef lit_regex_cpp
#define lit_regex_cpp

#include "regex/lit_regex.h"

using namespace std;

lit_regex::lit_regex(int imput, int literal) :
	regex(imput), literal(literal) {
	finite = true;
	limit = 1;
}

vector<int>* lit_regex::generate(int seed) {
	if (seed == 0) {
		return new vector<int>{literal};
	}
	return nullptr;
}

eNFA* lit_regex::to_eNFA() const {
	int_set* finals = new int_set(2,{1});
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>(2,vector<int_set*>{});
	int imput = get_imput();
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < imput; j++) {
			transition->at(i).push_back(new int_set(2));
		}
	}
	transition->at(0).at(literal)->insert(1);
	vector<int_set*>* e_links = new vector<int_set*>{new int_set(2), new int_set(2)};
	return new eNFA(imput,2,0,finals,transition,e_links);
}

string lit_regex::to_string() const {
	return std::to_string(literal);
}

#endif