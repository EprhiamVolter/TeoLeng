#ifndef eps_regex_h
#define eps_regex_h

#include "regex/regex.h"

class eps_regex : public regex {
	public:
		eps_regex(int imput);
		vector<int>* generate(int seed);
		eNFA* to_eNFA() const;
		string to_string() const;	
};

#endif