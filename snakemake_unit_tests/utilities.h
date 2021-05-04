/*!
 @file utilities.h
 @brief general utility functions
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
#define SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "boost/regex.hpp"

namespace snakemake_unit_tests {
/*!
  @brief prune superfluous content from snakemake content line
  @param s line to be pruned down to minimal essential content
  @param loaded_lines currently processed snakemake file lines
  @param line_number counter of line in file, for error reporting
  @return pruned line

  for the moment, this removes:

  - things that look like docstrings (""" content """)

  - comments (all text after unescaped '#')

  - trailing whitespace (tab or space)
 */
std::string remove_comments_and_docstrings(
    const std::string &s, const std::vector<std::string> &loaded_lines,
    unsigned *line_number);
/*!
  @brief take a comma/space delimited list of filenames and break them up into a
  vector
  @param s input list; intended to be from snakemake log data
  @param target vector in which to store data
 */
void split_comma_list(const std::string &s, std::vector<std::string> *target);
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
