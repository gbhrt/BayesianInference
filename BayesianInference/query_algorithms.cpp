#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "header.h"

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

	//compute: P(q,e1...en) e1...en in E 
	float prob = comp_prob(BayesNet, events, multiply_count, add_count);

	//compute: P(not(q),e1...en) e1...en in E .
	float complement_prob = -1.0; 
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
	if (alpha > 1e-20)
		prob = prob / alpha;
	else
		cout << "error - cannot normalize" << endl;

	return create_answer(prob, add_count, multiply_count);//create a string containing the probability, add count, multiply count
}

string query_algorithm_2_and_3(BayesianNetwork & BayesNet, ConditionalData query)
{
	int multiply_count = 0;
	int add_count = 0;
	vector<int> given_vars,hidden_vars;

	//save given variables i.e. (Q,e1,...en)
	given_vars.push_back(query.Q.var);
	for (int i = 0; i < query.E_vec.size(); i++)
		given_vars.push_back(query.E_vec[i].var);

	//remove every variable that is not an ancestor of a query variable or evidence variable
	remove_unnecessary_variables(BayesNet, given_vars);

	//save non hidden variables:
	for (int i = 0; i < BayesNet.variables.size(); i++)
	{
		if (is_in(given_vars, i) < 0 && !BayesNet.removed_variables[i])//BayesNet.variables[i].num != -1)
			hidden_vars.push_back(BayesNet.variables[i].num);
	}

	//initialize factors, instantain evidences
	vector<Factor> factors = intitialize_factors(BayesNet, query.E_vec);
	
	//choose variable elimenation ordering:
	if(query.algorithm_type == 2)
		hidden_vars = abc_order(BayesNet, hidden_vars);
	if (query.algorithm_type == 3)
		hidden_vars = heuristic_order(BayesNet, hidden_vars);

	//eliminate the hidden variables:
	int var;
	for(int i = 0; i<hidden_vars.size();i++)//free_vars are ordered according to the abc or a heuristic method
	{
		var = hidden_vars[i];
		//get all factors containing the choosed variable
		vector<Factor> factors_with_var = find_and_remove_factors(factors, var);
		print_factors(factors_with_var, "factors with: " + NumberToString(var));

		if (factors_with_var.size() > 0)//if factors containing the current variable exist
		{
			//join all factors with the current variable
			Factor joined_factor = join_factors(factors_with_var,multiply_count);
			print_factor(joined_factor, "joint factor");

			//eliminate the joined factor
			Factor eliminated_factor = eliminate_factor(joined_factor, var,add_count);//eliminate var from the jointed factor
			
			if (eliminated_factor.variables.size() > 0)//if not, probability is always 1.0, hence unnececary
			{
				cout << "eliminated factor" << endl;
				print_factor(eliminated_factor, "eliminated factor");
				factors.push_back(eliminated_factor);//add the factor to the factor list
			}

		}
	}

	//after eliminating all hidden variables - join all remaining factors
	vector<Factor> factors_with_var = find_and_remove_factors(factors, query.Q.var);//get all factors containing the query variable
	print_factors(factors_with_var, "factors with: " + NumberToString(query.Q.var));
	Factor joined_factor = join_factors(factors_with_var,multiply_count);//join all factors with the query variable
	print_factor(joined_factor, "joint factor");

	//normalize
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




