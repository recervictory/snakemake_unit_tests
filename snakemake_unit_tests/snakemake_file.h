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

#include <stdexcept>
#include <string>
#include <vector>

#include "snakemake_unit_tests/rule_block.h"

namespace snakemake_unit_tests {
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
   @brief load and parse a snakemake file
   @param filename name of file to load
   @param base_dir directory from which to base
   relative file paths

   designed to be called recursively to handle
   include statements
  */
  void load_file(const std::string &filename, const std::string &base_dir);

 private:
  /*!
    @brief minimal contents of snakemake file as blocks of code
   */
  std::vector<rule_block> _blocks;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILE_H_
