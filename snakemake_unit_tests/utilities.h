/*!
 @file utilities.h
 @brief general utility functions
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
#define SNAKEMAKE_UNIT_TESTS_UTILITIES_H_

#include <array>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "boost/regex.hpp"

namespace snakemake_unit_tests {
/*!
  \brief type indicator for possible open string structures
 */
typedef enum { single_tick, single_quote, triple_tick, triple_quote, none } quote_type;

/*!
  \brief determine what a newly encountered ' or "
  means in the context of previously encountered marks
  @param current_line python line currently being processed
  @param active_quote_type if string or literal is currently open,
  the type of opening delimiter found
  @param parse_index index of character being processed in current line
  @param string_open whether there is a currently active single
  delimiter string
  @param literal_open whether there is a currently active triple
  delimiter literal
 */
void resolve_string_delimiter(const std::string &current_line, quote_type *active_quote_type, unsigned *parse_index,
                              bool *string_open, bool *literal_open);
/*!
  \brief add a processed line to a set of processed line
  @param resolved_line line to be added to aggregated set
  @param aggregated_line previous lines ending with explicit
  line extensions that need to be added as well
  @param results currently existing set of resolved lines
 */
void append_resolved_line(const std::string &resolved_line, std::string *aggregated_line,
                          std::vector<std::string> *results);
/*!
  \brief prune superfluous content from snakemake content line
  @param lines all loaded lines from file
  @param verbose whether to emit verbose logging output to cout
  @return input data with comments and unassigned string literals pruned

  this modification of the function replaces the initial implementation
  of this function that only partially addressed the problem and
  was known to break in certain toxic corner cases
 */
std::vector<std::string> lexical_parse(const std::vector<std::string> &lines, bool verbose = false);
/*!
  @brief take a comma/space delimited list of filenames and break them up into a
  vector
  @param s input list; intended to be from snakemake log data
  @param target vector in which to store data
 */
void split_comma_list(const std::string &s, std::vector<std::string> *target);

/*!
@brief execute a system command and capture its results
@param cmd system command to execute
@param fail_on_error whether python errors should trigger immediate exception
@param emit_error_logging whether, in the case that the executed command returns an error code,
any captured output should be emitted to std::cerr
@return captured results, line by line

https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
*/
std::vector<std::string> exec(const std::string &cmd, bool fail_on_error, bool emit_error_logging = true);

}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_UTILITIES_H_
