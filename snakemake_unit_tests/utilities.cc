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
  // need to somewhat improve comment parsing, to handle situations where
  // comment characters are present but not indicating comments
  // basically, all of this is extremely (overly) simple handling
  // of quotation marks and escape characters
  char open_quote = '\0';
  unsigned comment_char_location = 0;
  bool comment_char_present = false;
  for (unsigned i = 0; i < res.size() && !comment_char_present; ++i) {
    if (res.at(i) == '\'' || res.at(i) == '\"') {
      if (open_quote == res.at(i)) {
        // count preceding escapes
        unsigned escapes = 0;
        for (int j = static_cast<int>(i) - 1; j >= 0; --j) {
          if (res.at(j) == '\\') {
            ++escapes;
          } else {
            break;
          }
        }
        if (escapes % 2 == 0) {
          open_quote = '\0';
        }
      } else if (open_quote == '\0') {
        open_quote = res.at(i);
      }
    } else if (res.at(i) == '#') {
      // count preceding escapes
      unsigned escapes = 0;
      for (int j = static_cast<int>(i) - 1; j >= 0; --j) {
        if (res.at(j) == '\\') {
          ++escapes;
        } else {
          break;
        }
      }
      if (escapes % 2 == 0 && open_quote == '\0') {
        comment_char_location = i;
        comment_char_present = true;
      }
    }
  }
  if (comment_char_present) {
    res = res.substr(0, comment_char_location);
  }
  // remove trailing whitespace
  res = res.substr(0, res.find_last_not_of(" \t") + 1);
  // maybe add other things later?
  // all done i hope
  return res;
}
