#include<bits/stdc++.h>
#include "Source1.cpp"
#include "Main.cpp"
using namespace std;

string moduleName;

struct node {
  string name;
  string lib;
  string inst;
  double AAT;
  double RAT;
  bool aatFlag;
  bool ratFlag;
  double slack;
  vector<tuple<node*,double>> prev;
  vector<tuple<node*,double>> next;
};

map<string,node*> m;
vector<node*> inp,oup;
map<string,vector<node*>> vi;
map<string,node*> vo;
vector<node*> vn;
map <string,map<string,node*>> ionn;

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
            str.erase(0,str.find(word)+word.length()+1);
            moduleName = str.substr(0,str.find_first_of(" ("));
            cout << moduleName << " " << moduleName.length() << endl;
            break;
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
              vn.clear();
              while(str.length() > 1)
              {
                //cout << word << word.length() << endl;
                string pin = word.substr(1,word.find('(')-1);
                //cout << pin << pin.length() << endl;
                word.erase(0,word.find('(')+1);
                string preName = word.substr(0, word.find(')'));
                //cout << preName << preName.length() << endl;
                 if(pin[0] != 'Z' && pin[0] != 'Y' && pin[0] != 'Q' && pin[0] != 'o')
                 {
                   node* nn = new node();
                   nn-> name = pin;
                   nn -> lib = libName;
                   nn -> inst = instName;
                   nn -> RAT = 999999;
                   nn -> aatFlag = false;
                   nn -> ratFlag = false;
                   node* prevNode = m[preName];
                   tuple<node*,double> t;
                   get<0>(t) = prevNode;
                   nn->prev.push_back(t);
                   get<0>(t) = nn;
                   prevNode->next.push_back(t);
                   vn.push_back(nn);
                   ionn[nn->name][nn->inst] = nn;
                   cout << "Entry: " << nn->name << endl;
                   cout << "Previuos Node: " << prevNode->name << endl;
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
                        vo[instName] = fin;
                        tuple<node*,double> t;
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
                      vo[instName] = Znode;
                      cout << "Gate Out: " << Znode->name << endl;
                    }
                  }
                  else
                  cout << "Not included Y node !!" << endl;
                }
                 str.erase(0,str.find(' ')+1);
                 word = str.substr(0,str.find(' '));
               }
              vi[instName] = vn;
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
    d = ob[i].delay*time_unit;
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
                get<1>(starting->next[i]) = d;
            for(int i = 0; i < ending->prev.size(); i++)
              if((get<0>(ending->prev[i])) == starting)
                  get<1>(ending->prev[i]) = d;
  }
}


//Actual Arrival Time
double AAT(node* out)
{
  if(out->prev.size() == 0)
    return out -> AAT;
  else
    for(int i = 0; i < out->prev.size(); i++)
      if(!(get<0>(out->prev[i]))->aatFlag)
      {
        (get<0>(out->prev[i]))->aatFlag = true;
        return out -> AAT = max(out -> AAT,AAT(get<0>(out->prev[i]))) + get<1>(out->prev[i]);
      }
      else
          return out -> AAT = max(out->AAT,(get<0>(out->prev[i]))-> AAT) + get<1>(out->prev[i]);
}

//Required Arrival Time
double RAT(node* in)
{
  if(in->next.size() == 0)
    return in -> RAT;
  else
    for(int i = 0; i < in->next.size(); i++)
          if(!(get<0>(in->next[i]))->ratFlag)
          {
            (get<0>(in->next[i]))->ratFlag = true;
            return in -> RAT = min(in->RAT,RAT(get<0>(in->next[i]))) - get<1>(in->next[i]);
          }
          else
              return in -> RAT = min(in->RAT,(get<0>(in->next[i]))-> RAT) - get<1>(in->next[i]);
}

//Traverse the DAG to get the Print the pathes with corresponding values
void traverse(node* root,double d)
{
  if(root == NULL)
    return;
  cout << "Node : " << root->name << " Delay: " << d << " AAT: " << root -> AAT << " RAT: " << root-> RAT <<endl;
  for(int i = 0; i < root->next.size(); i++)
    traverse((get<0>(root->next[i])),(get<1>(root->next[i])));
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

    parse_and_get_delay();

    wireDelaySetup(out_branches);

  for(map<string,vector<node*>>::iterator it = vi.begin(); it != vi.end(); ++it)
    for(int i = 0; i < it->second.size(); i++)
      {
        node* ynode = vo[it->first];
        tuple<node*,double> t;
        get<0>(t) = it->second[i];
        ynode->prev.push_back(t);
        get<0>(t) = ynode;
        it->second[i]->next.push_back(t);
      }
    //Traversal of every output
    for(int i = 0; i < oup.size();i++)
      AAT(oup[i]);
    m["Y"]->RAT = 0.005;
    //Traversal of every input
    for(int i = 0; i < inp.size();i++)
        {
          cout << "Path: " << i+1 << endl;
          RAT(inp[i]);
          traverse(inp[i],0);
          cout << endl << endl;
        }
  fill_lib();
  return 0;
}
