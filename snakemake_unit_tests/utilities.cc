/*!
  \file utilities.cc
  \brief implementations of general utility functions
  \author Cameron Palmer
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/utilities.h"

std::string snakemake_unit_tests::remove_comments_and_docstrings(
    const std::string &s, const std::vector<std::string> &loaded_lines,
    unsigned *line_number) {
  std::string res = s, line = "";
  if (!line_number)
    throw std::logic_error(
        "null line counter pointer provided to remove_comments_and_docstrings");
  // interpret string literals in a way that is consistent with python
  // interpretation
  /*
    the problem is as follows: string literals are often considered either
    docstrings or ignored comments. the exception is when they are assigned to a
    variable, in which case they are *not* ignored and processed by the
    interpreter. they can also span multiple lines.
   */
  const boost::regex variable_assigned_string_literal("^ *[^=]+ *= *\"\"\".*$");
  const boost::regex floating_string_literal("^ *\"\"\".*$");
  boost::smatch string_literal_match;
  bool found_docstring_terminator = false;
  // find the beginning of a string literal with variable assignment:
  //    varname = """...
  if (boost::regex_match(s, string_literal_match,
                         variable_assigned_string_literal)) {
    // this content *must be maintained* and returned to the caller
    std::ostringstream res;
    res << s;
    // if the variable assigned string literal spans multiple lines
    if (s.find("\"\"\"") == s.rfind("\"\"\"")) {
      // aggregate the content into a single line
      while (*line_number < loaded_lines.size() &&
             !found_docstring_terminator) {
        res << std::endl << loaded_lines.at(*line_number);
        ++*line_number;
        found_docstring_terminator = line.find("\"\"\"") != std::string::npos;
      }
    }
    return res.str();
  } else if (boost::regex_match(s, string_literal_match,
                                floating_string_literal)) {
    // if the unassigned string literal spans multiple lines
    if (s.find("\"\"\"") == s.rfind("\"\"\"")) {
      // just consume the content, don't bother to aggregate
      while (*line_number < loaded_lines.size() &&
             !found_docstring_terminator) {
        line = loaded_lines.at(*line_number);
        ++*line_number;
        found_docstring_terminator = line.find("\"\"\"") != std::string::npos;
      }
    }
    return "";
  }

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

void snakemake_unit_tests::split_comma_list(const std::string &s,
                                            std::vector<std::string> *target) {
  if (!target)
    throw std::runtime_error("null target vector to split_comma_list");
  target->clear();
  std::string::size_type loc = 0, cur = 0;
  while (true) {
    loc = s.find(", ", cur);
    if (loc == std::string::npos) {
      target->push_back(s.substr(cur));
      break;
    } else {
      target->push_back(s.substr(cur, loc - cur));
      cur = loc + 2;
    }
  }
}
