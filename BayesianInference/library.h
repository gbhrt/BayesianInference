#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>

using namespace std;

class Factor
{
public:
	vector<int> variables;
	vector<int> max_values;
	vector<float> prob_table;

	int get_prob_table_index(vector<int> value_numbers);
	float get_prob(vector<int> values);
	void set_prob(vector<int> values, float prob);//set prob to

};
class Var
{
public:
	string name;
	int num;
	vector<string> Values;
	vector< Var * > parents;//
	vector<float> cpt;
	int current_value;

	float get_prob_from_values(vector<int> value_numbers);
	float get_prob(int value);//return probability from the cpt table, given vector of parents value and this variable state
	
	int get_cpt_index(vector<int> value_numbers);//return the index of the index in cpt vector, given vector of parents value indexes
	
	int get_value_num(string value_name);//return the index of the value
	

};
class BayesianNetwork
{
public:
	vector<bool> removed_variables;

	vector<Var> variables;
	int get_var_num(string var_name);
	
};

struct Event//
{
	int var;//variable number
	int value;//the current value of this variable
};

class ConditionalData
{
public:
	Event Q;//Query - always single
	vector<Event> E_vec;//multiple evidences
	int algorithm_type;
};





void load_data(BayesianNetwork & BayesNet, vector<ConditionalData> & queries, string input_file_name);
string query_algorithm_1(BayesianNetwork & BayesNet, ConditionalData query);
string query_algorithm_2_and_3(BayesianNetwork &  BayesNet, ConditionalData query);
void save_data(string output_file, vector<string> answers);

bool get_next_combinations(vector <int> & state, vector <int> max_states);

//from function_lib:
Factor eliminate_factor(Factor factor, int var, int & add_count);
vector<Factor> find_and_remove_factors(vector<Factor> & factors, int var);
Factor join_factors(vector<Factor>factors, int & multiply_count);//factors size >=2
//vector<Factor> find_and_erase_smallest_factors(vector<Factor> & factors);
Factor join_2_factors(Factor A, Factor B, int & multiply_count);
void print_factors(vector<Factor> factors, string name);
void print_factor(Factor factor, string name);
void print_factor(Factor factor);
Factor union_variables(Factor A, Factor B, vector<int> & indexes);
Factor union_variables(Factor A, Factor B);
vector<int> abc_order(BayesianNetwork & BayesNet, vector<int>  free_vars);
int choose_variable_const_order(int & count);
vector<Factor> intitialize_factors(BayesianNetwork & BayesNet, vector<Event> evidences);
int comp_table_lenght(Factor factor);
int is_in(vector<int> varC, int indB);
int is_in_evidences(int var, vector<Event> evidences);
float comp_prob(BayesianNetwork & BayesNet, vector<Event> events, int & multiply_count, int & add_count);//compute the probability of the given events
vector<Event> combine_events(vector<Event> events, vector<int> hiden_variables, vector<int> hiden_variables_values);
vector<int> get_hiden_variables(BayesianNetwork & BayesNet, vector<Event> events);
float comp_joint_probability(BayesianNetwork & BayesNet, vector<Event> events, int & multiply_count);//vars - set of events, the value of their parents assumed to be known
void update_net(BayesianNetwork & BayesNet, vector<Event> events);//update current values of the given variables
string create_answer(float prob, int add_count, int multiply_count);
void remove_unnecessary_variables(BayesianNetwork & BayesNet, vector<int> given_vars);

