#ifndef int_set_cpp
#define int_set_cpp

#include "int_set.h"

using namespace std;

int_set::int_set(int limit, bool full) : limit(limit) {
	if (!full) {
		size = 0;
		code = 0;
		grid = vector<int> (limit, -1);
		list = vector<int> ();
		list.reserve(limit);
	} else {
		size = limit;
		code = (1 << limit) - 1;
		grid = vector<int> ();
		list = vector<int> ();
		grid.reserve(limit);
		list.reserve(limit);
		for (int i = 0; i < limit; i++) {
			grid.push_back(i);
			list.push_back(i);
		}
	}
}

int_set::int_set(int limit, initializer_list<int> init) : int_set(limit) {
	for (int val : init) {
		insert(val);
	}
}

int_set::int_set(initializer_list<bool> init) : int_set(init.size()) {
	int i = 0;
	for (int val : init) {
		if (val) insert(i);
		i++;
	}
}

int_set::int_set(int limit, int from_int) : int_set(limit) {
	for (int i = 0; i < limit; i++) {
		if ((from_int >> i) % 2 == 1) {
			insert(i);
		}
	}
}

int_set::int_set(const int_set& original) {
	cout << "shouldn't happen\n";
	limit = original.limit;
	size = original.size;
	code = original.code;
	grid = original.grid;
	list = original.list;
}

int_set::int_set(const int_set& original, bool complement, int offset) {
	if (offset == 0) {
		if (!complement) {
			limit = original.limit;
			size = original.size;
			code = original.code;
			grid = original.grid;
			list = original.list;
		} else {
			limit = original.get_limit();
			size = 0;
			code = (1 << limit) - 1 - original.code;
			grid = vector<int> (limit,-1);
			list = vector<int> ();
			list.reserve(limit);
			for (int i = 0; i < limit; i++) {
				if (!original.elem(i)) {
					grid.at(i) = size;
					list.push_back(i);
					size++;
				}
			}
			
		}
	} else {
		if (!complement) {
			limit = original.get_limit() + offset;
			size = 0;
			code = 0;
			grid = vector<int>(limit,-1);
			list = vector<int>();
			list.reserve(limit);
			if (offset > 0) {
				for (int i : original.list) {
					insert(i + offset);
				}
			} else {
				for (int i : original.list) {
					if (i - offset >= 0) {
						insert(i + offset);
					}
				}
			}
		} else {
			limit = original.get_limit() + offset;
			size = limit;
			code = (1 << limit) - 1;
			grid = vector<int> ();
			list = vector<int> ();
			grid.reserve(limit);
			list.reserve(limit);
			for (int i = 0; i < limit; i++) {
				grid.push_back(i);
				list.push_back(i);
			}
			if (offset > 0) {
				for (int i : original.list) {
					remove(i + offset);
				}
			} else {
				for (int i : original.list) {
					if (i - offset >= 0) {
						remove(i + offset);
					}
				}
			}
		}
	}
}

int int_set::get_limit() const {
	return limit;
}

int int_set::get_size() const {
	return size;
}

int int_set::get_code() const {
	return code;
}

vector<int> int_set::get_grid() const {
	return grid;
}

const vector<int>& int_set::get_list() const {
	return list;
}

void int_set::set_limit(int new_limit, bool fill, bool shrink) {
	if (new_limit < limit) {
		for (int i = new_limit; i < limit; i++) {
			remove(i);
		}
		grid.resize(new_limit, -1);
		if (shrink) {
			grid.shrink_to_fit();
			list.shrink_to_fit();
			list.reserve(new_limit);
		}
	} else if (new_limit > limit) {
		grid.resize(new_limit, -1);
		list.reserve(new_limit);
		if (fill) {
			for (int i = limit; i < new_limit; i++) {
				insert(i);
			}
		}
	}
	limit = new_limit;
}

void int_set::clear() {
	for (int i : list) {
		grid.at(i) = -1;
	}
	list.clear();
	size = 0;
	code = 0;
}

void int_set::fill() {
	if (size < limit) {
		size = limit;
		code = (1 << limit) - 1;
		grid.clear();
		list.clear();
		grid.reserve(limit);
		list.reserve(limit);
		for (int i = 0; i < limit; i++) {
			grid.push_back(i);
			list.push_back(i);
		}
	}
}

int int_set::insert(int value) {
	if (grid.at(value) == -1) {
		grid.at(value) = size;
		list.push_back(value);
		size++;
		code += 1 << value;
		return 1;
	} else {
		return 0;
	}
}

int int_set::remove(int value) {
	if (grid.at(value) != -1) {
		size--;
		code -= 1 << value;
		int top = list.at(size);
		int pos = grid.at(value);
		list.at(pos) = top;
		grid.at(top) = pos;
		grid.at(value) = -1;
		list.pop_back();
		return -1;
	} else {
		return 0;
	}		
}

bool int_set::elem(int value) const {
	return grid.at(value) != -1;
}

int int_set::get() const {
	if (size == 0) throw 1;
	return list.at(0);
}

int int_set::append(int_set* set) {
	int ret = 0;
	for (int i : *set) {
		ret += insert(i);
	}
	return ret;
}

int int_set::operator+=(int_set* set) {
	int ret = 0;
	for (int i : *set) {
		ret += insert(i);
	}
	return ret;
}

int int_set::substract(int_set* set) {
	int ret = 0;
	if (set->size <= size) {
		for (int i : *set) {
			ret += remove(i);
		}
	} else {
		int i = 0;
		while (i < size) {
			if (set->elem(list.at(i))) {
				ret += remove(list.at(i));
			} else {
				i++;
			}
		}
	}
	return ret;
}

int int_set::operator-=(int_set* set) {
	int ret = 0;
	for (int i : *set) {
		ret += remove(i);
	}
	return ret;
}

bool int_set::equals(int_set* set) {
	return code == set->code;
}

bool int_set::operator==(int_set* set) {
	return code == set->code;
}

bool int_set::contains(int_set* set) {
	for (int i : *set) {
		if (!elem(i)) {
			return false;
		}
	}
	return true;
}

bool int_set::intersect(int_set* set) const {
	if (set->get_size() < size) {
		for (int i : *set) {
			if (elem(i)) {
				return true;
			}
		}
	} else {
		for (int i : list) {
			if (set->elem(i)) {
				return true;
			}
		}
	}
	return false;
}

vector<int>::iterator int_set::begin() { 
	return list.begin(); 
}

vector<int>::iterator int_set::end() { 
	return list.end(); 
}

ostream& operator<<(ostream& os, const int_set& set) {
	for (int i : set.grid) {
		os << (i != -1);
	}
	return os;
}

#endif