#ifndef NTM_h
#define NTM_h

#include <functional>
#include <set>
#include <tuple>
#include "functions.h"
#include "automaton.h"
#include "tape.h"

using namespace std;

class NTM : public automaton {
	private:
		int tape_symbols;
		int blank;
		int state_count;
		int initial_state;
		int acept_state;
		vector<vector<set<tuple<int,int,bool>>>>* transition;
		
	public:
		NTM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<set<tuple<int,int,bool>>>>* transition, function<int(int)>* res_right = &zero, function<int(int)>* res_left = &id);
		static NTM safe_NTM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<set<tuple<int,int,bool>>>>* transition, function<int(int)>* res_right = &zero, function<int(int)>* res_left = &id);
		bool run(vector<int>* word);
		void print();
		
	private:
		function<int(int)>* reserve_left;
		function<int(int)>* reserve_right;
		bool run(tape* tape, int state);
};

#endif