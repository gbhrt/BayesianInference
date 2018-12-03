#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "library.h"
using namespace std;

int Factor::get_prob_table_index(vector<int> value_numbers)
{
	int index = 0;//if no parents index is 0
	int skip = 1;
	for (int i = 0; i < value_numbers.size(); i++)
	{
		index += value_numbers[i] * skip;
		skip *= variables.size();
	}
	return index;
}
float Factor::get_prob(vector<int> value_numbers)
{
	float prob = 0;// = cpt[get]
	return prob;
}

float Var::get_prob(int value)//return probability from the cpt table, given vector of parents value and this variable state
{
	vector<int> parents_value_numbers;
	for (int i = 0; i < parents.size(); i++)
	{
		parents_value_numbers.push_back(parents[i]->current_value);
	}
	float prob = cpt[get_cpt_index(parents_value_numbers)][value];
	return prob;
}
int Var::get_cpt_index(vector<int> value_numbers)//return the index of the index in cpt vector, given vector of parents value indexes
{
	int index = 0;//if no parents index is 0
	int skip = 1;
	for (int i = 0; i < value_numbers.size(); i++)
	{
		index += value_numbers[i] * skip;
		skip *= parents[i]->Values.size();
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



