#ifndef NFA_h
#define NFA_h

#include "automaton.h"
#include "int_set.h"
#include "DFA.h"
#include "regex/regex.h"

using namespace std;

class regex;

class NFA : public automaton {
	private:
		int state_count;
		int initial_state;
		int_set* final_states;
		vector<vector<int_set*>>* transition;
		
	public:
		NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition);
		static NFA safe_NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition);
		bool run(vector<int>* word);
		DFA* equivalent_DFA() const;
		DFA* equivalent_connex_DFA() const;
		regex* equivalent_regex();
		void print() const;
		
	private:
		void compute_DFA(int x, int pos, int_set *it, int_set* fin, vector<vector<int>>* tran) const;
		regex* my_regex;
};

#endif