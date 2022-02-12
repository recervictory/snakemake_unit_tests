/*!
  \file utilities.cc
  \brief implementations of general utility functions
  \author Cameron Palmer
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/utilities.h"

std::vector<std::string> snakemake_unit_tests::lexical_parse(
    const std::vector<std::string> &lines) {
  unsigned current_line = 0;
  bool string_open = false, literal_open = false;
  std::string aggregated_line = "", resolved_line = "";
  std::vector<std::string> results;
  quote_type active_quote_type = none;
  while (current_line < lines.size()) {
    // parse current line
    unsigned parse_index = 0;
    // if the parser is not currently dealing with an open string, skip starting
    // indentation. recall that the files are expected to be run through
    // snakefmt before this code, so we can assume some sanity.
    if (!string_open && !literal_open) {
      parse_index = lines.at(current_line).find_first_not_of(" \t");
    }
    bool line_consumed = false;
    while (parse_index < lines.at(current_line).size()) {
      if (lines.at(current_line).at(parse_index) == '\\') {
        // escape. determine context.
        // only care if this is line extension that's not embedded in a string
        // literal
        if (parse_index == lines.at(current_line).size() - 1 && !string_open &&
            !literal_open) {
          // this is line extension. add the line to the accumulator but do not
          // flush it make sure to strip the extension character
          resolved_line = lines.at(current_line)
                              .substr(0, lines.at(current_line).size() - 1);
          aggregated_line += resolved_line;
          ++current_line;
          line_consumed = true;
          break;
        }
      } else if (lines.at(current_line).at(parse_index) == '\'' ||
                 lines.at(current_line).at(parse_index) == '"') {
        resolve_string_delimiter(lines.at(current_line), &active_quote_type,
                                 &parse_index, &string_open, &literal_open);
      } else if (lines.at(current_line).at(parse_index) == '#') {
        // if we're not currently inside a string, this is a comment
        if (!string_open && !literal_open) {
          // this is a comment, terminate the line after removing this
          resolved_line = lines.at(current_line).substr(0, parse_index);
          concatenate_string_literals(resolved_line, &aggregated_line,
                                      &results);
          ++current_line;
          line_consumed = true;
          break;
        } else {
          // this comment character is embedded in a string and should be
          // skipped
          ++parse_index;
        }
      } else {
        // just a standard character, nothing to do
        ++parse_index;
      }
    }
    if (!line_consumed) {
      // the only issue here is whether we are currently inside a string.
      // if we are, this line isn't over.
      if (string_open || literal_open) {
        // add to aggregator
        aggregated_line += lines.at(current_line);
      } else {
        // finalize the line
        resolved_line = lines.at(current_line);
        concatenate_string_literals(resolved_line, &aggregated_line, &results);
      }
      ++current_line;
    }
  }
  if (!aggregated_line.empty()) {
    resolved_line = aggregated_line;
    aggregated_line = "";
    concatenate_string_literals(resolved_line, &aggregated_line, &results);
  }
  return results;
}

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
        found_docstring_terminator =
            loaded_lines.at(*line_number).find("\"\"\"") != std::string::npos;
        ++*line_number;
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
        found_docstring_terminator =
            loaded_lines.at(*line_number).find("\"\"\"") != std::string::npos;
        ++*line_number;
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

void snakemake_unit_tests::resolve_string_delimiter(
    const std::string &current_line, quote_type *active_quote_type,
    unsigned *parse_index, bool *string_open, bool *literal_open) {
  if (!active_quote_type || !parse_index || !string_open || !literal_open) {
    throw std::runtime_error(
        "null pointer provided to resolve_string_delimiter");
  }
  if (*parse_index >= current_line.size()) {
    throw std::runtime_error(
        "resolve_string_delimiter: index is out of line boundaries");
  }
  if (current_line.at(*parse_index) != '"' &&
      current_line.at(*parse_index) != '\'') {
    throw std::runtime_error(
        "resolve_string_delimiter: current parse index "
        "does not point to recognized string delimiter character");
  }
  // potentially a new string or closed string; depends on context
  // if this is escaped, then it's nothing
  if (*parse_index && current_line.at(*parse_index - 1) == '\\') {
    // find out how many escapes precede this character;
    // an even number means this is actually a quote; an
    // odd number means it's escaped and should be ignored
    unsigned n_escapes = 0;
    for (int i = *parse_index - 1; i >= 0 && current_line.at(i) == '\\';
         --i, ++n_escapes) {
    }
    if (n_escapes % 2) {
      // this is nothing, just continue parsing
      ++*parse_index;
      return;
    }
  }
  // determine what type of quote this is
  quote_type new_quote_type;
  if (*parse_index < current_line.size() - 2 &&
      current_line.at(*parse_index + 1) == current_line.at(*parse_index) &&
      current_line.at(*parse_index + 2) == current_line.at(*parse_index)) {
    if (current_line.at(*parse_index) == '"') {
      new_quote_type = triple_quote;
    } else {
      new_quote_type = triple_tick;
    }
  } else {
    if (current_line.at(*parse_index) == '"') {
      new_quote_type = single_quote;
    } else {
      new_quote_type = single_tick;
    }
  }
  if (*string_open) {
    // this has various interpretations
    if (*active_quote_type == new_quote_type ||
        (*active_quote_type == single_tick && new_quote_type == triple_tick) ||
        (*active_quote_type == single_quote &&
         new_quote_type == triple_quote)) {
      // the string is closed
      *string_open = false;
      // even if this was triple quote, just increment once, as only one is
      // consumed
    }
    // in any case, even if there is a mismatch in type here, and the character
    // should be skipped
    ++*parse_index;
  } else if (*literal_open) {
    // if this current thing is not a literal close, then it's a continuation
    if ((*active_quote_type == triple_tick && new_quote_type == triple_quote) ||
        (*active_quote_type == triple_quote && new_quote_type == triple_tick)) {
      // the thing we just found is just string content, as it's embedded in a
      // different type string
      *parse_index += 3;
    } else if (new_quote_type == single_quote ||
               new_quote_type == single_tick) {
      // again, just embedded string content
      ++*parse_index;
    } else {
      // the literal is closed
      *literal_open = false;
      *parse_index += 3;
    }
  } else {
    // the thing wasn't open, but it sure is now
    if (new_quote_type == triple_quote || new_quote_type == triple_tick) {
      *literal_open = true;
      *parse_index += 3;
    } else {
      *string_open = true;
      ++*parse_index;
    }
    *active_quote_type = new_quote_type;
  }
}

void snakemake_unit_tests::concatenate_string_literals(
    const std::string &resolved_line, std::string *aggregated_line,
    std::vector<std::string> *results) {
  if (!aggregated_line || !results) {
    throw std::runtime_error(
        "null pointer provided to concatenate_string_literals");
  }
  // if there is aggregated line content, that means that a previous line ended
  // in a slash line extension. this needs to be glued together first
  std::string candidate = resolved_line;
  if (!aggregated_line->empty()) {
    candidate = *aggregated_line + resolved_line;
    *aggregated_line = "";
  }
  // problem: we have a line to add, but in some cases it may need to be glued
  // to the previously processed line, if that line ended in a string delimiter
  // and this line started with one
  if (results->empty()) {
    // can't merge if there's nothing there
    results->push_back(candidate);
  } else if ((candidate.at(candidate.find_first_not_of(" \t")) == '\'' ||
              candidate.at(candidate.find_first_not_of(" \t")) == '"') &&
             (results->rbegin()->at(
                  results->rbegin()->find_last_not_of(" \t")) == '\'' ||
              results->rbegin()->at(
                  results->rbegin()->find_last_not_of(" \t")) == '"')) {
    // we can't deal with incompatible delimiters, even though they're valid
    // for concatenation. the hack solution is to glue the lines together with
    // an embedded true newline
    *results->rbegin() += "\n" + candidate;
  } else {
    results->push_back(candidate);
  }
}
