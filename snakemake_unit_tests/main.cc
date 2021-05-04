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
#include "snakemake_unit_tests/yaml_reader.h"

/*!
  @brief main program implementation
  @param argc number of command line entries, including program name
  @param argv array of command line entries
  @return exit code: 0 on success, nonzero otherwise
 */
int main(int argc, char **argv) {
  // parse command line input
  snakemake_unit_tests::cargs ap(argc, argv);
  snakemake_unit_tests::params p;
  // if help is requested or no flags specified
  if (ap.help() || argc == 1) {
    // print a help message and exist
    ap.print_help(std::cout);
    return 0;
  }

  p = ap.set_parameters();

  // parse the top-level snakefile and all include files (hopefully)
  snakemake_unit_tests::snakemake_file sf;
  // express snakefile as path relative to top-level pipeline dir
  std::string snakefile_str = p.snakefile.string();
  std::string pipeline_str = p.pipeline_top_dir.string();
  snakefile_str = snakefile_str.substr(snakefile_str.find(pipeline_str) +
                                       pipeline_str.size() + 1);
  if (p.verbose) {
    std::cout << "computed snakefile is \"" << snakefile_str << "\""
              << std::endl;
  }
  sf.load_everything(boost::filesystem::path(snakefile_str), p.pipeline_top_dir,
                     &p.exclude_rules, p.verbose);

  // as a debug step, report the parsed contents of the snakefile
  if (p.verbose) {
    std::cout << "printing blocks..." << std::endl;
    sf.print_blocks(std::cout);
  }
  // parse the log file to determine the solved system of rules and outputs
  snakemake_unit_tests::solved_rules sr;
  sr.load_file(p.snakemake_log.string());

  // new feature: python integration to resolve ambiguous rules
  // create empty workspace for run
  // should have: added files and directories
  // should not have: snakefile
  // TODO(cpalmer718): determine if workspace requires inputs or outputs?
  //   probably not, as this isn't rule-specific, I hope
  sr.create_empty_workspace(p.output_test_dir, p.pipeline_top_dir,
                            p.added_files, p.added_directories);
  // do things in this location
  do {
    // scan the rule set for blockers
    if (p.verbose) {
      std::cout << "running a python/snakemake logic resolution pass"
                << std::endl;
    }
    sf.resolve_with_python(p.output_test_dir / ".snakemake_unit_tests",
                           p.pipeline_top_dir, p.pipeline_run_dir, p.verbose,
                           false);
  } while (sf.contains_blockers());

  // remove the location
  sr.remove_empty_workspace(p.output_test_dir);

  // refactor: move postflight snakefile checks to after the python passes
  sf.postflight_checks(&p.exclude_rules);

  // rules. support: compile rules. references in rule blocks
  // new: collect 'rules.' dependencies in map
  sf.aggregate_rulesdot();

  // iterate over the solved rules, emitting them with modifiers as desired
  sr.emit_tests(sf, p.output_test_dir, p.pipeline_top_dir, p.pipeline_run_dir,
                p.inst_dir, p.exclude_rules, p.added_files, p.added_directories,
                p.update_snakefiles || p.update_all,
                p.update_added_content || p.update_all,
                p.update_inputs || p.update_all,
                p.update_outputs || p.update_all,
                p.update_pytest || p.update_all, p.include_entire_dag);

  // if requested, report final configuration settings to test directory
  if (p.update_config || p.update_all) {
    p.report_settings(p.output_test_dir / "unit" / "config.yaml");
  }
  std::cout << "all done woo!" << std::endl;
  return 0;
}
