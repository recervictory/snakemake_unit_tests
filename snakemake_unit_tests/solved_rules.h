/*!
 @file solved_rules.h
 @brief simple representation of parsed snakemake dag
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_
#define SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_

#include <algorithm>
#include <deque>
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "boost/regex.hpp"
#include "boost/smart_ptr.hpp"
#include "snakemake_unit_tests/snakemake_file.h"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
/*!
  @class recipe
  @brief from the snakemake log, a simple description
  of how input(s) lead to output(s) via a rule
 */
class recipe {
 public:
  /*!
    @brief constructor
   */
  recipe() : _rule_name(""), _log("") {}
  /*!
    @brief copy constructor
    @param obj existing recipe object
   */
  recipe(const recipe &obj)
      : _rule_name(obj._rule_name), _inputs(obj._inputs), _outputs(obj._outputs), _log(obj._log) {}
  /*!
    @brief destructor
   */
  ~recipe() throw() {}

  /*!
    @brief access rule name
    @return rule name
   */
  const std::string &get_rule_name() const { return _rule_name; }
  /*!
    @brief set rule name
    @param s new rule name
   */
  void set_rule_name(const std::string &s) { _rule_name = s; }
  /*!
    @brief access list of input files
    @return vector storing all input filenames; may be empty
  */
  const std::vector<boost::filesystem::path> &get_inputs() const { return _inputs; }
  /*!
    @brief add an input filename
    @param s new input filename
   */
  void add_input(const std::string &s) { _inputs.push_back(s); }
  /*!
    @brief access list of output files
    @return vector storing all output filenames; shouldn't be empty
   */
  const std::vector<boost::filesystem::path> &get_outputs() const { return _outputs; }
  /*!
    @brief add an output filename
    @param s new output filename
   */
  void add_output(const std::string &s) { _outputs.push_back(s); }
  /*!
    @brief access log filename
    @return log filename, if given; else empty string
   */
  const std::string &get_log() const { return _log; }
  /*!
    @brief set log filename
    @param s new log filename
   */
  void set_log(const std::string &s) { _log = s; }
  /*!
    @brief clear all stored contents
   */
  void clear() {
    _rule_name = _log = "";
    _inputs.clear();
    _outputs.clear();
  }

 private:
  friend class solved_rulesTest;
  /*!
    @brief extracted name of rule from log file
   */
  std::string _rule_name;
  /*!
    @brief snakemake solved input files to rule

    parsed from ", " delimited list from log output
   */
  std::vector<boost::filesystem::path> _inputs;
  /*!
    @brief snakemake solved output files to rule

    parsed from ", " delimited list from log output
   */
  std::vector<boost::filesystem::path> _outputs;
  /*!
    @brief snakemake solved log file for rule

    only exists if rule has log block, and nothing is
    currently done with this information even if present
   */
  std::string _log;
};
/*!
  @class solved_rules
  @brief store parsed simplified version of snakemake dag,
  focusing simply on which rule is used to generate which output
  from which input
 */
class solved_rules {
 public:
  /*!
    @brief constructor
   */
  solved_rules() {}
  /*!
    @brief copy constructor
    @param obj existing solved_rules object
   */
  solved_rules(const solved_rules &obj) : _recipes(obj._recipes), _output_lookup(obj._output_lookup) {}
  /*!
    @brief destructor
   */
  ~solved_rules() throw() {}
  /*!
    @brief load solved recipes from a snakemake log file
    @param filename name of snakemake logfile to parse
   */
  void load_file(const std::string &filename);
  /*!
    @brief emit tests from parsed snakemake information
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param pipeline_top_dir parent directory of snakemake pipeline used to
    generate corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param pipeline_run_dir directory in which pipeline was run, relative to
    pipeline_top_dir. if you run the pipeline in the top level directory,
    this should simply be '.'
    @param inst_dir directory in snakemake_unit_tests repo containing
    installation files (when conda mode is enabled, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst)
    @param exclude_rules map of rules to skip tests for
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
    @param update_snakefiles controls whether to print snakefiles
    @param update_added_content controls whether to copy added files and
    directories
    @param update_inputs controls whether to copy rule inputs
    @param update_outputs controls whether to copy rule outputs
    @param update_pytest controls whether to copy pytest infrastructure
    @param include_entire_dag controls whether to override default
    behavior and emit all rules, instead of just the target
    @param files_outside_workspace for logging, a collector for
    files that exist outside of the self-contained workspace, which
    will not be copied into the self-contained unit tests
  */
  void emit_tests(const snakemake_file &sf, const boost::filesystem::path &output_test_dir,
                  const boost::filesystem::path &pipeline_top_dir, const boost::filesystem::path &pipeline_run_dir,
                  const boost::filesystem::path &inst_dir, const std::map<std::string, bool> &exclude_rules,
                  const std::vector<boost::filesystem::path> &added_files,
                  const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles,
                  bool update_added_content, bool update_inputs, bool update_outputs, bool update_pytest,
                  bool include_entire_dag, std::map<std::string, bool> *files_outside_workspace) const;
  /*!
    @brief emit snakefile from parsed snakemake information
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param workspace_path top level of emitted workspace
    @param rec target rule for emission
    @param dependent_rulenames all rule names that should be included in
    the output
    @param requires_phony_all whether the file needs an all target injected.
    this should only be included at top level
    @return how many of the targets were found in the snakefile or its
    dependencies
  */
  unsigned emit_snakefile(const snakemake_file &sf, const boost::filesystem::path &workspace_path,
                          const boost::shared_ptr<recipe> &rec, const std::map<std::string, bool> &dependent_rulenames,
                          bool requires_phony_all) const;
  /*!
    @brief create a test directory
    @param rec recipe/rule entry for which a workspace should be created
    @param sf snakemake_file object with rule definitions corresponding
    to loaded log data
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param test_parent_path '.tests/unit' by default
    @param pipeline_top_dir parent directory of snakemake pipeline used to
    generate corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param pipeline_run_dir directory in which pipeline was run, relative to
    pipeline_top_dir. if you run the pipeline in the top level directory,
    this should simply be '.'
    @param test_inst_py snakemake_unit_tests installed test.py script
    installation files (when conda mode is enabled, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst)
    @param extra_required_recipes map of recipes to spike into the snakefile
    in addition to target rule. this is the intended injection point for
    ad hoc `rules.`-style rule handling
    @param exclude_rules map of rules to skip tests for
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
    @param update_snakefiles controls whether to print snakefiles
    @param update_added_content controls whether to copy added files and
    directories
    @param update_inputs controls whether to copy rule inputs
    @param update_outputs controls whether to copy rule outputs
    @param update_pytest controls whether to copy pytest infrastructure
    @param include_entire_dag controls whether to override default
    behavior and emit all rules, instead of just the target
    @param files_outside_workspace for logging, a collector for
    files that exist outside of the self-contained workspace, which
    will not be copied into the self-contained unit tests
  */
  void create_workspace(const boost::shared_ptr<recipe> &rec, const snakemake_file &sf,
                        const boost::filesystem::path &output_test_dir, const boost::filesystem::path &test_parent_path,
                        const boost::filesystem::path &pipeline_top_dir,
                        const boost::filesystem::path &pipeline_run_dir, const boost::filesystem::path &test_inst_py,
                        const std::map<boost::shared_ptr<recipe>, bool> &extra_required_recipes,
                        const std::map<std::string, bool> &exclude_rules,
                        const std::vector<boost::filesystem::path> &added_files,
                        const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles,
                        bool update_added_content, bool update_inputs, bool update_outputs, bool update_pytest,
                        bool include_entire_dag, std::map<std::string, bool> *files_outside_workspace) const;
  /*!
    @brief create an empty workspace for python testing
    @param output_test_dir output directory for tests (e.g. '.tests/')
    @param pipeline_dir parent directory of snakemake pipeline used to generate
    corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces
    @param files_outside_workspace for logging, a collector for
    files that exist outside of the self-contained workspace, which
    will not be copied into the self-contained unit tests
  */
  void create_empty_workspace(const boost::filesystem::path &output_test_dir,
                              const boost::filesystem::path &pipeline_dir,
                              const std::vector<boost::filesystem::path> &added_files,
                              const std::vector<boost::filesystem::path> &added_directories,
                              std::map<std::string, bool> *files_outside_workspace) const;

  /*!
    @brief recursively remove empty workspace after python integration is
    complete
    @param output_test_dir output diretory for tests (e.g. '.tests/')
   */
  void remove_empty_workspace(const boost::filesystem::path &output_test_dir) const;

  /*!
    @brief copy files/folders enumerated in vector to a location
    @param contents files or folders to be copied
    @param source_prefix parent directory of source files/folders
    @param target_prefix directory destination of files/folders
    @param rule_name label for error reporting
    @param files_outside_workspace for logging, a collector for
    files that exist outside of the self-contained workspace, which
    will not be copied into the self-contained unit tests
   */
  void copy_contents(const std::vector<boost::filesystem::path> &contents, const boost::filesystem::path &source_prefix,
                     const boost::filesystem::path &target_prefix, const std::string &rule_name,
                     std::map<std::string, bool> *files_outside_workspace) const;

  /*!
    @brief report phony all target controlling test snakemake run
    @param out stream to which to write data
    @param targets phony targets for all rule
   */
  void report_phony_all_target(std::ostream &out, const std::vector<boost::filesystem::path> &targets) const;
  /*!
    @brief copy over pytest script with certain additions
    @param parent_dir parent directory of test workspace
    @param test_dir parent directory of all unit tests for the pipeline
    @param rule_name name of rule whose test is being emitted
    @param snakefile_relative_path relative path of snakefile in pipeline dir
    @param pipeline_run_dir relative path of snakemake execution within pipeline
    @param extra_comparison_exclusions vector of files to exclude from pytest
    comparisons
    @param inst_test_py snakemake_unit_tests test.py script location
   */
  void report_modified_test_script(const boost::filesystem::path &parent_dir, const boost::filesystem::path &test_dir,
                                   const std::string &rule_name, const boost::filesystem::path &snakefile_relative_path,
                                   const boost::filesystem::path &pipeline_run_dir,
                                   const std::vector<boost::filesystem::path> &extra_comparison_exclusions,
                                   const boost::filesystem::path &inst_test_py) const;
  /*!
    @brief copy over helper launcher with certain additions
    @param parent_dir parent directory of test workspace
    @param test_dir parent directory of all unit tests for the pipeline
    @param inst_launcher_script snakemake_unit_tests pytest_launcher.bash script location
   */
  void report_modified_launcher_script(const boost::filesystem::path &parent_dir,
                                       const boost::filesystem::path &test_dir,
                                       const boost::filesystem::path &inst_launcher_script) const;
  /*!
    @brief parse output of test snakemake run to determine whether rules are missing from dag
    @param snakemake_exec stored output of test snakemake execution
    @param target any rule names found in informative snakemake errors

    this fallback method is designed specifically to handle toxic uses of snakemake `rules.` notation
   */
  void find_missing_rules(const std::vector<std::string> &snakemake_exec, std::map<std::string, bool> *target) const;
  /*!
    @brief add rules and all dependencies starting from a particular leaf
    @param rec leaf to start adding things from
    @param include_entire_dag controls whether to override default
    behavior and emit all rules, instead of just the target
    @param target storage for included nodes
   */
  void add_dag_from_leaf(const boost::shared_ptr<recipe> &rec, bool include_entire_dag,
                         std::map<boost::shared_ptr<recipe>, bool> *target) const;

 private:
  friend class solved_rulesTest;
  /*!
    @brief abstract set of solved recipe entries in a log file
   */
  std::vector<boost::shared_ptr<recipe> > _recipes;
  /*!
    @brief allow lookup of output->recipe for dependency resolution
   */
  std::map<boost::filesystem::path, boost::shared_ptr<recipe> > _output_lookup;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_
