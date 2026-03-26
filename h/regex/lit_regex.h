#ifndef lit_regex_h
#define lit_regex_h

#include "regex/regex.h"

class lit_regex : public regex {
	private:
		int literal;
		
	public:
		lit_regex(int imput, int literal);
		vector<int>* generate(int seed);
		eNFA* to_eNFA() const;
		string to_string() const;	
};

#endif