#ifndef eNFA_cpp
#define eNFA_cpp

#include "eNFA.h"
#include <iostream>

using namespace std;

eNFA::eNFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition, vector<int_set*>* e_links) : 
	automaton(imput), state_count(states), initial_state(initial), final_states(finals), transition(transition), e_links(e_links), e_clause(nullptr), e_clause_valid(false) {
}

eNFA eNFA::safe_eNFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition, vector<int_set*>* e_links) {
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
	if (e_links == nullptr) throw 12;
	if (e_links->size() != states) throw 13;
	for (int i = 0; i < states; i++) {
		if (e_links->at(i) == nullptr) throw 14;
		if (e_links->at(i)->get_limit() != states) throw 15;
	}
	return eNFA(imput, states, initial, finals, transition, e_links);
}

bool eNFA::run(vector<int>* word) {
	get_e_clause();
	int_set* states(e_clause->at(initial_state));
	for (int i : *word) {
		int_set* next = new int_set(state_count);
		for (int state : *states) {
			for (int next_state : *(transition->at(state).at(i))) {
				next->append(e_clause->at(next_state));
			}
		}
		states = next;
	}
	return states->intersect(final_states);
}

int eNFA::get_state_count() const {
	return state_count;
}

int eNFA::get_initial_state() const {
	return initial_state;
}

int_set* eNFA::get_final_states() const {
	return final_states;
}

vector<vector<int_set*>>* eNFA::get_transition() const {
	return transition;
}

vector<int_set*>* eNFA::get_e_links() const {
	return e_links;
}


vector<int_set*>* eNFA::get_e_clause() {
	if (!e_clause_valid) {
		if (e_clause != nullptr) {
			delete e_clause;
		}
		e_clause = new vector<int_set*>;
		for (int i = 0; i < state_count; i++) {
			e_clause->push_back(new int_set(state_count,{i}));
		}
		int_set to_explore(state_count);
		for (int i = 0; i < state_count; i++) {
			to_explore.append(e_links->at(i));
			while (to_explore.get_size() > 0) {
				e_clause->at(i)->append(&to_explore);
				for (int j : to_explore) {
					if (j > i) {
						to_explore.append(e_links->at(j));
					} else if (j < i) {
						e_clause->at(i)->append(e_clause->at(j));
					}
				}
				to_explore.substract(e_clause->at(i));
			}
		}
	}
	return e_clause;
}

void eNFA::clear_metadata() {
	e_clause_valid = false;
}

void eNFA::set_state_count(int new_states) {
	e_clause_valid = false;
	state_count = new_states;
}

void eNFA::set_initial_state(int new_initial) {
	initial_state = new_initial;
}

NFA* eNFA::equivalent_NFA() {
	int_set* NFA_finals = new int_set(*final_states,false);
	get_e_clause();
	if (e_clause->at(initial_state)->intersect(final_states)) {
		NFA_finals->insert(initial_state);
	}
	vector<vector<int_set*>>* NFA_transition = new vector<vector<int_set*>>(state_count,vector<int_set*>());
	for (int i = 0; i < state_count; i++) {
		for (int j = 0; j < get_imput(); j++) {
			int_set* dest = new int_set(state_count);
			for (int state1 : *(e_clause->at(i))) {
				for (int state2 : *(transition->at(state1).at(j))) {
					dest->append(e_clause->at(state2));
				}
			}
			NFA_transition->at(i).push_back(dest);
		}
	}
	return new NFA(get_imput(), state_count, initial_state, NFA_finals, NFA_transition);
}

void eNFA::print() const {
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
	
	cout << "e_links\n";
	for (int_set* dest : *e_links) {
		cout << *dest << "|";
	}
	cout << endl;
}

#endif