#include <iostream>
#include <vector>
#include "automaton.h"
#include "DFA.h"
#include "NFA.h"
#include "eNFA.h"
#include "DPDA.h"
#include "NPDA.h"
#include "TM.h"
#include "NTM.h"
#include "grammar.h"
#include "regex/regex.h"
#include "regex/empty_regex.h"
#include "regex/eps_regex.h"
#include "regex/lit_regex.h"
#include "regex/alter_regex.h"
#include "regex/concat_regex.h"
#include "regex/kleene_regex.h"
#include "CFG.h"
#include "UG.h"

using namespace std;

/* TODO:

- add many metadata
- CYK algorithm
- CFG to GNF
 - GNF to NDPA
 - NDPA to GNF?
- CFG to FNF (Floyd Normal Form)
- make as many pointers as possible static
- RG
 - RG to NFA
 - NFA to RG
- regex to eNFA
 - make eNFA friend for set_state_count();
 - regex saves the eNFA
- DFA to regex
- parse regex
- UG to NTM
- NTM to TM
- TM to UG
- properly comment everything
- tests
*/

void print_vec(vector<int>* p) {
	if (p != nullptr) {
		for (int i : *p) {
			cout << '|' << i;
		}
		cout << '|';
	}
	cout << endl;
}

void example_eNFA2NFA() {
	// L = (0(0|1))*
	
	int_set* finals = new int_set(3,{2});
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>
	{{new int_set(3,{1}),new int_set(3,{})},
	 {new int_set(3,{0}),new int_set(3,{0})},
	 {new int_set(3,{}),new int_set(3,{})}};
	vector<int_set*>* e_links = new vector<int_set*>
	{new int_set(3,{2}),new int_set(3,{}),new int_set(3,{})};
	eNFA* enfa = new eNFA(eNFA::safe_eNFA(2,3,0,finals,transition,e_links));
	enfa->print();
	
	NFA* nfa = enfa->equivalent_NFA();
	nfa->print();
}

void example_NFA2DFA() {
	// L = ((0|1)*1(0|1))*
	
	int_set* finals = new int_set(3,{0});
	vector<vector<int_set*>>* transition = new vector<vector<int_set*>>
	{{new int_set(3,{1}),new int_set(3,{1,2})},
	 {new int_set(3,{1}),new int_set(3,{1,2})},
	 {new int_set(3,{0}),new int_set(3,{0})}};
	NFA* nfa = new NFA(NFA::safe_NFA(2,3,0,finals,transition));
	
	DFA* dfa1 = nfa->equivalent_DFA();
	dfa1->print(); // 8 states
	dfa1->remove_unreachables();
	dfa1->print(); // 5 states
	DFA* dfa2 = nfa->equivalent_connex_DFA();
	dfa2->print(); // 5 states, equal to dfa1 except for states order
	dfa2->minimize();
	dfa2->print(); // 4 states
}

void example_minimize_DFA() {
	int_set* finals;
	vector<vector<int>>* transition;
	DFA* a;
	
	finals = new int_set(6,{});
	transition = new vector<vector<int>>{{0,1},{2,3},{0,4},{5,3},{2,3},{0,4}};
	a = new DFA(DFA::safe_DFA(2,6,0,finals,transition));
	cout << "1: " << a->minimize() << endl;
	a->print();
	
	finals = new int_set(6,{0,1,2,3,4,5});
	transition = new vector<vector<int>>{{0,1},{2,3},{0,4},{5,3},{2,3},{0,4}};
	a = new DFA(DFA::safe_DFA(2,6,0,finals,transition));
	cout << "2: " << a->minimize() << endl;
	a->print();
	
	finals = new int_set(6,{3,4,5});
	transition = new vector<vector<int>>{{0,1},{2,3},{0,4},{5,3},{2,3},{0,4}};
	a = new DFA(DFA::safe_DFA(2,6,0,finals,transition));
	cout << "3: " << a->minimize() << endl;
	a->print();
	
	finals = new int_set(6,{3});
	transition = new vector<vector<int>>{{0,1},{2,3},{0,4},{5,3},{2,3},{0,4}};
	a = new DFA(DFA::safe_DFA(2,6,0,finals,transition));
	cout << "4: " << a->minimize() << endl;
	a->print();
	
	finals = new int_set(3,{2});
	transition = new vector<vector<int>>{{0,1},{1,0},{0,0}};
	a = new DFA(DFA::safe_DFA(2,3,0,finals,transition));
	cout << "5: " << a->minimize() << endl;
	a->print();	
}

void example_run() {
	// L = (0|1)*(11|101|110)
	
	int_set* finals1 = new int_set(6,{3,4,5});
	vector<vector<int>>* transition1 = new vector<vector<int>>{{0,1},{2,3},{0,4},{5,3},{2,3},{0,4}};
	DFA* a1 = new DFA(DFA::safe_DFA(2,6,0,finals1,transition1));
	
	int_set* finals2 = new int_set(5,{3,4});
	vector<vector<int_set*>>* transition2 = new vector<vector<int_set*>>
	{{new int_set(5,{0}),new int_set(5,{0,1})},
	 {new int_set(5,{2}),new int_set(5,{3})},
	 {new int_set(5,{}),new int_set(5,{4})},
	 {new int_set(5,{4}),new int_set(5,{})},
	 {new int_set(5,{}),new int_set(5,{})}};
	NFA* a2 = new NFA(NFA::safe_NFA(2,5,0,finals2,transition2));
	
	int_set* finals3 = new int_set(10,{9});
	vector<vector<int_set*>>* transition3 = new vector<vector<int_set*>>
	{{new int_set(10,{0}),new int_set(10,{0})},
	 {new int_set(10,{}),new int_set(10,{2})},
	 {new int_set(10,{}),new int_set(10,{9})},
	 {new int_set(10,{}),new int_set(10,{4})},
	 {new int_set(10,{5}),new int_set(10,{})},
	 {new int_set(10,{}),new int_set(10,{9})},
	 {new int_set(10,{}),new int_set(10,{7})},
	 {new int_set(10,{}),new int_set(10,{8})},
	 {new int_set(10,{9}),new int_set(10,{})},
	 {new int_set(10,{}),new int_set(10,{})}};
	vector<int_set*>* e_links3 = new vector<int_set*>
	{new int_set(10,{1,3,6}),new int_set(10,{}),new int_set(10,{}),new int_set(10,{}),new int_set(10,{}),
	new int_set(10,{}),new int_set(10,{}),new int_set(10,{}),new int_set(10,{}),new int_set(10,{})};
	eNFA* a3 = new eNFA(eNFA::safe_eNFA(2,10,0,finals3,transition3,e_links3));

	int_set* finals4 = new int_set(2,{1});
	vector<vector<DPDA_oper>>* transition4 = new vector<vector<DPDA_oper>>
	{{DPDA_oper(new vector<tuple<int,vector<int>>>{{0,{0}},{0,{1}}}),
	  DPDA_oper(new vector<tuple<int,vector<int>>>{{0,{2}},{1,{0}}}),
	  DPDA_oper(new vector<tuple<int,vector<int>>>{{0,{0}},{1,{1}}})},
	 {DPDA_oper(new vector<tuple<int,vector<int>>>{{1,{2}},{1,{0}}}),
	  DPDA_oper(new vector<tuple<int,vector<int>>>{{0,{2}},{1,{0}}}),
	  DPDA_oper(new vector<tuple<int,vector<int>>>{{0,{0}},{1,{1}}})}};
	DPDA* a4 = new DPDA(DPDA::safe_DPDA(false, 2, 3, 2, 0, 0, finals4, transition4, new function<int(int)>(lineal(0, 1))));
	
	int_set* finals5 = new int_set(3,{2});
	vector<vector<vector<set<tuple<int,vector<int>>>>>>* transition5 = new vector<vector<vector<set<tuple<int,vector<int>>>>>>
	{{{{{0,{0,0}}},{{0,{0,1}},{1,{}}}},{{{0,{1,0}},{1,{}}},{{0,{1,1}},{2,{}}}}},{{{},{}},{{},{}}},{{{},{}},{{},{}}}};
	vector<vector<set<tuple<int,vector<int>>>>>* e_links5 = new vector<vector<set<tuple<int,vector<int>>>>>
	{{{},{}},{{},{{2,{}}}},{{},{}}};
	NPDA* a5 = new NPDA(NPDA::safe_NPDA(false, 2, 2, 3, 0, 0, finals5, transition5, e_links5, new function<int(int)>(lineal(0, 1))));
	
	vector<vector<tuple<int,int,bool>>>* transition6 = new vector<vector<tuple<int,int,bool>>>
	{{{0,0,1},{0,1,1},{1,2,0}},
	 {{2,0,0},{1,1,0},{3,2,1}},
	 {{3,0,1},{2,1,0},{3,2,1}},
	 {{3,0,1},{4,1,1},{6,2,1}},
	 {{4,0,1},{5,1,1},{6,2,1}}};
	TM* a6 = new TM(TM::safe_TM(2,3,2,5,0,5,transition6));
	
	vector<vector<set<tuple<int,int,bool>>>>* transition7 = new vector<vector<set<tuple<int,int,bool>>>>
	{{{{0,0,1}},{{0,1,1},{1,1,1}},{}},
	 {{{2,0,1}},{{3,1,1}},{}},
	 {{},{{4,1,1}},{}},
	 {{{4,0,1}},{},{{5,2,0}}},
	 {{},{},{{5,2,0}}}};
	NTM* a7 = new NTM(NTM::safe_NTM(2,3,2,5,0,5,transition7));
	
	vector<vector<int>> words = {{},{0},{1},{1,1},{1,1,0},{0,0,1},{0,1,0,1},{1,0,1,1},{1,1,0,0},{1,0,1,0},{0,0,0,0,0},{1,1,1,1,1},{1,1,0,1,1}};
	vector<automaton*> atms{a1,a2,a3,a4,a5,a6,a7}; 
	for (automaton* atm : atms) {
		for (vector<int> word : words) {
			cout << atm->run(new vector<int>(word));
		}
		cout << endl;
	}
}

void exapmle_regex() {
	lit_regex* zero = new lit_regex(2,0);
	lit_regex* one = new lit_regex(2,1);
	concat_regex* two = new concat_regex(2,one,zero);
	kleene_regex* train  = new kleene_regex(2,two);
	
	cout << zero->to_string() << endl;
	cout << one->to_string() << endl;
	cout << two->to_string() << endl;
	cout << train->to_string() << endl << endl;
	
	for (int i = 0; i < 10; i++) {
		vector<int>* word = train->generate(i);
		print_vec(word);
		delete word;
	}
	
	alter_regex* both = new alter_regex(2,zero,one);
	kleene_regex* all  = new kleene_regex(2,both);
	concat_regex* start1 = new concat_regex(2,one,all);
	alter_regex* base2 = new alter_regex(2,zero,start1);

	cout << endl;
	cout << both->to_string() << endl;
	cout << all->to_string() << endl;
	cout << start1->to_string() << endl;
	cout << base2->to_string() << endl << endl;
	
	for (int i = 0; i < 10; i++) {
		vector<int>* word = base2->generate(i);
		print_vec(word);
		delete word;
	}
	
	concat_regex* end1 = new concat_regex(2,all,one);
	concat_regex* preend1 = new concat_regex(2,end1,both);
	kleene_regex* lang = new kleene_regex(2,preend1);
	
	cout << endl;
	cout << end1->to_string() << endl;
	cout << preend1->to_string() << endl;
	cout << lang->to_string() << endl << endl;
	
	for (int i = 0; i < 10; i++) {
		vector<int>* word = lang->generate(i);
		print_vec(word);
		delete word;
	}
}

void exapmle_CFG() {
	vector<vector<vector<int>>>* rules = new vector<vector<vector<int>>>{{{2,2},{0,2,1},{}}};
	CFG* cfg = new CFG(CFG::safe_CFG(2,1,2,rules));
	
	for (int i = 0; i < 220; i++) {
		cout << "seed:" << i << ' ';
		vector<int>* word = cfg->generate(i);
		print_vec(word);
		delete word;
	}	
}

void example_UG() {
	vector<tuple<vector<int>,vector<int>>>* rules = new vector<tuple<vector<int>,vector<int>>>
	{{{3},{}},{{3},{0,3,1,2}},{{0,1},{1,0}},{{0,2},{2,0}},{{1,2},{2,1}},{{2,1},{1,2}}};
	UG* ug = new UG(UG::safe_UG(3,1,3,rules));
	
	vector<int>* word = ug->generate(33);
	print_vec(word);
	
	for (int i = 0; i < 40; i++) {
		cout << "seed:" << i << ' ';
		vector<int>* word = ug->generate(i);
		print_vec(word);
		delete word;
	}	
}

void example_CFG_simplify() {
	vector<vector<vector<int>>>* rules = new vector<vector<vector<int>>>
	{{{4},{6},{9,8},{10}},
	 {{7,0,3},{7,0,7}},
	 {{7,1,4},{7,1,7}},
	 {{0,1},{3,4},{}},
	 {{8},{8,0}},
	 {{0,7,1,7},{1,7,0,7},{7},{}},
	 {{6,6}},
	 {{1,3},{0,4},{7}},
	 {{3},{2}}};
	CFG* cfg = new CFG(CFG::safe_CFG(2,9,2,rules));
	cfg->print();
	
	cout << endl;
	cfg->remove_eps_productions();
	cfg->print();
	
	cout << endl;
	cfg->remove_unit_productions();
	cfg->print();
	
	cout << endl;
	cfg->remove_repeated_rules();
	cfg->print();
	
	cout << endl;
	cfg->remove_non_positives();
	cfg->print();
	
	cout << endl;
	cfg->remove_unreachables();
	cfg->print();
	
/*
S -> B | D | GF | H
A -> EaA | EaE
B -> EbB | EbE
C -> ab | AB | eps
D -> F | Fa
E -> aEbE | bEaE | E | eps
F -> DD
G -> bA | aB | E
H -> A | S

S -> 0 | 0A | 0E | 1 | 1B | E1E | E1B | E0 | E0A | E0E | E1
A -> 0 | 0A | 0E | E0 | E0A | E0E
B -> 1 | 1B | 1E | E1 | E1B | E1E
E -> 01 | 01E | 0E1 | 0E1E | 10 | 10E | 1E0 | 1E0E

original idea:
S -> A | B
A -> EaA | EaE
B -> EbB | EbE
E -> aEbE | bEaE | eps
*/
}

void example_CFG_Chomsky() { 
	vector<vector<vector<int>>>* rules = new vector<vector<vector<int>>>
	{{{4},{6},{9,8},{10}},
	 {{7,0,3},{7,0,7}},
	 {{7,1,4},{7,1,7}},
	 {{0,1},{3,4},{}},
	 {{8},{8,0}},
	 {{0,7,1,7},{1,7,0,7},{7},{}},
	 {{6,6}},
	 {{1,3},{0,4},{7}},
	 {{3},{2}}};
	CFG* cfg = new CFG(CFG::safe_CFG(2,9,2,rules));
	cfg->print();
	
	cout << endl;
	cfg->convert_to_CNF();
	cfg->print();
	
	cout << endl;
	cfg->remove_useless();
	cfg->print();
}

void example_regex2eNFA(){
	eNFA * enfa;
	NFA * nfa;
	DFA * dfa;
	
	lit_regex* zero = new lit_regex(2,0);
	lit_regex* one = new lit_regex(2,1);
	concat_regex* two = new concat_regex(2,one,zero);
	kleene_regex* train  = new kleene_regex(2,two);
	alter_regex* both = new alter_regex(2,zero,one);
	kleene_regex* all  = new kleene_regex(2,both);
	concat_regex* start1 = new concat_regex(2,one,all);
	alter_regex* base2 = new alter_regex(2,zero,start1);
	concat_regex* end1 = new concat_regex(2,all,one);
	concat_regex* preend1 = new concat_regex(2,end1,both);
	kleene_regex* lang = new kleene_regex(2,preend1);
	
	vector<regex*> reg_vec = {zero, one, two, train, both, all, start1, base2, end1, preend1, lang};
	vector<regex*> reg_test = {preend1};
	for (regex* reg : reg_vec) {
		cout << endl << reg->to_string() << endl;
		enfa = reg->equivalent_eNFA();
		nfa = enfa->equivalent_NFA();
		dfa = nfa->equivalent_connex_DFA();
		dfa->minimize();
		dfa->print();
	}
	
}

int main () {

	return 0;
}