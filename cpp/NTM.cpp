#ifndef NTM_cpp
#define NTM_cpp

#include <iostream>
#include "NTM.h"
#include "tape.h"

using namespace std;

NTM::NTM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<set<tuple<int,int,bool>>>>* transition, function<int(int)>* res_right, function<int(int)>* res_left) :
	automaton(imput), tape_symbols(tape), blank(blank), state_count(states), initial_state(initial), acept_state(acept), transition(transition), reserve_right(res_right), reserve_left(res_left) {
}

NTM NTM::safe_NTM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<set<tuple<int,int,bool>>>>* transition, function<int(int)>* res_right, function<int(int)>* res_left) {
	if (imput < 0) throw 1;
	if (states < 0) throw 2;
	if (initial < 0) throw 3;
	if (acept < states) throw 4;
	if (transition == nullptr) throw 5;
	if (transition->size() != states) throw 6;
	for (int i = 0; i < states; i++) {
		vector<set<tuple<int,int,bool>>> row = transition->at(i);
		if (row.size() != tape) throw 7;
		for (int j = 0; j < tape; j++) {
			for (tuple<int,int,bool> tup : row.at(j)) {
				if (get<0>(tup) < 0) throw 8;
				if (get<1>(tup) < 0) throw 9;
				if (get<1>(tup) >= tape) throw 10;
			}
		}
	}
	return NTM(imput, tape, blank, states, initial, acept, transition, res_right, res_left);
}

bool NTM::run(vector<int>* word) {
	tape* run_tape = new tape(word, blank, (*reserve_right)(word->size()), (*reserve_left)(word->size()));
	return run(run_tape, initial_state);
}

bool NTM::run(tape* tape, int state) {
	if (state == acept_state) {
		return true;
	}
	if (state < state_count) {
		int read = tape->read();
		for (tuple<int,int,bool> tup : transition->at(state).at(read)) {
			tape->write(get<1>(tup));
			tape->move(get<2>(tup));
			if (run(tape, get<0>(tup))) {
				return true;
			}
			tape->move(!get<2>(tup));
		}
		tape->write(read);
	}
	return false;
}


void NTM::print() {
	cout <<	"imput_symbols:" << get_imput() << 
			" tape_symbols:" << tape_symbols <<
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" acept_state:" << acept_state << 
			" blank:" << blank << endl;
	for (vector<set<tuple<int,int,bool>>> row : *transition) {
		for (set<tuple<int,int,bool>> col : row) {
			for (tuple<int,int,bool> tup : col) {
				cout << '(' << get<0>(tup) << ',' << get<1>(tup) << ',' << (get<2>(tup) ? 'R' : 'L') << "),";
			}
			cout << '|';
		}
		cout << endl;
	}
}

#endif