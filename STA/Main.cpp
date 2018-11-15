#include <cstdlib>
#include <limits>
#include <cassert>
#include <cstdlib>
#include <cctype>

#include "parser_helper.h"
vector<LibParserCellInfo> All_Cells_Early;
vector<LibParserCellInfo> All_Cells_Late;
float calc(string, string, string, float, float,bool,bool,bool);
void fill_lib(){
	cout << "Parsing Early lib..." << endl;
	LibParser lp_Early("crc32d16N_Early.txt");

	//double maxTransition = 0.0;
	bool valid = true; // lp.read_default_max_transition(maxTransition);

	assert(valid);
	//cout << "The default max transition defined is " << maxTransition << endl;
	int readCnt = 0;
	do {
		LibParserCellInfo cell;
		valid = lp_Early.read_cell_info(cell);

		if (valid) {
			++readCnt;

			//cout << cell << endl; // prints whole cell data
		}
		All_Cells_Early.push_back(cell);
	} while (valid);

	cout << "Read " << readCnt << " number of early library cells" << endl;

	cout << "Finished parsing early lib." << endl;

	cout << "Parsing Late lib..." << endl;
	LibParser lp_Late("crc32d16N_Late.txt");

	//maxTransition = 0.0;
	valid = true; // lp.read_default_max_transition(maxTransition);

	assert(valid);
	//cout << "The default max transition defined is " << maxTransition << endl;
	readCnt = 0;
	do {
		LibParserCellInfo cell;
		valid = lp_Late.read_cell_info(cell);

		if (valid) {
			++readCnt;

			//cout << cell << endl; // prints whole cell data
		}
		All_Cells_Late.push_back(cell);
	} while (valid);

	cout << "Read " << readCnt << " number of late library cells" << endl;

	cout << "Finished parsing late lib." << endl;
}



// Example function that uses VerilogParser class to parse the given ISPD-12 verilog
// file. The extracted data is simply printed out in this example.
int main() {
	fill_lib();
	float delay=0.0;
	float slew = 0.0;
	// index 2 then index 1 i.e trans then cload 
	// first var is the gate name
	// second var is the input pin name
	// third var is the output pin name
	// 4th var is the index 2 i.e slew/ transition time
	// 5th var is the index1 i.e cload
	// 5th var is the early or late vector to be used
	// 6th var is the rise or fall 
	// 7th var is the delay or slew 

	delay = calc("INV_X2", "a", "o", 11.18034,4.177682, false, true, true); // inputs for the function 
	cout << "Delay: " << delay << endl;
	cout << "---------------------------------------------" << endl;
	slew = calc("INV_X2", "a", "o", 11.18034,4.177682, false, true, false); // inputs for the function 
	cout << "slew: " << slew << endl;

	
	system("pause");
	return 0;
}
// output is the delay of rise 
//,bool EarlyL, bool Risefall,bool delayS

float calc(string G_name, string In_name, string Out_name, float in_slew, float cload, bool EarlyL, bool Risefall, bool delayS)
{

	if (EarlyL) // if early
	{
		if (Risefall) // if rise
		{
			if (delayS) // if delay
			{

				for (int i = 0; i < All_Cells_Early.size(); i++)
				{
					if (All_Cells_Early[i].name == G_name)
					{
						if (All_Cells_Early[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Early[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;


								for (int x = 0; x < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices.size(); x++)
								{
									if (All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices.size(); j++)
								{
									if (All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}

								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind2][ind1];
									return rise_delay;
								}
								else if ((found != true) && (found2 == true)) // if the slew is correct and the cload is in corect......
								{
									// ind2 it is the index of in the vector for the slew time
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									float y;
									int index2;
									float t1;
									float t2;
									float t00;
									bool newflag = false;

									int sizeofload = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices.size();

									diff = abs(All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0]) || (cload > All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0];
											x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 2];
											x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												newflag = true;
												index = mz;
												diff = diff2;
											}

										}

										if (newflag == false)
										{
											index = 0;
										}

										x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[index-1];		// x1
										x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[index];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}
									cout << " cload values " << endl;
									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;
									
									cout << " Trans " << endl;
									cout << ind2 << endl;
									cout << " aho: "<<All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[ind2] << endl;

									if (cload < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0])
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[0][ind2];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[1][ind2];

										t00 = t1 - (cload - x1)*((t2 - t1) / (x2 - x1));
										return t00;
									}
									else if (cload > All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload-1])
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[sizeofload - 2][ind2];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[sizeofload - 1][ind2];


										t00 = t1 + (cload - x2)*((t2 - t1) / (x2 - x1));
										return t00;
									}
									else
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[sizeofload-2][ind2];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[sizeofload-1][ind2];


										t00 = t1 + (cload - x1)*((t2 - t1) / (x2 - x1));
										return t00;
									}


									

								}
								else if ((found == true) && (found2 != true)) // if the cload is correct and the slew is in corect
								{
									// ind1 it is the index of in the vector for the cload
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									float y;
									int index2;
									float y1;
									float y2;
									float t1;
									float t2;
									float t00;
									bool newflag = false;
									int sizeoftrans = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices.size();

									diff = abs(All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0] - in_slew);
									// out of the range of the cloads

									if ((in_slew < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0]) || (in_slew > All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0];
											y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (in_slew > All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1];
											index = sizeoftrans - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[mz] - in_slew);

											if (diff2 < diff)
											{
												newflag = true;
												index = mz;
												diff = diff2;
											}

										}
										if (newflag == false)
										{
											index = 0;
										}


										y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[index];		
										y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[index + 1];	
										/*x1 = 0.1;
										x2 = 0.3;*/
									}
									cout << " cload values " << endl;
									cout << " higher " << y2 << endl;
									cout << " Lower  " << y1 << endl;

									cout << " Trans " << endl;
									cout << ind1 << endl;
									cout << " aho: " << All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[ind1] << endl;

									if (in_slew < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0])
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][0];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][1];


										t00 = t1 - (in_slew - y1)*((t2 - t1) / (y2 - y1));
										return t00;
									}
									else if (in_slew > All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans-1])
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][sizeoftrans - 2];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][sizeoftrans - 1];
										
										t00 = t1 + (in_slew - y2)*((t2 - t1) / (y2 - y1));
										return t00;
									}
									else
									{
										t1 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][sizeoftrans - 2];
										t2 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[ind1][sizeoftrans - 1];


										t00 = t1 + (in_slew - y1)*((t2 - t1) / (y2 - y1));
										return t00;
									}




								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									bool newflag=false;
									bool newflag2=false;
									int sizeofload = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices.size();
									int sizeoftrans = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices.size();

									diff = abs(All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0]) || (cload > All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[0];
											x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 2];
											x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;



											diff2 = abs(All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												newflag = true;
												index = mz;
												diff = diff2;
											}

										}

										if (newflag == false)
										{
											index = 0;
										}

										x1 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[index];		// x1
										x2 = All_Cells_Early[i].timingArcs[0].riseDelay.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0]) || (in_slew > All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[0];
											y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												newflag2 = true;
												index2 = cz;
												diff3 = diff4;
											}

										}
										if (newflag2 == false)
										{
											index2 = 0;
										}
										y1 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[index2];			// y1 
										y2 = All_Cells_Early[i].timingArcs[0].riseDelay.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Early[i].timingArcs[0].riseDelay.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}
			}
			else // if Slew
			{
				for (int i = 0; i < All_Cells_Early.size(); i++)
				{
					if (All_Cells_Early[i].name == G_name)
					{
						if (All_Cells_Early[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Early[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices.size(); x++)
								{
									if (All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices.size(); j++)
								{
									if (All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Early[i].timingArcs[0].riseTransition.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices.size();
									int sizeoftrans = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices.size();
									bool newflag = false;
									bool newflag2 = false;

									diff = abs(All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[0]) || (cload > All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[0])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[0];
											x2 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 2];
											x2 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												newflag = true;
												index = mz;
												diff = diff2;
											}

										}

										if (newflag == false)
										{
											index = 0;
										}

										x1 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[index];		// x1
										x2 = All_Cells_Early[i].timingArcs[0].riseTransition.loadIndices[index+1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[0]) || (in_slew > All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[0])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[0];
											y2 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												newflag2 = true;
												index2 = cz;
												diff3 = diff4;
											}

										}
										if (newflag2 == false)
										{
											index2 = 0;
										}
										y1 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[index2];			// y1 
										y2 = All_Cells_Early[i].timingArcs[0].riseTransition.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Early[i].timingArcs[0].riseTransition.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Early[i].timingArcs[0].riseTransition.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Early[i].timingArcs[0].riseTransition.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Early[i].timingArcs[0].riseTransition.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}
			}


		}
		else // if fall
		{
			if (delayS) // if delay
			{
				for (int i = 0; i < All_Cells_Early.size(); i++)
				{
					if (All_Cells_Early[i].name == G_name)
					{
						if (All_Cells_Early[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Early[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices.size(); x++)
								{
									if (All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices.size(); j++)
								{
									if (All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Early[i].timingArcs[0].fallDelay.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices.size();
									int sizeoftrans = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices.size();

									diff = abs(All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[0]) || (cload > All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[0])
										{
											x1 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[0];
											x2 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 2];
											x2 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												index = mz;
												diff = diff2;
											}

										}


										x1 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[index];		// x1
										x2 = All_Cells_Early[i].timingArcs[0].fallDelay.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[0]) || (in_slew > All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[0])
										{
											y1 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[0];
											y2 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												index2 = cz;
												diff3 = diff4;
											}

										}
										y1 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[index2];			// y1 
										y2 = All_Cells_Early[i].timingArcs[0].fallDelay.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Early[i].timingArcs[0].fallDelay.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Early[i].timingArcs[0].fallDelay.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Early[i].timingArcs[0].fallDelay.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Early[i].timingArcs[0].fallDelay.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}

			}
			else // if Slew 
			{
				for (int i = 0; i < All_Cells_Early.size(); i++)
				{
					if (All_Cells_Early[i].name == G_name)
					{
						if (All_Cells_Early[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Early[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices.size(); x++)
								{
									if (All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices.size(); j++)
								{
									if (All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Early[i].timingArcs[0].fallTransition.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices.size();
									int sizeoftrans = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices.size();

									diff = abs(All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[0]) || (cload > All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[0])
										{
											x1 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[0];
											x2 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 2];
											x2 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												index = mz;
												diff = diff2;
											}

										}


										x1 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[index];		// x1
										x2 = All_Cells_Early[i].timingArcs[0].fallTransition.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[0]) || (in_slew > All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[0])
										{
											y1 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[0];
											y2 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												index2 = cz;
												diff3 = diff4;
											}

										}
										y1 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[index2];			// y1 
										y2 = All_Cells_Early[i].timingArcs[0].fallTransition.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Early[i].timingArcs[0].fallTransition.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Early[i].timingArcs[0].fallTransition.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Early[i].timingArcs[0].fallTransition.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Early[i].timingArcs[0].fallTransition.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}
			}
		}


	}
	else // if late
	{
		if (Risefall) // if rise
		{
			if (delayS) // if delay
			{
				for (int i = 0; i < All_Cells_Late.size(); i++)
				{
					if (All_Cells_Late[i].name == G_name)
					{
						if (All_Cells_Late[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Late[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices.size(); x++)
								{
									if (All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices.size(); j++)
								{
									if (All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Late[i].timingArcs[0].riseDelay.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices.size();
									int sizeoftrans = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices.size();
									bool flag = false;
									bool flag2 = false;

									diff = abs(All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[0]) || (cload > All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[0])
										{
											x1 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[0];
											x2 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 2];
											x2 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												flag = true;
												index = mz;
												diff = diff2;
											}

										}

										if (flag == false)
										{

											index = 0;
										}

										x1 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[index];		// x1
										x2 = All_Cells_Late[i].timingArcs[0].riseDelay.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[0]) || (in_slew > All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[0])
										{
											y1 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[0];
											y2 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												flag2 = true;
												index2 = cz;
												diff3 = diff4;
											}

										}
										if (flag2 == false)
										{
											index2 = 0;
										}
										y1 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[index2];			// y1 
										y2 = All_Cells_Late[i].timingArcs[0].riseDelay.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Late[i].timingArcs[0].riseDelay.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Late[i].timingArcs[0].riseDelay.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Late[i].timingArcs[0].riseDelay.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Late[i].timingArcs[0].riseDelay.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}

			}
			else // if Slew
			{
				for (int i = 0; i < All_Cells_Late.size(); i++)
				{
					if (All_Cells_Late[i].name == G_name)
					{
						if (All_Cells_Late[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Late[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices.size(); x++)
								{
									if (All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices.size(); j++)
								{
									if (All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}

									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Late[i].timingArcs[0].riseTransition.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices.size();
									int sizeoftrans = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices.size();
									bool flag = false;
									bool flag2 = false;

									diff = abs(All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[0]) || (cload > All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[0])
										{
											x1 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[0];
											x2 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 2];
											x2 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												flag = true;
												index = mz;
												diff = diff2;
											}

										}

										if (flag == false)
										{
											index = 0;
										}

										x1 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[index];		// x1
										x2 = All_Cells_Late[i].timingArcs[0].riseTransition.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[0]) || (in_slew > All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[0])
										{
											y1 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[0];
											y2 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												flag2 = true;
												index2 = cz;
												diff3 = diff4;
											}

										}
										if (flag2 == false)
										{
											index2 = 0;
										}
										y1 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[index2];			// y1 
										y2 = All_Cells_Late[i].timingArcs[0].riseTransition.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Late[i].timingArcs[0].riseTransition.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Late[i].timingArcs[0].riseTransition.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Late[i].timingArcs[0].riseTransition.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Late[i].timingArcs[0].riseTransition.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}
			}
		}
		else // if fall
		{
			if (delayS) // if delay
			{
				for (int i = 0; i < All_Cells_Late.size(); i++)
				{
					if (All_Cells_Late[i].name == G_name)
					{
						if (All_Cells_Late[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Late[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices.size(); x++)
								{
									if (All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices.size(); j++)
								{
									if (All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Late[i].timingArcs[0].fallDelay.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices.size();
									int sizeoftrans = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices.size();

									diff = abs(All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[0]) || (cload > All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[0])
										{
											x1 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[0];
											x2 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 2];
											x2 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												index = mz;
												diff = diff2;
											}

										}


										x1 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[index];		// x1
										x2 = All_Cells_Late[i].timingArcs[0].fallDelay.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[0]) || (in_slew > All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[0])
										{
											y1 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[0];
											y2 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												index2 = cz;
												diff3 = diff4;
											}

										}
										y1 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[index2];			// y1 
										y2 = All_Cells_Late[i].timingArcs[0].fallDelay.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Late[i].timingArcs[0].fallDelay.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Late[i].timingArcs[0].fallDelay.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Late[i].timingArcs[0].fallDelay.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Late[i].timingArcs[0].fallDelay.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}

			}
			else // if Slew
			{
				for (int i = 0; i < All_Cells_Late.size(); i++)
				{
					if (All_Cells_Late[i].name == G_name)
					{
						if (All_Cells_Late[i].timingArcs[0].fromPin == In_name)
						{
							if (All_Cells_Late[i].timingArcs[0].toPin == Out_name)
							{
								int ind1;
								int ind2;
								bool found = false;
								bool found2 = false;
								float rise_delay;
								for (int x = 0; x < All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices.size(); x++)
								{
									if (All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[x] == cload)
									{
										ind1 = x;
										found = true;
										break;
									}
									else found = false;
								}
								for (int j = 0; j < All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices.size(); j++)
								{
									if (All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[j] == in_slew)
									{
										ind2 = j;
										found2 = true;
										break;
									}
									else found2 = false;
								}
								if ((found == true) && (found2 == true)) // that it is found in the table
								{
									rise_delay = All_Cells_Late[i].timingArcs[0].fallTransition.tableVals[ind2][ind1];
									return rise_delay;
								}
								else // the in_slew or the cload are not found 
								{
									float diff = 0.0;
									float x1;
									float x2;
									int index;
									float diff3 = 0.0;
									int index2;
									float y1;
									float y2;
									float x01;
									float x20;
									float y01;
									float y20;
									float t11;
									float t12;
									float t21;
									float t22;
									float t00;
									int sizeofload = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices.size();
									int sizeoftrans = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices.size();

									diff = abs(All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[0] - cload);
									// out of the range of the cloads

									if ((cload < All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[0]) || (cload > All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1]))
									{

										if (cload < All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[0])
										{
											x1 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[0];
											x2 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[1];
											/*x1 = 0.1;
											x2 = 0.3;*/
											index = 0;
										}
										else if (cload > All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1])
										{
											x1 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 2];
											x2 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[sizeofload - 1];
											index = sizeofload - 2;
											/*x1 = 0.1;
											x2 = 0.3;*/

										}
									}
									else // within the range of the values 
									{
										for (int mz = 1; mz < All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices.size(); mz++)
										{
											float diff2 = 0.0;


											diff2 = abs(All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[mz] - cload);

											if (diff2 < diff)
											{
												index = mz;
												diff = diff2;
											}

										}


										x1 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[index];		// x1
										x2 = All_Cells_Late[i].timingArcs[0].fallTransition.loadIndices[index + 1];	// x2
										/*x1 = 0.1;
										x2 = 0.3;*/
									}

									cout << " higher " << x2 << endl;
									cout << " Lower  " << x1 << endl;


									diff3 = abs(All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[0] - in_slew);

									if ((in_slew < All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[0]) || (in_slew > All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1]))
									{

										if (in_slew < All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[0])
										{
											y1 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[0];
											y2 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[1];
											index2 = 0;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
										else if (in_slew > All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1])
										{
											y1 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 2];
											y2 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[sizeoftrans - 1];
											index2 = sizeoftrans - 2;
											/*y1 = 0.35;
											y2 = 1.43;*/
										}
									}
									else
									{
										for (int cz = 1; cz < All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices.size(); cz++)
										{
											float diff4 = 0.0;


											diff4 = abs(All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[cz] - in_slew);

											if (diff4 < diff3)
											{
												index2 = cz;
												diff3 = diff4;
											}

										}
										y1 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[index2];			// y1 
										y2 = All_Cells_Late[i].timingArcs[0].fallTransition.transitionIndices[index2 + 1];	   //  y2
										/*y1 = 0.35;
										y2 = 1.43;*/
									}


									cout << " higher2 " << y2 << endl;
									cout << " Lower2  " << y1 << endl;

									x01 = (cload - x1) / (x2 - x1);
									x20 = (x2 - cload) / (x2 - x1);
									y01 = (in_slew - y1) / (y2 - y1);
									y20 = (y2 - in_slew) / (y2 - y1);
									cout << "x01: " << x01 << endl;
									cout << "x20: " << x20 << endl;
									cout << "y01: " << y01 << endl;
									cout << "y20: " << y20 << endl;
									//t11 = 0.1937;
									//t12 = 0.7280;			// dola test case el fl PDF
									//t21 = 0.2327;
									//t22 = 0.7676;
									t11 = All_Cells_Late[i].timingArcs[0].fallTransition.tableVals[index][index2]; // index and index2
									t12 = All_Cells_Late[i].timingArcs[0].fallTransition.tableVals[index][index2 + 1];	// index and index2+1
									t21 = All_Cells_Late[i].timingArcs[0].fallTransition.tableVals[index + 1][index2]; // index+1 and index2
									t22 = All_Cells_Late[i].timingArcs[0].fallTransition.tableVals[index + 1][index2 + 1]; // index+1 and index2+1
									cout << "t11: " << t11 << endl;
									cout << "t12: " << t12 << endl;
									cout << "t21: " << t21 << endl;
									cout << "t22: " << t22 << endl;


									t00 = x20 * y20 * t11 + x20 * y01 * t12 + x01 * y20 * t21 + x01 * y01 * t22;
									return t00;
								}
							}
						}
					}
				}
			}

		}
		
	}



	
	
	
}