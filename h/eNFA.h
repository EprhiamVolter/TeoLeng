#ifndef eNFA_h
#define eNFA_h

#include "automaton.h"
#include "int_set.h"
#include "NFA.h"

using namespace std;

class eNFA : public automaton {
	private:
		int state_count;
		int initial_state;
		int_set* final_states;
		vector<vector<int_set*>>* transition;
		vector<int_set*>* e_links;
		
	public:
		eNFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition, vector<int_set*>* e_links);
		static eNFA safe_eNFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition, vector<int_set*>* e_links);
		bool run(vector<int>* word);
		int get_state_count() const;
		int get_initial_state() const;
		int_set* get_final_states() const;
		vector<vector<int_set*>>* get_transition() const;
		vector<int_set*>* get_e_links() const;
		vector<int_set*>* get_e_clause();
		void clear_metadata();
		void set_state_count(int new_states);
		void set_initial_state(int new_initial);
		NFA* equivalent_NFA();
		void print() const;
		
	private:
		vector<int_set*>* e_clause;
		bool e_clause_valid;
		
		friend class alter_regex;
		friend class concat_regex;
		friend class kleene_regex;
};

#endif