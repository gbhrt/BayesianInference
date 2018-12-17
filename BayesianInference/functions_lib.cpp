#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream> // stringstream
#include <iomanip> // setprecision

#include "library.h"

using namespace std;

bool get_next_combinations(vector <int> & state, vector <int> max_states)//return the next state given the state and the maximum size of every place in the vector 
{
	bool end = false;
	if (state.size() == 0)
		end = true;
	else
	{
		state[0]++;
		for (int i = 0; i < state.size(); i++)
		{
			if (state[i] == max_states[i])
			{
				if (i == state.size() - 1)
				{

					end = true;
					break;
				}
				else
				{
					state[i + 1]++;//add 1 to the next place and set all lowers to 0 (as in regular counting)
					for (int j = 0; j <= i; j++)
						state[j] = 0;

				}
			}
			else
				break;
		}
	}
	return end;
}

void update_net(BayesianNetwork & BayesNet, vector<Event> events)//update current values of the given variables
{
	for (int i = 0; i < events.size(); i++)
	{
		BayesNet.variables[events[i].var].current_value = events[i].value;
	}
}

//compute the joint probability of a set of arrays using the chain rule and conditional independence
float comp_joint_probability(BayesianNetwork & BayesNet, vector<Event> events, int & multiply_count)//vars - set of events, the value of their parents assumed to be known
{
	float total_prob = -1.0;
	for (int i = 0; i < events.size(); i++)// for each event
	{

		float prob = BayesNet.variables[events[i].var].get_prob(events[i].value);
		if (total_prob < 0)
			total_prob = prob;
		else
		{
			total_prob *= prob;
			multiply_count++;
		}

	}
	return total_prob;
}

vector<int> get_hiden_variables(BayesianNetwork & BayesNet, vector<Event> events)
{
	vector<int> hiden_variables;
	int j;
	for (int var_num = 0; var_num < BayesNet.variables.size(); var_num++)//for all variables in the bayesian network
	{
		for (j = 0; j < events.size(); j++)
		{
			if (var_num == events[j].var)
				break;
		}
		if (j == events.size())
			hiden_variables.push_back(var_num);
	}
	return hiden_variables;
}
vector<Event> combine_events(vector<Event> events, vector<int> hiden_variables, vector<int> hiden_variables_values)
{
	vector<Event> all_events = events;
	for (int i = 0; i < hiden_variables.size(); i++)
	{
		Event new_event = Event();
		new_event.var = hiden_variables[i];
		new_event.value = hiden_variables_values[i];
		all_events.push_back(new_event);
	}
	return all_events;
}
float comp_prob(BayesianNetwork & BayesNet, vector<Event> events, int & multiply_count, int & add_count)//compute the probability of the given events
{
	float prob = -1.0;
	vector<Event> all_events;//values of all variables in the network
	vector<int> hiden_variables = get_hiden_variables(BayesNet, events);

	//for every event combination:
	vector<int> hiden_variables_values(hiden_variables.size(), 0);//first state - all 0
	vector<int> hiden_variables_max_values;
	for (int i = 0; i < hiden_variables.size(); i++)
		hiden_variables_max_values.push_back(BayesNet.variables[hiden_variables[i]].Values.size());//the number of possible values of each variable
	do
	{
		//the current events vector includes the given events and the current combination of the values of the remain variables
		all_events = combine_events(events, hiden_variables, hiden_variables_values);

		update_net(BayesNet, all_events);//update all variables
		float tmp = comp_joint_probability(BayesNet, all_events, multiply_count);
		if (prob < 0)//at the first time. to avoid adding one more
			prob = tmp;
		else
		{
			prob += tmp;
			add_count++;
		}

	} while (!get_next_combinations(hiden_variables_values, hiden_variables_max_values));


	return prob;
}

int is_in_evidences(int var, vector<Event> evidences)
{
	for (int i = 0; i < evidences.size(); i++)
		if (var == evidences[i].var)
			return i;
	return -1;
}
int is_in(vector<int> varC, int indB)
{
	for (int indC = 0; indC < varC.size(); indC++)
		if (varC[indC] == indB)
			return indC;
	return -1;
}
int comp_table_lenght(Factor factor)
{
	int lenght = 1;//compute the size of cpt table
	for (int i = 0; i < factor.variables.size(); i++)
		lenght *= factor.max_values[i];
	return lenght;
}

vector<Factor> intitialize_factors(BayesianNetwork & BayesNet, vector<Event> evidences)
{
	vector<Factor> factors;
	for (int var = 0; var < BayesNet.variables.size(); var++)
	{
		if (BayesNet.removed_variables[var])//(BayesNet.variables[var].num == -1)
			continue;
		//int var = BayesNet.variables[i].num;
		Factor factor = Factor();
		vector<int> values;
		vector<bool> set_by_evidence;


		for (int parent_ind = 0; parent_ind < BayesNet.variables[var].parents.size(); parent_ind++)
		{
			
			int parent_num = BayesNet.variables[var].parents[parent_ind]->num;
			if (BayesNet.removed_variables[parent_num])//(parent_num == -1)
				continue;
			int ind = is_in_evidences(parent_num, evidences);
			if (ind <0)
			{

				factor.variables.push_back(parent_num);
				factor.max_values.push_back(BayesNet.variables[parent_num].Values.size());
				values.push_back(-1);//save all for getting prob from the local tcp table
				set_by_evidence.push_back(false);
			}
			else
			{
				values.push_back(evidences[ind].value);//save all for getting prob from the local tcp table
				set_by_evidence.push_back(true);
			}
		}
		int ind = is_in_evidences(var, evidences);
		if (ind<0)
		{
			factor.variables.push_back(var);
			factor.max_values.push_back(BayesNet.variables[var].Values.size());
			values.push_back(-1);
			set_by_evidence.push_back(false);
		}
		else
		{
			values.push_back(evidences[ind].value); // save all for getting prob from the local tcp table
			set_by_evidence.push_back(true);
		}
		//factor.prob_table = copy_cpt(BayesNet, var, evidences);
		if (factor.variables.size() != 0)//if all variables are evidences
		{
			int lenght = comp_table_lenght(factor);//compute the size of cpt table


			factor.prob_table.resize(lenght, -1);

			vector<float> prob_table;
			vector<int> values_options(factor.variables.size(), 0);//first state - all 0
			do
			{
				int ind = 0;
				for (int i = 0; i < values.size(); i++)//update values as the values options, dont update values they already exist (from evidence)
				{

					if (set_by_evidence[i])
					{
						continue;
					}
					values[i] = values_options[ind];
					ind++;
				}
				float prob = BayesNet.variables[var].get_prob_from_values(values);
				factor.set_prob(values_options, prob);

			} while (!get_next_combinations(values_options, factor.max_values));
		}
		factors.push_back(factor);

	}
	return factors;
}

int choose_variable_const_order(int & count)
{
	vector<int> order = { 2,1 };
	if (count > order.size() - 1)
		return -1;
	int var = order[count];
	count++;
	return var;
}
vector<int> abc_order(BayesianNetwork & BayesNet, vector<int>  free_vars)//, int current_var
{
	vector<int> ordered_vars;

	while (free_vars.size())
	{

		int current_var = free_vars[0];

		char min_char = BayesNet.variables[free_vars[0]].name[0];//
		
		for (int i = 1; i < free_vars.size(); i++)//find the lowest value
			if (BayesNet.variables[free_vars[i]].name[0] < min_char)//lower then the current variable and not in Q or evidence //
			{
				min_char = BayesNet.variables[free_vars[i]].name[0];
				current_var = BayesNet.variables[free_vars[i]].num;
			}
		free_vars.erase(free_vars.begin() + is_in(free_vars, current_var));
		cout << BayesNet.variables[current_var].name[0] << endl;
		ordered_vars.push_back(current_var);
	}

	return ordered_vars;

}

Factor union_variables(Factor A, Factor B)
{
	Factor C;
	for (int indA = 0; indA < A.variables.size(); indA++)//insert all A
	{
		C.variables.push_back(A.variables[indA]);
		C.max_values.push_back(A.max_values[indA]);
	}
	for (int indB = 0; indB < B.variables.size(); indB++)//and all B if not already exist
	{
		if (is_in(C.variables, B.variables[indB])<0)
		{
			C.variables.push_back(B.variables[indB]);
			C.max_values.push_back(B.max_values[indB]);
		}
	}
	return C;
}
Factor union_variables(Factor A, Factor B, vector<int> & indexes)
{
	Factor C;
	for (int indA = 0; indA < A.variables.size(); indA++)//insert all A
	{
		C.variables.push_back(A.variables[indA]);
		C.max_values.push_back(A.max_values[indA]);
	}
	for (int indB = 0; indB < B.variables.size(); indB++)//and all B if not already exist
	{
		int ind = is_in(C.variables, B.variables[indB]);
		if (ind < 0)
		{
			C.variables.push_back(B.variables[indB]);
			C.max_values.push_back(B.max_values[indB]);
			indexes.push_back(C.variables.size() - 1);
		}
		else
			indexes.push_back(ind);
	}
	return C;
}
void print_factor(Factor factor)
{
	cout << "Factor" << endl;
	for (int i = 0; i < factor.variables.size(); i++)
		cout << factor.variables[i] << "\t";
	cout << "probability" << endl;
	vector<int> values_options(factor.variables.size(), 0);//first state - all 0
	do
	{
		float prob = factor.get_prob(values_options);
		for (int i = 0; i < values_options.size(); i++)
			cout << values_options[i] << "\t";
		cout << prob << endl;
	} while (!get_next_combinations(values_options, factor.max_values));
}
void print_factor(Factor factor, string name)
{
	cout << "Factor: " << name << endl;
	print_factor(factor);
}
void print_factors(vector<Factor> factors, string name)
{
	cout << name << endl;
	for (int i = 0; i < factors.size(); i++)
		print_factor(factors[i]);
}
Factor join_2_factors(Factor A, Factor B, int & multiply_count)
{
	vector<int>B_indexses;//save the indexes of the variables in factor B in factor C
	Factor C = union_variables(A, B, B_indexses);
	int lenght = comp_table_lenght(C);
	C.prob_table.resize(lenght, -1);
	vector<int> values_options(C.variables.size(), 0);//first state - all 0
	do
	{
		vector<int> A_values, B_values;
		for (int i = 0; i < A.variables.size(); i++)//the union of the variables inserted A variables at the begining
		{
			A_values.push_back(values_options[i]);
		}
		for (int i = 0; i < B.variables.size(); i++)
		{
			B_values.push_back(values_options[B_indexses[i]]);
		}
		float probA = A.get_prob(A_values);
		float probB = B.get_prob(B_values);
		float prob = probA*probB;
		multiply_count++;
		C.set_prob(values_options, prob);
	} while (!get_next_combinations(values_options, C.max_values));
	return C;
}

Factor find_and_erase_smallest_factor(vector<Factor> & factors)
{
	int min_size = factors[0].prob_table.size();
	int min_ind = 0;
	for (int i = 0; i < factors.size(); i++)
	{
		if (factors[i].prob_table.size() < min_size)
		{
			min_size = factors[i].prob_table.size();
			min_ind = i;
		}
	}
	Factor min_factor = factors[min_ind];
	factors.erase(factors.begin() + min_ind);
	return min_factor;
}
//vector<Factor> find_and_erase_smallest_factors(vector<Factor> & factors)
//{
//	int min_size = factors[0].prob_table.size() / factors[0].variables.size();//number of rows in the probability table
//	int min_i = 0;
//	int min_j = 0;
//	for (int i = 0; i < factors.size(); i++)
//	{
//		for (int j = i + 1; j < factors.size(); j++)
//		{
//			if (factors[i].prob_table.size() < min_size)
//			{
//				min_size = factors[i].prob_table.size() / factors[0].variables.size();
//				min_i = i;
//				min_j = j;
//			}
//		}
//	}
//	vector<Factor> min_factors = { factors[min_i],factors[min_j] };
//	factors.erase(factors.begin() + min_j);//j > i
//	factors.erase(factors.begin() + min_i);
//	return min_factors;
//}
Factor join_factors(vector<Factor>factors, int & multiply_count)//factors size >=2
{
	Factor joined_factor = factors[0];//for the case that joining just one factor;
	
	while (factors.size() >= 2)
	{
		Factor min_factor1 = find_and_erase_smallest_factor(factors);
		Factor min_factor2 = find_and_erase_smallest_factor(factors);
		joined_factor = join_2_factors(min_factor1, min_factor2, multiply_count);
		if(factors.size() > 0)
			factors.push_back(joined_factor);
	}
	//if (factors.size() == 1)
	//	multiply_count+= comp_table_lenght(factors[0]);


	return joined_factor;
}
vector<Factor> find_and_remove_factors(vector<Factor> & factors, int var)
{
	vector<Factor> factors_with_var;
	vector<Factor> factors_without_var;
	for (int i = 0; i < factors.size(); i++)
	{
		if (is_in(factors[i].variables, var) >= 0)
			factors_with_var.push_back(factors[i]);
		else
			factors_without_var.push_back(factors[i]);
	}
	factors = factors_without_var;
	return factors_with_var;
}
Factor eliminate_factor(Factor factor, int var, int & add_count)//
{
	int ind = is_in(factor.variables, var);//index of the variable to eliminate in all variables in the factor
	Factor new_factor = factor;//the new factor is as factor without the eliminated variable
	new_factor.variables.erase(new_factor.variables.begin() + ind);
	if (new_factor.variables.size() == 0)
		return new_factor;

	new_factor.max_values.erase(new_factor.max_values.begin() + ind);
	new_factor.prob_table.resize(comp_table_lenght(new_factor), -1);//resize the probability table of the new factor
	vector<int> values_options(new_factor.max_values.size(), 0);//first state - all 0. size -1 
	do
	{

		vector<int> factor_values = values_options;
		factor_values.insert(factor_values.begin() + ind, 0);
		float prob = factor.get_prob(factor_values);
		for (int var_value = 1; var_value < factor.max_values[ind]; var_value++)//sum all options of the eliminated variable
		{
			factor_values[ind] = var_value;
			prob += factor.get_prob(factor_values);
			add_count++;
		}
		new_factor.set_prob(values_options, prob);
	} while (!get_next_combinations(values_options, new_factor.max_values));
	return new_factor;
}
int get_parent(BayesianNetwork & BayesNet, int var, vector<int> & ansestors)
{
	int num = -1;
	for (int i = 0; i < BayesNet.variables[var].parents.size(); i++)
		if (is_in(ansestors, BayesNet.variables[var].parents[i]->num) == -1)
			return BayesNet.variables[var].parents[i]->num;
	return num;
}
void find_ansestors(BayesianNetwork & BayesNet, int var, vector<int> & ansestors)
{

	//int parent = get_parent(BayesNet, var, ansestors);
	//if (parent == -1)
	//{
	//	ansestors.push_back(var);
	//	
	//}
	//else
	//{
	//	find_ansestors(BayesNet, parent, ansestors);
	//	
	//}

	int parent = get_parent(BayesNet, var, ansestors);
	while (parent != -1)
	{
		find_ansestors(BayesNet, parent, ansestors);
		parent = get_parent(BayesNet, var, ansestors);
	}
	ansestors.push_back(var);
	return;

}
void remove_unnecessary_variables(BayesianNetwork & BayesNet, vector<int> given_vars)
{
	vector<int> ansestors;
	for (int i = 0; i < given_vars.size(); i++)
	{
		int parent_ind = 0;
		find_ansestors(BayesNet, given_vars[i], ansestors);
	}

	BayesNet.removed_variables.clear();
	for (int i = 0; i < BayesNet.variables.size(); i++)
	{
		if (is_in(ansestors, i)>=0)
			BayesNet.removed_variables.push_back(false);
		else
			BayesNet.removed_variables.push_back(true);
	}
	//for (int i = 0; i < BayesNet.variables.size(); i++)
	//{
	//	int erase_var = BayesNet.variables[i].num;
	//	if (is_in(ansestors, BayesNet.variables[i].num) < 0)
	//	{
	//		//BayesNet.variables.erase(BayesNet.variables.begin() + i, BayesNet.variables.begin() + i + 1);
	//		BayesNet.variables[erase_var].num = -1;
	//		for (int j = 0; j < BayesNet.variables.size(); j++)
	//		{
	//			for (int par_ind = 0; par_ind < BayesNet.variables[j].parents.size(); par_ind++)
	//			{
	//				if (BayesNet.variables[j].parents[par_ind]->num == erase_var)
	//					BayesNet.variables[j].parents[par_ind]->num = -1;
	//				//BayesNet.variables[j].parents.erase(BayesNet.variables[j].parents.begin() + par_ind, BayesNet.variables[j].parents.begin() + par_ind + 1);
	//			}
	//		}
	//	}

	//}
}
string create_answer(float prob, int add_count, int multiply_count)
{
	string answer_str;
	stringstream stream;
	stream << fixed << setprecision(5) << prob;//5 points precision after the point
	answer_str = stream.str();
	answer_str.append(",");
	answer_str.append(to_string(add_count));
	answer_str.append(",");
	answer_str.append(to_string(multiply_count));

	cout << "answer: " << answer_str << endl;
	return answer_str;
}
