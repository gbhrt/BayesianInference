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
	vector<string> Values;
	vector< Var * > parents;//
	vector< vector<float> > cpt;
	int current_value;

	float get_prob(int value);//return probability from the cpt table, given vector of parents value and this variable state
	
	int get_cpt_index(vector<int> value_numbers);//return the index of the index in cpt vector, given vector of parents value indexes
	
	int get_value_num(string value_name);//return the index of the value
	

};
class BayesianNetwork
{
public:
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

void save_data(string output_file, vector<string> answers);

bool get_next_combinations(vector <int> & state, vector <int> max_states);