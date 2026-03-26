#ifndef alter_regex_h
#define alter_regex_h

#include <functional>
#include "regex/regex.h"

class alter_regex : public regex {
	private:
		regex* reg1;
		regex* reg2;
		
	public:
		alter_regex(int imput, regex* reg1, regex* reg2);
		vector<int>* generate(int seed);
		eNFA* to_eNFA() const;
		string to_string() const;
		
	private:
		function<vector<int>*(int)> generator;
};

#endif