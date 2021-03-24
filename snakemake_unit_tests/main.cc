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
#include "snakemake_unit_tests/solved_rules.h"

/*!
  @brief main program implementation
  @param argc number of command line entries, including program name
  @param argv array of command line entries
  @return exit code: 0 on success, nonzero otherwise
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
  bool verbose = ap.verbose();
  boost::filesystem::path output_test_dir = ap.get_output_test_dir();
  boost::filesystem::path snakefile = ap.get_snakefile();
  std::string pipeline_run_draft = ap.get_pipeline_dir();
  if (pipeline_run_draft.empty()) {
    // behavior: if not specified, select it as the directory above
    // wherever the snakefile is installed
    pipeline_run_draft = snakefile.remove_trailing_separator()
                             .parent_path()
                             .parent_path()
                             .string();
  }
  boost::filesystem::path pipeline_run_dir = pipeline_run_draft;
  boost::filesystem::path inst_dir = ap.get_inst_dir();
  if (!boost::filesystem::is_directory(inst_dir))
    throw std::runtime_error("provided inst directory \"" + inst_dir.string() +
                             "\" does not exist");
  std::string snakemake_log = ap.get_snakemake_log();
  std::vector<std::string> added_files = ap.get_added_files();
  std::vector<std::string> added_directories = ap.get_added_directories();
  std::vector<std::string> exclude_rules = ap.get_exclude_rules();
  // add "all" to exclusion list, always
  // it's ok if it dups with user specification, it's uniqued later
  exclude_rules.push_back("all");

  // parse the top-level snakefile and all include files (hopefully)
  snakemake_unit_tests::snakemake_file sf;
  sf.load_file(snakefile.filename().string(),
               snakefile.remove_trailing_separator().parent_path().string(),
               verbose);

  // as a debug step, report the parsed contents of the snakefile
  if (verbose) {
    sf.print_blocks(std::cout);
  }
  // parse the log file to determine the solved system of rules and outputs
  snakemake_unit_tests::solved_rules sr;
  sr.load_file(snakemake_log);

  // iterate over the solved rules, emitting them with modifiers as desired
  // TODO(cpalmer718): make responsive to cli, actually implement, etc.
  sr.emit_tests(sf, output_test_dir, pipeline_run_dir, inst_dir, exclude_rules,
                added_files, added_directories);

  std::cout << "all done woo!" << std::endl;
  return 0;
}
