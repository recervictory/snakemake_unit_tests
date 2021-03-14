/*!
  @file main.cc
  @brief main entry/exit for software. interprets command line arguments,
  dispatches tasks, exits
  @author Cameron Palmer
  @copyright Released under the MIT License. Copyright
  2021 Cameron Palmer
 */

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <map>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/rule_block.h"

/*!
  \brief main program implementation
  @param argc number of command line entries, including program name
  @param argv array of command line entries
 */
int main(int argc, char **argv) {
  // parse command line input
  snakemake_unit_tests::cargs ap(argc, argv);
  // if help is requested or no flags specified
  if (ap.help() || argc == 1) {
    // print a help message and exist
    ap.print_help(std::cout);
    return 0;
  }

  std::string output_test_dir = ap.get_output_test_dir();
  std::string snakemake_log = ap.get_snakemake_log();
  std::vector<std::string> added_files = ap.get_added_files();

  std::cout << "all done woo!" << std::endl;
  return 0;
}
