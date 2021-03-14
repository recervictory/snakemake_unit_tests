/*!
 @file utilities.h
 @brief general utility functions
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
#define SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

#include <string>

namespace snakemake_unit_tests {
/*!
  @brief prune superfluous content from snakemake content line
  @param s line to be pruned down to minimal essential content
  @return pruned line

  for the moment, this removes:

  - things that look like docstrings (""" content """)

  - comments (all text after unescaped '#')

  - trailing whitespace (tab or space)
 */
std::string remove_comments_and_docstrings(const std::string &s);
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
