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
  snakemake_file() {}
  /*!
    @brief copy constructor
    @param obj existing snakemake_file object
   */
  snakemake_file(const snakemake_file &obj) : _blocks(obj._blocks) {}
  /*!
    @brief destructor
   */
  ~snakemake_file() throw() {}

  /*!
    @brief load and parse snakemake pipeline
    @param filename name of top-level snakefile
    @param base_dir directory from which to base relative file paths
    @param verbose whether to emit verbose logging output
   */
  void load_everything(const boost::filesystem::path &filename,
                       const boost::filesystem::path &base_dir, bool verbose);

  /*!
   @brief parse a snakemake file
   @param loaded_lines lines of file to parse
   @param insertion_point list iterator to where to insert content
   @param filename name of file for informative errors
   @param global_indentation indentation depth of file's include directive
   @param verbose whether to emit verbose
   logging output
  */
  void parse_file(
      const std::vector<std::string> &loaded_lines,
      std::list<boost::shared_ptr<rule_block> >::iterator insertion_point,
      const boost::filesystem::path &filename, unsigned global_indentation,
      bool verbose);

  /*!
    @brief load all lines from a file into memory
    @param filename name of file to load
    @param target where to store the loaded lines
   */
  void load_lines(const boost::filesystem::path &filename,
                  std::vector<std::string> *target) const;

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

 private:
  /*!
    @brief minimal contents of snakemake file as blocks of code
   */
  std::list<boost::shared_ptr<rule_block> > _blocks;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
