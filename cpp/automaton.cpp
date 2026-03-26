#ifndef automaton_cpp
#define automaton_cpp

#include "automaton.h"

automaton::automaton(int imput) : 
	imput_symbols(imput) {
}

int automaton::get_imput() const {
	return imput_symbols;
}

#endif