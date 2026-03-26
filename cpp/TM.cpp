#ifndef TM_cpp
#define TM_cpp

#include <iostream>
#include "TM.h"
#include "tape.h"

using namespace std;

TM::TM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<tuple<int,int,bool>>>* transition, function<int(int)>* res_right, function<int(int)>* res_left) :
	automaton(imput), tape_symbols(tape), blank(blank), state_count(states), initial_state(initial), acept_state(acept), transition(transition), reserve_right(res_right), reserve_left(res_left) {
}

TM TM::safe_TM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<tuple<int,int,bool>>>* transition, function<int(int)>* res_right, function<int(int)>* res_left) {
	if (imput < 0) throw 1;
	if (states < 0) throw 2;
	if (initial < 0) throw 3;
	if (acept < states) throw 4;
	if (transition == nullptr) throw 5;
	if (transition->size() != states) throw 6;
	for (int i = 0; i < states; i++) {
		vector<tuple<int,int,bool>> row = transition->at(i);
		if (row.size() != tape) throw 7;
		for (int j = 0; j < tape; j++) {
			tuple<int,int,bool> tup = row.at(j);
			if (get<0>(tup) < 0) throw 8;
			if (get<1>(tup) < 0) throw 9;
			if (get<1>(tup) >= tape) throw 10;
		}
	}
	return TM(imput, tape, blank, states, initial, acept, transition, res_right, res_left);
}

bool TM::run(vector<int>* word) {
	int state = initial_state;
	int word_size = word->size();
	tape tape(word, blank, (*reserve_right)(word_size), (*reserve_left)(word_size));
	while (state < state_count) {
		tuple<int,int,bool> tup = transition->at(state).at(tape.read());
		state = get<0>(tup);
		tape.write(get<1>(tup));
		tape.move(get<2>(tup));
	}
	return state == acept_state;
}

void TM::print() {
	cout <<	"imput_symbols:" << get_imput() << 
			" tape_symbols:" << tape_symbols <<
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" acept_state:" << acept_state << 
			" blank:" << blank << endl;
	for (vector<tuple<int,int,bool>> row : *transition) {
		for (tuple<int,int,bool> tup : row) {
			cout << '(' << get<0>(tup) << ',' << get<1>(tup) << ',' << (get<2>(tup) ? 'R' : 'L') << "),";
		}
		cout << endl;
	}
}

#endif