#ifndef eNFA_h
#define eNFA_h

#include "automaton.h"
#include "int_set.h"
#include "NFA.h"
#include "regex/regex.h"

class NFA;
class regex;

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
		vector<int_set*>* get_e_clause();
		void clear_metadata();
		void set_initial_state(int new_initial);
		NFA* equivalent_NFA();
		regex* kleene();
		void print() const;
		
	private:
		vector<int_set*>* e_clause;
		bool e_clause_valid;
		regex* my_regex;
		
		friend class alter_regex;
		friend class concat_regex;
		friend class kleene_regex;
};

#endif