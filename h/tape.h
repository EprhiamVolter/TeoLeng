#ifndef tape_h
#define tape_h

#include <vector>

using namespace std;

class tape {
	private:
		int head;
		int blank;
		vector<int>* positives;
		vector<int>* negatives;
		
	public:
		tape(vector<int>* word, int blank, int reserve_right = 0, int reserve_left = 0);
		void move(bool right);
		int read();
		void write(int x);
};

#endif