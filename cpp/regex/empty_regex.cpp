#ifndef empty_regex_cpp
#define empty_regex_cpp

#include "regex/empty_regex.h"

empty_regex::empty_regex(int imput) :
	regex(imput) {
	finite = true;
	limit = 0;
}

vector<int>* empty_regex::generate(int seed) {
	return nullptr;
}

eNFA* empty_regex::to_eNFA() const {
	int_set* finals = new int_set(1);
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>{{}};
	int imput = get_imput();
	for (int i = 0; i < imput; i++) {
		transition->at(0).push_back(new int_set(1));
	}
	vector<int_set*>* e_links = new vector<int_set*>{new int_set(1)};
	return new eNFA(imput,1,0,finals,transition,e_links);
}

string empty_regex::to_string() const {
	return "∅";
}

#endif