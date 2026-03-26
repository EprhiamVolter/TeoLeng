#ifndef regex_h
#define regex_h

#include <iostream>
#include "grammar.h"
#include "eNFA.h"

using namespace std;

class regex : public grammar {	
	public:
		regex(int imput);
		bool get_finite() const;
		int get_limit() const;
		eNFA* equivalent_eNFA();
		virtual string to_string() const = 0;
		
	protected:
		bool finite;
		int limit;
		virtual eNFA* to_eNFA() const = 0;
		
	private:
		eNFA* my_eNFA;
};

#endif