/*!
 @file rule_block.h
 @brief simple representation of parsed snakemake rule
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#ifndef SNAKEMAKE_UNIT_TESTS_RULE_BLOCK_H_
#define SNAKEMAKE_UNIT_TESTS_RULE_BLOCK_H_

#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
/*!
  @class rule_block
  @brief command line argument parser using boost::program_options
*/
class rule_block {
 public:
  /*!
    @brief default constructor
   */
  rule_block()
      : _rule_name(""),
        _base_rule_name(""),
        _global_indentation(0),
        _local_indentation(0) {}
  /*!
    @brief copy constructor
    @param obj existing rule block
   */
  rule_block(const rule_block &obj)
      : _rule_name(obj._rule_name),
        _base_rule_name(obj._base_rule_name),
        _named_blocks(obj._named_blocks),
        _code_chunk(obj._code_chunk),
        _global_indentation(obj._global_indentation),
        _local_indentation(obj._local_indentation) {}
  /*!
    @brief destructor
   */
  ~rule_block() throw() {}

  /*!
    @brief load a rule block or python chunk from a snakemake file vector
    @param loaded_lines vector of snakemake file lines to process
    @param filename name of the loaded snakemake file. only used for
    informative error messages
    @param global_indentation base indentation for entire file,
    defaulting to 0 for standard file loads and incrementing by
    4 for each level of indentation the include directive had
    within its python block
    @param verbose whether to report verbose logging output
    @param current_line currently probed line tracker
    @return whether a rule was successfully loaded

    this function will parse out a single rule from a snakemake file.
    it is designed to be called until it returns false.
   */
  bool load_content_block(const std::vector<std::string> &loaded_lines,
                          const boost::filesystem::path &filename,
                          unsigned global_indentation, bool verbose,
                          unsigned *current_line);

  /*!
    @brief having found a rule declaration, load its blocks
    @param loaded_lines vector of snakemake lines to process
    @param filename name of the loaded snakemake file. only used
    for informative error messages
    @param verbose whether to report verbose logging output
    @param current_line currently probed line tracker
    @return whether a rule was successfully loaded
   */
  bool consume_rule_contents(const std::vector<std::string> &loaded_lines,
                             const boost::filesystem::path &filename,
                             bool verbose, unsigned *current_line);

  /*!
    @brief set the name of the rule
    @param s new name of rule
   */
  void set_rule_name(const std::string &s) { _rule_name = s; }

  /*!
    @brief get the name of the rule
    @return the name of the rule

    @warning regex matching is currently borked, this entry may be garbage
   */
  const std::string &get_rule_name() const { return _rule_name; }

  /*!
    @brief set the name of the base rule for derived rules
    @param s new name of base rule
   */
  void set_base_rule_name(const std::string &s) { _base_rule_name = s; }

  /*!
    @brief get the name of the base rule for derived rules
    @return the name of the base rule if applicable, or an empty string
   */
  const std::string &get_base_rule_name() const { return _base_rule_name; }

  /*!
    @brief determine whether this block is a snakemake file include directive
    @return whether the block is an include directive, hopefully
   */
  bool is_include_directive() const;

  /*!
    @brief determine how much indentation an include directive enjoyed
    @return indentation of include directive, along with inherited global depth
   */
  unsigned get_include_depth() const;

  /*!
    @brief if the block is an include directive, get the file that is included
    @return the included filename
   */
  std::string get_recursive_filename() const;

  /*!
    @brief report mildly formatted contents to a stream
    @param out an open stream to which to write formatted contents
   */
  void print_contents(std::ostream &out) const;

  /*!
    @brief get internal storage of code chunk as const reference
    @return code chunk as const reference
  */
  const std::vector<std::string> &get_code_chunk() const { return _code_chunk; }

  /*!
    @brief get named blocks of rule body
    @return named blocks of rule body, or empty map
   */
  const std::map<std::string, std::string> &get_named_blocks() const {
    return _named_blocks;
  }

  /*!
    @brief provide candidate base rule block definitions for derived rules
    @param provider_name name of candidate base rule providing this information
    @param block_name name of block definition from the base rule
    @param block_values contents of block from the base rule

    for safety, the rule will check that such a suggestion is sane, and
    respond accordingly if not
   */
  void offer_base_rule_contents(const std::string &provider_name,
                                const std::string &block_name,
                                const std::string &block_values);

  /*!
    @brief manually inject code into this block
    @param s code to inject

    required for the redesign of the parser
   */
  void add_code_chunk(const std::string &s) { _code_chunk.push_back(s); }

 private:
  /*!
    @brief apply hackjob nonsense to flatten certain relative paths by one level
    @param s possible relative path to flatten
    @return the input line with any changes applied

    this is flagged to be handled some other way
    TODO(cpalmer718): find any better way of flattening relative include paths
   */
  std::string reduce_relative_paths(const std::string &s) const;

  /*!
    @brief return a string containing some number of whitespaces
    @param bonus extra whitespace to add beyond global and local default
   */
  std::string indentation(unsigned bonus) const;

  /*!
    @brief replace embedded newlines in a string with newlines plus indentation
    @param s input string, possibly with embedded newlines
    @param bonus extra indentation to apply beyond global and local default
    @return reformatted input string

    this is intended for use with rule block contents, and as such does
    not interact with the implied newline at the end of the string
   */
  std::string apply_indentation(const std::string &s, unsigned bonus) const;

  /*!
    @brief clear out internal storage
   */
  void clear();
  /*!
    @brief declared rule name

    needs to be unique, but should be if the snakemake run was successful
   */
  std::string _rule_name;
  /*!
    @brief name of base rule for derived rules

    only a snakemake 6.0+ feature. for these rules, some entries
    will be populated later
   */
  std::string _base_rule_name;
  /*!
    @brief arbitrary named blocks and their contents

    input and output blocks need to be handled specially,
    but the others can be arbitrarily specified.
   */
  std::map<std::string, std::string> _named_blocks;
  /*!
    @brief arbitrary python code chunk that can exist between rules

    this wasn't the original intention of this class, but rather than
    deal with derived classes and other nonsense, since arbitrary code blocks
    follow the same logic as rule blocks, they can just be stored in their
    own copy of this class
   */
  std::vector<std::string> _code_chunk;
  /*!
    @brief allow for global indentation of conditionally included files

    files included within python blocks should inherit the base depth
    of their include directive.
  */
  unsigned _global_indentation;
  /*!
    @brief allow for local indentation of conditionally included rules

    rules included within python blocks should temporarily have escalated
    indentation, which determines the end of the rule block later in the file
   */
  unsigned _local_indentation;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_RULE_BLOCK_H_
