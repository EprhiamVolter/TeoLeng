#ifndef functions_h
#define functions_h

#include <functional>

using namespace std;

extern function<int(int)> zero;

extern function<int(int)> id;

function<int(int)> lineal(double m, int n);

#endif