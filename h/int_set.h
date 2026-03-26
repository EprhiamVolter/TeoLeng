#ifndef int_set_h
#define int_set_h

#include <vector>
#include <iostream>

using namespace std;

class int_set { 
	private:
		int limit;
		int size;
		int code;
		vector<int> grid;
		vector<int> list;
		
	public:
		int_set(int limit, bool full = false);
		int_set(int limit, initializer_list<int> init);
		int_set(initializer_list<bool> init);
		int_set(int limit, int from_int);
		int_set(const int_set& original);
		int_set(const int_set& original, bool complement, int offset = 0);
		int get_limit() const;
		int get_size() const;
		int get_code() const;
		vector<int> get_grid() const;
		const vector<int>& get_list() const;
		void set_limit(int new_limit, bool fill = false, bool shrink = false);
		void clear();
		void fill();
		int insert(int value);
		int remove(int value);
		bool elem(int value) const;
		int get() const;
		int append(int_set* set);
		int operator+=(int_set* set);
		int substract(int_set* set);
		int operator-=(int_set* set);
		bool equals(int_set* set);
		bool operator==(int_set* set);
		bool contains(int_set* set);
		bool intersect(int_set* set) const;
		vector<int>::iterator begin();
		vector<int>::iterator end();
		friend ostream& operator<<(ostream& os, const int_set& set);
};

#endif