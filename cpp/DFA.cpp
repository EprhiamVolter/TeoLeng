#ifndef DFA_cpp
#define DFA_cpp

#include <iostream>
#include "DFA.h"

using namespace std;

DFA::DFA(int imput, int states, int initial, int_set* finals, vector<vector<int>> * transition) : 
	automaton(imput), state_count(states), initial_state(initial), final_states(finals), transition(transition) {
}

DFA DFA::safe_DFA(int imput, int states, int initial, int_set* finals, vector<vector<int>>* transition) {
	if (imput < 0) throw 1;
	if (states < 1) throw 2;
	if (initial < 0) throw 3;
	if (initial >= states) throw 4;
	if (finals == nullptr) throw 5;
	if (finals->get_limit() != states) throw 6;
	if (transition == nullptr) throw 7;
	if (transition->size() != states) throw 8;
	for (int i = 0; i < states; i++) {
		vector<int> row = transition->at(i);
		if (row.size() != imput) throw 9;
		for (int j = 0; j < imput; j++) {
			int des_state = row.at(j);
			if (des_state < 0) throw 10;
			if (des_state >= states) throw 11;
		}
	}
	return DFA(imput, states, initial, finals, transition);
}

bool DFA::run(vector<int>* word) {
	int state = initial_state;
	for (int i : *word) {
		state = transition->at(state).at(i);
	}
	return final_states->elem(state);
}

// removes unreachables states, reducing state_count and remaping final_states and transition so the states are in 0..(n-1)
int DFA::remove_unreachables() {
	int_set reachable(state_count);
	int_set explore(state_count,{initial_state});
	while (explore.get_size() > 0) {
		int current = explore.get();
		reachable.insert(current);
		for (int i : transition->at(current)) {
			if (!reachable.elem(i)) {
				explore.insert(i);
			}
		}
		explore.remove(current);
	}
	int new_size = reachable.get_size();
	if (new_size < state_count) {
		int * map = new int[state_count];
		int pos = 0;
		for (int i = 0; i < state_count; i++) {
			if (reachable.elem(i)) {
				map[i] = pos;
				pos++;
			}
		}
		initial_state = map[initial_state];
		int_set* new_finals = new int_set(new_size);
		for (int i : *final_states) {
			if (reachable.elem(i)) {
				new_finals->insert(map[i]);
			}
		}
		delete final_states;
		final_states = new_finals;
		vector<vector<int>>* new_transition = new vector<vector<int>>(new_size,vector<int>());
		for (int i : reachable) {
			for (int j : transition->at(i)) {
				new_transition->at(map[i]).push_back(map[j]);
			}
		}
		delete transition;
		transition = new_transition;
		int ret = state_count;
		state_count = new_size;
		delete[] map;
		return ret - new_size;
	}
	return 0;
}

// special case of rl_reduction that is faster if DFA recognizes nothing
int DFA::empty_reduction() {
	if (final_states->get_size() == 0) {
		delete final_states;
		delete transition;
		final_states = new int_set(1);
		transition = new vector<vector<int>>({vector<int>(get_imput(),0)});
		int ret = state_count;
		state_count = 1;
		return ret - 1;
	}
	return 0;
}

// special case of rl_reduction that is faster if DFA recognizes everything
int DFA::full_reduction() {
	if (final_states->get_size() == state_count) {
		delete final_states;
		delete transition;
		final_states = new int_set(1,true);
		transition = new vector<vector<int>>({vector<int>(get_imput(),0)});
		int ret = state_count;
		state_count = 1;
		return ret - 1;
	}
	return 0;
}

// if DFA has no unreachable states, it reduces them to the minimum possible DFA that recognizes the same language
int DFA::rl_reduction() {
	if (final_states->get_size() == 0) {
		return empty_reduction();
	}
	if (final_states->get_size() == state_count) {
		return full_reduction();
	}
	vector<vector<int>> rl(2,vector<int>());
	vector<int_set*> equiv;
	equiv.push_back(new int_set(*final_states, true));
	equiv.push_back(new int_set(*final_states, false));
	int * map = new int[state_count];
	for (int i = 0; i < state_count; i++) {
		map[i] = final_states->elem(i) ? 1 : 0;
	}
	int process = 0;
	while (process < equiv.size()) {
		rl.clear();
		process = equiv.size();
		for (int i = 0; i < process; i++) {
			rl.push_back(vector<int>());
			rl.at(i).reserve(get_imput());
			for (int state : transition->at(equiv.at(i)->get())) {
				rl.at(i).push_back(map[state]);
			}
			int_set* broken = new int_set(state_count);
			for (int state : *(equiv.at(i))) {
				bool broke = false;
				vector<int> trstate = transition->at(state);
				for (int j = 0; j < get_imput(); j++) {
					broke = broke || (map[trstate.at(j)] != rl.at(i).at(j));
				}
				if (broke) {
					broken->insert(state);
				}
			}
			if (broken->get_size() > 0) {
				int new_class = equiv.size();
				equiv.push_back(broken);
				for (int b : *broken) {
					map[b] = new_class;
				}
				equiv.at(i)->substract(broken);
			} else {
				delete broken;
			}
		}
	}
	int new_size = equiv.size();
	if (new_size < state_count) {
		initial_state = map[initial_state];
		int_set* new_finals = new int_set(new_size);
		for (int state : *final_states) {
			new_finals->insert(map[state]);
		}
		delete final_states;
		final_states = new_finals;
		vector<vector<int>>* new_transition = new vector<vector<int>>(new_size,vector<int>(get_imput(),0));
		for (int i = 0; i < new_size; i++) {
			for (int j = 0; j < get_imput(); j++) {
				new_transition->at(i).at(j) = map[transition->at(equiv.at(i)->get()).at(j)];
			}
		}
		delete[] map;
		delete transition;
		transition = new_transition;
		int ret = state_count;
		state_count = equiv.size();
		return ret - new_size;
	}
	delete[] map;
	return 0;
}

// applies reductions in appropiate order
int DFA::minimize() {
	int u = remove_unreachables();
	int r = rl_reduction();
	return u + r;
}

void DFA::print() const {
	cout <<	"imput_symbols:" << get_imput() << 
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" final_states:" << *final_states << endl;
	for (vector<int> row : *transition) {
		for (int dest : row) {
			cout << dest << ',';
		}
		cout << endl;
	}
}

#endif