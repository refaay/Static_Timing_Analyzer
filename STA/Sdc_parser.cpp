#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;


double period;
double Input_delay;
double Output_delay;
double Input_trans;
double load;

void parse_sdc()
{
	ifstream infile; // input file
	infile.open("test.sdc"); // open it


	if (!infile.fail())
	{ // opened
		while (!infile.eof())
		{ // till end
			string line, x, y, z, a, b, c; // from file, from stringstream
			getline(infile, line); // a line from the blif
			stringstream line2;
			line2 << line; // to stringstream
			line2 >> x; // separate

			if (x == "create_clock"){
				line2 >> y;
				line2 >> z;
				period = atof(z.c_str());
			}

			else if (x == "set_input_delay")

			{
				line2 >> y;
				line2 >> z;
				line2 >> a;
				Input_delay = atof(a.c_str());;


			}

			else if (x == "set_output_delay")

			{
				line2 >> y;
				line2 >> z;
				line2 >> a;
				Output_delay = atof(a.c_str());;


			}

			else if (x == "set_input_transition")

			{
				line2 >> y;
				line2 >> z;
				line2 >> a;
				Input_trans = atof(a.c_str());;


			}


			else if (x == "set_load")

			{
				line2 >> y;

				load = atof(y.c_str());;


			}

		}


	}
}
