#ifndef grammar_cpp
#define grammar_cpp

#include "grammar.h"

grammar::grammar(int imput) : 
	imput_symbols(imput) {
}

int grammar::get_imput() const {
	return imput_symbols;
}

#endif