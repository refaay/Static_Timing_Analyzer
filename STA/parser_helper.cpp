//////////////////////////////////////////////////////////////////
//
//
//  Helper functions and classes to parse the ISPD 2012 contest
//  benchmark files.
//
//  This code is provided for description purposes only. The contest
//  organizers cannot guarantee that the provided code is free of
//  bugs or defects. !!!! USE THIS CODE AT YOUR OWN RISK !!!!!
//
//
//  The contestants are free to use these functions as-is or make
//  modifications. If the contestants choose to use the provided
//  code, they are responsible for making sure that it works as
//  expected.
//
//  The code provided here has no real or implied warranties.
//
//
////////////////////////////////////////////////////////////////////

#include <cstdlib>
#include <limits>
#include <cassert>
#include <cctype>

#include "parser_helper.h"

bool is_special_char (char c) {

  static const char specialChars[] = {'(', ')', ',', ':', ';', '/', '#', '-', '[', ']', '{', '}', '*', '\"', '\\'} ;

  for (int i=0; i < sizeof(specialChars); ++i) {
    if (c == specialChars[i])
      return true ;
  }

  return false ;
}

// Read the next line and return it as a list of tokens skipping white space and special characters
// The return value indicates success/failure.
bool read_line_as_tokens (istream& is, vector<string>& tokens) {

  tokens.clear() ;

  string line ;
  std::getline (is, line) ;

  while (is && tokens.empty()) {

    string token = "" ;

    for (int i=0; i < line.size(); ++i) {
      char currChar = line[i] ;

      if (std::isspace (currChar) || is_special_char(currChar)) {

        if (!token.empty()) {
          // Add the current token to the list of tokens
          tokens.push_back(token) ;
          token.clear() ;
        }
        // else if the token is empty, simply skip the whitespace or special char

      } else {
        // Add the char to the current token
        token.push_back(currChar) ;
      }

    }

    if (!token.empty())
      tokens.push_back(token) ;


    if (tokens.empty())
      // Previous line read was empty. Read the next one.
      std::getline (is, line) ;
  }

  //for (int i=0; i < tokens.size(); ++i)
  //  cout << tokens[i] << " " ;
  //cout << endl ;

  return !tokens.empty() ;
}

// No need to parse the 3D LUTs, because they will be ignored
void LibParser::_skip_lut_3D () {

  std::vector<string> tokens ;

  bool valid = read_line_as_tokens (is, tokens) ;
  assert (valid) ;
  assert (tokens[0] == "index_1") ;
  assert (tokens.size() >= 2) ;
  int size1 = tokens.size() - 1 ;

  valid = read_line_as_tokens (is, tokens) ;
  assert (valid) ;
  assert (tokens[0] == "index_2") ;
  assert (tokens.size() >= 2) ;
  int size2 = tokens.size() - 1 ;

  valid = read_line_as_tokens (is, tokens) ;
  assert (valid) ;
  assert (tokens[0] == "index_3") ;
  assert (tokens.size() >= 2) ;
  int size3 = tokens.size() - 1 ;

  valid = read_line_as_tokens (is, tokens) ;
  assert (valid) ;
  assert (tokens.size() == 1 && tokens[0] == "values") ;

  for (int i=0; i < size1; ++i) {
    for (int j=0; j < size2; ++j) {

      valid = read_line_as_tokens (is, tokens) ;
      assert (valid) ;
      assert (tokens.size() == size3) ;
    }
  }

}

void LibParser::_begin_read_lut (LibParserLUT& lut) {

  std::vector<string> tokens ;
  bool valid = read_line_as_tokens (is, tokens) ;

  assert (valid) ;
  assert (tokens[0] == "index_1") ;
  assert (tokens.size() >= 2) ;

  int size1 = tokens.size()-1 ;
  lut.loadIndices.resize(size1) ;
  for (int i=0; i < tokens.size()-1; ++i) {

    lut.loadIndices[i] = std::atof(tokens[i+1].c_str()) ;
  }

  valid = read_line_as_tokens (is, tokens) ;

  assert (valid) ;
  assert (tokens[0] == "index_2") ;
  assert (tokens.size() >= 2) ;

  int size2 = tokens.size()-1 ;
  lut.transitionIndices.resize(size2) ;
  for (int i=0; i < tokens.size()-1; ++i) {

    lut.transitionIndices[i] = std::atof(tokens[i+1].c_str()) ;
  }

  valid = read_line_as_tokens (is, tokens) ;
  assert (valid) ;
  assert (tokens.size() == 1 && tokens[0] == "values") ;

  lut.tableVals.resize(size1) ;
  for (int i=0 ; i < lut.loadIndices.size(); ++i) {
    valid = read_line_as_tokens (is, tokens) ;
    assert (valid) ;
    assert (tokens.size() == lut.transitionIndices.size()) ;

    lut.tableVals[i].resize(size2) ;
    for (int j=0; j < lut.transitionIndices.size(); ++j) {
      lut.tableVals[i][j] = std::atof(tokens[j].c_str()) ;
      lut.lib_lut[lut.loadIndices[i]][lut.transitionIndices[j]] = lut.tableVals[i][j];

    }
  }


}

void LibParser::_begin_read_timing_info (string toPin, LibParserTimingInfo& timing) {

  timing.toPin = toPin ;

  bool finishedReading = false ;

  std::vector<string> tokens ;
  while (!finishedReading) {

	  bool valid = read_line_as_tokens(is, tokens);
	  assert(valid);
	  assert(tokens.size() >= 1);

	  if (tokens[0] == "cell_fall") {
		  _begin_read_lut(timing.fallDelay);

	  }
	  else if (tokens[0] == "cell_rise") {
		  _begin_read_lut(timing.riseDelay);

	  }
	  else if (tokens[0] == "fall_transition") {
		  _begin_read_lut(timing.fallTransition);

	  }
	  else if (tokens[0] == "rise_transition") {
		  _begin_read_lut(timing.riseTransition);

	  }
	  else if (tokens[0] == "fall_constraint") { // Refaay added
		  valid = read_line_as_tokens(is, tokens);
		  timing.fallConstraint = std::atof(tokens[1].c_str());
		  // _skip_lut_3D() ; // will ignore fall constraints

	  }
	  else if (tokens[0] == "rise_constraint") {// seq not implemented
		  valid = read_line_as_tokens(is, tokens);
		  timing.riseConstraint = std::atof(tokens[1].c_str());
		  //_skip_lut_3D() ; // will ignore rise constraints

	  }
	  else if (tokens[0] == "timing_sense") {
		  timing.timingSense = tokens[1];

	  }
	  else if (tokens[0] == "related_pin") {

		  assert(tokens.size() == 2);
		  timing.fromPin = tokens[1];

	  }
	  else if (tokens[0] == "End") {

		  assert(tokens.size() == 2);
		  assert(tokens[1] == "timing");
		  finishedReading = true;

	  }
	  else if (tokens[0] == "timing_type") {
		  timing.timingType = tokens[1]; // Refaay added

	  }
	  else if (tokens[0] == "related_output_pin") {
		  // ignore data

	  }	//else if (tokens[0] == "values") {
	  // ignore data

	  //}
	  else {

		  cout << "Error: Unknown keyword: " << tokens[0] << endl;
		  assert(false); // unknown keyword
	  }

  }


}

void LibParser::_begin_read_pin_info (string pinName, LibParserCellInfo& cell, LibParserPinInfo& pin) {

  pin.name = pinName ;
  pin.isClock = false ;
  pin.maxCapacitance = std::numeric_limits<double>::max() ;

  bool finishedReading = false ;

  std::vector<string> tokens ;
  while (!finishedReading) {

    bool valid = read_line_as_tokens (is, tokens) ;
    assert (valid) ;
    assert (tokens.size() >= 1) ;

    if (tokens[0] == "direction") {

      assert (tokens.size() == 2) ;
      if (tokens[1] == "input")
        pin.isInput = true ;
      else if (tokens[1] == "output")
        pin.isInput = false ;
      else
        assert (false) ; // undefined direction

    } else if (tokens[0] == "capacitance") {

      assert (tokens.size() == 2) ;
      pin.capacitance = std::atof(tokens[1].c_str()) ;

    } else if (tokens[0] == "max_capacitance") {

      assert (tokens.size() == 2) ;
      pin.maxCapacitance = std::atof(tokens[1].c_str()) ;


    } else if (tokens[0] == "timing") {

      cell.timingArcs.push_back(LibParserTimingInfo()) ; // add an empty TimingInfo object
      _begin_read_timing_info (pinName, cell.timingArcs.back()) ; // pass the empty object to the function to be filled
      cell.lib_timing[cell.timingArcs.back().fromPin][cell.timingArcs.back().toPin] = cell.timingArcs.back();
    } else if (tokens[0] == "clock") {

      pin.isClock = true ;
      cell.isSequential = true;
    } else if (tokens[0] == "End") {

      assert (tokens.size() == 2) ;
      assert (tokens[1] == "pin") ;
      finishedReading = true ;

    } else if (tokens[0] == "function") {

      // ignore data

    } else if (tokens[0] == "min_capacitance") {

      // ignore data

    } else if (tokens[0] == "nextstate_type") {

      // ignore data

    } else {
      cout << "Error: Unknown keyword: " << tokens[0] << endl ;
      assert (false) ; // unknown keyword

    }

  }


}

void LibParser::_begin_read_cell_info (string cellName, LibParserCellInfo& cell) {

  cell.name = cellName ;
  cell.isSequential = false ;
  cell.dontTouch = false ;

  bool finishedReading = false ;

  std::vector<string> tokens ;
  while (!finishedReading) {

    bool valid = read_line_as_tokens (is, tokens) ;
    assert (valid) ;
    assert (tokens.size() >= 1) ;

    if (tokens[0] == "cell_leakage_power") {

      assert (tokens.size() == 2) ;
      cell.leakagePower = std::atof(tokens[1].c_str()) ;

    } else if (tokens[0] == "cell_footprint") {

      assert (tokens.size() == 2) ;
      cell.footprint = tokens[1] ;

    } else if (tokens[0] == "area") {

      assert (tokens.size() == 2) ;
      cell.area = std::atof(tokens[1].c_str()) ;

    } else if (tokens[0] == "clocked_on") { // Refaay: never

      cell.isSequential = true ;

    } else if (tokens[0] == "dont_touch") { // Refaay: never

      cell.dontTouch = true ;

    } else if (tokens[0] == "pin") {

      assert (tokens.size() == 2) ;

      cell.pins.push_back(LibParserPinInfo()) ; // add empty PinInfo object
      _begin_read_pin_info (tokens[1], cell, cell.pins.back()) ; // pass the new PinInfo object to be filled
      cell.lib_pin[tokens[1]] = cell.pins.back(); // Refaay
    } else if (tokens[0] == "End") {

      assert (tokens.size() == 3) ;
      assert (tokens[1] == "cell") ;
      assert (tokens[2] == cellName) ;
      finishedReading = true ;

    } else if (tokens[0] == "cell_footprint") {

      // ignore data

    } else if (tokens[0] == "ff") {

      // ignore data

    } else if (tokens[0] == "next_state") {

      // ignore data

    } else if (tokens[0] == "dont_use") {

      // ignore data

    } else {

      cout << "Error: Unknown keyword: " << tokens[0] << endl ;
      assert (false) ; // unknown keyword
    }
  }

}

// Read the default max_transition defined for the library.
// Return value indicates if the last read was successful or not.
// This function must be called in the beginning before any read_cell_info function call.
bool LibParser::read_default_max_transition (double& maxTransition) {

  maxTransition = 0.0 ;
  vector<string> tokens ;

  bool valid = read_line_as_tokens (is, tokens) ;

  while (valid) {
	 // cout << "Refaay\n";

    if (tokens.size() == 2 && tokens[0] == "default_max_transition") {
      maxTransition = std::atof(tokens[1].c_str()) ;
      return true ;
    }
    valid = read_line_as_tokens (is, tokens) ;
  }

  return false ;
}

// Read the next standard cell definition.
// Return value indicates if the last read was successful or not.
bool LibParser::read_cell_info (LibParserCellInfo& cell) {

  vector<string> tokens ;
  bool valid = read_line_as_tokens (is, tokens) ;


  while (valid) {

    if (tokens.size() == 2 && tokens[0] == "cell") {
      _begin_read_cell_info (tokens[1], cell) ;

      return true ;
    }

    valid = read_line_as_tokens (is, tokens) ;
  }

  return false ;
}

ostream& operator<< (ostream& os, LibParserLUT& lut) {

  if (lut.loadIndices.empty() && lut.transitionIndices.empty() && lut.tableVals.empty())
    return os ;

    // We should have either all empty or none empty.
  assert (!lut.loadIndices.empty() && !lut.transitionIndices.empty() && !lut.tableVals.empty()) ;

  assert (lut.tableVals.size() == lut.loadIndices.size()) ;
  assert (lut.tableVals[0].size() == lut.transitionIndices.size()) ;

  cout << "\t" ;
  for (int i=0; i < lut.transitionIndices.size(); ++i) {
    cout << lut.transitionIndices[i] << "\t" ;
  }
  cout << endl ;


  for (int i=0; i < lut.loadIndices.size(); ++i) {
    cout << lut.loadIndices[i] << "\t" ;

    for (int j=0; j < lut.transitionIndices.size(); ++j)
      cout << lut.tableVals[i][j] << "\t" ;

    cout << endl ;

  }

  return os ;
}

ostream& operator<< (ostream& os, LibParserTimingInfo& timing) {

  cout << "Timing info from " << timing.fromPin << " to " << timing.toPin << ": " << endl ;
  cout << "Timing sense: " << timing.timingSense << endl ;

  cout << "Fall delay LUT: " << endl ;
  cout << timing.fallDelay ;

  cout << "Rise delay LUT: " << endl ;
  cout << timing.riseDelay ;

  cout << "Fall transition LUT: " << endl ;
  cout << timing.fallTransition ;

  cout << "Rise transition LUT: " << endl ;
  cout << timing.riseTransition ;

  return os ;
}

ostream& operator<< (ostream& os, LibParserPinInfo& pin) {

  cout << "Pin " << pin.name << ":" << endl ;
  cout << "capacitance: " << pin.capacitance << endl ;
  cout << "maxCapacitance: " << pin.maxCapacitance << endl ;
  cout << "isInput? " << (pin.isInput ? "true" : "false") << endl ;
  cout << "isClock? " << (pin.isClock ? "true" : "false") << endl ;
  cout << "End pin" << endl ;

  return os ;
}

ostream& operator<< (ostream& os, LibParserCellInfo& cell) {

  cout << "Library cell " << cell.name << ": " << endl ;

  cout << "Footprint: " << cell.footprint << endl ;
  cout << "Leakage power: " << cell.leakagePower << endl ;
  cout << "Area: " << cell.area << endl ;
  cout << "Sequential? " << (cell.isSequential ? "yes" : "no") << endl ;
  cout << "Dont-touch? " << (cell.dontTouch ? "yes" : "no") << endl ;

  cout << "Cell has " << cell.pins.size() << " pins: " << endl ;
  for (int i=0; i < cell.pins.size(); ++i) {
    cout << cell.pins[i] << endl ;
  }

  cout << "Cell has " << cell.timingArcs.size() << " timing arcs: " << endl ;
  for (int i=0; i < cell.timingArcs.size(); ++i) {
    cout << cell.timingArcs[i] << endl ;
  }

  cout << "End of cell " << cell.name << endl << endl ;

  return os ;
}
