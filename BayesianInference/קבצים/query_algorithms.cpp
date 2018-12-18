#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "library.h"

vector<int> get_neighbors(BayesianNetwork & BayesNet, int var)
{
	vector<int> neighbors;
	for (int i = 0; i < BayesNet.variables[var].parents.size(); i++)//all parents
		//if (!BayesNet.removed_variables[BayesNet.variables[var].parents[i]->num])//(BayesNet.variables[var].parents[i]->num != -1)
			neighbors.push_back(BayesNet.variables[var].parents[i]->num);

	for (int i = 0; i < BayesNet.variables.size(); i++)//all childs
	{
		//if (!BayesNet.removed_variables[i])//(BayesNet.variables[i].num != -1)
			for (int j = 0; j < BayesNet.variables[i].parents.size(); j++)//all parents
				if (BayesNet.variables[i].parents[j]->num == var)// &&  (!BayesNet.removed_variables[BayesNet.variables[i].parents[j]->num]))
				{
					neighbors.push_back(BayesNet.variables[i].num);
					break;
				}
	}
	return neighbors;
}
void add_edges(vector < vector<int> > & neighbor_list,int var, vector<bool> removed_variables)//add edges between all neighbors of var
{
	for (int i = 0; i < neighbor_list[var].size(); i++)
	{
		int neighbor1 = neighbor_list[var][i];
		for (int j = 0; j < neighbor_list[var].size(); j++)
		{
			int neighbor2 = neighbor_list[var][j];
			if(is_in(neighbor_list[neighbor1], neighbor2)<0 && neighbor2 != neighbor1 && !removed_variables[neighbor1] && !removed_variables[neighbor2])
				neighbor_list[neighbor1].push_back(neighbor2);
		}
	}
}

//int get_fill_num(vector < vector<int> > neighbor_list, vector<bool> removed_variables,int var)
//{
//
//}
//int find_min_fill(vector < vector<int> > neighbor_list, vector<int> unmarked_vars, vector<bool> removed_variables)
//{
//	int min_fill = pow(neighbor_list.size(),2);//the maximum possible number
//	int min_var = 0;
//	int j;
//	for (j = 0; j < unmarked_vars.size(); j++)
//	{
//		if (!removed_variables[unmarked_vars[j]])
//		{
//			min_var = unmarked_vars[j];
//			break;
//		}
//	}
//	for (int i = j; i < unmarked_vars.size(); i++)
//	{
//		int fill_num = get_fill_num(i, neighbor_list, removed_variables);
//		if (!removed_variables[neighbors_num])
//			if (neighbors_num < min_neighbors)
//			{
//				min_neighbors = neighbors_num;
//				min_var = unmarked_vars[i];
//			}
//	}
//	return min_var;
//}
int find_min_neighbor(vector < vector<int> > neighbor_list, vector<int> unmarked_vars,vector<bool> removed_variables)
{
	int min_neighbors = neighbor_list.size();//the maximum possible number
	int min_var = 0;
	int j;
	for (j = 0; j < unmarked_vars.size(); j++)
	{
		if (!removed_variables[unmarked_vars[j]])
		{
			min_var = unmarked_vars[j];
			break;
		}
	}
	for (int i = j; i < unmarked_vars.size(); i++)
	{
		int neighbors_num = neighbor_list[unmarked_vars[i]].size();
		if(!removed_variables[unmarked_vars[i]])
			if (neighbors_num < min_neighbors)
			{
				min_neighbors = neighbors_num;
				min_var = unmarked_vars[i];
			}
	}
	return min_var;
}
vector<int> heuristic_order(BayesianNetwork & BayesNet, vector<int> free_vars)
{
	vector < vector<int> > neighbor_list;
	for (int i = 0; i < BayesNet.variables.size(); i++)
		//if(!BayesNet.removed_variables[i])//(BayesNet.variables[i].num != -1)
			neighbor_list.push_back(get_neighbors(BayesNet, i));

	vector<int> ordered_vars;
	vector<int> unmarked_vars = free_vars;
	for (int i = 0; i < free_vars.size(); i++)
	{
		int var = find_min_neighbor(neighbor_list, unmarked_vars, BayesNet.removed_variables);
		cout << BayesNet.variables[var].name[0] << endl;
		ordered_vars.push_back(var);
		int var_ind = is_in(unmarked_vars, var);
		unmarked_vars.erase(unmarked_vars.begin() + var_ind, unmarked_vars.begin() + var_ind+1);
		add_edges(neighbor_list,var, BayesNet.removed_variables);
	}

	return ordered_vars;
}

string query_algorithm_2_and_3(BayesianNetwork & BayesNet, ConditionalData query)
{
	int multiply_count = 0;
	int add_count = 0;
	vector<int> given_vars,free_vars;
	given_vars.push_back(query.Q.var);
	for (int i = 0; i < query.E_vec.size(); i++)
		given_vars.push_back(query.E_vec[i].var);

	remove_unnecessary_variables(BayesNet, given_vars);
	for (int i = 0; i < BayesNet.variables.size(); i++)
	{
		if (is_in(given_vars, i) < 0 && !BayesNet.removed_variables[i])//BayesNet.variables[i].num != -1)
			free_vars.push_back(BayesNet.variables[i].num);
	}
	vector<Factor> factors = intitialize_factors(BayesNet, query.E_vec);//initialize factors, instantain evidences
	
	//choose variable elimenation ordering:
	if(query.algorithm_type == 2)
		free_vars = abc_order(BayesNet, free_vars);
	if (query.algorithm_type == 3)
		free_vars = heuristic_order(BayesNet, free_vars);

	int var;
	for(int i = 0; i<free_vars.size();i++)//free_vars are ordered according to the abc or a heuristic method
	{
		var = free_vars[i];
		vector<Factor> factors_with_var = find_and_remove_factors(factors, var);//get all factors containing the choosed variable
		print_factors(factors_with_var, "factors with: " + NumberToString(var));
		if (factors_with_var.size() > 0)
		{

			Factor joined_factor = join_factors(factors_with_var,multiply_count);//join all factors with var
			print_factor(joined_factor, "joint factor");

			Factor eliminated_factor = eliminate_factor(joined_factor, var,add_count);//eliminate var from the jointed factor
			
			if (eliminated_factor.variables.size() > 0)//if not, probability is always 1.0, hence unnececary
			{
				cout << "eliminated factor" << endl;
				print_factor(eliminated_factor, "eliminated factor");
				factors.push_back(eliminated_factor);
			}

		}
	}

	var = query.Q.var;
	vector<Factor> factors_with_var = find_and_remove_factors(factors, var);//get all factors containing the query variable
	print_factors(factors_with_var, "factors with: " + NumberToString(var));
	Factor joined_factor = join_factors(factors_with_var,multiply_count);//join all factors with var
	print_factor(joined_factor, "joint factor");

	vector<int>values;
	values.push_back(0);
	float alpha = joined_factor.get_prob(values);
	for (int i = 1; i < joined_factor.max_values[0]; i++)//assuming one and just one variable exist
	{
		values[0] = i;
		float prob = joined_factor.get_prob(values);
		alpha += prob;
		add_count++;
	}

	values[0] = query.Q.value;
	float prob = joined_factor.get_prob(values);
	if (alpha > 1e-20)
		prob = prob / alpha;
	else
		cout << "error - cannot normalize" << endl;


	return create_answer(prob, add_count, multiply_count);
}


string query_algorithm_1(BayesianNetwork & BayesNet, ConditionalData query)
{	
	int multiply_count = 0;
	int add_count = 0;
	update_net(BayesNet, query.E_vec);//update the given values of the Bayesian network variables according to the evidences

	//given query P(q|E), q is a single event (query), E a set of events (evidences)

	vector<Event> events;//contain (q,e1...en) e1...en in E  
	events.push_back(query.Q);
	for (int i = 0; i < query.E_vec.size(); i++)
		events.push_back(query.E_vec[i]);

	float prob = comp_prob(BayesNet, events, multiply_count, add_count);//compute: P(q,e1...en) e1...en in E 

	float complement_prob = -1.0;//the probability of P(not(q),e1...en) e1...en in E . 
	for (int Q_value = 0; Q_value < BayesNet.variables[query.Q.var].Values.size(); Q_value++)
	{
		if (query.Q.value == Q_value)//dont include the actual value of Q
			continue;
		events[0].value = Q_value;
		float p = comp_prob(BayesNet, events, multiply_count, add_count);//compute probability
		if (complement_prob < 0)// answers
			complement_prob = p;
		else
		{
			complement_prob += p;
			add_count++;
		}
	}

	float alpha = prob + complement_prob;
	add_count++;
	if (alpha > 1e-10)
		prob = prob / alpha;
	else
		cout << "error - cannot normalize" << endl;

	return create_answer(prob, add_count, multiply_count);//create a string containing the probability, add count, multiply count
}

