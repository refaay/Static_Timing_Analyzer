#include <bits/stdc++.h>
using namespace std;

string modelName;
int gateLevel;

struct node{
  string name = "";
  string lib = "";
  string delay = "";
  vector<node*> prev;
  vector<node*> nxt;
};

map<string,node*> m; //mapping all nodes by its name in the blif

void lineHandler(string str)
{
  string word;
  word = str.substr(0,str.find_first_of(" \n"));
  int c = 0;
  if(word == ".model")
    c = 1;
  else if (word == ".inputs")
    c = 2;
  else if (word == ".outputs")
    c = 3;
  else if (word == ".gate" || word == ".subckt")
    c = 4;
  switch(c){
    case 1: modelName = str.substr(str.find(word)+word.length()+1,str.find_first_of(" \n"));
                   cout << "Model Name is :" << modelName << endl;
                   break;
    case 2: str.erase(0,str.find(word)+word.length()+1);
                      word = str.substr(0,str.find_first_of(" \n"));
                      while(str.length() > 0)
                           {
                            cout << "Input: " << " " << word << endl;
                            node * in = new node();
                            in->name = word;
                            m[in->name] = in;
                            str.erase(0,str.find(word)+word.length()+1);
                            word = str.substr(0,str.find_first_of(" \n"));
                           }
                      cout << endl << endl;
                      break;
    case 3: str.erase(0,str.find(word)+word.length()+1);
                      word = str.substr(0,str.find_first_of(" \n"));
                      while(str.length() > 0)
                           {
                            cout << "Output: " << " " << word << endl;
                            node * out = new node();
                            out -> name = word;
                            m[out->name] = out;
                            str.erase(0,str.find(word)+word.length()+1);
                            word = str.substr(0,str.find_first_of(" \n"));
                           }
                      cout << endl << endl;
                      break;
    case 4: str.erase(0,str.find(word)+word.length()+1);
              string gateName = str.substr(0,str.find_first_of(" \n"));
              str.erase(0,str.find(gateName)+gateName.length()+1);
              word = str.substr(0,str.find_first_of(" \n"));
              gateLevel++;
              while(str.length() > 0)
                   {
                      cout << "Gate: " << " " << gateName << endl;
                      if(word[0] != 'Y')
                      {
                        node * nn = new node();
                        nn -> name = word[0]+to_string(gateLevel);
                        cout << "Entry: " << nn->name << endl;
                        nn -> lib = gateName;
                        word = word.erase(0,2);
                        string preName = word;
                        if(m.count(preName))
                        {  cout << "Previous Node Exists: ";
                           node* yNode = m[preName];
                           cout << yNode->name << endl;
                           nn->prev.push_back(yNode);
                           yNode->nxt.push_back(nn);
                        }
                        else
                          {
                            node * waitNode = new node();
                            waitNode->name = preName;
                            cout << "Wait Entry: " << waitNode->name << endl;
                            waitNode->nxt.push_back(nn);
                            nn->prev.push_back(waitNode);
                            m[waitNode->name] = waitNode;
                          }
                      }
                      else
                      {
                        string yName = word.substr(2,word.length()-2);
                        if(m.count(yName))
                        {   cout << "Existing Y node: ";
                            node* yNode = m[yName];
                            cout << yNode->name << endl;
                            yNode->lib = gateName;
                        }
                        else
                        {
                          cout << "New Y node: ";
                          node* newY = new node();
                          newY->name = yName;
                          cout << newY->name << endl;
                          newY->lib = gateName;
                          m[newY->name] = newY;
                        }
                      }
                      str.erase(0,str.find(word)+word.length()+1);
                      word = str.substr(0,str.find_first_of(" \n"));
                   }
                   cout << endl << endl;
                   break;

  };

  // while(str.length() > 0)
  //  {
  //   cout << word << " " << word.length() << endl;
  //   str.erase(0,str.find(word)+word.length()+1);
  //   //TODO: Logic of the each .<word>
  //   word = str.substr(0,str.find_first_of(" \n"));
  //  }
}
int main()
{   gateLevel = 0;
    string fileAdd;
    string blifLine;
    cout << "Enter Blif File address: " ;
    cin >> fileAdd;
    ifstream x(fileAdd.c_str());
    if(!x.fail())
      while(getline(x,blifLine))
      {
        lineHandler(blifLine);
      }
    else
      cout << "Invalid Address!!";


  return 0;
}
