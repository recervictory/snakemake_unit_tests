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

#include <list>
#include <map>
#include <stdexcept>
#include <string>
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
  snakemake_file() : _tag_counter(1) {}
  /*!
    @brief copy constructor
    @param obj existing snakemake_file object
   */
  snakemake_file(const snakemake_file &obj)
      : _blocks(obj._blocks),
        _included_files(obj._included_files),
        _tag_counter(obj._tag_counter) {}
  /*!
    @brief destructor
   */
  ~snakemake_file() throw() {}

  /*!
    @brief load and parse snakemake pipeline
    @param filename name of top-level snakefile
    @param base_dir directory from which to base relative file paths
    @param exclude_rules excluded rule set, for communicating problematic
    rules back upstream
    @param verbose whether to emit verbose logging output
   */
  void load_everything(const boost::filesystem::path &filename,
                       const boost::filesystem::path &base_dir,
                       std::vector<std::string> *exclude_rules, bool verbose);

  /*!
   @brief parse a snakemake file
   @param loaded_lines lines of file to parse
   @param insertion_point list iterator to where to insert content
   @param filename name of file for informative errors
   @param verbose whether to emit verbose
   logging output
  */
  void parse_file(
      const std::vector<std::string> &loaded_lines,
      std::list<boost::shared_ptr<rule_block> >::iterator insertion_point,
      const boost::filesystem::path &filename, bool verbose);

  /*!
    @brief load all lines from a file into memory
    @param filename name of file to load
    @param target where to store the loaded lines
   */
  void load_lines(const boost::filesystem::path &filename,
                  std::vector<std::string> *target) const;

  /*!
    @brief report on internal discrepancies in the snakefile load results
    @param exclude_rules excluded rule set, for communicating problematic
    rules back upstream

    this is particularly designed to detect known unsupported features in
    snakemake/python interactions, specifically conditional inclusion of files
    or rules that relies on python logic and/or leads to inconsistent results
    if not correctly parsed.

    these features are flagged to be correctly supported in a later patch
   */
  void detect_known_issues(std::vector<std::string> *exclude_rules);

  /*!
    @brief populate derived rules with base rule blocks

    required after full load for snakemake 6.0 compatibility
   */
  void resolve_derived_rules();

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
  const std::list<boost::shared_ptr<rule_block> > &get_blocks() const {
    return _blocks;
  }

  /*!
    @brief report all code blocks but a single requested rule to file
    @param rule_name string name of requested rule
    @param out open output stream to which to write data
   */
  void report_single_rule(const std::string &rule_name,
                          std::ostream &out) const;

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
    @param base_dir directory from which to base relative file paths
    @param verbose whether to provide verbose logging output
   */
  void resolve_with_python(const boost::filesystem::path &workspace,
                           const boost::filesystem::path &base_dir,
                           bool verbose);

  /*!
    @brief execute a system command and capture its results
    @param cmd system command to execute
    @return captured results, line by line

    https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
   */
  std::vector<std::string> exec(const std::string &cmd) const;
  /*!
    @brief parse python reporting output to tags or tags and values
    @param vec captured lines from python output
    @param target results collector

    null strings in map value corresponds to rule tags
   */
  void capture_python_tag_values(
      const std::vector<std::string> &vec,
      std::map<std::string, std::string> *target) const;
  /*!
    @brief resolve derived rules and check for sanity
    @param exclude_rules flagged rules to be excluded

    note that more content will be added here presumably once more snakemake
    features are supported
   */
  void postflight_checks(std::vector<std::string> *exclude_rules);

 private:
  /*!
    @brief minimal contents of snakemake file as blocks of code
   */
  std::list<boost::shared_ptr<rule_block> > _blocks;
  /*!
    @brief included files to report on print statements
   */
  std::vector<boost::shared_ptr<snakemake_file> > _included_files;
  /*!
    @brief internal counter of assigned tags to rules
   */
  unsigned _tag_counter;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
