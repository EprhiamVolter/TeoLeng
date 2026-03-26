#ifndef DPDA_h
#define DPDA_h

#include <functional>
#include "functions.h"
#include "automaton.h"
#include "int_set.h"

using namespace std;

struct DPDA_oper {
	bool epsilon;
	tuple<int,vector<int>>* eps_oper;
	vector<tuple<int,vector<int>>>* read_oper;
	
	DPDA_oper(tuple<int,vector<int>>* eps_oper);
	DPDA_oper(vector<tuple<int,vector<int>>>* read_oper);
};

class DPDA : public automaton {
	private:
		bool require_empty;
		int stack_symbols;
		int state_count;
		int initial_state;
		int initial_stack;
		int_set* final_states;
		vector<vector<DPDA_oper>>* transition;
		
	public:
		DPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals, vector<vector<DPDA_oper>>* transition, function<int(int)>* reserve = &id);
		static DPDA safe_DPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals, vector<vector<DPDA_oper>>* transition, function<int(int)>* reserve = &id);
		bool run(vector<int>* word);
		void print();
		
	private:
		function<int(int)>* reserve;
};

#endif