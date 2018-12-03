#include <iostream>
#include <string>
#include <fstream>
#include <vector>


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

void pause_exit()
{
	cout << "press any key to exit the program" << endl;
	getchar();
	exit(1);
}

string replace_substr(string str,  string search,  string replace) 
{
	for (size_t pos = 0; ; pos += replace.length()) {
		// Locate the substring to replace
		pos = str.find(search, pos);
		if (pos == string::npos) break;
		// Replace by erasing and inserting
		str.erase(pos, search.length());
		str.insert(pos, replace);
	}
	return str;
}

string remove_equal_sign(string str)
{
	size_t pos = 0;
	pos = str.find("=");
	if (pos == 0)
		str.erase(0, 1);
	else
	{
		cout << "error in input file format - cannot remove_equal_sign" << endl;
		pause_exit();
	}
	return str;
}
vector<string> split_line(string str)//split a string to words (remove spaces and ",")
{
	vector<string> tokens;
	size_t comma_pos;
	size_t space_pos;
	size_t end_pos;
	size_t  min_pos = -1;
	size_t pos = 0;
	size_t var_name_lenght;
	do
	{
		min_pos = -1;
		comma_pos = str.find(",");
		space_pos = str.find(" ");
		end_pos = str.length();
		if (comma_pos < min_pos && comma_pos != string::npos)
			min_pos = comma_pos;
		if (space_pos < min_pos && space_pos != string::npos)
			min_pos = space_pos;
		if (end_pos < min_pos && end_pos != string::npos)
			min_pos = end_pos;

		if (min_pos != -1)
		{
			if (min_pos == 0)
			{
				str.erase(0, 1);
			}
			else
			{
				var_name_lenght = min_pos;
				tokens.push_back(str.substr(0, var_name_lenght));
				str.erase(0, var_name_lenght);
			}
		}
	} while (str.length() != 0);

	return tokens;

}

string run_to_line(vector<string> line_buffer,int & line_index, string search_word)
{
	bool find_flag = false;
	for(line_index; line_index<line_buffer.size(); line_index++)
	{
		if (line_buffer[line_index].find(search_word) != string::npos)
		{
			find_flag = true;
			break;
		}
	}
	if (find_flag)
		return line_buffer[line_index];
	else
		return string("error");
}
int read_variable_data(vector<string> line_buffer, int & line_index, BayesianNetwork &BayesNet)//
{
	string line;
	vector<string> tokens;
	int start_line_index = line_index;
	for (int i = 0; i < BayesNet.variables.size(); i++)//read in variables
	{
		line = run_to_line(line_buffer, line_index, "Var");
		tokens = split_line(line);
		int var_num = BayesNet.get_var_num(tokens[1]);

		line = run_to_line(line_buffer, line_index, "Values:");
		tokens = split_line(line);
		for (int i = 1; i < tokens.size(); i++)
			BayesNet.variables[var_num].Values.push_back(tokens[i]);

		line = run_to_line(line_buffer, line_index, "Parents:");
		tokens = split_line(line);
		for (int i = 1; i < tokens.size(); i++)
		{
			int parent_num = BayesNet.get_var_num(tokens[i]);
			if (parent_num != -1)
			{
				BayesNet.variables[var_num].parents.push_back(&BayesNet.variables[parent_num]);
			}
		}
	}
	line_index = start_line_index;//start again from begging of variables
	for (int i = 0; i < BayesNet.variables.size(); i++)//read tcp tables variables
	{
		line = run_to_line(line_buffer, line_index, "Var");
		tokens = split_line(line);
		int var_num = BayesNet.get_var_num(tokens[1]);

		vector<float> probs(BayesNet.variables[var_num].Values.size(), -1);
		int cpt_lenght = 1;//compute the size of cpt table
		for (int i = 0; i < BayesNet.variables[var_num].parents.size(); i++)
			cpt_lenght *= BayesNet.variables[var_num].parents[i]->Values.size();
		BayesNet.variables[var_num].cpt.resize(cpt_lenght, probs);

		line = run_to_line(line_buffer, line_index, "CPT:");
		line_index++;
		for(int l = line_index;l<line_buffer.size();l++)
		{
			if (line_buffer[l] == "")//read until an empty line
				break;
			tokens = split_line(line_buffer[l]);
			vector<int> value_numbers;
			for (int i = 0; i < BayesNet.variables[var_num].parents.size(); i++)//read all parents values
			{
				value_numbers.push_back(BayesNet.variables[var_num].parents[i]->get_value_num(tokens[i]));
			}
			int value_index = BayesNet.variables[var_num].get_cpt_index(value_numbers);
			float prob_sum = 0;
			for (int i = BayesNet.variables[var_num].parents.size(); i < tokens.size(); i += 2)//read var values and prob and insert to cpt
			{
				int value_num = BayesNet.variables[var_num].get_value_num(remove_equal_sign(tokens[i]));
				/*int prob_cpt_index = BayesNet.variables[var_num].get_prob_cpt_index(value_num);*/
				BayesNet.variables[var_num].cpt[value_index][value_num] = stof(tokens[i + 1]);
				prob_sum += BayesNet.variables[var_num].cpt[value_index][value_num];
			}
			for (int i = 0; i < BayesNet.variables[var_num].cpt[value_index].size(); i++)//compute the last probability that is not explictly in the cpt table
			{
				if (BayesNet.variables[var_num].cpt[value_index][i] < 0)
					BayesNet.variables[var_num].cpt[value_index][i] = 1 - prob_sum;
			}
		}
	}
		





		//for (int i = 0; i < BayesNet.variables[var_num].parents.size(); i++)//one dimention for each parent and one for this var itself 
		//{
		//	int parent_num = BayesNet.variables[var_num].parents;
		//	int parent_values = 
		//	vector<float> vect(n, 0);
		//}

	
	return 0;
}

ConditionalData read_Query(string line, BayesianNetwork &BayesNet)
{
	ConditionalData query = ConditionalData();
	vector<string> tokens;
	//remove unneccery signs:
	line = replace_substr(line, "(", " ");
	line = replace_substr(line, ")", " ");
	line = replace_substr(line, "|", " ");
	line = replace_substr(line, "=", " ");
	tokens = split_line(line);


	query.Q.var = BayesNet.get_var_num(tokens[1]);//first token is "P" therfore save from [1]
	query.Q.value = BayesNet.variables[query.Q.var].get_value_num(tokens[2]);//value
	for (int i = 3; i < tokens.size() - 2; i += 2)//save evidences, up to algorithm type
	{
		Event Evidence = Event();
		Evidence.var = BayesNet.get_var_num(tokens[i]);
		Evidence.value = BayesNet.variables[Evidence.var].get_value_num(tokens[i + 1]);
		query.E_vec.push_back(Evidence);
	}

	query.algorithm_type = stoi(tokens[tokens.size() - 1]);


	return query;
}

vector<string> read_data_from_file(string input_file_name)
{
	string line;
	vector<string> line_buffer;
	fstream in_file;
	in_file.open(input_file_name);
	if (in_file.is_open())
	{
		string line;
		while (getline(in_file, line))
		{
			line_buffer.push_back(line);
		}
		cout << "Input data was loaded\n";
		in_file.close();
	}
	else
	{
		cout << "cannot open file: " << input_file_name << endl;
		pause_exit();
	}
	return line_buffer;
}
void load_data(BayesianNetwork & BayesNet, vector<ConditionalData> & queries, string input_file_name)
{
	int line_index = 0;
	vector<string> line_buffer = read_data_from_file(input_file_name);
	vector<string> tokens;
	string line;

	//find "Network"
	
	line = run_to_line(line_buffer, line_index, "Network");
	if (line == "error")
	{
		cout << "error - No Network found in file\n";
		pause_exit();
	}
	else
		cout << "Network input\n";

	line = run_to_line(line_buffer, line_index, "Variables");
	if (line == "error")
	{
		cout << "error - No Variables found in file\n";
		pause_exit();
	}
	tokens = split_line(line);
	Var variable = Var();
	for (int i = 1; i < tokens.size(); i++)
	{
		variable.name = tokens[i];
		BayesNet.variables.push_back(variable);//allocate all variables and save their names
	}


	//read variables until end

	read_variable_data(line_buffer, line_index, BayesNet);



	//find "Queries"
	line = run_to_line(line_buffer, line_index, "Queries");
	if (line == "error")
	{
		cout << "error - No Queries found in file\n";
		pause_exit();
	}





	//read queries until end
	line_index++;
	for(line_index; line_index<line_buffer.size();line_index++)
	{
		if (line_buffer[line_index] == "")//read until an empty line
			break;
		queries.push_back(read_Query(line_buffer[line_index], BayesNet));
	}

	return;
}

void save_data(string output_file,vector<string> answers)
{
	ofstream out_file;
	out_file.open(output_file);
	for (int i = 0; i < answers.size(); i++)
	{
		out_file << answers[i] << endl;
	}
	cout << "Answers to queries saved to " << output_file << endl;

}