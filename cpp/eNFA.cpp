#ifndef eNFA_cpp
#define eNFA_cpp

#include <iostream>
#include "eNFA.h"
#include "regex/empty_regex.h"
#include "regex/eps_regex.h"
#include "regex/lit_regex.h"
#include "regex/alter_regex.h"
#include "regex/concat_regex.h"
#include "regex/kleene_regex.h"

using namespace std;

eNFA::eNFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition, vector<int_set*>* e_links) : 
	automaton(imput), state_count(states), initial_state(initial), final_states(finals), transition(transition), e_links(e_links), e_clause(nullptr), e_clause_valid(false), my_regex(nullptr) {
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

regex* eNFA::kleene() {
	if (my_regex == nullptr) {
		if (final_states->get_size() > 0) {
			vector<vector<vector<regex*>>> R(state_count + 1, vector<vector<regex*>>(state_count, vector<regex*>(state_count, nullptr)));
			for (int j = 0; j < state_count; j++) {
				for (int k = 0; k < state_count; k++) {
					int_set syms(get_imput());
					for (int s = 0; s < get_imput(); s++) {
						if (transition->at(j).at(s)->elem(k)) {
							syms.insert(s);
						}
					}
					regex* zjk;
					if (j == k || e_links->at(j)->elem(k)) {
						zjk = new eps_regex(get_imput());
						for (int s : syms) {
							zjk = new alter_regex(get_imput(), zjk, new lit_regex(get_imput(), s));
						}
					} else {
						if (syms.get_size() > 0) {
							int s0 = syms.get();
							syms.remove(s0);
							zjk = new lit_regex(get_imput(), s0);
							for (int s : syms) {
								zjk = new alter_regex(get_imput(), zjk, new lit_regex(get_imput(), s));
							}
						} else {
							zjk = new empty_regex(get_imput());
						}
					}
					R.at(0).at(j).at(k) = zjk;
				}
			}
			for (int i = 0; i < state_count; i++) {
				for (int j = 0; j < state_count; j++) {
					for (int k = 0; k < state_count; k++) {
						R.at(i+1).at(j).at(k) = new alter_regex(get_imput(),
							R.at(i).at(j).at(k),
							new concat_regex(get_imput(), 
								R.at(i).at(j).at(i),
								new concat_regex(get_imput(),
									new kleene_regex(get_imput(), R.at(i).at(i).at(i)),
									R.at(i).at(i).at(k))));
					}
				}
			}
			int fin = final_states->get();
			final_states->remove(fin);
			my_regex = R.at(state_count).at(initial_state).at(fin);
			for (int f : *final_states) {
				my_regex = new alter_regex(get_imput(), my_regex, R.at(state_count).at(initial_state).at(f));
			}
			final_states->insert(fin);
		} else {
			my_regex = new empty_regex(get_imput());
		} 
	}
	return my_regex;
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