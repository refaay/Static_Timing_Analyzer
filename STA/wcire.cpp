#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

using namespace std;

struct branch{
	string start;
	string end;
	string type;
	double delay;
};

struct Edge{
	string source;
	string destination;
	string resistance;
};

struct NET{
	string name;
	string total_capacitance;
	vector <branch> Net_Branches;
	map <string, string> Net_Nodes;
	vector <Edge> Net_Edges;
	double delay_unit;
};


struct Port{
	string name;
	string type;
	double load;
};


struct out_branch{
	string start;
	string end;
	double delay;
};

vector <out_branch> out_branches;
vector <Port> Ports;
vector <Edge> Edges_copy;;
map<string, double> delays;
multimap <string, Edge> Edges_mapping;
map <string, string> Name_mapping;

double time_unit;
double res_copy;
bool first = true;
double delay_copy;
double get_c = 0;
bool name_mapping_exists = false;

void get_cap(NET wire, string new_edge)

{
	typedef std::multimap<string, Edge>::iterator MMAPIterator;
	// It returns a pair representing the range of elements with key equal to 'c'
	std::pair<MMAPIterator, MMAPIterator> result = Edges_mapping.equal_range(new_edge);
	int count = distance(result.first, result.second);
	if (count == 0)
	{
		get_c = get_c + atof(wire.Net_Nodes[new_edge].c_str());
		return;
	}
	else
	{
		for (MMAPIterator it = result.first; it != result.second; it++)
		{

			Edge x = it->second;
			string s = x.source;
			string d = x.destination;
			get_c = get_c + atof(wire.Net_Nodes[s].c_str());


			get_cap(wire, d);


		}
	}


}

void get_delay(NET wire, string new_edge, double res_so_far, double delay_so_far, string f)
{

	typedef std::multimap<string, Edge>::iterator MMAPIterator;

	// It returns a pair representing the range of elements with key equal to 'c'
	std::pair<MMAPIterator, MMAPIterator> result = Edges_mapping.equal_range(new_edge);

	int count = distance(result.first, result.second);

	if (count == 0)
	{
		string n = wire.Net_Branches[0].start;
		string r = "";
		string t = "";
		if (name_mapping_exists)
		{
			int i = 0;
			for (i = 0; i < n.length(); i++)
			{
				if (n[i] == ':')
					break;
				else r += n[i];
			}
			int j = 0;
			for (j = 0; j < f.length(); j++)
			{
				if (f[j] == ':')
					break;
				else t += f[j];
			}
			t = Name_mapping[t];
			r = Name_mapping[r];
			for (i = i; i < n.length(); i++)
			{
				r += n[i];
			}

			for (j = j; j < f.length(); j++)
			{
				t += f[j];
			}

		}

		else{
			t = f;
			r = n;
		}
		//cout << "======================================"<< endl;

		string last = "NET " + wire.name + "\n" + "Start: " + r + " ---->" + "End: " + t;
		//cout << last << " " << delay_so_far << endl;
		out_branch n_branch;
		n_branch.start = r;
		n_branch.end = t;
		n_branch.delay = delay_so_far;
		out_branches.push_back(n_branch);
		delays[last] = delay_so_far;
		//cout << "======================================" << endl;

		return;
	}

	else if (count == 1)
	{


		for (MMAPIterator it = result.first; it != result.second; it++)
		{

			Edge x = it->second;
			string s = x.source;
			string d = x.destination;
			double r = atof(x.resistance.c_str());
			//res_so_far += r;
			double cap = atof(wire.Net_Nodes[d].c_str());
			//double cap = get_cap(wire, d, 0);
			//delay_so_far += res_so_far * cap;
			if (first){
				first = false;

				get_delay(wire, d, r, (r * cap), d);
			}
			else
				get_delay(wire, d, res_so_far + r, delay_so_far + ((res_so_far + r) * cap), d);

		}
	}


	else
	{
		for (MMAPIterator it = result.first; it != result.second; it++)
		{
			Edge x = it->second;
			Edges_copy.push_back(x);

		}


		for (int i = 0; i < Edges_copy.size(); i++)
		{

			get_c = 0;

			for (int j = 0; j < Edges_copy.size(); j++)
			{
				if (i != j){
					Edge x = Edges_copy[j];
					string s = x.source;
					string d = x.destination;
					get_cap(wire, d);
					get_c = get_c;
				}
			}

			Edge x = Edges_copy[i];
			string s = x.source;
			string d = x.destination;
			double r = atof(x.resistance.c_str());
			double cap_1 = atof(wire.Net_Nodes[d].c_str());

			double cap = get_c;
			if (first){
				first = false;

				get_delay(wire, d, r, (r * cap_1) + (r * cap), d);
			}
			else
				get_delay(wire, d, res_so_far + r, delay_so_far + ((res_so_far + r) * cap_1) + (res_so_far * cap), d);
		}
	}
}


void parse_and_get_delay()
{
	ifstream infile; // input file
	infile.open("post-par (SPEF).spef"); // open it


	if (!infile.fail())
	{ // opened
		while (!infile.eof())
		{ // till end
			string line, x, y, z, a, b, c; // from file, from stringstream
			getline(infile, line); // a line from the blif
			stringstream line2;
			line2 << line; // to stringstream
			line2 >> x; // separate
			if (line == "*NAME_MAP")
			{

				name_mapping_exists = true;
				do {
					stringstream line2; // between file and separate strings
					getline(infile, line); // a line from the blif
					if (line != "*PORTS" && line.size() > 2)
					{
						line2 << line; // to stringstream
						line2 >> x; // separate
						line2 >> y; // separate strings
						Name_mapping[x] = y;
					}

				} while (line != "*PORTS");
			}

			if (line == "*PORTS")
			{


				while (getline(infile, line))
				{
					if (line.empty())
						break;
					stringstream line2;
					line2 << line; // to stringstream
					line2 >> x; // separate
					line2 >> y; // separate
					line2 >> z; // separate
					line2 >> a; // separate

					Port new_port;
					if (name_mapping_exists)
						new_port.name = Name_mapping[x];
					else
						new_port.name = x;
					new_port.type = y;
					new_port.load = atof(a.c_str());
					Ports.push_back(new_port);
				}

			}


			else if (x == "*C_UNIT")
			{
				stringstream line2;
				string new_line;
				line2 << line; // to stringstream
				line2 >> x; // separate
				line2 >> y; // separate
				line2 >> z; // separate
				getline(infile, new_line); // a line from the blif
				stringstream line3;
				line3 << new_line; // to stringstream
				line3 >> a; // separate
				line3 >> b; // separate
				line3 >> c; // separate
				if (z == "FF")
				{
					if (c == "OHM")
						time_unit = 1.0 / 1000.0;
					else if (c == "KOHM")
						time_unit = 1.0;
					else if (c == "MOHM")
						time_unit = 1000.0;

				}

				else 	if (z == "PF")
				{

					if (c == "OHM")
						time_unit = 1.0;
					else if (c == "KOHM")
						time_unit = 1000.0;
					else if (c == "MOHM")
						time_unit = 1000000.0;

				}


			}


			else if (x == "*D_NET")
			{
				NET new_net;
				stringstream line2;
				line2 << line; // to stringstream
				line2 >> x; // separate
				line2 >> y; // separate
				line2 >> z; // separate
				if (name_mapping_exists)
					new_net.name = Name_mapping[y];
				else
					new_net.name = y;

				new_net.total_capacitance = z;
				do{
					getline(infile, line); // a line from the blif

				} while (line != "*CONN");
				if (line == "*CONN")
				{

					/*CONN SECTION */
					string start;
					getline(infile, line); // a line from the blif
					if (line != "*CAP" && line.size() > 2)
					{
						stringstream line2;
						line2 << line; // to stringstream
						line2 >> x; // separate
						line2 >> y; // separate
						line2 >> a; // separate

						z = y;
						if (x == "*P"){
							if (a == "I")
							start = z;
						}
						else if (x == "*I"){
							if (a == "O")
								start = z;
						}

					}

					do {

						getline(infile, line); // a line from the blif
						if (line != "*CAP" && line.size() > 2)
						{
							stringstream line2;
							line2 << line; // to stringstream
							line2 >> x; // separate
							line2 >> y; // separate
							line2 >> a;
							branch new_branch;
							z = y;
							if (x == "*P"){
								if (a == "I")
								start = z;
							}
							else if (x == "*I"){
								if (a == "O")
									start = z;
							}
							new_branch.start = start;
							new_branch.end = z;
							new_net.Net_Branches.push_back(new_branch);
						}

					} while (line != "*CAP");

				}

				if (line == "*CAP")
				{
					/* CAP SECTION*/
					getline(infile, line); // a line from the blif
					if (line != "*RES" && line.size() > 2)
					{
						stringstream line2;
						line2 << line; // to stringstream
						line2 >> x; // separate
						line2 >> y; // separate
						line2 >> z;

						new_net.Net_Nodes[y] = z;


					}

					do {

						getline(infile, line); // a line from the blif
						if (line != "*RES" && line.size() > 2)
						{
							stringstream line2;
							line2 << line; // to stringstream
							line2 >> x; // separate
							line2 >> y; // separate
							line2 >> z;
							if (z.at(0) != '*')
							{
								new_net.Net_Nodes[y] = z;

							}
						}

					} while (line != "*RES");
				}

				if (line == "*RES")
				{
					/* RES SECTION*/
					getline(infile, line); // a line from the blif
					if (line != "*END" && line.size() > 2)
					{
						stringstream line2;
						string w;
						line2 << line; // to stringstream
						line2 >> x; // separate
						line2 >> y; // separate
						line2 >> z;
						line2 >> w;

						Edge new_edge;
						new_edge.source = y;
						new_edge.destination = z;
						new_edge.resistance = w;
						//Edges_mapping[y] = new_edge;
						Edges_mapping.insert(make_pair(y, new_edge));
						new_net.Net_Edges.push_back(new_edge);
					}

					do {

						getline(infile, line); // a line from the blif
						if (line != "*END" && line.size() > 2)
						{
							string w;
							stringstream line2;
							line2 << line; // to stringstream
							line2 >> x; // separate
							line2 >> y; // separate
							line2 >> z;
							line2 >> w;
							Edge new_edge;
							new_edge.source = y;
							new_edge.destination = z;
							new_edge.resistance = w;
							//Edges_mapping[y] = new_edge;
							Edges_mapping.insert(make_pair(y, new_edge));

							new_net.Net_Edges.push_back(new_edge);

						}

					} while (line != "*END");
				}



				get_delay(new_net, new_net.Net_Branches[0].start, 0, 0, "");
			}
		}
	}
	else {
		cout << "Cannot open input file!\n";
	}
}

// int main(){
// 	parse_and_get_delay();
// 	cout << "=====================DELAYS=======================" << endl;
// 	map<string, double>::iterator it;
// 	for(int i = 0; i < out_branches.size();i++)
// 		cout << out_branches[i].start << " " << out_branches[i].end << " " <<out_branches[i].delay*time_unit <<endl;
// 	return 0;
// }
