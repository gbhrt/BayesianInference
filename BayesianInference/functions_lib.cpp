#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <sstream> // stringstream
#include <iomanip> // setprecision

#include "header.h"

using namespace std;

string NumberToString(float Number)//becuase it use only c++98
{
	ostringstream ss;
	ss << Number;
	return ss.str();
}

float StringToFloat(const string &Text)//becuase it use only c++98
{
	istringstream ss(Text);
	float result;
	return ss >> result ? result : 0;
}
int StringToInt(const string &Text)//becuase it use only c++98
{
	istringstream ss(Text);
	float result;
	return ss >> result ? result : 0;
}
void pause_exit()
{
	cout << "press any key to exit the program" << endl;
	getchar();
	//exit(1);
}

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


string create_answer(float prob, int add_count, int multiply_count)//combine probability, add count and multiply count to an answer string
{
	string answer_str;
	stringstream stream;
	stream << fixed << setprecision(5) << prob;//5 points precision after the point
	answer_str = stream.str();
	answer_str.append(",");
	answer_str.append(NumberToString(add_count));
	answer_str.append(",");
	answer_str.append(NumberToString(multiply_count));

	cout << "answer: " << answer_str << endl;
	return answer_str;
}
