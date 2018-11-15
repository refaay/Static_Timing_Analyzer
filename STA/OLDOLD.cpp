#include<bits/stdc++.h>
#include "Lotfy.cpp"
#include "lib.cpp"
#include "Yahia.cpp"
#include "Main.cpp"
#include "Sdc_parser.cpp"
using namespace std;

string moduleName;

struct node {
  string name = "";
  string lib = "";
  string inst;
  double AAT = 0;
  double RAT = 99999999;
  double slack = 0;
  double early_slack = 0;
  double early_AAT = 999999;
  double early_RAT = 0;
  double CL = 0;
  bool aatFlag;
  bool ratFlag;
  bool visit = true;
  bool done = true;
  vector<tuple<node*,double,double,double,double>> prev;\
   //node,delay,slew,early_dalay,early_slew
  vector<tuple<node*,double,double,double,double>> next;\
   //node,delay,slew,early_dalay,early_slew
};

vector<pair<double,vector<node*>>> all_paths;

map<string,node*> m; //Inputs & Wires & Outputs
vector<node*> inp,oup; //Inputs & Outputs of the module
map<string,vector<node*>> vi; //Input of Gates to link with the output
map<string,vector<node*>> vo; //Output of Gates to Link with the input
vector<node*> vn,vm; //Vector extending the outputs and input of one instance
map <string,map<string,node*>> ionn; //mappers to nodes
vector<string> wo; // Output Gate to wire identifier
int inputSlew = 33;

int gateLevel;

void lineHandler(string str)
{
  string word;
  bool cells = true;
  word = str.substr(0,str.find_first_of(" ("));
  int c = 0;
  //Mapper of .V File line conditions
  if(word == "module")
    {
      c = 1;
      cells = false;
    }
    else if (str.find("input") != -1)
      {
         str.erase(0,str.find("input")+6);
         c = 2;
         cells = false;
      }
    else if(str.find("output") != -1)
      {
        str.erase(0,str.find("output")+7);
        c = 3;
        cells = false;
      }
    else if(str.find("wire ") != -1)
      {
        str.erase(0,str.find("wire ")+5);
        c = 4;
        cells = false;
      }
    else if(str.find("cells\n") != -1)
      {
        str.erase(0,str.find("cells\n")+6);
        cells = true;
        c = 5;
      }
    else if(str.find("endmodule") != -1)
      c = 6;
    else if(cells)
      {
        str.erase(0,1);
        c = 5;
      }
  switch (c) {
    case 1: //Module Name extraction
          {
            str.erase(0,str.find(word)+word.length()+1);
            moduleName = str.substr(0,str.find_first_of(" ("));
            cout << moduleName << " " << moduleName.length() << endl;
            node* so = new node();
            so->name = "so";
            m[so->name] = so;
            node* si = new node();
            si->name = "si";
            m[si->name] = si;
            break;
          }
    case 2: //Inputs Creations
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* in = new node();
              in -> name = word;
              in -> RAT = 999999;
              in->aatFlag = false;
              in ->ratFlag = false;
              m[in->name] = in;
              node* si = m["si"];
              tuple<node*,double,double,double,double> t;
              get<0> (t) = si;
              in->prev.push_back(t);
              get<0> (t) = in;
              si->next.push_back(t);
              inp.push_back(in);
              cout << "Input: " << m[in->name]->name << " " << m[in->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    case 3: //Outputs Creations
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* out = new node();
              out -> name = word;
              out -> RAT = 999999;
              out ->aatFlag = false;
              out -> ratFlag = false;
              m[out->name] = out;
              node* so = m["so"];
              tuple<node*,double,double,double,double> t;
              get<0> (t) = so;
              out->next.push_back(t);
              get<0> (t) = out;
              so->prev.push_back(t);
              oup.push_back(out);
              cout << "Output: " << m[out->name]->name << " " << m[out->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    case 4: //Wire nodes creation
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* wire = new node();
              wire -> name = word;
              wire -> RAT = 999999;
              wire -> aatFlag = false;
              wire -> ratFlag = false;
              m[wire->name] = wire;
              cout << "Wire: " << m[wire->name]->name << " " << m[wire->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    case 5: //Cells Handler
    {
              gateLevel++;
              string libName = str.substr(0,str.find(' '));
              str.erase(0,str.find(libName)+libName.length()+1);
              string instName = str.substr(0,str.find(' '));
              str.erase(0,str.find(instName)+instName.length()+1);
              str.erase(remove(str.begin(),str.end(),','),str.end());
              cout << "Library Name Gate: " << libName << ' ' << libName.length() << endl;
              cout << "Instance Name: " << instName << ' ' << instName.length() <<endl;
              // cout << str << ' '<< str.length() << endl;
              str.erase(0,2); // erase "( " characters from the string
              word = str.substr(0,str.find(' '));
              vn.clear();
              vm.clear();
              while(str.length() > 1)
              {
                //cout << word << word.length() << endl;
                string pin = word.substr(1,word.find('(')-1);
                //cout << pin << pin.length() << endl;
                word.erase(0,word.find('(')+1);
                string preName = word.substr(0, word.find(')'));
                //cout << preName << preName.length() << endl;
				//if (pin[0] != 'Z' && pin[0] != 'Y' && pin[0] != 'Q' && pin[0] != 'o') // pin is input
				if (lib_early[libName].lib_pin[pin].isInput) // pin is input Refaay
                 {
                   node* nn = new node();
                   nn-> name = pin;
                   nn -> lib = libName;
                   nn -> inst = instName;
                   nn -> RAT = 999999;
                   nn -> aatFlag = false;
                   nn -> ratFlag = false;
                   node* prevNode = m[preName];
                   tuple<node*,double,double,double,double> t;
                   get<0>(t) = prevNode;
                   nn->prev.push_back(t);
                   get<0>(t) = nn;
                   prevNode->next.push_back(t);
                   vn.push_back(nn);
                   ionn[nn->name][nn->inst] = nn;
                   cout << "Entry: " << nn->name << endl;
                   cout << "Previuos Node: " << prevNode->name << endl;
                   if(lib_early[libName].isSequential && !lib_early[libName].lib_pin[pin].isClock)
                   {
                     node* so = m["so"];
                     tuple<node*,double,double,double,double> t;
                     get<0>(t) = so;
                     nn->next.push_back(t);
                     get<0>(t) = nn;
                     so->prev.push_back(t);
                     oup.push_back(nn);
                     vm.push_back(nn);
                   }
                 }
                else
                {
                  if(m.count(preName))
                  {
                    bool notout = true;
                    node* Znode = m[preName];
                    for(int i = 0; i < oup.size();i++)
                      if(oup[i] == Znode)
                      {
                        node* fin = new node();
                        fin->name = pin;
                        fin->lib = libName;
                        fin->inst = instName;
                        fin -> RAT = 999999;
                        fin -> aatFlag = false;
                        fin -> ratFlag = false;
                        ionn[fin->name][fin->inst] = fin;
                        // vo[instName] = fin;
                        vm.push_back(fin);
                        tuple<node*,double,double,double,double> t;
                        get<0>(t) = fin;
                        Znode->prev.push_back(t);
                        get<0>(t) = Znode;
                        fin->next.push_back(t);
                        notout = false;
                        cout << "Output: " << Znode-> name << endl;
                        cout << "Gate out: " << fin->name << endl;
                      }
                    if(notout)
                    {
                      Znode->lib = libName;
                      Znode->inst = instName;
                      Znode->name = pin;
                      Znode -> RAT = 999999;
                      Znode -> aatFlag = false;
                      Znode -> ratFlag = false;
                      ionn[Znode->name][Znode->inst] = Znode;
                      wo.push_back(preName);
                      // vo[instName] = Znode;
                      vm.push_back(Znode);
                      cout << "Gate Out: " << Znode->name << endl;
                    }
                  }
                  else
                  cout << "Not included Y node !!" << endl;
                }
                 str.erase(0,str.find(' ')+1);
                 word = str.substr(0,str.find(' '));
               }
              //Keep track of all inputs and outputs belong to same instance
              vi[instName] = vn;
              vo[instName] = vm;
              cout << endl << endl;
              break;
           }
    case 6: cout << "End of .V File" << endl;
  }

}

//To test how many branches are connected to a certain end
void getBranch(string start)
{
  node * nn = m[start];
  for(int i = 0; i < nn->next.size();i++)
    {
        cout << "Branch name: " << (get<0>(nn->next[i]))-> name << " " << (get<0>(nn->next[i]))->inst << endl;
    }
}

//Setup the delays from the wcire.cpp into the physical nodes
void wireDelaySetup(vector<out_branch> ob)
{
  string start,end;
  string instNameS,instNameE,pinNameS,pinNameE;
  double d;
  node* starting;
  node* ending;
  for(int i = 0; i < ob.size(); i++)
  {
    start = ob[i].start;
    end = ob[i].end;
    d = ob[i].delay;
    bool col = true;
    cout << "Start: " << start << " " << "End: " << end << " " << "Delay: " << d << endl << endl;
    if(start.find(':') != -1)
    {
      instNameS = start.substr(0,start.find(":"));
      start.erase(0,start.find(':')+1);
      pinNameS = start;
    }
    else
    {
      pinNameS = start;
      instNameS = "";
      col = false;
    }
    if(col)
      starting = ionn[pinNameS][instNameS];
    else
      starting = m[pinNameS];
      col = true;
    if(end.find(':') != -1)
    {
      instNameE = end.substr(0,end.find(":"));
      end.erase(0,end.find(':')+1);
      pinNameE = end;
    }
    else
    {
      pinNameE = end;
      instNameE = "";
      col = false;
    }
    if(col)
      ending = ionn[pinNameE][instNameE];
    else
      ending = m[pinNameE];
        for(int i = 0; i < starting->next.size(); i++)
              if((get<0>(starting->next[i])) == ending)
                {
                  get<1>(starting->next[i]) = d; //late
                  get<3>(starting->next[i]) = d; //early
                }
            for(int i = 0; i < ending->prev.size(); i++)
              if((get<0>(ending->prev[i])) == starting)
                {
                  get<1>(ending->prev[i]) = d; //late
                  get<3>(ending->prev[i]) = d; //early
                }
  }
}

//Actual Arrival Time
double AAT(node* out)
{
  if(out->prev.size() == 0)
    return out -> AAT;
  else
    for(int i = 0; i < out->prev.size(); i++)
      // if(!(get<0>(out->prev[i]))->aatFlag)
      // {
      //   (get<0>(out->prev[i]))->aatFlag = true;
        out -> AAT = max(out -> AAT,AAT(get<0>(out->prev[i])) + get<1>(out->prev[i]));
      // }
      // else
      //     return out -> AAT = max(out->AAT,(get<0>(out->prev[i]))-> AAT) + get<1>(out->prev[i]);
      return out-> AAT;
}

double early_AAT(node* out)
{
  if(out == m["si"])
    return 0;

  if(out->prev.size() == 0)
    return out -> early_AAT;
  else
    for(int i = 0; i < out->prev.size(); i++){
      // if(!(get<0>(out->prev[i]))->aatFlag)
      // {
      //   (get<0>(out->prev[i]))->aatFlag = true;
        out -> early_AAT = min(out -> early_AAT,early_AAT(get<0>(out->prev[i])) + get<3>(out->prev[i]));
        cout << out -> early_AAT << " " << (get<0>(out->prev[i]))->early_AAT << " " << get<3>(out->prev[i])<< endl;
      }
      // }
      // else
      //     return out -> AAT = max(out->AAT,(get<0>(out->prev[i]))-> AAT) + get<1>(out->prev[i]);
      return out-> early_AAT;
}


//Required Arrival Time
double RAT(node* in)
{
  if(in->next.size() == 0)
    return in -> RAT;
  else
    for(int i = 0; i < in->next.size(); i++)
          // if(!(get<0>(in->next[i]))->ratFlag)
          // {
          //   (get<0>(in->next[i]))->ratFlag = true;
          in -> RAT = min(in->RAT,RAT(get<0>(in->next[i]))- get<1>(in->next[i]));
          // }
          // else
          //     return in -> RAT = min(in->RAT,(get<0>(in->next[i]))-> RAT) - get<1>(in->next[i]);
          return in -> RAT;
}

double early_RAT(node* in)
{
  if(in->next.size() == 0)
    return in -> early_RAT;
  else
    for(int i = 0; i < in->next.size(); i++)
          // if(!(get<0>(in->next[i]))->ratFlag)
          // {
          //   (get<0>(in->next[i]))->ratFlag = true;
          in -> early_RAT = max(in->early_RAT,early_RAT(get<0>(in->next[i]))- get<3>(in->next[i]));
          // }
          // else
          //     return in -> RAT = min(in->RAT,(get<0>(in->next[i]))-> RAT) - get<1>(in->next[i]);
          return in -> early_RAT;
}


string get_inst_name(string x)
{
	string r = "";
	for (int i = 0; i < x.length(); i++)
	{
		if (x[i] != ':')
			r += x[i];
		else
			break;
	}
	return r;


}



string get_pin_name(string x)
{
	string r = "";
	int i = 0;
	for (i = 0; i < x.length(); i++)
	{
		if (x[i] == ':')
			break;
	}
	for (int j = i+1; j < x.length(); j++)
	{
		r += x[j];
	}
	return r;


}


void CL_calculate()
{
	for (int i = 0; i < wo.size(); i++)
	{
		node * master = m[wo[i]];
		double cap = wire_capacitances[wo[i]];
		string n = master->inst + ':' + master->name;
		double c = 0;
		for (int j = 0; j < out_branches.size(); j++)
		{
			if (out_branches[j].start == n){

				string inst_name = get_inst_name(out_branches[j].end);
				string pin = get_pin_name(out_branches[j].end);
				string lib_name = ionn[pin][inst_name]->lib;
				c += lib_early[lib_name].lib_pin[pin].capacitance;

			}

		}
		master->CL = cap + c;
	}

}



struct out_branch_slew_final{
	string start;
	string end;
	double slew;
};

vector<out_branch_slew_final> out_branches_slews_final;

void calculate_slew()
{
	for (int i = 0; i < out_branches.size(); i++)
	{
		for (int j = 0; j < out_branches_slews.size(); j++)
		{
			if (out_branches[i].start == out_branches_slews[j].start &&  out_branches[i].end == out_branches_slews[j].end)
			{
				out_branch_slew_final new_branch_slew;
				new_branch_slew.start = out_branches[i].start;
				new_branch_slew.end = out_branches[i].end;
				new_branch_slew.slew = fmax(sqrt(2 *out_branches_slews[j].slew - pow((out_branches[i].delay), 2)), 0);

				out_branches_slews_final.push_back(new_branch_slew);

			}

		}


	}
}

//Setup the delays from the wcire.cpp into the physical nodes
void wireSlewSetup(vector<out_branch_slew_final> ob)
{
  string start,end;
  string instNameS,instNameE,pinNameS,pinNameE;
  double s;
  node* starting;
  node* ending;
  for(int i = 0; i < ob.size(); i++)
  {
    start = ob[i].start;
    end = ob[i].end;
    ob[i].slew = sqrt(pow(ob[i].slew,2)+pow(inputSlew,2));
    s = ob[i].slew;
    bool col = true;
    cout << "Start: " << start << " " << "End: " << end << " " << "Slew: " << s << endl << endl;
    if(start.find(':') != -1)
    {
      instNameS = start.substr(0,start.find(":"));
      start.erase(0,start.find(':')+1);
      pinNameS = start;
    }
    else
    {
      pinNameS = start;
      instNameS = "";
      col = false;
    }
    if(col)
      starting = ionn[pinNameS][instNameS];
    else
      starting = m[pinNameS];
      col = true;
    if(end.find(':') != -1)
    {
      instNameE = end.substr(0,end.find(":"));
      end.erase(0,end.find(':')+1);
      pinNameE = end;
    }
    else
    {
      pinNameE = end;
      instNameE = "";
      col = false;
    }
    if(col)
      ending = ionn[pinNameE][instNameE];
    else
      ending = m[pinNameE];
        for(int i = 0; i < starting->next.size(); i++)
              if((get<0>(starting->next[i])) == ending)
                {
                  get<2>(starting->next[i]) = s;
                  get<4>(starting->next[i]) = s;
                }
            for(int i = 0; i < ending->prev.size(); i++)
              if((get<0>(ending->prev[i])) == starting)
                {
                  get<2>(ending->prev[i]) = s;
                  get<4>(ending->prev[i]) = s;
                }
  }
}

void connectInOut()
{
  double d,s,e_d,e_s;
  for(map<string,vector<node*>>::iterator it = vi.begin(); it != vi.end(); ++it)
    for(int i = 0; i < it->second.size(); i++)
    {
      vector<node*> yvector = vo[it->first];
      for(int y = 0; y < yvector.size(); y++)
        if(lib_early[it->second[i]->lib].lib_timing[it->second[i]->name].count(yvector[y]->name)){
            /*d = calc(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL, false, true, true); //getdelay();
            s = calc(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL, false, true, false); //getslew();
            e_d = calc(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL,  true, true, true);
            e_s =calc(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL, true, true, false);*/
            d = getdelay_late(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL); //getdelay();
            s = getslew_late(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL); //getslew();
            e_d = getdelay(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL);
            e_s =getslew(it->second[i]->lib,it->second[i]->name,yvector[y]->name,get<2>(it->second[i]->prev[0]),yvector[y]->CL);

            cout << "Library: " << it->second[i]->lib << " Input Name: " << it->second[i]->name << " Output Name: " << yvector[y]->name << " Slew of Previuos: " << get<2>(it->second[i]->prev[0]) << " Capacitve Load: " << yvector[y]->CL << endl << endl;
            cout << "Delay in Gate: " << e_d << " Slew in Gate: " << e_s << endl << endl;
            tuple<node*,double,double,double,double> t;
            get<0>(t) = yvector[y];
            get<1>(t) = d;
            get<2>(t) = s;
            get<3>(t) = e_d;
            get<4>(t) = e_s;
            it->second[i]->next.push_back(t);
            get<0>(t) = it->second[i];
            get<1>(t) = d;
            get<2>(t) = s;
            get<3>(t) = e_d;
            get<4>(t) = e_s;
            yvector[y]->prev.push_back(t);
            cout << "Input: " << it->second[i]->inst+":"+it->second[i]-> name << " is connected to " << "Output gate: " << yvector[y]->inst+":"+yvector[y]->name << endl << endl;
          }
    }
}

//Traverse the DAG to get the Print the pathes with corresponding values
void traverse(node* root,double d,double s,double e_d,double e_s, pair<double,vector<node*>>& first)
{
  if(root == NULL)
    return;
    first.second.push_back(root);
    if(root!=m["so"])first.first+=root->slack;
  root-> slack = root -> RAT - root -> AAT;
  root -> early_slack = root -> early_AAT - root -> early_RAT;
  cout << "Node : " << setw(8) << root->name << " Delay: " << setw(8) << d << " Slew:" << setw(8) << s <<" AAT: " << setw(8) << root -> AAT << " RAT: " << setw(8) << root-> RAT << " SLACK: " << setw(8) << root-> slack \
  <<" Early AAT: " << setw(8) << root -> early_AAT << " Early RAT: " << setw(8) << root-> early_RAT << " Early SLACK: " << setw(8) << root-> early_slack <<\
   " Early Delay: " << setw(8) << e_d << " Early Slew:" << setw(8 ) << e_s << endl << endl;
   pair<double,vector<node*>> temp;
   temp.first = first.first;
   for(int i=0; i<first.second.size(); i++){
     temp.second.push_back(first.second[i]);
   }
  for(int i = 0; i < root->next.size(); i++)
    if(i == 0)traverse((get<0>(root->next[i])),(get<1>(root->next[i])),(get<2>(root->next[i])),(get<3>(root->next[i])),(get<4>(root->next[i])), first);
    else{
      all_paths.push_back(temp);
      traverse((get<0>(root->next[i])),(get<1>(root->next[i])),(get<2>(root->next[i])),(get<3>(root->next[i])),(get<4>(root->next[i])), all_paths.back() );
    }
}


//Traverse the DAG to get the Print the pathes with corresponding values
void tra2(node* root,double d,double s,double e_d,double e_s)
{
  if(root == NULL)    return;
  if(root == m["si"]) cout << "--------------------Start of path----------------\n";
  if(root->next.size() == 0) {cout << "--------------------End of path----------------\n\n"; root->done = false; tra2(m["si"],0,inputSlew,0,inputSlew); return;}
  root->visit = false;
  //  root->done = true;
    //bool allf = true;
  root-> slack = root -> RAT - root -> AAT;
  root -> early_slack = root -> early_AAT - root -> early_RAT;
  //cout << root -> name << " " << root->next.size() << endl;
  for(int i = 0; i < root->next.size(); i++){
    if((get<0>(root->next[i]))->visit || (get<0>(root->next[i]))->done)
        {
          cout << "Node : " << setw(8) << (get<0>(root->next[i]))->name << " Delay: " << setw(8) << (get<1>(root->next[i])) << " Slew:" << setw(8) << (get<2>(root->next[i])) <<" AAT: " << setw(8) << (get<0>(root->next[i]))-> AAT << " RAT: " << setw(8) << root-> RAT << " SLACK: " << setw(8) << root-> slack \
          <<" Early AAT: " << setw(8) << (get<0>(root->next[i]))-> early_AAT << " Early RAT: " << setw(8) << (get<0>(root->next[i])) -> early_RAT << " Early SLACK: " << setw(8) << (get<0>(root->next[i]))-> early_slack <<\
           " Early Delay: " << setw(8) << (get<3>(root->next[i])) << " Early Slew:" << setw(8 ) << (get<4>(root->next[i])) << endl << endl;

           //root-> visit = false;
        //  allf = false;
          if (root -> done) tra2((get<0>(root->next[i])),(get<1>(root->next[i])),(get<2>(root->next[i])),(get<3>(root->next[i])),(get<4>(root->next[i])));
          /*if(i == root->next.size() - 1){
            root -> done = false;
          }*/
          return;
        }
        /*else if(i == root->next.size() - 1){
          root -> done = false;
          //cout << "Node : " << setw(8) << (get<0>(root->next[i]))->name << " Delay: " << setw(8) << (get<1>(root->next[i])) << " Slew:" << setw(8) << (get<2>(root->next[i])) <<" AAT: " << setw(8) << (get<0>(root->next[i]))-> AAT << " RAT: " << setw(8) << root-> RAT << " SLACK: " << setw(8) << root-> slack \
          <<" Early AAT: " << setw(8) << (get<0>(root->next[i]))-> early_AAT << " Early RAT: " << setw(8) << (get<0>(root->next[i])) -> early_RAT << " Early SLACK: " << setw(8) << (get<0>(root->next[i]))-> early_slack <<\
           " Early Delay: " << setw(8) << (get<3>(root->next[i])) << " Early Slew:" << setw(8 ) << (get<4>(root->next[i])) << endl << endl;
           //tra2((get<0>(root->next[i])),(get<1>(root->next[i])),(get<2>(root->next[i])),(get<3>(root->next[i])),(get<4>(root->next[i])));
           return;
         }*/

  }
}


void PrettyBranchTraverse(node * n,double delay,double slew)
{
  static int c = 0;
  if( n == m["so"])
    {
      cout << "Path: " << ++c << endl;
      return;
    }
  for(int i = 0; i < n->next.size(); i++)
    PrettyBranchTraverse((get<0>(n->next[i])),(get<1>(n->next[i])),(get<2>(n->next[i])));
    if(n != m["si"] && n->visit)
      {
        cout << "Node : " << n->name << " Delay: " << delay << " Slew:" << slew <<" AAT: " << n-> AAT << " RAT: " << n-> RAT << " Visited: "<< n->visit <<endl ;
        n-> visit = false;
      }

}

void tr(node * n,double delay,double slew)
{
  static int c = 0;
  if( n == m["so"])
    {
      cout << "Path: " << ++c << endl;
      return;
    }
    bool allf = true;
  for(int i = 0; i < n->next.size(); i++)
    tr((get<0>(n->next[i])),(get<1>(n->next[i])),(get<2>(n->next[i])));

  /*  if(n != m["si"] && n->visit)
      {
        cout << "Node : " << n->name << " Delay: " << delay << " Slew:" << slew <<" AAT: " << n-> AAT << " RAT: " << n-> RAT << " Visited: "<< n->visit <<endl ;
        n-> visit = false;
        allf = false;
      }
      else if(allf && i == n->next.size() - 1){
        cout << "Node : " << n->name << " Delay: " << delay << " Slew:" << slew <<" AAT: " << n-> AAT << " RAT: " << n-> RAT << " Visited: "<< n->visit <<endl ;
      }*/

}


void SetRATForOut()
{
  for(int i = 0; i < oup.size(); i++)
  {

    if(oup[i]->lib != "")
    {
      //cout << "aa";
      //cout << oup[i] -> lib << " " << oup[i] -> name << " " << lib_late[oup[i] -> lib].lib_timing["ck"].size() << endl;
      for(map<string,map<string,LibParserTimingInfo>>::iterator it = lib_late[oup[i] -> lib].lib_timing.begin(); it != lib_late[oup[i] -> lib].lib_timing.end(); ++it )
      {
        if(lib_late[oup[i] -> lib].lib_pin[it->first].isClock)
            {
              cout <<"Refaay: " << it->first << endl;
              for(int j = 0; j < oup[i]->prev.size();j++)
              {
                cout << "Noor: " << (get<0>(oup[i]->prev[j]))->name << endl;
                if((get<0>(oup[i]->prev[j]))->name == it->first)
                {
                  oup[i] -> RAT = period + (get<0>(oup[i]->prev[j])) -> early_AAT - lib_late[oup[i] -> lib].lib_timing[it->first][oup[i] -> name].riseConstraint;
                  oup[i] -> early_RAT = (get<0>(oup[i]->prev[j])) -> AAT + lib_early[oup[i] -> lib].lib_timing[it->first][oup[i] -> name].riseConstraint;
                  cout << "D RAT: " << oup[i] -> RAT << endl;
                }
              }

            }
          }
    }
    else
    {
      oup[i] -> RAT = Output_delay;
      oup[i] -> CL = load;
      cout << " OUT DELAY: " << oup[i] -> RAT << " CLOAD " << oup[i] -> CL << " NODE "  << oup[i] -> name <<endl;
    }
  }
}

void SetDelaySlewOut()
{
  for(int i = 0; i < oup.size(); i++)
  {
    node* so = m["so"];
    for(int j = 0; j < so->prev.size(); j++)
    {
      if(get<0>(so->prev[j]) == oup[i])
      {
        if(oup[i]->lib != "")
        {
          cout << "DFF Exists" << endl;
          get<1>(oup[i]->next[0]) = 25; //Delay
          get<2>(oup[i]->next[0]) = 5; //Slew
          get<1>(so->prev[j]) = 25; //Delay
          get<2>(so->prev[j]) = 5; //Slew
        }
      }
    }
  }
}

void criticalPath(node* root,double d,double s,double e_d,double e_s)
{
  double ms = root->slack;
  for(int i = 0; i < root->next.size(); i++)
    {
      if((get<0>(root->next[i]))-> slack == ms)
      {
        cout << "Node : " << setw(8) << root->name << " Delay: " << setw(8) << d << " Slew:" << setw(8) << s <<" AAT: " << setw(8) << root -> AAT << " RAT: " << setw(8) << root-> RAT << " SLACK: " << setw(8) << root-> slack \
      <<" Early AAT: " << setw(8) << root -> early_AAT << " Early RAT: " << setw(8) << root-> early_RAT << " Early SLACK: " << setw(8) << root-> early_slack <<\
       " Early Delay: " << setw(8) << e_d << " Early Slew:" << setw(8 ) << e_s << endl << endl;
       criticalPath((get<0>(root->next[i])),(get<1>(root->next[i])),(get<2>(root->next[i])),(get<3>(root->next[i])),(get<4>(root->next[i])));
     }

    }
}

bool func(pair<double,vector<node*>> a, pair<double,vector<node*>> b){
  return a.first < b.first;
}

int main()
{
	fill_lib(); // Refaay
  parse_sdc(); // Lotfy
  string fileAdd;
  string blifLine;
  gateLevel = 0;
  cout << "Enter .V File address: " ;
  cin >> fileAdd;
  ifstream x(fileAdd.c_str());
  if(!x.fail())
    while(getline(x,blifLine,';'))
    {
      lineHandler(blifLine);
    }
  else
    cout << "Invalid Address!!";

  parse_and_get_delay();
  calculate_slew();
  CL_calculate();
	cout << "End of wire setup\n";
  //Output to input linkage
  // for(map<string,vector<node*>>::iterator it = vi.begin(); it != vi.end(); ++it)
  //   for(int i = 0; i < it->second.size(); i++)
  //     {
  //       node* ynode = vo[it->first];
  //       tuple<node*,double,double,double,double> t;
  //       get<0>(t) = it->second[i];
  //       ynode->prev.push_back(t);
  //       get<0>(t) = ynode;
  //       it->second[i]->next.push_back(t);
  //     }
  wireDelaySetup(out_branches);
  wireSlewSetup(out_branches_slews_final);
  connectInOut();
  cout << "Start traversal: \n";
    //Traversal of every output
    // for(int i = 0; i < oup.size();i++)
    //   AAT(oup[i]);
    AAT(m["so"]);
    early_AAT(m["so"]);
    SetRATForOut();
    // m["si"]->RAT = 0.005;
    RAT(m["si"]);
    early_RAT(m["si"]);
    pair<double,vector<node*>> first;
    first.first = 0;
    all_paths.push_back(first);
    traverse(m["si"],0,inputSlew,0,inputSlew, all_paths.back());
    cout << "----------------------Circuit Paths --------------------\n";
    //criticalPath(m["si"],0,inputSlew,0,inputSlew);
    for(int u=0; u<all_paths.size(); u++){
      all_paths[u].first = 0;
      for(int i=0; i<all_paths[u].second.size(); i++)
        if(all_paths[u].second[i] != m["so"])all_paths[u].first+= all_paths[u].second[i]->slack;// << " (" << all_paths[u].second[i]->slack << ") ";
        /*
      while(!(all_paths[u].empty())){
        cout << " " << (all_paths[u].front())->name;
        all_paths[u].pop_back();
      }*/
      cout << endl;
    }
    sort(all_paths.begin(), all_paths.end(), func);
    for(int u=0; u<all_paths.size(); u++){
      cout << all_paths[u].first << " ";
      for(int i=0; i<all_paths[u].second.size(); i++)
        cout << all_paths[u].second[i]->name << " (" << all_paths[u].second[i]->slack << ") ";
        /*
      while(!(all_paths[u].empty())){
        cout << " " << (all_paths[u].front())->name;
        all_paths[u].pop_back();
      }*/
      cout << endl;
    }
    // tra2(m["si"],0,inputSlew,0,inputSlew);

    // PrettyBranchTraverse(m["si"],0,inputSlew);
    //Traversal of every input
    // for(int i = 0; i < inp.size();i++)
    //     {
    //       cout << "Path: " << i+1 << endl;
    //       RAT(inp[i]);
    //       traverse(inp[i],0,inputSlew);
    //       cout << endl << endl;
    //     }
  // Yahia
  // delay = getdelay("BUF_X1", "a", "o", 57.1577, 1.5); // inputs for the function
  // cout << "Delay: " << delay << endl;
  // cout << lib_early[it->second[i]->lib].lib_timing[it->second[i]->name].count(
  // CL_calculate();
  // SetRATForOut();
  // cout << "From Outputs " << endl;
  // for(int i = 0; i < oup.size(); i++)
  // {
  //     cout << "Delay: " << get<1>(oup[i]->next[0]) << " Slew: " << get<2>(oup[i]->next[0]) << endl;
  // }
  // cout << "From SO" << endl;
  // for(int j = 0; j < m["so"]->prev.size(); j++)
  // {
  //   cout << "Delay: " << get<1>(m["so"]->prev[j]) << " Slew: " << get<2>(m["so"]->prev[j]) << endl;
  // }
  return 0;
}
