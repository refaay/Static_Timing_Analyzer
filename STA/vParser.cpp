#include<bits/stdc++.h>
#include "wcire.cpp"
using namespace std;

string moduleName;

struct node {
  string name;
  string lib;
  string inst;
  double delay;
  double AAT;
  double RAT;
  double slack;
  vector<node*> prev;
  vector<node*> next;
};

map<string,node*> m;
int gateLevel;

void lineHandler(string str)
{
  string word;
  bool cells = true;
  word = str.substr(0,str.find_first_of(" ("));
  int c = 0;
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
    case 1: str.erase(0,str.find(word)+word.length()+1);
            moduleName = str.substr(0,str.find_first_of(" ("));
            cout << moduleName << " " << moduleName.length() << endl;
            break;
    case 2: //str.erase(0,str.find(word)+word.length()+1);
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* in = new node();
              in -> name = word;
              in -> delay = 1;
              m[in->name] = in;
              cout << "Input: " << m[in->name]->name << " " << m[in->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    case 3: //str.erase(0,str.find(word)+word.length()+1);
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* out = new node();
              out -> name = word;
              out -> delay = 0;
              m[out->name] = out;
              cout << "Output: " << m[out->name]->name << " " << m[out->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    case 4: //str.erase(0,str.find(word)+word.length()+1);
            word = str.substr(0,str.find('\n'));
            while(str.length() > 0)
            {
              node* wire = new node();
              wire -> name = word;
              wire -> delay = 0;
              m[wire->name] = wire;
              cout << "Wire: " << m[wire->name]->name << " " << m[wire->name]->name.length() << endl;
              str.erase(0,str.find(word)+word.length()+1);
              word = str.substr(0,str.find('\n'));
            }
            break;
    //Cells Handler
    case 5: {
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
              while(str.length() > 1)
              {
                //cout << word << word.length() << endl;
                string pin = word.substr(1,word.find('(')-1);
                //cout << pin << pin.length() << endl;
                word.erase(0,word.find('(')+1);
                string preName = word.substr(0, word.find(')'));
                //cout << preName << preName.length() << endl;
                 if(pin[0] != 'Z' && pin[0] != 'Y' && pin[0] != 'S' && pin[0] != 'Q')
                 {
                   node* nn = new node();
                   nn-> name = pin;
                   nn -> lib = libName;
                   nn -> inst = instName;
                   nn -> delay = 0;
                   node* prevNode = m[preName];
                   nn->prev.push_back(prevNode);
                   prevNode->next.push_back(nn);
                   cout << "Entry: " << nn->name << endl;
                   cout << "Previuos Node: " << prevNode->name << endl;
                 }
                else
                {
                  if(m.count(preName))
                  {
                    node* Znode = m[preName];
                    Znode->lib = libName;
                    Znode->inst = instName;
                    cout << "Y: " << Znode->name << " " << endl;
                  }
                  else
                  cout << "Not included Y node !!" << endl;
                }
                 str.erase(0,str.find(' ')+1);
                 word = str.substr(0,str.find(' '));
               }
              cout << endl << endl;
              break;
           }
    case 66: cout << "End of .V File" << endl;
  }

}

void getBranch(string start,string end)
{
  node * nn = m[start];
  for(int i = 0; i < nn->next.size();i++)
    cout << "Branch name: " << nn->next[i]->name << " " << nn->next[i]->inst << endl;
}

//TODO: Re-solve dublication of an instance in a map
void wireDelaySetup(vector<out_branch> ob)
{
  string start,end;
  string instNameS,instNameE,pinNameS,pinNameE;
  double d;
  for(int i = 0; i < ob.size(); i++)
  {
    start = ob[i].start;
    end = ob[i].end;
    d = ob[i].delay;
    cout << "Start: " << start << " " << "End: " << end << " " << "Delay: " << d << endl;
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
    }
    cout << "Start PIN INFO: " << pinNameS << " " << instNameS << endl;
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
    }
    cout << "End PIN INFO: " << pinNameE << " " << instNameE << endl;
  }
}

//Actual Arrival Time
double AAT(node* out)
{
  if(out == NULL)
    return 0;
  else
    for(int i = 0; i < out->prev.size(); i++)
      return out -> AAT = max(out -> AAT,AAT(out->prev[i])) + out-> delay;

}

//Retrival Arrival Time
double RAT(node* in)
{
  if(in == NULL)
    return 0;
  else
    for(int i = 0; i < in->next.size(); i++)
      return in -> RAT = min(in -> RAT,RAT(in->next[i])) - in -> delay;
}

int main()
{
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
    // string start,end;
    // cout << "Enter Wire name: ";
    // cin >> start;
    // getBranch(start,end);
    parse_and_get_delay();
    cout << "=====================DELAYS=======================" << endl;
    wireDelaySetup(out_branches);
  	// map<string, double>::iterator it;
  	// for (it = delays.begin(); it != delays.end(); it++)
  	// {
  	// 	cout << it->first << "  delay = " << fixed << it->second * time_unit << " PS" << endl;
  	// }
  	// cout << "====================PORTS=========================" << endl;
  	// for (int i = 0; i < Ports.size(); i++)
  	// {
  	// 	cout << "Port Name " << Ports[i].name << " Type " << Ports[i].type << " Load " << Ports[i].load << endl;
  	// }
    AAT(m["N3"]);
    cout << m["N3"]->AAT << endl;
  return 0;
}
