#ifndef TM_h
#define TM_h

#include <functional>
#include <tuple>
#include "functions.h"
#include "automaton.h"

using namespace std;

class TM : public automaton {
	private:
		int tape_symbols;
		int blank;
		int state_count;
		int initial_state;
		int acept_state;
		vector<vector<tuple<int,int,bool>>>* transition;
		
	public:
		TM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<tuple<int,int,bool>>>* transition, function<int(int)>* res_right = &zero, function<int(int)>* res_left = &id);
		static TM safe_TM(int imput, int tape, int blank, int states, int initial, int acept, vector<vector<tuple<int,int,bool>>>* transition, function<int(int)>* res_right = &zero, function<int(int)>* res_left = &id);
		bool run(vector<int>* word);
		void print();
		
	private:
		function<int(int)>* reserve_left;
		function<int(int)>* reserve_right;
};

#endif