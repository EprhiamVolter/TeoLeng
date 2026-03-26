#ifndef automaton_h
#define automaton_h

#include <vector>

using namespace std;

class automaton {
	private:
		int imput_symbols;
		
	public:
		automaton(int imput);
		int get_imput() const;
		virtual bool run(vector<int>* word) = 0;
};

#endif