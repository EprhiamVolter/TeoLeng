#ifndef DFA_h
#define DFA_h

#include "automaton.h"
#include "int_set.h"

using namespace std;

class DFA : public automaton {
	private:
		int state_count;
		int initial_state;
		int_set* final_states;
		vector<vector<int>>* transition;
		
	public:
		DFA(int imput, int states, int initial, int_set* finals, vector<vector<int>>* transition);
		static DFA safe_DFA(int imput, int states, int initial, int_set* finals, vector<vector<int>>* transition);
		bool run(vector<int>* word);
		int remove_unreachables();
		int empty_reduction();
		int full_reduction();
		int rl_reduction();
		int minimize();
		void print() const;
};

#endif