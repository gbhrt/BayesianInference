#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include "library.h"

using namespace std;

int main()
{
	//vector<string> input_files_names = { "input1.txt" ,"input2.txt" ,"input3.txt" ,"input4.txt" ,"input5.txt" ,"input6.txt" ,"input7.txt", "input8.txt" };
	//vector<string> output_files_names = { "output1.txt" ,"output2.txt" ,"output3.txt" ,"output4.txt" ,"output5.txt" ,"output6.txt" ,"output7.txt","output8.txt" };
	vector<string> input_files_names = { "input3.txt" };
	vector<string> output_files_names = { "output3.txt" };
	for (int i = 0; i < input_files_names.size(); i++)
	{
		string input_file_name = input_files_names[i];
		string output_file_name = output_files_names[i];
		/*string input_file_name = "input2.txt";
		string output_file_name = "output2.txt";*/
		

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
	}

	system("pause");
	return 0;
}