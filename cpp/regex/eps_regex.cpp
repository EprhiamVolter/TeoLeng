#ifndef eps_regex_cpp
#define eps_regex_cpp

#include "regex/eps_regex.h"

eps_regex::eps_regex(int imput) :
	regex(imput) {
	finite = true;
	limit = 1;
}

vector<int>* eps_regex::generate(int seed) {
	if (seed == 0) {
		return new vector<int>();
	}
	return nullptr;
}

eNFA* eps_regex::to_eNFA() const {
	int_set* finals = new int_set(1, true);
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>{{}};
	int imput = get_imput();
	for (int i = 0; i < imput; i++) {
		transition->at(0).push_back(new int_set(1));
	}
	vector<int_set*>* e_links = new vector<int_set*>{new int_set(1)};
	return new eNFA(imput,1,0,finals,transition,e_links);
}

string eps_regex::to_string() const {
	return "e";
}

#endif