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
    std::ifstream &input, const std::string &filename) {
  // be sure input stream is open
  if (!input.is_open()) return false;
  // clear out internals, just to be safe
  clear();
  // define variables for processing
  std::string line = "", block_name = "", block_contents = "";
  std::string::size_type line_indentation = 0;
  unsigned line_number = 1;
  // define regex patterns for testing
  const boost::regex standard_rule_declaration("^rule ([^ ]+): *$");
  const boost::regex derived_rule_declaration(
      "^use rule ([^ ]+) as ([^ ]+) with: *$");
  const boost::regex named_block_tag("^    ([a-zA-Z_\\-]+):(.*)$");
  // define state flags
  bool within_rule_block = false;
  while (input.peek() != EOF) {
    // read a line from file
    getline(input, line);
    // remove comments
    line = remove_comments_and_docstrings(line);
    // skip past empty lines, though not all of these are even valid snakemake
    if (line.empty() || line.find_first_not_of("\t ") == std::string::npos)
      continue;
    // switch behavior depending on state
    if (within_rule_block) {
      // all remaining lines must be indented. any lack of indentation means the
      // rule is done
      if (line.at(0) != ' ') {
        return true;
      }
      // use pythonic indentation to flag an arbitrary number of named blocks
      line_indentation = line.find_first_not_of(" \t");
      // expose this to user space?
      if (line_indentation == 4) {
        // enforce named tag here
        boost::smatch named_block_tag_result;
        if (boost::regex_match(line, named_block_tag_result, named_block_tag)) {
          block_name = named_block_tag_result[1];
          block_contents =
              remove_comments_and_docstrings(named_block_tag_result[2]);
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
            line = remove_comments_and_docstrings(line);
            line_indentation = line.find_first_not_of(" \t");
            // if a line that's not contents is found
            if (line_indentation < 5) {
              // return it to the file
              input.seekg(input.tellg() -
                          static_cast<std::ifstream::pos_type>(line.size()));
              // the block is aggregated. add it to the rule block
              _named_blocks[block_name] = block_contents;
              // proceed to the next possible block
              break;
            } else {
              // TODO(cpalmer718): deal with entries extending across multiple
              // lines? aggregate the contents with some formatting
              block_contents += line + "\n";
            }
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
        set_rule_name(regex_result[1]);
        // derived rules declare a base rule from which they inherit certain
        // fields. setting those certain fields must be deferred until all rules
        // are available.
        set_base_rule_name(regex_result[2]);
      } else {
        // if the entry is not a rule declaration, it is treated as some form
        // of arbitrary python/snakemake code.
        _code_chunk.push_back(line);
        // load any number of successive indented lines
        while (input.peek() != EOF) {
          getline(input, line);
          line = remove_comments_and_docstrings(line);
          // if after pruning there's nothing here, ignore it
          if (line.empty()) continue;
          line_indentation = line.find_first_not_of(" \t");
          if (line_indentation > 0) {
            // this is a continuation of the previous command line, and should
            // be added
            _code_chunk.push_back(line);
          } else {
            // this is either a different command or a rule block
            // return it to the file
            input.seekg(input.tellg() -
                        static_cast<std::ifstream::pos_type>(line.size()));
            // parse is complete
            return true;
          }
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
  return within_rule_block;
}

bool snakemake_unit_tests::rule_block::is_include_directive() const {
  // TODO(cpalmer718): add implementation
}

std::string snakemake_unit_tests::rule_block::get_recursive_filename() const {
  // TODO(cpalmer718): add implementation
}

void snakemake_unit_tests::rule_block::clear() {
  _rule_name = _base_rule_name = "";
  _named_blocks.clear();
  _code_chunk.clear();
}
