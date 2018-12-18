#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "header.h"


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
