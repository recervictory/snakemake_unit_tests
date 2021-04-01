/*!
  @file rule_block.cc
  @brief implementations of rule block class in
  snakemake_unit_tests project
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/rule_block.h"

std::string snakemake_unit_tests::rule_block::reduce_relative_paths(
    const std::string &s) const {
  std::vector<std::string> reduced_relative_paths;
  reduced_relative_paths.push_back("../envs");
  reduced_relative_paths.push_back("../scripts");
  reduced_relative_paths.push_back("../report");
  std::string line = s;
  // hackjob nonsense:
  /*
    reduce by one level relative paths from within workflow/rules.
    this is necessary because recursively included files from rules/
    are flattened by one level when loaded.
  */
  for (std::vector<std::string>::const_iterator iter =
           reduced_relative_paths.begin();
       iter != reduced_relative_paths.end(); ++iter) {
    if (line.find(*iter) != std::string::npos) {
      line = line.substr(0, line.find(*iter)) + iter->substr(3) +
             line.substr(line.find(*iter) + iter->size());
    }
  }
  return line;
}

bool snakemake_unit_tests::rule_block::load_content_block(
    const std::vector<std::string> &loaded_lines,
    const boost::filesystem::path &filename, unsigned global_indentation,
    bool verbose, unsigned *current_line) {
  if (!current_line)
    throw std::runtime_error(
        "null pointer for counter passed to load_content_block");
  // clear out internals, just to be safe
  clear();
  // there's nothing to do for global indentation here, only when
  // content is being reported later; so just store it for now
  _global_indentation = global_indentation;
  // define variables for processing
  std::string line = "";
  // define regex patterns for testing
  const boost::regex standard_rule_declaration("^( *)rule ([^ ]+):.*$");
  const boost::regex derived_rule_declaration(
      "^( *)use rule ([^ ]+) as ([^ ]+) with:.*$");

  if (*current_line >= loaded_lines.size()) return false;
  while (*current_line < loaded_lines.size()) {
    line = loaded_lines.at(*current_line);
    ++*current_line;
    if (verbose) {
      std::cout << "considering line \"" << line << "\"" << std::endl;
    }
    line = remove_comments_and_docstrings(line, loaded_lines, current_line);
    if (verbose) {
      std::cout << "line reduced to \"" << line << "\"" << std::endl;
    }
    if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
      continue;
    // hack: deal with flattening of some relative paths when snakefiles are
    // merged
    line = reduce_relative_paths(line);
    // if the line is a valid rule declaration
    boost::smatch regex_result;
    if (boost::regex_match(line, regex_result, standard_rule_declaration)) {
      if (verbose) {
        std::cout << "consuming rule with name \"" << regex_result[2] << "\""
                  << std::endl;
      }
      set_rule_name(regex_result[2]);
      _local_indentation = regex_result[1].str().size();
      return consume_rule_contents(loaded_lines, filename, verbose,
                                   current_line);
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
                                   current_line);
    } else {
      // new to refactor: this is arbitrary python and we're leaving it like
      // that
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
    unsigned *current_line) {
  std::ostringstream regex_formatter;
  regex_formatter << "^" << indentation(get_local_indentation() + 4)
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
    line = remove_comments_and_docstrings(line, loaded_lines, current_line);
    if (verbose) {
      std::cout << "line reduced (in block) to \"" << line << "\"" << std::endl;
    }
    if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
      continue;
    line = reduce_relative_paths(line);

    // all remaining lines must be indented. any lack of indentation means the
    // rule is done
    if (line.find_first_not_of(' ') <= get_local_indentation()) {
      *current_line = starting_line;
      return true;
    }
    // use pythonic indentation to flag an arbitrary number of named blocks
    line_indentation = line.find_first_not_of(" ");
    // expose this to user space?
    if (line_indentation == get_local_indentation() + 4) {
      // enforce named tag here
      boost::smatch named_block_tag_result;
      if (boost::regex_match(line, named_block_tag_result, named_block_tag)) {
        block_name = named_block_tag_result[1];
        block_contents = remove_comments_and_docstrings(
            named_block_tag_result[2], loaded_lines, current_line);
        // while additional block contents are theoretically available
        while (*current_line < loaded_lines.size()) {
          // deal with reverting multiline consumption of content
          starting_line = *current_line;
          line = loaded_lines.at(*current_line);
          ++*current_line;
          line =
              remove_comments_and_docstrings(line, loaded_lines, current_line);
          if (line.empty() || line.find_first_not_of(" ") == std::string::npos)
            continue;
          line_indentation = line.find_first_not_of(" ");
          line = reduce_relative_paths(line);

          // if a line that's not contents is found
          if (line_indentation <= get_local_indentation() + 4) {
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
      } else {
        throw std::runtime_error(
            "snakemake named block tag not found: file \"" + filename.string() +
            "\" line " + std::to_string(*current_line) + " entry \"" + line +
            "\"");
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
      return include_match[1].str().size() + get_global_indentation();
    }
  }
  throw std::runtime_error(
      "get_include_depth() called in code block "
      "that does not match include directive pattern");
}

void snakemake_unit_tests::rule_block::report_python_logging_code(
    std::ostream &out) const {
  // report contents. may eventually be used for printing to custom snakefile
  if (!get_code_chunk().empty()) {
    // if this is an include directive of any kind
    if (contains_include_directive()) {
      // it should not under any circumstances be a simple include on a filename
      if (is_simple_include_directive()) {
        throw std::logic_error(
            "report_python_logging_code: unincluded simple inclusion "
            "suggests parsing pass has not been performed");
      }
      // it should not under any circumstances be unresolved
      if (_resolution != UNRESOLVED) {
        throw std::logic_error(
            "report_python_logging_code: resolved complex inclusion "
            "suggests post-python loading has not been performed");
      }
      // report tag along with required expression for evaluation
      if (!(out << indentation(get_global_indentation() +
                               get_local_indentation())
                << "print(\"tag" << get_interpreter_tag() << ": {}\".format("
                << get_filename_expression() << "))" << std::endl))
        throw std::runtime_error("complex include printing error");
    } else {
      // regardless of resolution, print other code as-is
      for (std::vector<std::string>::const_iterator iter =
               get_code_chunk().begin();
           iter != get_code_chunk().end(); ++iter) {
        if (!(out << indentation(get_global_indentation())
                  << apply_indentation(*iter, get_global_indentation())
                  << std::endl))
          throw std::runtime_error("code chunk printing error");
      }
    }
  } else {  // is a rule
    // if the rule has not already been flagged as untouched
    if (_resolution == RESOLVED_INCLUDED || _resolution == UNRESOLVED) {
      if (!(out << indentation(get_global_indentation() +
                               get_local_indentation())
                << "print(\"tag" << get_interpreter_tag() << "\")" << std::endl
                << std::endl
                << std::endl))
        throw std::runtime_error("rule interpreter code printing failure");
    }
  }
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
        std::cout << "updating rule \"" << get_rule_name()
                  << "\" to included status" << std::endl;
        return true;
      } else {
        // the tag is for an ambiguous include directive
        set_resolution(RESOLVED_INCLUDED);
        std::cout << "updating ambiguous include \"" << _code_chunk.at(0)
                  << "\" to \"" << indentation(get_include_depth())
                  << "include: \"" << finder->second << "\"\"" << std::endl;
        _code_chunk.at(0) = indentation(get_include_depth()) + "include: \"" +
                            finder->second + "\"";
        return false;
      }
    }
    std::cout << "cannot find tag" << get_interpreter_tag() << std::endl;
  }
  return true;
}

void snakemake_unit_tests::rule_block::print_contents(std::ostream &out) const {
  // report contents. may eventually be used for printing to custom snakefile
  if (!get_code_chunk().empty()) {
    for (std::vector<std::string>::const_iterator iter =
             get_code_chunk().begin();
         iter != get_code_chunk().end(); ++iter) {
      if (!(out << indentation(get_global_indentation())
                << apply_indentation(*iter, get_global_indentation())
                << std::endl))
        throw std::runtime_error("code chunk printing error");
    }
  } else {
    if (!(out << indentation(get_global_indentation() + get_local_indentation())
              << "rule " << get_rule_name() << ":" << std::endl))
      throw std::runtime_error("rule name printing failure");
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
        if (!(out << indentation(get_global_indentation() +
                                 get_local_indentation() + 4)
                  << iter->first << ":"
                  << apply_indentation(iter->second, get_global_indentation())
                  << std::endl))
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
        if (!(out << indentation(get_global_indentation() +
                                 get_local_indentation() + 4)
                  << iter->first << ":"
                  << apply_indentation(iter->second, get_global_indentation())
                  << std::endl))
          throw std::runtime_error("named block printing failure");
      }
    }
    // get last blocks
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (low_priority_blocks.find(iter->first) != low_priority_blocks.end()) {
        if (!(out << indentation(get_global_indentation() +
                                 get_local_indentation() + 4)
                  << iter->first << ":"
                  << apply_indentation(iter->second, get_global_indentation())
                  << std::endl))
          throw std::runtime_error("named block printing failure");
      }
    }
    // for snakefmt compatibility: emit two empty lines at the end of a rule
    if (!(out << std::endl << std::endl))
      throw std::runtime_error("rule padding printing error");
  }
}

void snakemake_unit_tests::rule_block::clear() {
  _rule_name = _base_rule_name = "";
  _named_blocks.clear();
  _code_chunk.clear();
}

void snakemake_unit_tests::rule_block::offer_base_rule_contents(
    const std::string &provider_name, const std::string &block_name,
    const std::string &block_values) {
  // make sure the suggestion is consistent with stored annotations
  if (provider_name.compare(get_base_rule_name()) ||
      get_base_rule_name().empty())
    return;
  // only update if there wasn't a definition in the derived rule itself
  if (_named_blocks.find(block_name) != _named_blocks.end()) return;
  // only now, perform the update
  _named_blocks[block_name] = block_values;
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
