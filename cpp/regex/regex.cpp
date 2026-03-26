#ifndef regex_cpp
#define regex_cpp

#include "regex/regex.h"

regex::regex(int imput) : 
	grammar(imput), my_eNFA(nullptr) {
}

bool regex::get_finite() const {  
	return finite;
}

int regex::get_limit() const {
	return limit;
}

eNFA* regex::equivalent_eNFA() {
	if (my_eNFA == nullptr) {
		my_eNFA = to_eNFA();
	}
	return my_eNFA;
}

#endif