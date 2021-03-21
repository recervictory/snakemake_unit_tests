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

#include "boost/filesystem.hpp"
#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/rule_block.h"
#include "snakemake_unit_tests/snakemake_file.h"

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

  // load command line options
  std::string output_test_dir = ap.get_output_test_dir();
  boost::filesystem::path snakefile = ap.get_snakefile();
  std::string snakemake_log = ap.get_snakemake_log();
  std::vector<std::string> added_files = ap.get_added_files();
  std::vector<std::string> added_directories = ap.get_added_directories();

  // parse the top-level snakefile and all include files (hopefully)
  snakemake_unit_tests::snakemake_file sf;
  sf.load_file(snakefile.leaf().string(), snakefile.branch_path().string());

  // as a debug step, report the parsed contents of the snakefile
  sf.print_blocks(std::cout);

  std::cout << "all done woo!" << std::endl;
  return 0;
}
