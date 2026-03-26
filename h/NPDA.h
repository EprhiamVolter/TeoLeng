#ifndef NPDA_h
#define NPDA_h

#include <functional>
#include <set>
#include "functions.h"
#include "automaton.h"
#include "int_set.h"

using namespace std;

class NPDA : public automaton {
	private:
		bool require_empty;
		int stack_symbols;
		int state_count;
		int initial_state;
		int initial_stack;
		int_set* final_states;
		vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition;
		vector<vector<set<tuple<int,vector<int>>>>>* e_links;
		
	public:
		NPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals,
			vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition, vector<vector<set<tuple<int,vector<int>>>>>* e_links, function<int(int)>* reserve = &id);
		static NPDA safe_NPDA(bool empty, int imput, int stack, int states, int ini_state, int ini_stack, int_set* finals,
			vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition, vector<vector<set<tuple<int,vector<int>>>>>* e_links, function<int(int)>* reserve = &id);
		bool run(vector<int>* word);
		void print();
		
	private:
		function<int(int)>* reserve;
		bool run(vector<int>* word, int pos, int state, int top, vector<int>* stack);
};

#endif