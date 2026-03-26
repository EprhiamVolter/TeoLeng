#ifndef empty_regex_h
#define empty_regex_h

#include "regex/regex.h"

class empty_regex : public regex {
	public:
		empty_regex(int imput);
		vector<int>* generate(int seed);
		eNFA* to_eNFA() const;
		string to_string() const;
};

#endif