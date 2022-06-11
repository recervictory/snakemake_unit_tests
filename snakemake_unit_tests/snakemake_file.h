/*!
 @file snakemake_file.h
 @brief representation of snakemake file as a sequence
 of rule blocks and code chunks
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
#define SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_

#include <array>
#include <deque>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/smart_ptr.hpp"
#include "snakemake_unit_tests/rule_block.h"

namespace snakemake_unit_tests {
/*!
@class snakemake_file
@brief abstract representation of snakefile
as a series of rules and python code chunks
*/
class snakemake_file {
 public:
  /*!
  @brief default constructor
 */
  snakemake_file() : _tag_counter(0), _updated_last_round(true) {
    _tag_counter.reset(new unsigned);
    *_tag_counter = 1;
  }
  /*!
  @brief construct a snakemake file with
  an initialized counter
  @param ptr pre-initialized counter, from root file
 */
  explicit snakemake_file(boost::shared_ptr<unsigned> ptr) : _tag_counter(ptr), _updated_last_round(true) {}
  /*!
  @brief copy constructor
  @param obj existing snakemake_file object
 */
  snakemake_file(const snakemake_file &obj)
      : _blocks(obj._blocks),
        _snakefile_relative_path(obj._snakefile_relative_path),
        _included_files(obj._included_files),
        _tag_counter(obj._tag_counter),
        _updated_last_round(obj._updated_last_round) {}
  /*!
  @brief destructor
 */
  ~snakemake_file() throw() {}

  /*!
  @brief load and parse snakemake pipeline
  @param filename name of top-level snakefile, relative to pipeline top level
  @param base_dir pipeline top level
  @param verbose whether to emit verbose logging output
 */
  void load_everything(const boost::filesystem::path &filename, const boost::filesystem::path &base_dir, bool verbose);

  /*!
 @brief parse a snakemake file
 @param loaded_lines lines of file to parse
 @param insertion_point list iterator to where to insert content
 @param filename name of file for informative errors
 @param verbose whether to emit verbose
 logging output
*/
  void parse_file(const std::vector<std::string> &loaded_lines,
                  std::list<boost::shared_ptr<rule_block>>::iterator insertion_point,
                  const boost::filesystem::path &filename, bool verbose);

  /*!
  @brief load all lines from a file into memory
  @param filename name of file to load
  @param target where to store the loaded lines
 */
  void load_lines(const boost::filesystem::path &filename, std::vector<std::string> *target) const;

  /*!
  @brief report on internal discrepancies in the snakefile load results
  @param include_rules included rule set, for helpful logging
  @param exclude_rules excluded rule set, for helpful logging

  most features originally flagged by this method are now supported by the program,
  and this logging function is more for coverage reporting than error handling.
 */
  void detect_known_issues(const std::map<std::string, bool> &include_rules,
                           const std::map<std::string, bool> &exclude_rules);

  /*!
  @brief print block contents to stream
  @param out open stream to which to write data

  this function is primarily intended for debugging purposes at this time
 */
  void print_blocks(std::ostream &out) const;

  /*!
  @brief get const access to internal block representation
  @return const reference to internal block list
 */
  const std::list<boost::shared_ptr<rule_block>> &get_blocks() const { return _blocks; }

  /*!
  @brief get mutable access to internal block representation
  @return non-const reference to internal block list
 */
  std::list<boost::shared_ptr<rule_block>> &get_blocks() { return _blocks; }

  /*!
  @brief report all code blocks but a single requested rule to file
  @param rule_names string names of requested rules
  @param out open output stream to which to write data
  @return how many target rules are present in this file
 */
  unsigned report_single_rule(const std::map<std::string, bool> &rule_names, std::ostream &out) const;

  /*!
  @brief whether the object's rules are unambiguously resolved
  @return whether the object's rules are unambiguously resolved

  this will involve querying the rules for their own diagnosis of
  their status. this logic is not yet fully clear to me.
 */
  bool fully_resolved() const;
  /*!
  @brief whether the object's rules contain any unresolved include directives
  @return whether the object's rules contain any unresolved include directives
 */
  bool contains_blockers() const;

  /*!
  @brief run the current rule set through python once
  @param workspace top level directory with added files and directories
  installed
  @param pipeline_top_dir top directory of pipeline installation
  @param pipeline_run_dir where the actual pipeline was initially run,
  relative to top level pipeline directory
  @param verbose whether to provide verbose logging output
  @param disable_resolution deactivate downstream processing on recursive
  calls
  @return whether the reporting terminated just after the first
  instance of an unresolved include directive. used to control
  recursive behavior.

  this is the top level entry point for a recursion pass through python
  reporting. this should only be called from the primary caller.
 */
  bool resolve_with_python(const boost::filesystem::path &workspace, const boost::filesystem::path &pipeline_top_dir,
                           const boost::filesystem::path &pipeline_run_dir, bool verbose, bool disable_resolution);

  /*!
  @brief run the current rule set through python once
  @param workspace top level directory with added files and directories
  installed
  @param pipeline_run_dir where the actual pipeline was initially installed
  @param verbose whether to provide verbose logging output
  @param tag_values reporter data from python pass
  @param output_name full output path of snakefile
  @return processing status; deprecated, flagged for removal

  this is the recursive entry point for a recursion pass through python
  reporting. this should be called for each file in turn.
 */
  bool process_python_results(const boost::filesystem::path &workspace, const boost::filesystem::path &pipeline_run_dir,
                              bool verbose, const std::map<std::string, std::string> &tag_values,
                              const boost::filesystem::path &output_name);

  /*!
  @brief execute a system command and capture its results
  @param cmd system command to execute
  @param fail_on_error whether python errors should trigger immediate exception
  @return captured results, line by line

  https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
 */
  std::vector<std::string> exec(const std::string &cmd, bool fail_on_error) const;
  /*!
  @brief parse python reporting output to tags or tags and values
  @param vec captured lines from python output
  @param target results collector

  null strings in map value corresponds to rule tags
 */
  void capture_python_tag_values(const std::vector<std::string> &vec, std::map<std::string, std::string> *target) const;
  /*!
  @brief resolve derived rules and check for sanity
  @param include_rules flagged rules to be included
  @param exclude_rules flagged rules to be excluded
 */
  void postflight_checks(const std::map<std::string, bool> &include_rules,
                         const std::map<std::string, bool> &exclude_rules);

  /*!
  @brief report relative path of snakefile this object represents
  @return relative path of original snakefile

  path is relative to pipeline top level
 */
  const boost::filesystem::path &get_snakefile_relative_path() const { return _snakefile_relative_path; }
  /*!
  @brief provide access to loaded file map
  @return const reference to loaded file map
 */
  const std::map<boost::filesystem::path, boost::shared_ptr<snakemake_file>> &loaded_files() const {
    return _included_files;
  }
  /*!
  @brief override update status in this file and all dependencies
  @param b new value for update status
 */
  void set_update_status(bool b) {
    _updated_last_round = b;
    for (std::map<boost::filesystem::path, boost::shared_ptr<snakemake_file>>::iterator iter = _included_files.begin();
         iter != _included_files.end(); ++iter) {
      iter->second->set_update_status(b);
    }
  }

  /*!
  @brief report loaded rules in this file and all dependencies
  @param aggregated_rules target for reporting
 */
  void report_rules(std::map<std::string, std::vector<boost::shared_ptr<rule_block>>> *aggregated_rules) const;

  /*!
    @brief for a single rule, get base rule name
    @param name name of rule to query
    @param target where to store detected parent, or empty string
    @return whether the rule was found
  */
  bool get_base_rule_name(const std::string &name, std::string *target) const;

 private:
  friend class snakemake_fileTest;
  /*!
  @brief minimal contents of snakemake file as blocks of code
 */
  std::list<boost::shared_ptr<rule_block>> _blocks;
  /*!
  @brief relative path to source snakefile from top level pipeline dir
 */
  boost::filesystem::path _snakefile_relative_path;
  /*!
  @brief included files to report on print statements
 */
  std::map<boost::filesystem::path, boost::shared_ptr<snakemake_file>> _included_files;
  /*!
  @brief internal counter of assigned tags to rules
 */
  boost::shared_ptr<unsigned> _tag_counter;
  /*!
  @brief whether any contained block updated its inclusion status last update
 */
  bool _updated_last_round;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
