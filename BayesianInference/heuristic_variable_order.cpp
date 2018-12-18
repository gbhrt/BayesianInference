#include "header.h"

vector<int> get_neighbors(BayesianNetwork & BayesNet, int var)
{
	vector<int> neighbors;
	for (int i = 0; i < BayesNet.variables[var].parents.size(); i++)//all parents
																	
		neighbors.push_back(BayesNet.variables[var].parents[i]->num);

	for (int i = 0; i < BayesNet.variables.size(); i++)//all childs
	{
		for (int j = 0; j < BayesNet.variables[i].parents.size(); j++)//all parents
			if (BayesNet.variables[i].parents[j]->num == var)
			{
				neighbors.push_back(BayesNet.variables[i].num);
				break;
			}
	}
	return neighbors;
}
void add_edges(vector < vector<int> > & neighbor_list, int var, vector<bool> removed_variables)//add edges between all neighbors of var
{
	for (int i = 0; i < neighbor_list[var].size(); i++)
	{
		int neighbor1 = neighbor_list[var][i];
		for (int j = 0; j < neighbor_list[var].size(); j++)
		{
			int neighbor2 = neighbor_list[var][j];
			if (is_in(neighbor_list[neighbor1], neighbor2)<0 && neighbor2 != neighbor1 && !removed_variables[neighbor1] && !removed_variables[neighbor2])
				neighbor_list[neighbor1].push_back(neighbor2);
		}
	}
}

int find_min_neighbor(vector < vector<int> > neighbor_list, vector<int> unmarked_vars, vector<bool> removed_variables)
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
		if (!removed_variables[unmarked_vars[i]])
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
		neighbor_list.push_back(get_neighbors(BayesNet, i));

	vector<int> ordered_vars;
	vector<int> unmarked_vars = free_vars;
	for (int i = 0; i < free_vars.size(); i++)
	{
		int var = find_min_neighbor(neighbor_list, unmarked_vars, BayesNet.removed_variables);
		cout << BayesNet.variables[var].name[0] << endl;
		ordered_vars.push_back(var);
		int var_ind = is_in(unmarked_vars, var);
		unmarked_vars.erase(unmarked_vars.begin() + var_ind, unmarked_vars.begin() + var_ind + 1);
		add_edges(neighbor_list, var, BayesNet.removed_variables);
	}

	return ordered_vars;
}