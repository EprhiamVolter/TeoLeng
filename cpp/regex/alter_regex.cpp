#ifndef alter_regex_cpp
#define alter_regex_cpp

#include "regex/alter_regex.h"

using namespace std;

alter_regex::alter_regex(int imput, regex* reg1, regex* reg2) :
	regex(imput), reg1(reg1), reg2(reg2) {
	if (reg1->get_finite() && reg2->get_finite()) {
		finite = true;
		int limit1 = reg1->limit;
		limit = limit1 + reg2->limit;
		generator = [reg1, reg2, limit1](int seed) {
			return seed < limit1 ? reg1->generate(seed) : reg2->generate(seed - limit1);
		};
	} else {
		finite = false;
		if (reg1->get_finite()) {
			int limit1 = reg1->limit;
			generator = [reg1, reg2, limit1](int seed) {
				if (seed < limit1) {
					return reg1->generate(seed);
				} else {
					return reg2->generate(seed - limit1);
				}
			};
		} else if (reg1->get_finite()) {
			int limit2 = reg2->limit;
			generator = [reg1, reg2, limit2](int seed) {
				if (seed < limit2) {
					return reg2->generate(seed);
				} else {
					return reg1->generate(seed - limit2);
				}
			};
		} else {
			generator = [reg1, reg2](int seed) {
				if (seed % 2 == 0) {
					return reg1->generate(seed / 2);
				} else {
					return reg1->generate((seed - 1) / 2);
				}
			};
		}
	}
	
}

vector<int>* alter_regex::generate(int seed) {
	return generator(seed);
}

eNFA* alter_regex::to_eNFA() const {
	eNFA* enfas[2] = {reg1->to_eNFA(), reg2->to_eNFA()};
	eNFA* enfa1 = enfas[0]; 
	eNFA* enfa2 = enfas[1];
	for (int i = 0; i < 2; ++i) {
		if (enfas[i]->get_state_count() == 1 && !enfas[i]->get_final_states()->elem(0)) { // empty_regex
			delete enfas[i];
			return enfas[1 - i]; 
		}
	}
	int enfa1_states = enfa1->get_state_count();
	int enfa2_states = enfa2->get_state_count();
	int imput = get_imput();
	int states = enfa1_states + enfa2_states + 2;
	int_set* finals = new int_set(states);
	finals->insert(states - 1);
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>{};
	for (int i = 0; i < enfa1_states; i++) {
		transition->push_back(vector<int_set*>());
		for (int j = 0; j < imput; j++) {
			int_set* to_add = new int_set(*(enfa1->get_transition()->at(i).at(j)),false);
			to_add->set_limit(states);
			transition->at(i).push_back(to_add);
		}
	}
	int i2 = enfa1_states;
	for (int i = 0; i < enfa2_states; i++) {
		transition->push_back(vector<int_set*>());
		for (int j = 0; j < imput; j++) {
			int_set* to_add = new int_set(*(enfa2->get_transition()->at(i).at(j)), false, enfa1_states);
			to_add->set_limit(states);
			transition->at(i2).push_back(to_add);
		}
		i2++;
	}
	for (int i = 0; i < 2; i++) {
		transition->push_back(vector<int_set*>());
		for (int j = 0; j < imput; j++) {
			transition->back().push_back(new int_set(states));
		}
	}
	
	vector<int_set*>* e_links = new vector<int_set*>{};
	for (int i = 0; i < enfa1_states; i++) {
		int_set* to_add = new int_set(*(enfa1->get_e_links()->at(i)),false);
		to_add->set_limit(states);
		e_links->push_back(to_add);
	}
	i2 = enfa1_states;
	for (int i = 0; i < enfa2_states; i++) {
		int_set* to_add = new int_set(*(enfa2->get_e_links()->at(i)), false, enfa1_states);
		to_add->set_limit(states);
		e_links->push_back(to_add);
		i2++;
	}
	e_links->push_back(new int_set(states));
	e_links->push_back(new int_set(states));
	
	e_links->at(states - 2)->insert(enfa1->get_initial_state());
	e_links->at(enfa1->get_final_states()->get())->insert(states - 1);
	e_links->at(states - 2)->insert(enfa2->get_initial_state() + enfa1_states);
	e_links->at(enfa2->get_final_states()->get() + enfa1_states)->insert(states - 1);
	delete enfa1;
	delete enfa2;
	return new eNFA(imput, states, states - 2, finals, transition, e_links);
}

string alter_regex::to_string() const {
	return reg1->to_string() + '|' + reg2->to_string();
}

#endif