#ifndef concat_regex_cpp
#define concat_regex_cpp

#include <cmath>
#include "regex/concat_regex.h"
#include "regex/alter_regex.h"

using namespace std;

concat_regex::concat_regex(int imput, regex* reg1, regex* reg2) :
	regex(imput), reg1(reg1), reg2(reg2) {
	bool finite1 = reg1->get_finite();
	bool finite2 = reg2->get_finite();
	if (!finite1 && !finite2) {
		finite = false;
		generator = [reg1,reg2](int seed) {
			int f = floor(sqrt(seed));
			int diff = seed - f*f;
			vector<int>* s1 = reg1->generate(diff < f ? f : diff - f);
			vector<int>* s2 = reg2->generate(diff < f ? diff : f);
			s1->insert(s1->end(), s2->begin(), s2->end());
			delete s2;
			return s1;
		};
	} else {
		int limit1 = reg1->limit;
		int limit2 = reg2->limit;
		if ((finite1 && limit1 == 0) || (finite2 && limit2 == 0)) {
			finite = true;
			limit = 0;
			generator = [](int seed) {
				return nullptr;
			};
		} else if (finite1 && finite2) {
			finite = true;
			limit = limit1 * limit2;
			generator = [reg1,reg2,limit2](int seed) {
				vector<int>* s1 = reg1->generate(seed / limit2);
				vector<int>* s2 = reg2->generate(seed % limit2);
				s1->insert(s1->end(), s2->begin(), s2->end());
				delete s2;
				return s1;
			};
		} else if (finite2) {
			finite = false;
			generator = [reg1,reg2,limit2](int seed) {
				vector<int>* s1 = reg1->generate(seed / limit2);
				vector<int>* s2 = reg2->generate(seed % limit2);
				s1->insert(s1->end(), s2->begin(), s2->end());
				delete s2;
				return s1;
			};
		} else {
			finite = false;
			generator = [reg1,reg2,limit1](int seed) {
				vector<int>* s1 = reg1->generate(seed % limit1);
				vector<int>* s2 = reg2->generate(seed / limit1);
				s1->insert(s1->end(), s2->begin(), s2->end());
				delete s2;
				return s1;
			};
		}
	}
	alter_regex* alter_reg1 = dynamic_cast<alter_regex*>(reg1);
	parenthesis1 = (alter_reg1 != nullptr);
	alter_regex* alter_reg2 = dynamic_cast<alter_regex*>(reg2);
	parenthesis2 = (alter_reg2 != nullptr);
}

vector<int>* concat_regex::generate(int seed) {
	return generator(seed);
}

eNFA* concat_regex::to_eNFA() const {
	eNFA* enfas[2] = {reg1->to_eNFA(), reg2->to_eNFA()};
	eNFA* enfa1 = enfas[0]; 
	eNFA* enfa2 = enfas[1];
	
	for (int i = 0; i < 2; ++i) {
		if (enfas[i]->get_state_count() == 1) {
			if (enfas[i]->get_final_states()->elem(0)) { // eps_regex
				delete enfas[i];
				return enfas[1 - i]; 
			} 
			else { // empty_regex
				delete enfas[1 - i];
				return enfas[i]; 
			}
		}
	}
	int enfa1_states = enfa1->get_state_count();
	int enfa2_states = enfa2->get_state_count();
	int imput = get_imput();
	int states = enfa1_states + enfa2_states;
	int_set* finals = new int_set(states);
	finals->insert(enfa2->get_final_states()->get() + enfa1_states);
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>{};
	for (int i = 0; i < enfa1_states; i++) {
		transition->push_back(vector<int_set*>());
		for (int j = 0; j < imput; j++) {
			int_set* to_add = new int_set(*(enfa1->get_transition()->at(i).at(j)),false);
			to_add->set_limit(states);
			transition->at(i).push_back(to_add);
		}
	}
	int i2 = enfa1_states;
	for (int i = 0; i < enfa2_states; i++) {
		transition->push_back(vector<int_set*>());
		for (int j = 0; j < imput; j++) {
			int_set* to_add = new int_set(*(enfa2->get_transition()->at(i).at(j)), false, enfa1_states);
			transition->at(i2).push_back(to_add);
		}
		i2++;
	}
	vector<int_set*>* e_links = new vector<int_set*>{};
	for (int i = 0; i < enfa1_states; i++) {
		int_set* to_add = new int_set(*(enfa1->get_e_links()->at(i)),false);
		to_add->set_limit(states);
		e_links->push_back(to_add);
	}
	i2 = enfa1_states;
	for (int i = 0; i < enfa2_states; i++) {
		int_set* to_add = new int_set(*(enfa2->get_e_links()->at(i)), false, enfa1_states);
		e_links->push_back(to_add);
		i2++;
	}
	e_links->at(enfa1->get_final_states()->get())->insert(enfa1_states + enfa2->get_initial_state());
	delete enfa1;
	delete enfa2;
	return new eNFA(imput, states, enfa1->get_initial_state(), finals, transition, e_links);
}

string concat_regex::to_string() const {
	return (parenthesis1 ? '(' + reg1->to_string() + ')': reg1->to_string()) + '.' + (parenthesis2 ? '(' + reg2->to_string() + ')': reg2->to_string());
}

/*
#include <cmath>
#include <chrono>
#include <immintrin.h>
#include <cstdint>

__attribute__((target("bmi2")))
void interleave() {
	pair<int,int> p;
	auto one = std::chrono::steady_clock::now();
	for (int x = 0; x < 268435456; x++) {
		int f = floor(sqrt(x));
		int diff = x - f*f;
		if (diff < f) {
			p = {f, diff};
		} else {
			p = {diff-f, f};
		}
	}
	auto two = std::chrono::steady_clock::now();
	for (int n = 0; n < 268435456; n++) {
		uint32_t x = _pext_u64(n, 0x5555555555555555); 
		uint32_t y = _pext_u64(n, 0xAAAAAAAAAAAAAAAA);
		p = {x,y};
	}
	auto three = std::chrono::steady_clock::now();
	
	std::chrono::duration<double> elapsed_seconds = two - one;
    std::cout << "Elapsed time: " << elapsed_seconds.count() << "s" << std::endl;
	std::chrono::duration<double> elapsed_seconds2 = three - two;
    std::cout << "Elapsed time: " << elapsed_seconds2.count() << "s" << std::endl;	
}
*/

#endif

