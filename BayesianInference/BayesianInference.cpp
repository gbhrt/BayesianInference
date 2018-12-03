#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "library.h"

using namespace std;




int main()
{
	BayesianNetwork BayesNet = BayesianNetwork();
	vector<ConditionalData> queries;

	load_data(BayesNet, queries,"input3.txt");//read data from input file and intialize bayesian network and save queries
	vector<string> answers;
	for (int i = 0; i < queries.size(); i++)//answer all queries 
	{
		switch (queries[i].algorithm_type)//choose the required algorithm
		{
		case 1:
			answers.push_back(query_algorithm_1(BayesNet, queries[i]));
			break;
		case 2:
			answers.push_back(query_algorithm_1(BayesNet, queries[i]));
			break;
		case 3:
			answers.push_back(query_algorithm_1(BayesNet, queries[i]));
			break;

		}
	}

	save_data("output1.txt", answers);

	system("pause");
	return 0;
}