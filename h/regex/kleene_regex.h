#ifndef kleene_regex_h
#define kleene_regex_h

#include <functional>
#include "regex/regex.h"

class kleene_regex : public regex {
	private:
		regex* reg1;
		
	public:
		kleene_regex(int imput, regex* reg1);
		vector<int>* generate(int seed);
		eNFA* to_eNFA() const;
		string to_string() const;
		
	private:
		function<vector<int>*(int)> generator;
		bool parenthesis;
};

#endif