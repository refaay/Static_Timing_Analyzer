#include <cstdlib>
#include <limits>
#include <cassert>
#include <cctype>

#include "parser_helper.cpp"

vector<LibParserCellInfo> All_Cells_Early;
vector<LibParserCellInfo> All_Cells_Late;
map <string, LibParserCellInfo> lib_early; // Refaay
map <string, LibParserCellInfo> lib_late; // Refaay

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
		valid = lp_Early.LibParser::read_cell_info(cell);

		if (valid) {
			++readCnt;

			//cout << cell << endl; // prints whole cell data
		}
		All_Cells_Early.push_back(cell);
		lib_early[cell.name] = cell;
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
		lib_late[cell.name] = cell;
	} while (valid);

	cout << "Read " << readCnt << " number of late library cells" << endl;

	cout << "Finished parsing late lib." << endl;
}

// Example function that uses VerilogParser class to parse the given ISPD-12 verilog
// file. The extracted data is simply printed out in this example.
// int main() {
// 	fill_lib();
//
// 	return 0;
// }
