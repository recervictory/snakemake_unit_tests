/*!
  @file rule_block.cc
  @brief implementations of rule block class in
  snakemake_unit_tests project
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/rule_block.h"

bool snakemake_unit_tests::rule_block::load_snakemake_rule(
    std::ifstream &input, const std::string &filename, bool verbose) {
  // be sure input stream is open
  if (!input.is_open()) return false;
  // clear out internals, just to be safe
  clear();
  // define variables for processing
  std::string line = "", block_name = "", block_contents = "", raw_line = "";
  std::string::size_type line_indentation = 0;
  unsigned line_number = 0;
  std::ostringstream raw_content;
  // define regex patterns for testing
  const boost::regex standard_rule_declaration("^rule ([^ ]+):.*$");
  const boost::regex derived_rule_declaration(
      "^use rule ([^ ]+) as ([^ ]+) with:.*$");
  const boost::regex named_block_tag("^    ([a-zA-Z_\\-]+):(.*)$");
  // define state flags
  bool within_rule_block = false;
  while (input.peek() != EOF) {
    // read a line from file
    getline(input, line);
    ++line_number;
    // remove comments
    if (verbose) {
      std::cout << "line before chomping is \"" << line << "\"" << std::endl;
    }
    line = remove_comments_and_docstrings(line, &input, &line_number);
    if (verbose) {
      std::cout << "line after chomping is \"" << line << "\"" << std::endl;
    }
    // skip past empty lines, though not all of these are even valid snakemake
    if (line.empty() || line.find_first_not_of("\t ") == std::string::npos)
      continue;
    // switch behavior depending on state
    if (within_rule_block) {
      // all remaining lines must be indented. any lack of indentation means the
      // rule is done
      if (line.at(0) != ' ') {
        if (verbose) {
          std::cout << "content: \"" << raw_content.str()
                    << "\"; verdict: rule block" << std::endl;
        }
        --line_number;
        // return it to the file
        input.seekg(input.tellg() -
                    static_cast<std::ifstream::pos_type>(raw_line.size() + 2));
        return true;
      }
      raw_content << "\n" << line;
      // use pythonic indentation to flag an arbitrary number of named blocks
      line_indentation = line.find_first_not_of(" \t");
      // expose this to user space?
      if (line_indentation == 4) {
        // enforce named tag here
        boost::smatch named_block_tag_result;
        if (boost::regex_match(line, named_block_tag_result, named_block_tag)) {
          block_name = named_block_tag_result[1];
          block_contents = remove_comments_and_docstrings(
              named_block_tag_result[2], &input, &line_number);
          // logic currently assumes linting will split any block contents into
          // their own lines
          if (!block_contents.empty()) {
            throw std::logic_error(
                "snakemake named block single-line specification detected: "
                "file \"" +
                filename + "\" line " + std::to_string(line_number) +
                " entry \"" + line + "\"");
          }
          // while additional block contents are theoretically available
          while (input.peek() != EOF) {
            getline(input, line);
            raw_line = line;
            ++line_number;
            line = remove_comments_and_docstrings(line, &input, &line_number);
            line_indentation = line.find_first_not_of(" \t");
            // if a line that's not contents is found
            if (line_indentation < 5) {
              --line_number;
              // return it to the file
              input.seekg(input.tellg() - static_cast<std::ifstream::pos_type>(
                                              raw_line.size() + 2));
              // the block is aggregated. add it to the rule block
              _named_blocks[block_name] = block_contents;
              // proceed to the next possible block
              break;
            } else {
              // TODO(cpalmer718): deal with entries extending across multiple
              // lines? aggregate the contents with some formatting
              raw_content << "\n" << raw_line;
              block_contents += line + "\n";
            }
          }
          if (input.peek() == EOF) {
            if (verbose) std::cout << "file terminating" << std::endl;
            // catch dangling blocks at the end of files
            if (_named_blocks.find(block_name) == _named_blocks.end())
              _named_blocks[block_name] = block_contents;
            return true;
          }
        } else {
          throw std::runtime_error(
              "snakemake named block tag not found: file \"" + filename +
              "\" line " + std::to_string(line_number) + " entry \"" + line +
              "\"");
        }
      }
    } else {
      // flag indentation that does not match assumptions
      raw_content << "\n" << line;
      if (line.at(0) == ' ') {
        throw std::runtime_error(
            "snakemake format error detected: file \"" + filename + "\" line " +
            std::to_string(line_number) + " entry \"" + line + "\"");
      }
      // if the line is a valid rule declaration
      boost::smatch regex_result;
      if (boost::regex_match(line, regex_result, standard_rule_declaration)) {
        set_rule_name(regex_result[1]);
      } else if (boost::regex_match(line, regex_result,
                                    derived_rule_declaration)) {
        set_rule_name(regex_result[2]);
        // derived rules declare a base rule from which they inherit certain
        // fields. setting those certain fields must be deferred until all rules
        // are available.
        set_base_rule_name(regex_result[1]);
      } else {
        // if the entry is not a rule declaration, it is treated as some form
        // of arbitrary python/snakemake code.
        _code_chunk.push_back(line);
        // load any number of successive indented lines
        while (input.peek() != EOF) {
          getline(input, line);
          raw_line = line;
          ++line_number;
          line = remove_comments_and_docstrings(line, &input, &line_number);
          // if after pruning there's nothing here, ignore it
          if (line.empty()) continue;
          line_indentation = line.find_first_not_of(" \t");
          if (line_indentation > 0) {
            // this is a continuation of the previous command line, and should
            // be added
            _code_chunk.push_back(line);
            raw_content << "\n" << raw_line;
          } else {
            // this is either a different command or a rule block
            // return it to the file
            input.seekg(input.tellg() - static_cast<std::ifstream::pos_type>(
                                            raw_line.size() + 2));
            --line_number;
            // parse is complete
            if (verbose) {
              std::cout << "content: \"" << raw_content.str()
                        << "\"; verdict: arbitrary code chunk" << std::endl;
            }
            return true;
          }
        }
        // if this is the end of the file, cool. that was the last code block.
        if (input.peek() == EOF) {
          if (verbose) {
            std::cout << "file terminating" << std::endl;
          }
          return true;
        }
        // if the entry is not a rule declaration, upstream logic has failed
        throw std::logic_error(
            "snakemake rule declaration expected but not found: file \"" +
            filename + "\" line " + std::to_string(line_number) + " entry \"" +
            line + "\"");
      }
      within_rule_block = true;
    }
  }
  // end of file. probably ok?
  if (verbose) {
    std::cout << "content: \"" << raw_content.str() << "\"; verdict: "
              << (within_rule_block ? "rule block" : "end of file")
              << std::endl;
  }

  return within_rule_block;
}

bool snakemake_unit_tests::rule_block::is_include_directive() const {
  // what is an include directive?
  const boost::regex include_directive("^include: *\"(.*)\".*$");
  if (get_code_chunk().size() == 1) {
    boost::smatch include_match;
    return boost::regex_match(*get_code_chunk().begin(), include_match,
                              include_directive);
  }
  return false;
}

std::string snakemake_unit_tests::rule_block::get_recursive_filename() const {
  // what is an include directive?
  const boost::regex include_directive("^include: *\"(.*)\".*$");
  if (get_code_chunk().size() == 1) {
    // TODO(cpalmer718): figure out why regex submatches are failing on osx for
    // both boost and std regex implementations
    boost::smatch include_match;
    if (boost::regex_match(*get_code_chunk().begin(), include_match,
                           include_directive)) {
      // return include_match[1].str();
      std::string ret = get_code_chunk().begin()->substr(8);
      ret = ret.substr(ret.find_first_not_of(" ") + 1);
      ret = ret.substr(0, ret.find("\""));
      return ret;
    }
  }
  throw std::runtime_error(
      "get_recursive_filename() called in code block "
      "that does not match include directive pattern");
}

void snakemake_unit_tests::rule_block::print_contents(std::ostream &out) const {
  // report contents. may eventually be used for printing to custom snakefile
  if (!get_code_chunk().empty()) {
    for (std::vector<std::string>::const_iterator iter =
             get_code_chunk().begin();
         iter != get_code_chunk().end(); ++iter) {
      if (!(out << *iter << std::endl))
        throw std::runtime_error("code chunk printing error");
    }
  } else {
    if (!(out << "rule " << get_rule_name() << ":" << std::endl))
      throw std::runtime_error("rule name printing failure");
    for (std::map<std::string, std::string>::const_iterator iter =
             get_named_blocks().begin();
         iter != get_named_blocks().end(); ++iter) {
      if (!(out << "    " << iter->first << ":\n" << iter->second << std::endl))
        throw std::runtime_error("named block printing failure");
    }
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
