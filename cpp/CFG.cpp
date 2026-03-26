#ifndef CFG_cpp
#define CFG_cpp

#include <list>
#include <set>
#include "CFG.h"

using namespace std;

CFG::CFG(int imput, int vars, int initial, vector<vector<vector<int>>>* rules) :
	grammar(imput), var_count(vars), initial_var(initial), rules(rules), 
	nullables(int_set(vars)), positives(int_set(vars)), reachables(int_set(vars)), usefuls(int_set(vars)), terminal_vars(vector<int>()),
	nullables_valid(false), positives_valid(false), reachables_valid(false), usefuls_valid(false), terminal_vars_valid(false),
	initial_rereachable(-1), has_unit_productions(-1), is_CNF(-1), is_GNF(-1) {
}

CFG CFG::safe_CFG(int imput, int vars, int initial, vector<vector<vector<int>>>* rules) {
	if (imput < 0) throw 1;
	if (vars < 1) throw 2;
	if (initial < imput) throw 3;
	int ve = imput + vars;
	if (initial >= ve) throw 4;
	if (rules == nullptr) throw 5;
	if (rules->size() != vars) throw 6;
	for (vector<vector<int>> rule_set : *rules) {
		for (vector<int> rule : rule_set) {
			for (int sym : rule) {
				if (sym < 0) throw 7;
				if (sym >= ve) throw 8;
			}
		}
	}
	return CFG(imput, vars, initial, rules);
}

// surjective function over accepted words, many cases of nullprt
vector<int>* CFG::generate(int seed) {
	list<int> word{initial_var};
	list<int>::iterator fst_var = word.begin();
	while (seed > 0 && fst_var != word.end()) {
		int sym = *fst_var;
		if (sym < get_imput()) {
			fst_var++;
		} else {
			int var = sym - get_imput();
			int var_rules = rules->at(var).size();
			if (var_rules == 0) {
				return nullptr;
			}
			vector<int>* rule = &rules->at(var).at(seed % var_rules);
			fst_var = word.erase(fst_var);
			fst_var = word.insert(fst_var, rule->begin(), rule->end());
			seed /= var_rules;
			if (var_rules == 1) {
				seed--;
			}
		}
	}
	while (fst_var != word.end()) {
		if (*fst_var >= get_imput()) {
			return nullptr;
		}
		fst_var++;
	}
	return new vector<int>(word.begin(), word.end());
}

int_set* CFG::get_nullables() {
	if (!nullables_valid) {
		nullables.clear();
		nullables.set_limit(var_count);
		bool nullables_changed = true;
		while (nullables_changed) {
			nullables_changed = false;
			for (int i = 0; i < var_count; i++) {
				if (!nullables.elem(i)) {
					int j_end = rules->at(i).size();
					for (int j = 0; j < j_end; j++) {
						int k_end = rules->at(i).at(j).size();
						int k;
						for (k = 0; k < k_end; k++) {
							int sym = rules->at(i).at(j).at(k);
							if (sym < get_imput() || !nullables.elem(sym - get_imput())) {
								k = k_end + 1;
							}
						}
						if (k == k_end) {
							nullables.insert(i);
							j = j_end;
							nullables_changed = true;
						}
					}
				}
			}
		}
		nullables_valid = true;
	}
	return &nullables;
}

int_set* CFG::get_positives() {
	if (!positives_valid) {
		positives.clear();
		positives.set_limit(var_count);
		bool positives_changed = true;
		int s = 0;
		while (positives_changed) {
			positives_changed = false;
			for (int i = 0; i < var_count; i++) {
				if (!positives.elem(i)) {
					int j_end = rules->at(i).size();
					for (int j = 0; j < j_end; j++) {
						int k_end = rules->at(i).at(j).size();
						int k = 0;
						for (k = 0; k < k_end; k++) {
							int sym = rules->at(i).at(j).at(k);
							if (sym >= get_imput() && !positives.elem(sym - get_imput())) {
								k = k_end + 1;
							}
						}
						if (k == k_end) {
							positives.insert(i);
							positives_changed = true;
							j = j_end;
						}
					}
				}
			}
			s++;
		}
		positives_valid = true;
	}
	return &positives;
}

int_set* CFG::get_reachables() {
	if (!reachables_valid) {
		reachables.clear();
		reachables.set_limit(var_count);
		int_set to_explore(var_count,{initial_var - get_imput()});
		while (to_explore.get_size() > 0) {
			int var = to_explore.get();
			to_explore.remove(var);
			reachables.insert(var);
			for (vector<int> production : rules->at(var)) {
				for (int sym : production) {
					if (sym >= get_imput() && !reachables.elem(sym - get_imput())) {
						to_explore.insert(sym - get_imput());
					}
				}
			}
		}
		reachables_valid = true;
	}
	return &reachables;
}

int_set* CFG::get_usefuls() {
	if (!usefuls_valid) {
		get_positives();
		if (positives.get_size() == var_count) {
			get_reachables();
			usefuls = int_set(reachables,false);
		} else {
			usefuls.clear();
			usefuls.set_limit(var_count);
			if (positives.elem(initial_var - get_imput())) {
				int_set to_explore(var_count,{initial_var - get_imput()});
				int_set to_add(var_count);
				while (to_explore.get_size() > 0) {
					int var = to_explore.get();
					to_explore.remove(var);
					usefuls.insert(var);
					for (vector<int> production : rules->at(var)) {
						int k = 0;
						int k_end = production.size();
						while (k < k_end) {
							int sym = production.at(k);
							if (sym >= get_imput() && !usefuls.elem(sym - get_imput())) {
								if (positives.elem(sym - get_imput())) {
									to_add.insert(sym - get_imput());
								} else {
									k = k_end + 1;
								}
							}
						}
						if (k == k_end) {
							to_explore.append(&to_add);
						}
						to_add.clear();
					}
				}
			}
		}
		usefuls_valid = true;
	}
	return &usefuls;
}

vector<int>* CFG::get_terminal_vars() {
	if (!terminal_vars_valid) {
		terminal_vars.clear();
		terminal_vars.assign(get_imput(),-1);
		for (int i = 0; i < var_count; i++) {
			if (rules->at(i).size() == 1 && rules->at(i).at(0).size() == 1 && rules->at(i).at(0).at(0) < get_imput()) {
				terminal_vars.at(rules->at(i).at(0).at(0)) = i;
			}
		}
		terminal_vars_valid = true;
	}
	return &terminal_vars;
}

bool CFG::get_initial_rereachable() {
	if (initial_rereachable == -1) {
		for (vector<vector<int>> row : *rules) {
			for (vector<int> production : row) {
				for (int sym : production) {
					if (sym == initial_var) {
						initial_rereachable = 1;
						return true;
					}
				}
			}
		}
		initial_rereachable == 0;
		return false;
	} 
	return initial_rereachable == 1;
}

bool CFG::get_has_nullables() {
	get_nullables();
	return nullables.get_size() > 0;
}

bool CFG::get_has_not_initial_nullables() {
	get_nullables();
	return nullables.get_size() > 1 || !nullables.elem(initial_var - get_imput());
}

bool CFG::get_has_unit_productions() {
	if (has_unit_productions == -1) {
		for (vector<vector<int>> row : *rules) {
			for (vector<int> production : row) {
				if (production.size() == 1 && production.at(0) >= get_imput()) {
					has_unit_productions == 1;
					return true;
				}
			}
		}
		has_unit_productions == 0;
		return false;		
	}
	return has_unit_productions == 1;
}

bool CFG::get_all_positive() {
	get_positives();
	return positives.get_size() == var_count;
}

bool CFG::get_all_reachable() {
	get_reachables();
	return reachables.get_size() == var_count;	
}

bool CFG::get_all_useful() {
	get_usefuls();
	return usefuls.get_size() == var_count;		
}

bool CFG::get_initial_positive() {
	get_positives();
	return positives.elem(initial_var - get_imput());
}

bool CFG::get_initial_nullable() {
	get_nullables();
	return nullables.elem(initial_var - get_imput());	
}

bool CFG::get_is_simple() {
	return !get_has_not_initial_nullables()  && !get_has_unit_productions() && get_all_useful();
}

bool CFG::get_is_CNF() {
	if (is_CNF == -1) {
		for (vector<vector<int>> row : *rules) {
			for (vector<int> production : row) {
				if ((production.size() > 2) || (production.size() == 0) ||
					(production.size() == 1 && production.at(0) >= get_imput()) || 
					(production.size() == 2 && (production.at(0) < get_imput() || production.at(1) < get_imput() ))) {
					is_CNF == 0;
					return false;
				}
			}
		}
		is_CNF == 1;
		return true;	
	}
	return is_CNF == 1; 
}

bool CFG::get_is_GNF() {
	if (is_GNF == -1) {
		for (vector<vector<int>> row : *rules) {
			for (vector<int> production : row) {
				int p_size = production.size();
				if (p_size == 0 || production.at(0) >= get_imput()) {
					is_GNF == 0;
					return false;
				}
				for (int i = 1; i < p_size; i++) {
					if (production.at(i) < get_imput()) {
						is_GNF == 0;
						return false;
					}
				}
			}
		}
		is_GNF == 1;
		return true;	
	}
	return is_GNF == 1; 
}

void CFG::compute_metadata() {
	get_nullables();
	get_positives();
	get_reachables();
	get_usefuls();
	get_terminal_vars();
	get_initial_rereachable();
	get_has_unit_productions();
	get_is_CNF();
	get_is_GNF();
}

void CFG::clear_metadata() {
	nullables_valid = positives_valid = reachables_valid = usefuls_valid = terminal_vars_valid = false;
	initial_rereachable = has_unit_productions = is_CNF = is_GNF = -1;
}

void CFG::remove_vars(int_set* vars) {
	if (vars->get_size() > 0) {
		if (vars->elem(initial_var - get_imput())) {
			rules->clear();
			rules->push_back(vector<vector<int>>());
			var_count = 1;
			initial_var = get_imput();
		} else {
			int * map = new int[var_count];
			int map_i = 0;
			for (int i = 0; i < var_count; i++) {
				if (!vars->elem(i)) {
					map[i] = map_i;
					map_i++;
				}
			}
			for (int i = 0; i < var_count; i++) {
				if (!vars->elem(i)) {
					map_i = map[i];
					rules->at(map_i) = rules->at(i);
					for (int j = 0; j < rules->at(map_i).size(); j++) {
						int rule_size = rules->at(map_i).at(j).size();
						for (int k = 0; k < rule_size; k++) {
							int sym = rules->at(map_i).at(j).at(k);
							if (sym >= get_imput()) {
								if (vars->elem(sym - get_imput())) {
									if (j + 1 < rules->at(map_i).size()) {
										vector<int> last_rule = rules->at(map_i).back();
										rules->at(map_i).at(j) = last_rule;
									}
									rules->at(map_i).pop_back();
									k = rule_size;
									j--;
								} else {
									rules->at(map_i).at(j).at(k) = map[sym - get_imput()] + get_imput();
								}
							}
						}
					}
				}
			}
			var_count -= vars->get_size();
			rules->resize(var_count);
			initial_var = map[initial_var];
			delete[] map;
		}
		nullables_valid = false;
		positives_valid = false;
		reachables_valid = false;
		usefuls_valid = false;
		terminal_vars_valid = false;
		initial_rereachable = (initial_rereachable == 0 ? 0 : -1);
		has_unit_productions = (has_unit_productions == 0 ? 0 : -1);
		is_CNF = (is_CNF == 1 ? 1 : -1);
		is_GNF = (is_GNF == 1 ? 1 : -1);	
	}
}

// removes all epsilon productions, except for S -> eps, with S = initial_var
// returns wheter there was S -> eps.
// if !keep_if_init, it removes that to (modifying the language!)
// if keep_if_init && new_init && initial_rereachable, if there was it removes it from S and creates a new initial_variable S2 -> S | eps
bool CFG::remove_eps_productions(bool keep_if_init, bool new_init) {
	bool ret = false;
	get_nullables();
	if (nullables.get_size() > 0) {
		if (nullables.elem(initial_var - get_imput())) {
			ret = true;
			if (keep_if_init && nullables.get_size() == 1 && !get_initial_rereachable()) {
				return ret;
			}
			if (keep_if_init && new_init && get_initial_rereachable()) {
				rules->push_back(vector<vector<int>>{{initial_var},{}});
				var_count++;
				nullables.set_limit(var_count, true);
				if (reachables_valid) reachables.set_limit(var_count, true);
				initial_rereachable = 0;
				has_unit_productions = 1;
				initial_var = get_imput() + var_count - 1;
			}
		}
		for (int i = 0; i < var_count; i++) {
			bool not_ini_i = (i + get_imput()) != initial_var;
			int j = 0;
			while (j < rules->at(i).size()) {
				if (rules->at(i).at(j).empty()) {
					if (!keep_if_init || not_ini_i) {
						if (j + 1 < rules->at(i).size()) {
							vector<int> last_rule = rules->at(i).back();
							rules->at(i).at(j) = last_rule;
						}
						rules->at(i).pop_back();
					}
				} else {
					vector<int>* rule = &rules->at(i).at(j);
					vector<vector<int>> to_add;
					for (vector<int>::iterator k = rule->begin(); k != rule->end();) {
						if (*k >= get_imput() && nullables.elem(*k - get_imput())) {
							vector<int> new_rule(rule->begin(),k);
							k++;
							new_rule.insert(new_rule.end(),k,rule->end());
							to_add.push_back(new_rule);
						} else {
							k++;
						}
					}
					rules->at(i).insert(rules->at(i).end(), to_add.begin(), to_add.end());
					j++;
				}
			}
		}
		if (keep_if_init) {
			if (get_initial_rereachable()) {
				nullables_valid = false;
			} else {
				nullables.clear();
				if (ret) {
					nullables.insert(initial_var - get_imput());
				}
			}
		} else {
			nullables.clear();
		}
		positives_valid = false;
		usefuls_valid = false;
		terminal_vars_valid = false;
		has_unit_productions = (has_unit_productions == 1 ? 1 : -1);
		is_CNF = (is_CNF == 1 ? 1 : -1);
		is_GNF = (is_GNF == 1 ? 1 : -1);
	}
	return ret;
}

void CFG::remove_unit_productions() {
	if (has_unit_productions != 0) {
		for (int i = 0; i < var_count; i++) {
			int_set units_here(var_count,{i});
			int j = 0;
			while (j < rules->at(i).size()) {
				if (rules->at(i).at(j).size() == 1 && rules->at(i).at(j).front() >= get_imput()) {
					int dest_var = rules->at(i).at(j).front() - get_imput();
					if (j + 1 < rules->at(i).size()) {
						vector<int> last_rule = rules->at(i).back();
						rules->at(i).at(j) = last_rule;
					}
					rules->at(i).pop_back();
					if (!units_here.elem(dest_var)) {
						units_here.insert(dest_var);
						rules->at(i).insert(rules->at(i).end(),rules->at(dest_var).begin(),rules->at(dest_var).end());
					}
				} else {
					j++;
				}
			}
		}
		reachables_valid = false;
		usefuls_valid = false;
		terminal_vars_valid = false;
		has_unit_productions = 0;
		is_CNF = (is_CNF == 1 ? 1 : -1);
		is_GNF = (is_GNF == 1 ? 1 : -1);
	}
}

void CFG::remove_repeated_rules() {
	bool invalidate = false;
	for (int i = 0; i < var_count; i++) {
		invalidate = true;
		set<vector<int>> uniques(rules->at(i).begin(),rules->at(i).end());
		rules->at(i).clear();
		rules->at(i).assign(uniques.begin(),uniques.end());
	}
	if (invalidate) {
		terminal_vars_valid = false;
	}
}

int CFG::remove_non_positives() {
	get_positives();
	if (positives.get_size() < var_count) {
		int_set non_positives(positives, true);
		remove_vars(&non_positives);
		positives_valid = true;
		positives.set_limit(var_count);
		positives.fill();
		return -non_positives.get_size();
	}
	return 0;
}

int CFG::remove_unreachables() {
	get_reachables();
	if (reachables.get_size() < var_count) {
		bool was_positive = positives_valid && positives.get_size() == var_count;
		int_set unreachables(reachables, true);
		remove_vars(&unreachables);
		reachables_valid = true;
		reachables.set_limit(var_count);
		reachables.fill();
		if (was_positive) {
			positives_valid = true;
			positives.set_limit(var_count);
			positives.fill();
			usefuls_valid = true;
			usefuls.set_limit(var_count);
			usefuls.fill();
		}
		return -unreachables.get_size();
	}
	return 0;
}

int CFG::simplify() {
	bool e = remove_eps_productions(true,true);
	remove_unit_productions();
	remove_repeated_rules();
	int p = remove_non_positives();
	int u = remove_unreachables();
	return (e ? 1 : 0) + p + u;
}

int CFG::remove_useless() {
	int p = remove_non_positives();
	int u = remove_unreachables();
	return p + u;	
}

int CFG::replace_unsolitary_terminals() {
	int for_ret = var_count;
	get_terminal_vars();
	if (is_CNF < 1) {
		int i_end = var_count;
		for (int i = 0; i < i_end; i++) {
			int j_end = rules->at(i).size();
			for (int j = 0; j < j_end; j++) {
				int k_end = rules->at(i).at(j).size();
				if (k_end > 1) {
					for (int k = 0; k < k_end; k++) {
						int sym  = rules->at(i).at(j).at(k);
						if (sym < get_imput()) {
							if (terminal_vars.at(sym) != -1) {
								rules->at(i).at(j).at(k) = terminal_vars.at(sym);
							} else {
								terminal_vars.at(sym) = get_imput() + var_count;
								var_count++;
								rules->push_back(vector<vector<int>>{{sym}});
								rules->at(i).at(j).at(k) = terminal_vars.at(sym);
							}
						}
					}
				}
			}
		}
		has_unit_productions = -1;
		if (nullables_valid) nullables.set_limit(var_count);
		if (positives_valid) nullables.set_limit(var_count,true);
		reachables_valid = false;
		usefuls_valid = false;
		is_CNF = -1;
		return var_count - for_ret;
	}
	return 0;
}

int CFG::binarize_productions() {
	if (is_CNF < 1) {
		int ret = 0;
		for (int i = 0; i < var_count; i++) {
			int j = 0;
			while (j < rules->at(i).size()) {
				vector<int>* ij = &rules->at(i).at(j);
				if (ij->size() > 2) {
					rules->push_back(vector<vector<int>>{vector<int>(ij->begin() + 1, ij->end())});
					ij->resize(2);
					ij->shrink_to_fit();
					ij->at(1) = get_imput() + var_count;
					var_count++;
					ret++;
				} else {
					j++;
				}
			}
		}
		nullables_valid = positives_valid = reachables_valid = usefuls_valid = terminal_vars_valid = false;
		is_CNF = -1;
		is_GNF = -1;
		return ret;
	}
	return 0;
}

int CFG::convert_to_CNF() {
	if (is_CNF < 1) {
		bool contains_eps = get_initial_nullable();
		int t = replace_unsolitary_terminals();
		int b = binarize_productions();
		remove_eps_productions(false);
		remove_unit_productions();
		remove_repeated_rules();
		if (contains_eps) {
			rules->at(initial_var - get_imput()).push_back(vector<int>{});
			nullables.insert(initial_var - get_imput());
		}
		is_CNF = 1;
		return t + b;
	}
	return 0;
}

void CFG::print() {
	cout << "imput_symbols:" << get_imput() <<
			" var_count:" << var_count << 
			" initial_var:" << initial_var << endl;
	string rules_str;
	for (int i = 0; i < var_count; i++) {
		rules_str += to_string(i + get_imput()) + " -> ";
		for (vector<int> rule : rules->at(i)) {
			rules_str.push_back(' ');
			for (int sym : rule) {
				rules_str += to_string(sym) + '.';
			}
			rules_str.pop_back();
			rules_str += " |";
		}
		rules_str.pop_back();
		rules_str += "\n";
	}
	cout << rules_str;
}

#endif