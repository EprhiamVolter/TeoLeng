#ifndef CFG_h
#define CFG_h

#include <tuple>
#include "grammar.h"
#include "int_set.h"

using namespace std;

class CFG : public grammar {	
	private:
		int var_count;
		int initial_var;
		vector<vector<vector<int>>>* rules;
		
	public:
		CFG(int imput, int vars, int initial, vector<vector<vector<int>>>* rules);
		static CFG safe_CFG(int imput, int vars, int initial, vector<vector<vector<int>>>* rules);
		vector<int>* generate(int seed);
		int_set* get_nullables();
		int_set* get_positives();
		int_set* get_reachables();
		int_set* get_usefuls();
		vector<int>* get_terminal_vars();
		bool get_initial_rereachable();
		bool get_has_nullables();
		bool get_has_not_initial_nullables();
		bool get_has_unit_productions();
		bool get_all_positive();
		bool get_all_reachable();
		bool get_all_useful();
		bool get_initial_positive();
		bool get_initial_nullable();
		bool get_is_simple();
		bool get_is_CNF();
		bool get_is_GNF();
		void compute_metadata();
		void clear_metadata();
		void remove_vars(int_set* vars);
		bool remove_eps_productions(bool keep_if_init = true, bool new_init = false);
		void remove_unit_productions();
		void remove_repeated_rules();
		int remove_non_positives();
		int remove_unreachables();
		int simplify();
		int remove_useless(); 
		int replace_unsolitary_terminals();
		int binarize_productions();
		int convert_to_CNF();
		void print();
		
	private:
		int_set nullables;
		int_set positives;
		int_set reachables;
		int_set usefuls;
		vector<int> terminal_vars;
		
		bool nullables_valid;
		bool positives_valid;
		bool reachables_valid;
		bool usefuls_valid;
		bool terminal_vars_valid;
		
		// 1: yes, 0: no, -1: unknown (to be computed upon calling get)
		int initial_rereachable;
		int has_unit_productions;
		int is_CNF;
		int is_GNF;
};

/*
1 - eps
2 - unit
3 - positive
4 - reachable

*/

#endif