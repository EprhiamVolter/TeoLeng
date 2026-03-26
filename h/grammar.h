#ifndef grammar_h
#define grammar_h

#include <vector>

using namespace std;

class grammar {
	private:
		int imput_symbols;
		
	public:
		grammar(int imput);
		int get_imput() const;
		virtual vector<int>* generate(int seed) = 0;
};

#endif