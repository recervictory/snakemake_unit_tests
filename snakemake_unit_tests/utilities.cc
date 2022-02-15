/*!
  \file utilities.cc
  \brief implementations of general utility functions
  \author Cameron Palmer
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/utilities.h"

std::vector<std::string> snakemake_unit_tests::lexical_parse(const std::vector<std::string> &lines) {
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
        if (parse_index == lines.at(current_line).size() - 1 && !string_open && !literal_open) {
          // this is line extension. add the line to the accumulator but do not
          // flush it make sure to strip the extension character
          resolved_line = lines.at(current_line).substr(0, lines.at(current_line).size() - 1);
          aggregated_line += resolved_line;
          ++current_line;
          line_consumed = true;
          break;
        }
      } else if (lines.at(current_line).at(parse_index) == '\'' || lines.at(current_line).at(parse_index) == '"') {
        resolve_string_delimiter(lines.at(current_line), &active_quote_type, &parse_index, &string_open, &literal_open);
      } else if (lines.at(current_line).at(parse_index) == '#') {
        // if we're not currently inside a string, this is a comment
        if (!string_open && !literal_open) {
          // this is a comment, terminate the line after removing this
          resolved_line = lines.at(current_line).substr(0, parse_index);
          append_resolved_line(resolved_line, &aggregated_line, &results);
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
        aggregated_line += lines.at(current_line) + "\n";
      } else {
        // finalize the line
        resolved_line = lines.at(current_line);
        append_resolved_line(resolved_line, &aggregated_line, &results);
      }
      ++current_line;
    }
  }
  if (!aggregated_line.empty()) {
    resolved_line = aggregated_line;
    aggregated_line = "";
    append_resolved_line(resolved_line, &aggregated_line, &results);
  }
  return results;
}

void snakemake_unit_tests::split_comma_list(const std::string &s, std::vector<std::string> *target) {
  if (!target) throw std::runtime_error("null target vector to split_comma_list");
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

void snakemake_unit_tests::resolve_string_delimiter(const std::string &current_line, quote_type *active_quote_type,
                                                    unsigned *parse_index, bool *string_open, bool *literal_open) {
  if (!active_quote_type || !parse_index || !string_open || !literal_open) {
    throw std::runtime_error("null pointer provided to resolve_string_delimiter");
  }
  if (*parse_index >= current_line.size()) {
    throw std::runtime_error("resolve_string_delimiter: index is out of line boundaries");
  }
  if (current_line.at(*parse_index) != '"' && current_line.at(*parse_index) != '\'') {
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
    for (int i = *parse_index - 1; i >= 0 && current_line.at(i) == '\\'; --i, ++n_escapes) {
    }
    if (n_escapes % 2) {
      // this is nothing, just continue parsing
      ++*parse_index;
      return;
    }
  }
  // determine what type of quote this is
  quote_type new_quote_type;
  if (*parse_index < current_line.size() - 2 && current_line.at(*parse_index + 1) == current_line.at(*parse_index) &&
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
    if (*active_quote_type == new_quote_type || (*active_quote_type == single_tick && new_quote_type == triple_tick) ||
        (*active_quote_type == single_quote && new_quote_type == triple_quote)) {
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
    } else if (new_quote_type == single_quote || new_quote_type == single_tick) {
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

void snakemake_unit_tests::append_resolved_line(const std::string &resolved_line, std::string *aggregated_line,
                                                std::vector<std::string> *results) {
  if (!aggregated_line || !results) {
    throw std::runtime_error("null pointer provided to append_resolved_line");
  }
  // if there is aggregated line content, that means that a previous line ended
  // in a slash line extension. this needs to be glued together first
  std::string candidate = resolved_line;
  // trailing whitespace on this line solution should be stripped before further
  // processing
  candidate = candidate.substr(0, candidate.find_last_not_of(" \t") + 1);
  if (!aggregated_line->empty()) {
    candidate = *aggregated_line + candidate;
    *aggregated_line = "";
  }
  // new: ignore string concatenation because the parser isn't smart
  // enough to resolve it as is
  results->push_back(candidate);
}
