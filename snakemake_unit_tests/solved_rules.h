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
#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "boost/regex.hpp"
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
      : _rule_name(obj._rule_name),
        _inputs(obj._inputs),
        _outputs(obj._outputs),
        _log(obj._log) {}
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
  const std::vector<std::string> &get_inputs() const { return _inputs; }
  /*!
    @brief add an input filename
    @param s new input filename
   */
  void add_input(const std::string &s) { _inputs.push_back(s); }
  /*!
    @brief access list of output files
    @return vector storing all output filenames; shouldn't be empty
   */
  const std::vector<std::string> &get_outputs() const { return _outputs; }
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
  std::string _rule_name;
  std::vector<std::string> _inputs;
  std::vector<std::string> _outputs;
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
  solved_rules(const solved_rules &obj) : _recipes(obj._recipes) {}
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
    @param pipeline_dir parent directory of snakemake pipeline used to generate
    corresponding log file (e.g.: X for X/workflow/Snakefile)
    @param inst_dir directory in snakemake_unit_tests repo containing
    installation files (when conda mode is enabled, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst)
    @param exclude_rules vector of rules to skip tests for
    @param added_files vector of additional files to add to test workspaces
    @param added_directories vector of additional directories to add to test
    workspaces

    TODO(cpalmer718): make this do what it actually has to do,
    the current content is just a proof of principle placeholder
  */
  void emit_tests(const snakemake_file &sf,
                  const boost::filesystem::path &output_test_dir,
                  const boost::filesystem::path &pipeline_dir,
                  const boost::filesystem::path &inst_dir,
                  const std::vector<std::string> &exclude_rules,
                  const std::vector<std::string> &added_files,
                  const std::vector<std::string> &added_directories) const;

 private:
  std::vector<recipe> _recipes;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SOLVED_RULES_H_
