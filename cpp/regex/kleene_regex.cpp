#ifndef kleene_regex_cpp
#define kleene_regex_cpp

#include "regex/kleene_regex.h"
#include "regex/concat_regex.h"
#include "regex/alter_regex.h"

using namespace std;

kleene_regex::kleene_regex(int imput, regex* reg1) :
	regex(imput), reg1(reg1) {
	bool finite1 = reg1->get_finite();
	if (finite1) {
		int limit1 = reg1->get_limit();
		if (limit1 == 0) {
			finite = true;
			limit = 0;
			generator = [](int seed) {
				return nullptr;
			};
		} else {
			finite = false;
			if (limit1 == 1) {
				vector<int>* word = reg1->generate(0);
				generator = [word](int seed) {
					vector<int>* ret = new vector<int>();
					for (int i = 0; i < seed; i++) {
						ret->insert(ret->end(), word->begin(), word->end());
					}
					return ret;
				};
			} else {
				generator = [reg1,limit1](int seed) {
					vector<int> list;
					int p = 1;
					int size = 0;
					while (seed >= p) {
						seed -= p;
						p *= limit1;
						size++;
					}
					list.reserve(size);
					for (int i = 0; i < size; i++) {
						list.push_back(seed % limit1);
						seed /= limit1;
					}
					vector<int>* ret = new vector<int>();
					for (int i : list) {
						vector<int>* word = reg1->generate(i);
						ret->insert(ret->end(), word->begin(), word->end());
						delete word;
					}
					return ret;
				};
			}
		}
	} else {
		finite = false;
		generator = [reg1](int seed) {
			vector<int> list;
			list.push_back(0);
			int i = seed;
			while (i > 0) {
				if (i % 2 == 0) {
					list.back()++;
				} else {
					list.push_back(0);
				}
				i = i >> 1;
			}
			list.pop_back();
			vector<int>* ret = new vector<int>();
			for (int i : list) {
				vector<int>* word = reg1->generate(i);
				ret->insert(ret->end(), word->begin(), word->end());
				delete word;
			}
			return ret;
		};
	}
	concat_regex* concat_reg1 = dynamic_cast<concat_regex*>(reg1);
	alter_regex* alter_reg1 = dynamic_cast<alter_regex*>(reg1);
	parenthesis = (concat_reg1 != nullptr || alter_reg1 != nullptr);
}

vector<int>* kleene_regex::generate(int seed) {
	return generator(seed);
}

eNFA* kleene_regex::to_eNFA() const {
	eNFA* enfa = reg1->equivalent_eNFA();
	if (enfa->state_count == 1) { // empty_regex or eps_regex
		return enfa; 
	}
	int imput =  enfa->get_imput();
	int enfa_states = enfa->state_count;
	int states = enfa_states + 2;
	enfa->state_count = states;
	for (int i = 0; i < enfa_states; i++) {
		for (int j = 0; j < imput; j++) {
			enfa->transition->at(i).at(j)->set_limit(states);
		}
		enfa->e_links->at(i)->set_limit(states);
	}
	enfa->transition->push_back(vector<int_set*>());
	enfa->transition->push_back(vector<int_set*>());
	for (int j = 0; j < imput; j++) {
		enfa->transition->at(enfa_states).push_back(new int_set(states));
		enfa->transition->at(enfa_states + 1).push_back(new int_set(states));
	}
	enfa->e_links->push_back(new int_set(states));
	enfa->e_links->push_back(new int_set(states));
	
	enfa->e_links->at(enfa->final_states->get())->insert(states - 1);
	enfa->e_links->at(enfa->final_states->get())->insert(enfa->initial_state);
	enfa->e_links->at(states - 2)->insert(enfa->initial_state);
	enfa->e_links->at(states - 2)->insert(states - 1);

	enfa->set_initial_state(states - 2);
	enfa->final_states->clear();
	enfa->final_states->set_limit(states);
	enfa->final_states->insert(states - 1);
	
	enfa->clear_metadata();

	return enfa;
}

string kleene_regex::to_string() const {
	return parenthesis ? '(' + reg1->to_string() + ")*" : reg1->to_string() + '*';
}

/*
#include <chrono>

void kleene_stuff() {
	vector<int> list;
	auto one = std::chrono::steady_clock::now();
	for (int n = 1; n < 268435456; n++) {
		list.clear();
		list.push_back(0);
		int i = n;
		while (i > 0) {
			if (i%2 == 0) {
				list.back()++;
			} else {
				list.push_back(0);
			}
			i = i >> 1;
		}
		list.pop_back();
		//print_vec(&list);
	}
	vector<int> out;
	auto two = std::chrono::steady_clock::now();
	for (int n = 1; n < 268435456; n++) {
		int i = n;
		out.clear();
		while (i > 0) {
			int zeros = __builtin_ctzll(i); 
			out.push_back(zeros);
			
			i >>= (zeros + 1);
		}
		//print_vec(&out);
	}
	auto three = std::chrono::steady_clock::now();
	
	std::chrono::duration<double> elapsed_seconds = two - one;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::chrono::duration<double> elapsed_seconds2 = three - two;
    std::cout << "Elapsed time: " << elapsed_seconds2.count() << "s" << std::endl;
	
	output:
	Elapsed time: 256.941s
	Elapsed time: 60.3s
}
*/

#endif