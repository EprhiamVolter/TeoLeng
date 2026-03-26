#ifndef NFA_cpp
#define NFA_cpp

#include <iostream>
#include "NFA.h"

using namespace std;

NFA::NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition) : 
	automaton(imput), state_count(states), initial_state(initial), final_states(finals), transition(transition) {
}

NFA NFA::safe_NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition) {
	if (imput < 0) throw 1;
	if (states < 1) throw 2;
	if (initial < 0) throw 3;
	if (initial >= states) throw 4;
	if (finals == nullptr) throw 5;
	if (finals->get_limit() != states) throw 6;
	if (transition == nullptr) throw 7;
	if (transition->size() != states) throw 8;
	for (int i = 0; i < states; i++) {
		vector<int_set*> row = transition->at(i);
		if (row.size() != imput) throw 9;
		for (int j = 0; j < imput; j++) {
			if (row.at(j) == nullptr) throw 10;
			if (row.at(j)->get_limit() != states) throw 11;
		}
	}
	return NFA(imput, states, initial, finals, transition);
}

bool NFA::run(vector<int>* word) {
	int_set states(state_count,{initial_state});
	for (int i : *word) {
		int_set next(state_count);
		for (int state : states) {
			next.append(transition->at(state).at(i));
		}
		states = next;
	}
	return states.intersect(final_states);
}

// constructs the equivalent DFA with 2^n states (likely most unreachable)
DFA* NFA::equivalent_DFA() const {
	int DFA_states = 1 << state_count;
	int DFA_initial = 1 << initial_state;
	int_set* iterator = new int_set(state_count);
	int_set* DFA_finals = new int_set(DFA_states);
	vector<vector<int>>* DFA_transition = new vector<vector<int>>(DFA_states,vector<int>());
	compute_DFA(state_count - 1, 0, iterator, DFA_finals, DFA_transition);
	return new DFA(get_imput(), DFA_states, DFA_initial, DFA_finals, DFA_transition);
}

void NFA::compute_DFA(int x, int pos, int_set* it, int_set* fin, vector<vector<int>>* tran) const {
	if (x == -1) {
		if (it->intersect(final_states)) {
			fin->insert(pos);
		}
		tran->at(pos).reserve(get_imput());
		for (int i = 0; i < get_imput(); i++) {
			int_set dest(state_count);
			for (int j : *it) {
				dest.append(transition->at(j).at(i));
			}
			tran->at(pos).push_back(dest.get_code());
		}
	} else {
		compute_DFA(x - 1, pos,it, fin, tran);
		it->insert(x);
		compute_DFA(x - 1, pos + (1 << x), it, fin, tran);
		it->remove(x);
	}
}

// constructs equivalent_DFA()->remove_unreachables() without processing unreachable states in the first place.
DFA* NFA::equivalent_connex_DFA() const {
	int max_states = 1 << state_count;
	vector<int_set*> equiv(1, new int_set(state_count,{initial_state}));
	vector<int> map(max_states,-1);
	map.at(1 << initial_state) = 0;
	int process = 0;
	int reserved = 1;
	vector<vector<int>>* DFA_transition = new vector<vector<int>>();
	while (process < reserved) {
		DFA_transition->push_back(vector<int>());
		DFA_transition->at(process).reserve(get_imput());
		for (int i = 0; i < get_imput(); i++) {
			int_set* dest = new int_set(state_count);
			for (int j : *(equiv.at(process))) {
				dest->append(transition->at(j).at(i));
			}
			int dest_pos = map.at(dest->get_code());
			if (dest_pos == -1) {
				map.at(dest->get_code()) = reserved;
				dest_pos = reserved;
				reserved++;
				equiv.push_back(dest);
			} else {
				delete dest;
			}
			DFA_transition->at(process).push_back(dest_pos);
		}
		process++;
	}
	int_set* DFA_finals = new int_set(reserved);
	for (int i = 0; i < reserved; i++) {
		if (equiv.at(i)->intersect(final_states)) {
			DFA_finals->insert(i);
		}
	}
	return new DFA(get_imput(), reserved, 0, DFA_finals, DFA_transition);
}

void NFA::print() const {
	cout <<	"imput_symbols:" << get_imput() << 
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" final_states:" << *final_states << endl;
	for (vector<int_set*> row : *transition) {
		for (int_set* dest : row) {
			cout << *dest << "|";
		}
		cout << endl;
	}
}

#endif