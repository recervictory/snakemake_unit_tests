/*!
  \file utilities.cc
  \brief implementations of general utility functions
  \author Cameron Palmer
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/utilities.h"

std::string snakemake_unit_tests::remove_comments_and_docstrings(
    const std::string &s, std::ifstream *input, unsigned *line_number) {
  std::string res = s, line = "";
  std::string::size_type loc = 0, cur = 0;
  if (input && !line_number)
    throw std::logic_error(
        "null line counter pointer provided with non-null input stream");
  // remove docstrings: any text embedded between a pair of triple quotes """
  if ((loc = res.find("\"\"\"", cur)) != std::string::npos) {
    if (loc != res.rfind("\"\"\"")) {
      res = res.substr(0, loc) + res.substr(res.find("\"\"\"", loc + 3) + 3);
      cur = loc + 3;
    } else {
      // allow multiline docstrings
      if (!input) {
        throw std::runtime_error(
            "null input pointer provided while scanning for multiline "
            "docstring");
      }
      bool found_docstring_terminator = false;
      while (input->peek() != EOF && !found_docstring_terminator) {
        getline(*input, line);
        ++*line_number;
        if (line.find("\"\"\"") != std::string::npos) {
          found_docstring_terminator = true;
          res += line.substr(line.find("\"\"\"") + 3);
        } else {
          res += line;
        }
      }
      if (!found_docstring_terminator)
        throw std::runtime_error(
            "unable to find terminating docstring quotes for docstring "
            "starting with: \"" +
            s + "\"");
    }
  }
  cur = 0;
  // remove comments: everything after the first unescaped '#', I think
  while ((loc = res.find("#", cur)) != std::string::npos) {
    if (loc == 0 || res.at(loc - 1) != '\\') {
      res = res.substr(0, loc);
      break;
    }
    cur = loc + 1;
  }
  // remove trailing whitespace
  res = res.substr(0, res.find_last_not_of(" \t") + 1);
  // maybe add other things later?
  // all done i hope
  return res;
}
