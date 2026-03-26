#ifndef DPDA_cpp
#define DPDA_cpp

#include <iostream>
#include "DPDA.h"

using namespace std;

DPDA_oper::DPDA_oper(tuple<int,vector<int>>* eps_oper) :
	epsilon(true), eps_oper(eps_oper), read_oper(nullptr) {
}

DPDA_oper::DPDA_oper(vector<tuple<int,vector<int>>>* read_oper) : 
	epsilon(false), eps_oper(nullptr), read_oper(read_oper) {
}

DPDA::DPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals, vector<vector<DPDA_oper>>* transition, function<int(int)>* reserve) :
	require_empty(empty), automaton(imput), stack_symbols(stack), state_count(states), initial_state(ini_state), initial_stack(ini_stack), final_states(finals), transition(transition), reserve(reserve) {
}

DPDA DPDA::safe_DPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals, vector<vector<DPDA_oper>>* transition, function<int(int)>* reserve) {
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
		vector<DPDA_oper> row = transition->at(i);
		if (row.size() != stack) throw 11;
		for (int j = 0; j < stack; j++) {
			DPDA_oper oper = row.at(j);
			if (oper.epsilon) {
				if (oper.read_oper != nullptr) throw 12;
				if (oper.eps_oper == nullptr) throw 13;
				tuple<int,vector<int>> tup = *oper.eps_oper;
				if (get<0>(tup) < 0) throw 14;
				if (get<0>(tup) >= states) throw 15;
				for (int k : get<1>(tup)) {
					if (k < 0) throw 16;
					if (k >= states) throw 17;
				}
			} else {
				if (oper.eps_oper != nullptr) throw 18;
				if (oper.read_oper == nullptr) throw 19;
				vector<tuple<int,vector<int>>>* read_oper = oper.read_oper;
				if (read_oper == nullptr) throw 20;
				if (read_oper->size() != imput) throw 21;
				for (int k = 0; k < imput; k++) {
					tuple<int,vector<int>> tup = read_oper->at(k);
					if (get<0>(tup) < 0) throw 22;
					if (get<0>(tup) >= states) throw 23;
					for (int l : get<1>(tup)) {
						if (l < 0) throw 24;
						if (l >= stack) throw 25;
					}
				}
			}
		}
	}
	return DPDA(empty, imput, stack, states, ini_state, ini_stack, finals, transition, reserve);
}

bool DPDA::run(vector<int>* word) {
	int i = 0;
	int size = word->size();
	int state = initial_state;
	vector<int> stack;
	stack.reserve((*reserve)(size));
	int top = initial_stack;
	DPDA_oper oper = transition->at(state).at(top);
	while (oper.epsilon || i < size) {
		tuple<int,vector<int>> tup;
		if (oper.epsilon) {
			tup = *(oper.eps_oper);
		} else {
			tup = oper.read_oper->at(word->at(i));
			i++;
		}
		state = get<0>(tup);
		for (int stack_symbol : get<1>(tup)) {
			stack.push_back(stack_symbol);
		}
		if (stack.empty()) {
			return i == size && final_states->elem(state);
		}
		top = stack.back();
		stack.pop_back();
		oper = transition->at(state).at(top);
	}
	return final_states->elem(state) && !require_empty;
}

void DPDA::print() {
	cout <<	"require_empty:" << require_empty <<
			" imput_symbols:" << get_imput() << 
			" stack_symbols:" << stack_symbols <<
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" initial_stack:" << initial_stack <<
			" final_states:" << *final_states << endl;
	for (int i = 0; i < state_count; i++) {
		cout << "state:" << i << endl;
		vector<DPDA_oper> row = transition->at(i);
		for (int j = 0; j < stack_symbols; j++) {
			cout << "top:" << j << " ";
			DPDA_oper oper = row.at(j);
			if (oper.epsilon) {
				tuple<int,vector<int>> tup = *(oper.eps_oper);
				cout << "eps- "<< get<0>(tup) << ':';
				for (int sym : get<1>(tup)) {
					cout << sym << '|';
				}
				cout << ' ';
			} else {
				for (tuple<int,vector<int>> tup : *(oper.read_oper)) {
					cout << get<0>(tup) << ':';
					for (int sym : get<1>(tup)) {
						cout << sym << '|';
					}
					cout << ' ';
				}
			}
			cout << endl;
		}
	}
	cout << endl;
}

#endif