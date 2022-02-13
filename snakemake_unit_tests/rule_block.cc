/*!
  @file rule_block.cc
  @brief implementations of rule block class in
  snakemake_unit_tests project
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/rule_block.h"

bool snakemake_unit_tests::rule_block::load_content_block(
    const std::vector<std::string> &loaded_lines,
    const boost::filesystem::path &filename, bool verbose,
    unsigned *current_line) {
  if (!current_line)
    throw std::runtime_error(
        "null pointer for counter passed to load_content_block");
  // clear out internals, just to be safe
  clear();
  // define variables for processing
  std::string line = "";
  // define regex patterns for testing
  const boost::regex standard_rule_declaration("^( *)rule ([^ ]+):.*$");
  const boost::regex checkpoint_rule_declaration("^( *)checkpoint ([^ ]+):.*$");
  const boost::regex derived_rule_declaration(
      "^( *)use rule ([^ ]+) as ([^ ]+) with:.*$");
  const boost::regex wildcard_constraints("^( *)wildcard_constraints:.*$");
  const boost::regex configfile("^( *)configfile:.*$");
  if (*current_line >= loaded_lines.size()) return false;
  while (*current_line < loaded_lines.size()) {
    line = loaded_lines.at(*current_line);
    ++*current_line;
    if (verbose) {
      std::cout << "considering line \"" << line << "\"" << std::endl;
    }
    // remove_comments_and_docstrings is deprecated by lexical parser
    if (verbose) {
      std::cout << "line reduced to \"" << line << "\"" << std::endl;
    }
    if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
      continue;
    // if the line is a valid rule declaration
    boost::smatch regex_result;
    if (boost::regex_match(line, regex_result, standard_rule_declaration) ||
        boost::regex_match(line, regex_result, checkpoint_rule_declaration)) {
      if (verbose) {
        std::cout << "consuming rule with name \"" << regex_result[2] << "\""
                  << std::endl;
      }
      set_rule_name(regex_result[2]);
      if (line.find_first_not_of(" ") == line.find("checkpoint")) {
        set_checkpoint(true);
      }
      _local_indentation = regex_result[1].str().size();
      return consume_rule_contents(loaded_lines, filename, verbose,
                                   current_line, 4);
    } else if (boost::regex_match(line, regex_result,
                                  derived_rule_declaration)) {
      if (verbose) {
        std::cout << "consuming derived rule with name \"" << regex_result[3]
                  << "\"" << std::endl;
      }
      set_rule_name(regex_result[3]);
      _local_indentation += regex_result[1].str().size();
      // derived rules declare a base rule from which they inherit certain
      // fields. setting those certain fields must be deferred until all rules
      // are available.
      set_base_rule_name(regex_result[2]);
      return consume_rule_contents(loaded_lines, filename, verbose,
                                   current_line, 4);
    } else if (boost::regex_match(line, regex_result, wildcard_constraints) ||
               boost::regex_match(line, regex_result, configfile)) {
      // assorted specifically handled snakemake directives
      if (verbose) {
        std::cout << "adding snakemake directive \"" << line << "\""
                  << std::endl;
      }
      _local_indentation += regex_result[1].str().size();
      --*current_line;
      return consume_rule_contents(loaded_lines, filename, verbose,
                                   current_line, 0);
    } else {
      // new to refactor: this is arbitrary python and we're leaving it like
      // that
      // TODO(cpalmer718): refactor include directives out of python directives
      if (verbose) {
        std::cout << "adding code chunk \"" << line << "\"" << std::endl;
      }
      _code_chunk.push_back(line);
      return true;
    }
  }
  // end of file. return if something actually loaded
  return !_rule_name.empty() || !_code_chunk.empty();
}

bool snakemake_unit_tests::rule_block::consume_rule_contents(
    const std::vector<std::string> &loaded_lines,
    const boost::filesystem::path &filename, bool verbose,
    unsigned *current_line, unsigned block_base_increment) {
  std::ostringstream regex_formatter;
  regex_formatter << "^"
                  << indentation(get_local_indentation() + block_base_increment)
                  << "([a-zA-Z_\\-]+):(.*)$";
  const boost::regex named_block_tag(regex_formatter.str());
  if (!current_line)
    throw std::runtime_error(
        "null pointer for counter passed to consume_rule_contents");
  std::string line = "", block_name = "", block_contents = "";
  std::string::size_type line_indentation = 0;
  unsigned starting_line = 0;
  while (*current_line < loaded_lines.size()) {
    // deal with reverting multiline consumption of content
    starting_line = *current_line;
    line = loaded_lines.at(*current_line);
    ++*current_line;
    if (verbose) {
      std::cout << "considering (in block) line \"" << line << "\""
                << std::endl;
    }
    // remove_comments_and_docstrings is deprecated by lexical parser
    if (verbose) {
      std::cout << "line reduced (in block) to \"" << line << "\"" << std::endl;
    }
    if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
      continue;

    // all remaining lines must be indented. any lack of indentation means the
    // rule is done
    // note that this now only affects things where block base increment is
    // nonzero. if it is zero, there is only one block being processed, and it
    // terminates after that block is consumed
    if (line.find_first_not_of(' ') <= get_local_indentation() &&
        (block_base_increment ||
         (!block_base_increment && !_named_blocks.empty()))) {
      *current_line = starting_line;
      return true;
    }
    // use pythonic indentation to flag an arbitrary number of named blocks
    line_indentation = line.find_first_not_of(" ");
    // expose this to user space?
    if (line_indentation == get_local_indentation() + block_base_increment) {
      // enforce named tag here
      boost::smatch named_block_tag_result;
      if (boost::regex_match(line, named_block_tag_result, named_block_tag)) {
        block_name = named_block_tag_result[1];
        block_contents = named_block_tag_result[2];
        // remove_comments_and_docstrings is deprecated by lexical parser
        // while additional block contents are theoretically available
        while (*current_line < loaded_lines.size()) {
          // deal with reverting multiline consumption of content
          starting_line = *current_line;
          line = loaded_lines.at(*current_line);
          ++*current_line;
          // remove_comments_and_docstrings is deprecated by lexical parser
          if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
            continue;
          line_indentation = line.find_first_not_of(" ");

          // if a line that's not contents is found
          if (line_indentation <=
              get_local_indentation() + block_base_increment) {
            *current_line = starting_line;
            if (verbose) {
              std::cout << "storing a block with name \"" << block_name
                        << "\" and contents \"" << block_contents << "\""
                        << std::endl;
            }
            // the block is aggregated. add it to the rule block
            _named_blocks[block_name] = block_contents;
            // proceed to the next possible block
            break;
          } else {
            // TODO(cpalmer718): deal with entries extending across multiple
            // lines? aggregate the contents with some formatting
            block_contents += "\n" + line;
          }
        }
        if (*current_line >= loaded_lines.size()) {
          // catch dangling blocks at the end of files
          if (_named_blocks.find(block_name) == _named_blocks.end()) {
            if (verbose) {
              std::cout << "storing a terminal block with name \"" << block_name
                        << "\" and contents \"" << block_contents << "\""
                        << std::endl;
            }
            _named_blocks[block_name] = block_contents;
          }
          return true;
        }
      } else if (_named_blocks.empty() &&
                 (line.at(line.find_first_not_of(" \t")) == '\'' ||
                  line.at(line.find_first_not_of(" \t")) == '"')) {
        if (_docstring.empty()) {
          _docstring = line;
        } else {
          _docstring += "\n" + line;
        }
      } else {
        std::cerr << "warning: in a rule parse, the line \"" << line
                  << "\" is found floating and is removed. if this behavior "
                  << "is not what you want, please file a bug report"
                  << std::endl;
        continue;
      }
    }
  }
  return true;
}

bool snakemake_unit_tests::rule_block::contains_include_directive() const {
  // what is an include directive?
  const boost::regex include_directive("^( *)include: *(.*) *$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    return boost::regex_match(*get_code_chunk().begin(), include_match,
                              include_directive);
  }
  return false;
}

std::string snakemake_unit_tests::rule_block::get_filename_expression() const {
  // what is an include directive?
  const boost::regex include_directive("^( *)include: *(.*) *$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    if (boost::regex_match(*get_code_chunk().begin(), include_match,
                           include_directive)) {
      return include_match[2].str();
    }
  }
  throw std::runtime_error(
      "get_filename_expression() called in code block "
      "that does not match include directive pattern");
}

bool snakemake_unit_tests::rule_block::is_simple_include_directive() const {
  // what is an include directive?
  const boost::regex include_directive("^( *)include: *\"(.*)\" *$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    return boost::regex_match(*get_code_chunk().begin(), include_match,
                              include_directive);
  }
  return false;
}

std::string snakemake_unit_tests::rule_block::get_standard_filename() const {
  // what is an include directive?
  const boost::regex include_directive("^( *)include: *\"(.*)\" *$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    if (boost::regex_match(*get_code_chunk().begin(), include_match,
                           include_directive)) {
      return include_match[2].str();
    }
  }
  throw std::runtime_error(
      "get_standard_filename() called in code block "
      "that does not match include directive pattern");
}

unsigned snakemake_unit_tests::rule_block::get_include_depth() const {
  // what is an include directive?
  const boost::regex include_directive("^( *)include: *(.*) *$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    if (boost::regex_match(*get_code_chunk().begin(), include_match,
                           include_directive)) {
      return include_match[1].str().size();
    }
  }
  throw std::runtime_error(
      "get_include_depth() called in code block "
      "that does not match include directive pattern");
}

bool snakemake_unit_tests::rule_block::report_python_logging_code(
    std::ostream &out) const {
  // report contents. may eventually be used for printing to custom snakefile
  if (!get_code_chunk().empty()) {
    // if this is an include directive of any kind
    if (contains_include_directive()) {
      // it can be resolved, in which case, it can sometimes be included
      if (_resolution == RESOLVED_INCLUDED) {
        if (!(out << *get_code_chunk().rbegin() << std::endl))
          throw std::runtime_error("include statement printing error");
      }
      // report tag along with required expression for evaluation
      if (!(out << indentation(get_local_indentation())
                << indentation(
                       get_code_chunk().rbegin()->find_first_not_of(" "))
                << "print(\"tag" << get_interpreter_tag() << ": {}\".format("
                << get_filename_expression() << "))" << std::endl))
        throw std::runtime_error("complex include printing error");
      // new: terminate immediately if this was an unresolved
      // include directive
      if (_resolution == UNRESOLVED) {
        return true;
      }
    } else {
      // regardless of resolution, print other code as-is
      for (std::vector<std::string>::const_iterator iter =
               get_code_chunk().begin();
           iter != get_code_chunk().end(); ++iter) {
        if (!(out << *iter << std::endl))
          throw std::runtime_error("code chunk printing error");
      }
    }
  } else if (!get_rule_name().empty()) {  // is a rule
    // new logic: must print tag each time, in case status changes later
    if (!(out << indentation(get_local_indentation()) << "print(\"tag"
              << get_interpreter_tag() << "\")" << std::endl
              << std::endl
              << std::endl))
      throw std::runtime_error("rule interpreter code printing failure");
  } else {  // is a snakemake metacontent block
    // rule name is empty but blocks are not.
    // switching to direct snakemake interpretation, in which case these
    // need to be included
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (!(out << indentation(get_local_indentation()) << iter->first << ":"
                << iter->second << std::endl))
        throw std::runtime_error("snakemake directive printing failure");
    }
  }
  return false;
}

bool snakemake_unit_tests::rule_block::update_resolution(
    const std::map<std::string, std::string> &tag_values) {
  std::map<std::string, std::string>::const_iterator finder;
  // tag==0 entries are python code that doesn't require inclusion tracking
  if (get_interpreter_tag()) {
    finder = tag_values.find("tag" + std::to_string(get_interpreter_tag()));
    if (finder != tag_values.end()) {
      // if the tag is for a rule
      if (finder->second.empty()) {
        set_resolution(RESOLVED_INCLUDED);
        return true;
      } else {
        // the tag is for an ambiguous include directive
        set_resolution(RESOLVED_INCLUDED);
        if (_resolved_included_filename.compare(finder->second)) {
          _resolved_included_filename = finder->second;
          return false;
        } else {
          return true;
        }
      }
    } else {
      // new: if it's not present and nonzero tag, flag as unincluded
      // however, this can be updated on additional passes; the logic
      // is that additional passes will keep occurring as long as
      // some new include directive appears on this next pass
      set_resolution(RESOLVED_EXCLUDED);
    }
  }
  return true;
}

void snakemake_unit_tests::rule_block::print_contents(std::ostream &out) const {
  // report contents. may eventually be used for printing to custom snakefile
  if (!get_code_chunk().empty()) {  // python code
    for (std::vector<std::string>::const_iterator iter =
             get_code_chunk().begin();
         iter != get_code_chunk().end(); ++iter) {
      if (!(out << *iter << std::endl))
        throw std::runtime_error("code chunk printing error");
    }
  } else if (!get_rule_name().empty()) {  // rule
    if (!get_base_rule_name().empty()) {
      if (!(out << indentation(get_local_indentation()) << "use rule "
                << get_base_rule_name() << " as " << get_rule_name()
                << " with:" << std::endl))
        throw std::runtime_error("base rule name printing failure");
    } else if (!(out << indentation(get_local_indentation())
                     << (is_checkpoint() ? "checkpoint " : "rule ")
                     << get_rule_name() << ":" << std::endl)) {
      throw std::runtime_error("rule name printing failure");
    }
    // if docstring is present, report it
    if (!_docstring.empty()) {
      if (!(out << _docstring << std::endl)) {
        throw std::runtime_error("docstring printing failure");
      }
    }
    // enforce restrictions on block order
    std::map<std::string, bool> high_priority_blocks, low_priority_blocks;
    high_priority_blocks["input"] = true;
    high_priority_blocks["output"] = true;
    low_priority_blocks["run"] = true;
    low_priority_blocks["shell"] = true;
    low_priority_blocks["script"] = true;
    low_priority_blocks["wrapper"] = true;
    low_priority_blocks["cwl"] = true;
    // get first blocks
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (high_priority_blocks.find(iter->first) !=
          high_priority_blocks.end()) {
        if (!(out << indentation(get_local_indentation() + 4) << iter->first
                  << ":" << iter->second << std::endl))
          throw std::runtime_error("named block printing failure");
      }
    }
    // get intermediate blocks
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (high_priority_blocks.find(iter->first) ==
              high_priority_blocks.end() &&
          low_priority_blocks.find(iter->first) == low_priority_blocks.end()) {
        if (!(out << indentation(get_local_indentation() + 4) << iter->first
                  << ":" << iter->second << std::endl))
          throw std::runtime_error("named block printing failure");
      }
    }
    // get last blocks
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (low_priority_blocks.find(iter->first) != low_priority_blocks.end()) {
        if (!(out << indentation(get_local_indentation() + 4) << iter->first
                  << ":" << iter->second << std::endl))
          throw std::runtime_error("named block printing failure");
      }
    }
    // for snakefmt compatibility: emit two empty lines at the end of a rule
    if (!(out << std::endl << std::endl))
      throw std::runtime_error("rule padding printing error");
  } else {
    // snakemake metacontent block
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (!(out << indentation(get_local_indentation()) << iter->first << ":"
                << iter->second << std::endl))
        throw std::runtime_error("named block printing failure");
    }
  }
}

void snakemake_unit_tests::rule_block::clear() {
  _rule_name = _base_rule_name = "";
  _named_blocks.clear();
  _code_chunk.clear();
}

std::string snakemake_unit_tests::rule_block::indentation(
    unsigned count) const {
  std::ostringstream o;
  for (unsigned i = 0; i < count; ++i) {
    o << ' ';
  }
  return o.str();
}

std::string snakemake_unit_tests::rule_block::apply_indentation(
    const std::string &s, unsigned count) const {
  // this is a multi-line string with embedded newlines. the newlines need
  // to be replaced by "\n[some number of spaces]"
  std::string res = s, indent = indentation(count);
  std::string::size_type loc = 0, cur = 0;
  while ((loc = res.find('\n', cur)) != std::string::npos) {
    res = res.substr(0, loc + 1) + indent + res.substr(loc + 1);
    cur = loc + 1 + indent.size();
  }
  return res;
}

void snakemake_unit_tests::rule_block::report_rulesdot_rules(
    std::map<std::string, bool> *target) const {
  if (!target)
    throw std::runtime_error("null pointer provided to report_rulesdot_rules");
  boost::regex pattern("rules\\.([^\\.]+)\\.");
  boost::sregex_token_iterator end;
  // scan both code chunk and block contents
  for (std::map<std::string, std::string>::const_iterator iter =
           _named_blocks.begin();
       iter != _named_blocks.end(); ++iter) {
    boost::sregex_token_iterator finder(iter->second.begin(),
                                        iter->second.end(), pattern, 0);
    for (; finder != end; ++finder) {
      (*target)[finder->str().substr(6, finder->str().size() - 7)] = true;
    }
  }
  for (std::vector<std::string>::const_iterator iter = _code_chunk.begin();
       iter != _code_chunk.end(); ++iter) {
    boost::sregex_token_iterator finder(iter->begin(), iter->end(), pattern, 0);
    for (; finder != end; ++finder) {
      (*target)[finder->str().substr(6, finder->str().size() - 7)] = true;
    }
  }
}
