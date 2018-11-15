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
	map <string, double> Nodes_delays;

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



struct out_branch_slew{
	string start;
	string end;
	double slew;
};


vector <out_branch> out_branches;
vector <out_branch_slew> out_branches_slews;
map<string, double> wire_capacitances;

vector <Port> Ports;
vector <Edge> Edges_copy;
map<string, double> delays;
multimap <string, Edge> Edges_mapping;
map <string, string> Name_mapping;
vector<NET> All_Nets;
double time_unit;
double res_copy;
bool first = true;
double delay_copy;
double get_c = 0;
bool name_mapping_exists = false;

void get_cap(NET &wire, string new_edge)

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

void get_delay(NET &wire, string new_edge, double res_so_far, double delay_so_far, string f, map <string, double> &Nodes_delays_copy)
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
		cout << last << " " << delay_so_far << endl;
		out_branch n_branch;
		n_branch.start = r;
		n_branch.end = t;
		Nodes_delays_copy[t] = delay_so_far;
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
			double cap = atof(wire.Net_Nodes[d].c_str());
			if (first){
				first = false;
				string r_2 = "";
				if (name_mapping_exists)
				{
					int i = 0;
					for (i = 0; i < d.length(); i++)
					{
						if (d[i] == ':')
							break;
						else r_2 += d[i];
					}

					r_2 = Name_mapping[r_2];


					for (i = i; i < d.length(); i++)
					{
						r_2 += d[i];
					}

				}
				else
					r_2 = d;

				Nodes_delays_copy[r_2] = r * cap;
				get_delay(wire, d, r, (r * cap), d, Nodes_delays_copy);
			}
			else{
				string r_2 = "";

				if (name_mapping_exists)
				{
					int i = 0;
					for (i = 0; i < d.length(); i++)
					{
						if (d[i] == ':')
							break;
						else r_2 += d[i];
					}
					r_2 = Name_mapping[r_2];
					for (i = i; i < d.length(); i++)
					{
						r_2 += d[i];
					}
				}
				else
					r_2 = d;


				Nodes_delays_copy[r_2] = delay_so_far + ((res_so_far + r) * cap);
				get_delay(wire, d, res_so_far + r, delay_so_far + ((res_so_far + r) * cap), d, Nodes_delays_copy);



			}

		}
	}


	else
	{
		Edges_copy.clear();

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

				get_delay(wire, d, r, (r * cap_1) + (r * cap), d, Nodes_delays_copy);
			}
			else
				get_delay(wire, d, res_so_far + r, delay_so_far + ((res_so_far + r) * cap_1) + (res_so_far * cap), d, Nodes_delays_copy);
		}

	}
}



void get_cap_slew(NET &wire, string new_edge)

{
	typedef std::multimap<string, Edge>::iterator MMAPIterator;
	// It returns a pair representing the range of elements with key equal to 'c'
	std::pair<MMAPIterator, MMAPIterator> result = Edges_mapping.equal_range(new_edge);
	int count = distance(result.first, result.second);
	if (count == 0)
	{
		get_c = get_c + atof(wire.Net_Nodes[new_edge].c_str());



		string r_2 = "";

		if (name_mapping_exists)
		{
			int i = 0;
			for (i = 0; i < new_edge.length(); i++)
			{
				if (new_edge[i] == ':')
					break;
				else r_2 += new_edge[i];
			}
			r_2 = Name_mapping[r_2];
			for (i = i; i < new_edge.length(); i++)
			{
				r_2 += new_edge[i];
			}
		}
		else
			r_2 = new_edge;



		get_c = get_c * wire.Nodes_delays[r_2];
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
			get_c = get_c * wire.Nodes_delays[s];

			get_cap_slew(wire, d);


		}
	}


}




void get_slew(NET &wire, string new_edge, double res_so_far, double slew_so_far, string f)
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
		out_branch_slew n_branch;
		n_branch.start = r;
		n_branch.end = t;
		n_branch.slew = slew_so_far;
		bool exists = false;
		for (int i = 0; i < out_branches_slews.size(); i++)
		{
			if (out_branches_slews[i].start == n_branch.start && out_branches_slews[i].end == n_branch.end)
				exists = true;

		}
		if (!exists)
		out_branches_slews.push_back(n_branch);
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
			double cap = atof(wire.Net_Nodes[d].c_str());


			string temp = "";
			if (name_mapping_exists)
			{
				int i = 0;
				for (i = 0; i < d.length(); i++)
				{
					if (d[i] == ':')
						break;
					else temp += d[i];
				}
				
				temp = Name_mapping[temp];
				for (i = i; i < d.length(); i++)
				{
					temp += d[i];
				}


			}

			else{
				temp = d;
			}



			double slew = wire.Nodes_delays[temp];
			if (first){
				first = false;
				string r_2 = "";
				if (name_mapping_exists)
				{
					int i = 0;
					for (i = 0; i < d.length(); i++)
					{
						if (d[i] == ':')
							break;
						else r_2 += d[i];
					}

					r_2 = Name_mapping[r_2];


					for (i = i; i < d.length(); i++)
					{
						r_2 += d[i];
					}

				}
				else
					r_2 = d;

				get_slew(wire, d, r, (r * cap * slew), d);
			}
			else{
				string r_2 = "";

				if (name_mapping_exists)
				{
					int i = 0;
					for (i = 0; i < d.length(); i++)
					{
						if (d[i] == ':')
							break;
						else r_2 += d[i];
					}
					r_2 = Name_mapping[r_2];
					for (i = i; i < d.length(); i++)
					{
						r_2 += d[i];
					}
				}
				else
					r_2 = d;


				get_slew(wire, d, res_so_far + r, slew_so_far + ((res_so_far + r) * cap * slew), d);



			}

		}
	}


	else
	{
		Edges_copy.clear();

		for (MMAPIterator it = result.first; it != result.second; it++)
		{

			Edge x = it->second;
			bool exi = false;
			for (int i = 0; i < Edges_copy.size(); i++)
			{
				if (Edges_copy[i].source == x.source && Edges_copy[i].destination == x.destination)
					exi = true;

			}
			if (!exi)
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
					get_cap_slew(wire, d);
					get_c = get_c;
				}
			}

			Edge x = Edges_copy[i];
			string s = x.source;
			string d = x.destination;
			double r = atof(x.resistance.c_str());
			double cap_1 = atof(wire.Net_Nodes[d].c_str());




			string r_2 = "";

			if (name_mapping_exists)
			{
				int i = 0;
				for (i = 0; i < d.length(); i++)
				{
					if (d[i] == ':')
						break;
					else r_2 += d[i];
				}
				r_2 = Name_mapping[r_2];
				for (i = i; i < d.length(); i++)
				{
					r_2 += d[i];
				}
			}
			else
				r_2 = d;







			double slew = wire.Nodes_delays[r_2];

			double cap = get_c;
			if (first){
				first = false;

				get_slew(wire, d, r, (r * cap_1) + (r * cap), d);
			}
			else
				get_slew(wire, d, res_so_far + r, slew_so_far + ((res_so_far + r) * cap_1 * slew + (res_so_far * cap)), d);
		}

	}

}














void parse_and_get_delay()
{
	ifstream infile; // input file
	infile.open("crc32d16N.spef"); // open it


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
				map <string, double> Nodes_delays_copy;
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
				wire_capacitances[new_net.name] = atof(z.c_str());
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
						bool found = false;
						z = y;
						if (x == "*P"){
							if (a == "I"){
								start = z;
								found = true;
							}

						}
						else if (x == "*I"){
							if (a == "O"){
								start = z;
								found = true;
							}
						}
						if (found){
							branch new_branch;
							new_branch.start = start;
							new_branch.end = z;
							new_net.Net_Branches.push_back(new_branch);
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
							bool found = false;

							if (x == "*P"){
								if (a == "I"){
									start = z;
									found = true;
								}
							}
							else if (x == "*I"){
								if (a == "O"){
									start = z;
									found = true;
								}
							}
							if (found){
								new_branch.start = start;
								new_branch.end = z;
								new_net.Net_Branches.push_back(new_branch);
							}
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
				string h = new_net.Net_Branches[0].start;


				string r_2 = "";
				if (name_mapping_exists)
				{
					int i = 0;
					for (i = 0; i < h.length(); i++)
					{
						if (h[i] == ':')
							break;
						else r_2 += h[i];
					}
					r_2 = Name_mapping[r_2];
					for (i = i; i < h.length(); i++)
					{
						r_2 += h[i];
					}
				}
				else
					r_2 = h;

				Nodes_delays_copy[r_2] = 0.0;
				get_delay(new_net, new_net.Net_Branches[0].start, 0, 0, "", Nodes_delays_copy);
				new_net.Nodes_delays = Nodes_delays_copy;
				All_Nets.push_back(new_net);
				get_slew(new_net, new_net.Net_Branches[0].start, 0, 0, "");

			}
		}
	}
	else {
		cout << "Cannot open input file!\n";
	}
}

















int main(){
	parse_and_get_delay();
	cout << "=====================DELAYS=======================" << endl;
	map<string, double>::iterator it;
	for (it = delays.begin(); it != delays.end(); it++)
	{
		cout << it->first << "  delay = " << fixed << it->second  << " PS" << endl;
	}


	/*
	cout << "====================PORTS=========================" << endl;
	for (int i = 0; i < Ports.size(); i++)
	{
		cout << "Port Name " << Ports[i].name << " Type " << Ports[i].type << " Load " << Ports[i].load << endl;
	}

	cout << "====================Nodes Delays=========================" << endl;

	map<string, double>::iterator it_2;
	NET x = All_Nets[0];
	for (int i = 0; i < All_Nets.size(); i++)
	{

		cout << "NET " << All_Nets[i].name << endl;
		for (it_2 = All_Nets[i].Nodes_delays.begin(); it_2 != All_Nets[i].Nodes_delays.end(); it_2++)
		{
			cout << it_2->first << "  delay = " << fixed << it_2->second * time_unit << " PS" << endl;
		}
		cout << "---------" << endl;

	}
	
	*/
	cout << "====================Slews=========================" << endl;
	int x = out_branches_slews.size();
	for (int i = 0; i < out_branches_slews.size(); i++)
		cout << out_branches_slews[i].start << " -----------------> " << out_branches_slews[i].end << "  Slew =  " << out_branches_slews[i].slew << endl;

	////////////////////////////////////////////////////

	cout << "====================Wire Capacitances=========================" << endl;


	map<string, double>::iterator it_cap;
	for (it_cap = wire_capacitances.begin(); it_cap != wire_capacitances.end(); it_cap++)
	{
		cout << it_cap->first << "  C = " << fixed << it_cap->second << endl;
	}

	system("pause");
	return 0;
}
