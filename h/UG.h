#ifndef UG_h
#define UG_h

#include <tuple>
#include "grammar.h"

using namespace std;

class UG : public grammar {	
	private:
		int var_count;
		int initial_var;
		vector<tuple<vector<int>,vector<int>>>* rules;
		
	public:
		UG(int imput, int vars, int initial, vector<tuple<vector<int>,vector<int>>>* rules);
		static UG safe_UG(int imput, int vars, int initial, vector<tuple<vector<int>,vector<int>>>* rules);
		vector<int>* generate(int seed);
};

#endif