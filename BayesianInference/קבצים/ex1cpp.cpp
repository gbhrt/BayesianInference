#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "header.h"

using namespace std;

int main()
{
		string input_file_name ( "input.txt");
		string output_file_name ( "output.txt" );

		BayesianNetwork BayesNet = BayesianNetwork();//contain all the data about the bayesian network.
		vector<ConditionalData> queries;//contain the queries from the file

		load_data(BayesNet, queries, input_file_name);//read data from input file and intialize bayesian network and save queries
		vector<string> answers;
		for (int i = 0; i < queries.size(); i++)//answer all queries 
		{
			if (queries[i].algorithm_type == 1)//choose the required algorithm
				answers.push_back(query_algorithm_1(BayesNet, queries[i]));
			else
				answers.push_back(query_algorithm_2_and_3(BayesNet, queries[i]));
		}

		save_data(output_file_name, answers);

	return 0;
}