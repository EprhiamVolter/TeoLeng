#ifndef UG_cpp
#define UG_cpp

#include <list>
#include "UG.h"

using namespace std;

UG::UG(int imput, int vars, int initial, vector<tuple<vector<int>,vector<int>>>* rules) :
	grammar(imput), var_count(vars), initial_var(initial), rules(rules) {
}

UG UG::safe_UG(int imput, int vars, int initial, vector<tuple<vector<int>,vector<int>>>* rules) {
	if (imput < 0) throw 1;
	if (vars < 1) throw 2;
	if (initial < imput) throw 3;
	int ve = imput + vars;
	if (initial >= ve) throw 4;
	if (rules == nullptr) throw 5;
	for (tuple<vector<int>,vector<int>> tup : *rules) {
		if (get<0>(tup).size() == 0) throw 6;
		for (int sym : get<0>(tup)) {
			if (sym < 0) throw 7;
			if (sym >= ve) throw 8;
		}
		for (int sym : get<0>(tup)) {
			if (sym < 0) throw 9;
			if (sym >= ve) throw 10;
		}
	}
	return UG(imput, vars, initial, rules);
}

#include <iostream>

void pvec(vector<int>* p) {
	if (p != nullptr) {
		for (int i : *p) {
			cout << '|' << i;
		}
		cout << '|';
	}
	cout << endl;
}



// surjective function over accepted words, many cases of nullprt
vector<int>* UG::generate(int seed) {
	int rules_size = rules->size();
	vector<int>*  word = new vector<int>{initial_var};
	int step = 0;
	while (seed > 0) {
		vector<tuple<int,int>> options;
		for (int i = 0; i < rules_size; i++) {
			vector<int> rule = get<0>(rules->at(i));
			int j_end = word->size() - rule.size() + 1;
			for (int j = 0; j < j_end; j++) {
				int k_end = j + rule.size();
				int k = j;
				int l = 0;
				while ((k < k_end) && (word->at(k) == rule.at(l))) {
					k++;
					l++;
				}
				if (k == k_end) {
					options.push_back({i,j});
				}
			}
		}
		vector<int>::iterator it = word->begin();
		while (it != word->end() && (*it < get_imput())) {
			it++;
		}
		if (it == word->end()) {
			options.push_back({-1,0});
		}
		if (options.size() == 0) {
			return nullptr;
		} else {
			int action = seed % options.size();
			seed /= options.size();
			if (options.size() == 1) {
				seed--;
			}
			if (get<0>(options.at(action)) >= 0) {
				vector<int>::iterator head = word->begin() + get<1>(options.at(action));
				tuple<vector<int>,vector<int>> rule = rules->at(get<0>(options.at(action)));
				head = word->erase(head, next(head, get<0>(rule).size()));
				word->insert(head, get<1>(rule).begin(), get<1>(rule).end());
			} else {
				return word;
			}
		}
	}
	for (vector<int>::iterator it = word->begin(); it != word->end(); it++) {
		if (*it >= get_imput()) {
			return nullptr;
		}
	}
	return word;
}

#endif