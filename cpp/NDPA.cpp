#ifndef NPDA_cpp
#define NPDA_cpp

#include <iostream>
#include "NPDA.h"

using namespace std;

NPDA::NPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals,
	vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition, vector<vector<set<tuple<int,vector<int>>>>>* e_links, function<int(int)>* reserve) :
	require_empty(empty), automaton(imput), stack_symbols(stack), state_count(states), initial_state(ini_state),
	initial_stack(ini_stack), final_states(finals), transition(transition), e_links(e_links), reserve(reserve) {
}

NPDA NPDA::safe_NPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals,
	vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition, vector<vector<set<tuple<int,vector<int>>>>>* e_links, function<int(int)>* reserve) {
	if (imput < 0) throw 1;
	if (states < 1) throw 2;
	if (ini_state < 0) throw 3;
	if (ini_state >= states) throw 4;
	if (ini_stack < 0) throw 5;
	if (ini_stack >= stack) throw 6;
	if (finals == nullptr) throw 7;
	if (finals->get_limit() != states) throw 8;
	if (transition == nullptr) throw 9;
	if (transition->size() != states) throw 10;
	for (int i = 0; i < states; i++) {
		vector<vector<set<tuple<int,vector<int>>>>> row = transition->at(i);
		if (row.size() != stack) throw 11;
		for (int j = 0; j < stack; j++) {
			vector<set<tuple<int,vector<int>>>> read_oper = row.at(j);
			if (read_oper.size() != imput) throw 12;
			for (int k = 0; k < imput; k ++) {
				for (tuple<int,vector<int>> tup : read_oper.at(k)) {
					if (get<0>(tup) < 0) throw 13;
					if (get<0>(tup) >= states) throw 14;
					for (int l : get<1>(tup)) {
						if (l < 0) throw 15;
						if (l >= stack) throw 16;
					}
				}
			}
		}
	}
	for (int i = 0; i < states; i++) {
		vector<set<tuple<int,vector<int>>>> row = e_links->at(i);
		if(row.size() != stack) throw 17;
		for (int j = 0; j < stack; j++) {
			for (tuple<int,vector<int>> tup : row.at(j)) {
				if (get<0>(tup) < 0) throw 18;
				if (get<0>(tup) >= states) throw 19;
				for (int k : get<1>(tup)) {
					if (k < 0) throw 20;
					if (k >= states) throw 21;
				}
			}
		}
	}
	return NPDA(empty, imput, stack, states, ini_state, ini_stack, finals, transition, e_links, reserve);
}

bool NPDA::run(vector<int>* word) {
	vector<int>* stack = new vector<int>();
	stack->reserve((*reserve)(word->size()));
	return run(word, 0, initial_state, initial_stack, stack);
}


bool NPDA::run(vector<int>* word, int pos, int state, int top, vector<int>* stack) {
	if (!require_empty && pos == word->size() && final_states->elem(state)) {
		return true;
	}
	if (pos < word->size()) {
		for (tuple<int,vector<int>> tup : transition->at(state).at(top).at(word->at(pos))) {
			for (int stack_symbol : get<1>(tup)) {
				stack->push_back(stack_symbol);
			}
			if (stack->empty()) {
				if (pos + 1 == word->size() && final_states->elem(get<0>(tup))) {
					return true;
				}
			} else {
				int new_top = stack->back();
				stack->pop_back();
				if (run(word, pos + 1, get<0>(tup), new_top, stack)) {
					return true;
				}
				stack->push_back(new_top);
			}
			stack->resize(stack->size() - get<1>(tup).size());
		}
	}
	for (tuple<int,vector<int>> tup : e_links->at(state).at(top)) {
		for (int stack_symbol : get<1>(tup)) {
			stack->push_back(stack_symbol);
		}
		if (stack->empty()) {
			if (pos == word->size() && final_states->elem(get<0>(tup))) {
				return true;
			}
		} else {
			int new_top = stack->back();
			stack->pop_back();
			if (run(word, pos, get<0>(tup), new_top, stack)) {
				return true;
			}
			stack->push_back(new_top);
		}
		stack->resize(stack->size() - get<1>(tup).size());
	}
	return false;
}

void NPDA::print() {
	cout <<	"require_empty:" << require_empty <<
			" imput_symbols:" << get_imput() << 
			" stack_symbols:" << stack_symbols <<
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" initial_stack:" << initial_stack <<
			" final_states:" << *final_states << endl;
	for (int i = 0; i < state_count; i++) {
		cout << "state:" << i << endl;
		vector<vector<set<tuple<int,vector<int>>>>> row = transition->at(i);
		for (int j = 0; j < stack_symbols; j++) {
			cout << "top:" << j << " ";
			vector<set<tuple<int,vector<int>>>> col = row.at(j);
			for (set<tuple<int,vector<int>>> oper : col) {
				for (tuple<int,vector<int>> tup : oper) {
					cout << get<0>(tup) << ':';
					for (int sym : get<1>(tup)) {
						cout << sym << ',';
					}
					cout << ' ';
				}
				cout << "|";
			}
			cout << endl;
		}
	}
	for (int i = 0; i < state_count; i++) {
		cout << "state:" << i << endl;
		vector<set<tuple<int,vector<int>>>> row = e_links->at(i);
		for (int j = 0; j < stack_symbols; j++) {
			cout << "top:" << j << " ";
			set<tuple<int,vector<int>>> oper = row.at(j);
			for (tuple<int,vector<int>> tup : oper) {
				cout << get<0>(tup) << ':';
				for (int sym : get<1>(tup)) {
					cout << sym << ',';
				}
				cout << ' ';
			}
			cout << '|' << endl;
		}
	}
}

#endif