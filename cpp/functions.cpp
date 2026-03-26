#ifndef functions_cpp
#define functions_cpp

#include "functions.h"

using namespace std;

function<int(int)> id = [](int x) {
	return x;
};

function<int(int)> zero = [](int x) {
	return 0;
};

function<int(int)> lineal(double m, int n) {
	return [m,n](int x) -> int {
		return int(m * x) + n;
	};
}

#endif 