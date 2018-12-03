#include <iostream>
#include <string>
#include <fstream>
#include <vector>
//#include <algorithm>
#include <sstream> // stringstream
#include <iomanip> // setprecision
#include "library.h"

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
	for(int i = 0 ;i < events.size(); i++)// for each event
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
	vector<int> hiden_variables_values(hiden_variables.size(),0);//first state - all 0
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

bool is_in_evidences(int var, vector<Event> evidences)
{
	for (int i = 0; i < evidences.size(); i++)
		if (var == evidences[i].var)
			return true;
	return false;
}
vector<Factor> intitialize_factors(BayesianNetwork BayesNet, vector<Event> evidences)
{
	vector<Factor> factors;
	for (int var = 0; var < BayesNet.variables.size(); var++)
	{
		Factor factor = Factor();
		if(!is_in_evidences(var, evidences))
			factor.variables.push_back(var);
		for (int parent = 0; parent < BayesNet.variables.size(); parent++)
		{
			if (!is_in_evidences(parent, evidences))
				factor.variables.push_back(parent);
			/*prob = BayesNet.variables[var].get_prob()
			factor.set_prob()*/
		}
		factors.push_back(factor);

	}
	return factors;
}

string query_algorithm_2(BayesianNetwork & BayesNet, ConditionalData query)
{
	//initialize factors
	vector<Factor> factors = intitialize_factors(BayesNet, query.E_vec);
	return "aa";
}


string query_algorithm_1(BayesianNetwork & BayesNet, ConditionalData query)
{
	string answer_str;
	int multiply_count = 0;
	int add_count = 0;
	update_net(BayesNet, query.E_vec);//update the given values of the Bayesian network

	//given query P(a|B), a is a single event, B a set of events
	//compute : P(a,bi) bi - all events in B
	vector<Event> events;
	events.push_back(query.Q);
	for (int i = 0; i < query.E_vec.size(); i++)
		events.push_back(query.E_vec[i]);

	float prob = comp_prob(BayesNet, events, multiply_count, add_count);

	float complement_prob = 0;
	//compute P(not(Q),E)
	for (int Q_value = 0; Q_value < BayesNet.variables[query.Q.var].Values.size(); Q_value++)
	{
		if (query.Q.value == Q_value)//dont include the actual value of Q
			continue;
		events[0].value = Q_value;
		complement_prob+= comp_prob(BayesNet, events, multiply_count, add_count);
	}

	float denominator = prob + complement_prob;
	add_count++;
	if (denominator > 1e-10)
		prob = prob / denominator;
	else
	{
		cout << "error - cannot normalize" << endl;
	}

	
	stringstream stream;
	stream << fixed << setprecision(5) << prob;//5 points precision after the point
	answer_str = stream.str();
	answer_str.append(", ");
	answer_str.append(to_string(add_count));
	answer_str.append(", ");
	answer_str.append(to_string(multiply_count));

	cout << "answer: " << answer_str << endl;
	return answer_str;
}

