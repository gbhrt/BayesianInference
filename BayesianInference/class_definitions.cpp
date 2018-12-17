#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "library.h"
using namespace std;

int Factor::get_prob_table_index(vector<int> values)
{
	int index = values[values.size() - 1];
	int skip = max_values[values.size() - 1];
	for (int i = 0; i < values.size() - 1; i++)
	{
		index += values[i] * skip;
		skip *= (max_values[i]);
	}
	return index;
}
float Factor::get_prob(vector<int> values)
{
	return prob_table[get_prob_table_index(values)];
}

void Factor::set_prob(vector<int> values, float prob)
{
	prob_table[get_prob_table_index(values)] = prob;
	return;
}
float Var::get_prob_from_values(vector<int> values)//get values of parents and of the variable, return probability from tcp
{
	return cpt[get_cpt_index(values)];
}
float Var::get_prob(int value)//return probability from the cpt table, given vector of parents value and this variable state
{
	vector<int> values;
	
	for (int i = 0; i < parents.size(); i++)
	{
		values.push_back(parents[i]->current_value);
	}
	values.push_back(value);
	float prob = cpt[get_cpt_index(values)];
	return prob;
}
int Var::get_cpt_index(vector<int> value_numbers)//return the index of the index in cpt vector, given vector of parents value indexes
{
	int index = value_numbers[value_numbers.size() - 1];
	int skip = Values.size();
	for (int i = 0; i < value_numbers.size()-1; i++)
	{
		index += value_numbers[i] * skip;
		skip *= (parents[i]->Values.size());//+Values.size()
	}
	return index;
}
int Var::get_value_num(string value_name)//return the index of the value
{
	/*int * num;
	num = find(variables.begin(), variables.end(), var_name);*/
	int i;
	for (i = 0; i<Values.size(); i++)
	{
		if (Values[i] == value_name)
			break;
	}
	if (i < Values.size())
		return i;
	else
	{
		//cout << "error - get_var_num cannot find: " << var_name << endl;
		return -1;
	}
}


int BayesianNetwork::get_var_num(string var_name)
{
	/*int * num;
	num = find(variables.begin(), variables.end(), var_name);*/
	int i;
	for (i = 0; i<variables.size(); i++)
	{
		if (variables[i].name == var_name)
			break;
	}
	if (i < variables.size())
		return i;
	else
	{
		//cout << "error - get_var_num cannot find: " << var_name << endl;
		return -1;
	}
}



