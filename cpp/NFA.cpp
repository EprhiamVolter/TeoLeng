#ifndef NFA_cpp
#define NFA_cpp

#include <iostream>
#include "NFA.h"
#include "regex/empty_regex.h"
#include "regex/eps_regex.h"
#include "regex/lit_regex.h"
#include "regex/alter_regex.h"
#include "regex/concat_regex.h"
#include "regex/kleene_regex.h"

using namespace std;

NFA::NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition) : 
	automaton(imput), state_count(states), initial_state(initial), final_states(finals), transition(transition), my_regex(nullptr) {
}

NFA NFA::safe_NFA(int imput, int states, int initial, int_set* finals, vector<vector<int_set*>>* transition) {
	if (imput < 0) throw 1;
	if (states < 1) throw 2;
	if (initial < 0) throw 3;
	if (initial >= states) throw 4;
	if (finals == nullptr) throw 5;
	if (finals->get_limit() != states) throw 6;
	if (transition == nullptr) throw 7;
	if (transition->size() != states) throw 8;
	for (int i = 0; i < states; i++) {
		vector<int_set*> row = transition->at(i);
		if (row.size() != imput) throw 9;
		for (int j = 0; j < imput; j++) {
			if (row.at(j) == nullptr) throw 10;
			if (row.at(j)->get_limit() != states) throw 11;
		}
	}
	return NFA(imput, states, initial, finals, transition);
}

bool NFA::run(vector<int>* word) {
	int_set states(state_count,{initial_state});
	for (int i : *word) {
		int_set next(state_count);
		for (int state : states) {
			next.append(transition->at(state).at(i));
		}
		states = next;
	}
	return states.intersect(final_states);
}

// constructs the equivalent DFA with 2^n states (likely most unreachable)
DFA* NFA::equivalent_DFA() const {
	if (state_count > 30) throw 1; // 1 << state_count causes overflow
	int DFA_states = 1 << state_count;
	int DFA_initial = 1 << initial_state;
	int_set* iterator = new int_set(state_count);
	int_set* DFA_finals = new int_set(DFA_states);
	vector<vector<int>>* DFA_transition = new vector<vector<int>>(DFA_states,vector<int>());
	compute_DFA(state_count - 1, 0, iterator, DFA_finals, DFA_transition);
	return new DFA(get_imput(), DFA_states, DFA_initial, DFA_finals, DFA_transition);
}

void NFA::compute_DFA(int x, int pos, int_set* it, int_set* fin, vector<vector<int>>* tran) const {
	if (x == -1) {
		if (it->intersect(final_states)) {
			fin->insert(pos);
		}
		tran->at(pos).reserve(get_imput());
		for (int i = 0; i < get_imput(); i++) {
			int_set dest(state_count);
			for (int j : *it) {
				dest.append(transition->at(j).at(i));
			}
			tran->at(pos).push_back(dest.get_code());
		}
	} else {
		compute_DFA(x - 1, pos,it, fin, tran);
		it->insert(x);
		compute_DFA(x - 1, pos + (1 << x), it, fin, tran);
		it->remove(x);
	}
}

// constructs equivalent_DFA()->remove_unreachables() without processing unreachable states in the first place.
DFA* NFA::equivalent_connex_DFA() const { 
	int max_states = 1 << state_count;
	vector<int_set*> equiv(1, new int_set(state_count,{initial_state}));
	vector<int> map(max_states,-1);
	map.at(1 << initial_state) = 0;
	int process = 0;
	int reserved = 1;
	vector<vector<int>>* DFA_transition = new vector<vector<int>>();
	while (process < reserved) {
		DFA_transition->push_back(vector<int>());
		DFA_transition->at(process).reserve(get_imput());
		for (int i = 0; i < get_imput(); i++) {
			int_set* dest = new int_set(state_count);
			for (int j : *(equiv.at(process))) {
				dest->append(transition->at(j).at(i));
			}
			int dest_pos = map.at(dest->get_code());
			if (dest_pos == -1) {
				map.at(dest->get_code()) = reserved;
				dest_pos = reserved;
				reserved++;
				equiv.push_back(dest);
			} else {
				delete dest;
			}
			DFA_transition->at(process).push_back(dest_pos);
		}
		process++;
	}
	int_set* DFA_finals = new int_set(reserved);
	for (int i = 0; i < reserved; i++) {
		if (equiv.at(i)->intersect(final_states)) {
			DFA_finals->insert(i);
		}
	}
	return new DFA(get_imput(), reserved, 0, DFA_finals, DFA_transition);
}

enum struct reg_type {
	Var, Eps, Lit, Alter, Concat, Kleene
};

struct reg_tree {
	reg_type type;
	int num;
	reg_tree* reg1;
	reg_tree* reg2;
};

reg_tree var_reg(int var) {
	reg_tree ret;
	ret.type = reg_type::Var;
	ret.num = var;
	ret.reg1 = nullptr;
	ret.reg2 = nullptr;
	return ret;
}

reg_tree eps_reg() {
	reg_tree ret;
	ret.type = reg_type::Eps;
	ret.reg1 = nullptr;
	ret.reg2 = nullptr;
	return ret;
}

reg_tree lit_reg(int lit) {
	reg_tree ret;
	ret.type = reg_type::Lit;
	ret.num = lit;
	ret.reg1 = nullptr;
	ret.reg2 = nullptr;
	return ret;
}

reg_tree alter_reg(reg_tree* reg1, reg_tree* reg2) {
	reg_tree ret;
	ret.type = reg_type::Alter;
	ret.reg1 = reg1;
	ret.reg2 = reg2;
	return ret;
}

reg_tree concat_reg(reg_tree* reg1, reg_tree* reg2) {
	reg_tree ret;
	ret.type = reg_type::Concat;
	ret.reg1 = reg1;
	ret.reg2 = reg2;
	return ret;
}

reg_tree kleene_reg(reg_tree* reg1) {
	reg_tree ret;
	ret.type = reg_type::Kleene;
	ret.reg1 = reg1;
	ret.reg2 = nullptr;
	return ret;
}

void replace_vars(reg_tree* &tree, int i, bool if_less, vector<vector<reg_tree*>>* equations) {
	if (tree != nullptr) {
		if (tree->type == reg_type::Var) {
			if (if_less ? tree->num < i : tree->num > i) {
				tree = equations->at(tree->num).back();
			}
		} else {
			replace_vars(tree->reg1, i, if_less, equations);
			replace_vars(tree->reg2, i, if_less, equations);
		}
	}
}

regex* reg_tree2regex(int imput, reg_tree* tree) {
	regex* ret;
	//cout << "t1\n";
	//cout << "Address: " << tree << endl;
	if (tree == nullptr) {
		//cout << "t2\n";
		ret = new empty_regex(imput);
	} else {
		//cout << "t3-1\n";
		regex* reg1;
		regex* reg2;
		//cout << "t3\n";
		//cout << "t4" << (tree == nullptr) << endl;
		//cout << "type :" << int(tree->type) << endl;
		switch (tree->type) {
			case reg_type::Eps:
				//cout << "s1\n";
				ret = new eps_regex(imput);
				break;
			case reg_type::Lit:
				//cout << "s2\n";
				ret = new lit_regex(imput, tree->num);
				break;
			case reg_type::Alter:
				//cout << "s3\n";
				reg1 = reg_tree2regex(imput, tree->reg1);
				//cout << "s3-2\n";
				reg2 = reg_tree2regex(imput, tree->reg2);
				//cout << "s3-3\n";
				ret = new alter_regex(imput, reg1, reg2);
				break;
			case reg_type::Concat:
				//cout << "s4 " << (tree == nullptr) << endl;
				//cout << "s4-1 " << (tree->reg1 == nullptr) << endl;
				reg1 = reg_tree2regex(imput, tree->reg1);
				//cout << "s4-2\n";
				reg2 = reg_tree2regex(imput, tree->reg2);
				//cout << "s4-2\n";
				ret = new concat_regex(imput, reg1, reg2);
				break;
			case reg_type::Kleene:
				//cout << "s5\n";
				reg1 = reg_tree2regex(imput, tree->reg1);
				//cout << "s5-1\n";
				ret = new kleene_regex(imput, reg1);
				break;
			default:
				throw 1;
		}
		//cout << "t5\n";
	}
	return ret;
}

void print_reg_tree(reg_tree* tree) {
	if (tree != nullptr) {
	switch (tree->type) {
			case reg_type::Eps:
				cout << "e";
				break;
			case reg_type::Lit:
				cout << tree->num;
				break;
			case reg_type::Alter:
				cout << '(';
				print_reg_tree(tree->reg1);
				cout << '|';
				print_reg_tree(tree->reg2);
				cout << ')';
				break;
			case reg_type::Concat:
				print_reg_tree(tree->reg1);
				print_reg_tree(tree->reg2);
				break;
			case reg_type::Kleene:
				cout << '(';
				print_reg_tree(tree->reg1);
				cout << ")*";
				break;
			case reg_type::Var:
				cout << 'v' << tree->num;
				break;
		}
	}
}

void print_equations(vector<vector<reg_tree*>>* eq) {
	for (int i = 0; i < eq->size(); i++) {
		cout << i << ":\n";
		for (int j = 0; j < eq->at(i).size(); j++) {
			cout << ' ' << j << ':';
			print_reg_tree(eq->at(i).at(j));
			cout << endl;
		}
	}
}

regex* NFA::equivalent_regex() {
	if (my_regex == nullptr) {
		if (final_states->get_size() > 0) {
			vector<vector<reg_tree*>> equations(state_count, vector<reg_tree*>(state_count + 1, nullptr)); 
			// first axis is the variables number, its equivalent to the alter of all its reg_trees in its vector, each precedeed by the var of second axis
			// except in state_count. Where the final substitution is constructed
			// it also has many repeated pointers. Handle with care
			for (int i = 0; i < state_count; i++) {
				for (int j = 0; j < get_imput(); j++) {
					for (int k : *(transition->at(i).at(j))) {
						reg_tree* new_lit = new reg_tree(lit_reg(j));
						if (equations.at(k).at(i) == nullptr) {
							reg_tree* new_var = new reg_tree(var_reg(i));
							reg_tree* new_concat = new reg_tree(concat_reg(new_var, new_lit));
							equations.at(k).at(i) = new_concat;
						} else {
							reg_tree* second = equations.at(k).at(i)->reg2;
							reg_tree* new_alter = new reg_tree(alter_reg(new_lit, second));
							equations.at(k).at(i)->reg2 = new_alter;
						}
					}
				}
			}
			equations.at(initial_state).at(state_count) = new reg_tree(eps_reg());
			cout << "initial\n";
			print_equations(&equations);
			for (int i = 0; i < state_count; i++) {
				cout << "stair step " << i << endl;
				for (int j = 0; j < i; j++) {
					replace_vars(equations.at(i).at(j), i, true, &equations);
				}
				cout << "replaced vars " << endl;
				print_equations(&equations);
				for (int j = 0; j < state_count; j++) {
					if (j != i && equations.at(i).at(j) != nullptr) {
						if (equations.at(i).at(state_count) != nullptr) {
							equations.at(i).at(state_count) = new reg_tree(alter_reg(equations.at(i).at(j), equations.at(i).at(state_count)));
						} else {
							equations.at(i).at(state_count) = equations.at(i).at(j);
						}
					}
				}
				if (equations.at(i).at(i) != nullptr) {
					reg_tree* new_kleene = new reg_tree(kleene_reg(equations.at(i).at(i)->reg2));
					if (equations.at(i).at(state_count) != nullptr) {
						equations.at(i).at(state_count) = new reg_tree(concat_reg(equations.at(i).at(state_count), new_kleene));
					} else {
						equations.at(i).at(state_count) = new_kleene;
					}
				}
				cout << "finished step " << i << endl;
				print_equations(&equations);
			}
			cout << "after stair\n";
			print_equations(&equations);
			return nullptr;
			for (int i = state_count - 1; i >= 0; i--) {
				replace_vars(equations.at(i).at(state_count), i, false, &equations);
			}
			cout << "regresive var replace\n";
			print_equations(&equations);
			int fin = final_states->get();
			final_states->remove(fin);
			cout << "fin is " << fin << endl;
			my_regex = reg_tree2regex(get_imput(), equations.at(fin).at(state_count));
			cout << my_regex->to_string() << endl;
			for (int f : *final_states) {
				cout << "f is " << f << endl;
				my_regex = new alter_regex(get_imput(), my_regex, reg_tree2regex(get_imput(), equations.at(f).at(state_count)));
				cout << my_regex->to_string() << endl;
			}
			final_states->insert(fin);
		} else {
			my_regex = new empty_regex(get_imput());
		} 
	}
	return my_regex;
}

void NFA::print() const {
	cout <<	"imput_symbols:" << get_imput() << 
			" state_count:" << state_count << 
			" initial_state:" << initial_state << 
			" final_states:" << *final_states << endl;
	for (vector<int_set*> row : *transition) {
		for (int_set* dest : row) {
			cout << *dest << "|";
		}
		cout << endl;
	}
}

#endif