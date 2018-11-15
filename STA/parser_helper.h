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

#ifndef _PARSER_HELPER_H
#define _PARSER_HELPER_H

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>

using std::cout ;
using std::endl ;
using std::istream ;
using std::ostream ;
using std::vector ;
using std::map ;
using std::string ;


/////////////////////////////////////////////////////////////////////
// 
// This class can be used to parse the specific verilog
// format as defined in the ISPD-12 contest benchmarks. It is not
// intended to be used as a generic verilog parser.
//
// See test_verilog_parser () function in parser_helper.cpp for an
// example of how to use this class.
//
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//
// The following classes can be used to parse the specific lib
// format as defined in the ISPD-12 contest benchmarks. They are not
// intended to be used as a generic lib parser.
//
// See test_lib_parser () function in parser_helper.cpp for an
// example of how to use these classes.
//
/////////////////////////////////////////////////////////////////////

// Look up table to store delay or slew functions
struct LibParserLUT {

  // Look up table is indexed by the output load and the input transition values
  // Example:
  //   Let L = loadIndices[i]
  //       T = transitionIndices[j]
  //   Then, the table value corresponding to L and T will be:
  //       table[i][j]
  //
  vector<double> loadIndices ;
  vector<double> transitionIndices ;
  vector<vector<double> > tableVals ;
  map <double,map<double,double>> lib_lut; // Refaay

} ;

ostream& operator<< (ostream& os, LibParserLUT& lut) ;

struct LibParserTimingInfo {

  string fromPin ;
  string toPin ;
  string timingSense ; // "non_unate" or "negative_unate" or "positive_unate".
  string timingType; // Refaay added
  // Note that ISPD-12 library will have only negative-unate combinational cells. The clock arcs
  // for sequentials will be non_unate (which can be ignored because of the simplified sequential
  // timing model for ISPD-12).


  LibParserLUT fallDelay ;
  LibParserLUT riseDelay ;
  LibParserLUT fallTransition ;
  LibParserLUT riseTransition ;
  double riseConstraint;
  double fallConstraint;
} ;

ostream& operator<< (ostream& os, LibParserTimingInfo& timing) ;

struct LibParserPinInfo {

  string name ; // pin name
  double capacitance ; // input pin cap (not defined for output pins)
  double maxCapacitance ; // the max load this pin can drive
  bool isInput ; // whether the pin is input or output pin
  bool isClock ; // whether the pin is a clock pin or not

  LibParserPinInfo () : capacitance (0.0), maxCapacitance (std::numeric_limits<double>::max()),
                        isInput(true), isClock(false) {}

} ;

ostream& operator<< (ostream& os, LibParserPinInfo& pin) ;

struct LibParserCellInfo {

  string name ; // cell name
  string footprint ; // only the cells with the same footprint are swappable
  double leakagePower ; // cell leakage power
  double area ; // cell area (will not be a metric for ISPD-12)
  bool isSequential ; // if true then sequential cell, else combinational
  bool dontTouch ; // is the sizer allowed to size this cell?

  vector<LibParserPinInfo> pins ;
  map <string,LibParserPinInfo> lib_pin; // Refaay

  vector<LibParserTimingInfo> timingArcs ;
  map <string,map<string,LibParserTimingInfo>> lib_timing; // Refaay

  LibParserCellInfo () : leakagePower (0.0), area (0.0), isSequential (false), dontTouch(false) {}

} ;

ostream& operator<< (ostream& os, LibParserCellInfo& cell) ;

// See test_lib_parser () function in parser_helper.cpp for an
// example of how to use this class.
class LibParser {

  std::ifstream is ;
  void _skip_lut_3D () ;
  void _begin_read_lut (LibParserLUT& lut) ;
  void _begin_read_timing_info (string pinName, LibParserTimingInfo& cell) ;
  void _begin_read_pin_info (string pinName, LibParserCellInfo& cell, LibParserPinInfo& pin) ;
  void _begin_read_cell_info (string cellName, LibParserCellInfo& cell) ;

public:
  LibParser (string filename): is(filename.c_str()) {}

  // Read the default max_transition defined for the library.
  // Return value indicates if the last read was successful or not.
  // This function must be called in the beginning before any read_cell_info function call.
  bool read_default_max_transition (double& maxTransition) ;

  // Read the next standard cell definition.
  // Return value indicates if the last read was successful or not.
  bool read_cell_info (LibParserCellInfo& cell) ;

} ;

#endif
