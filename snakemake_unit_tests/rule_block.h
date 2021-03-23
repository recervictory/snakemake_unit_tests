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
  rule_block() : _rule_name(""), _base_rule_name("") {}
  /*!
    @brief copy constructor
    @param obj existing rule block
   */
  rule_block(const rule_block &obj)
      : _rule_name(obj._rule_name),
        _base_rule_name(obj._base_rule_name),
        _named_blocks(obj._named_blocks),
        _code_chunk(obj._code_chunk) {}
  /*!
    @brief destructor
   */
  ~rule_block() throw() {}

  /*!
    @brief load a rule block from a snakemake file stream
    @param input an open file stream connected to a snakemake file
    @param filename name of the open snakemake file. only used for
    informative error messages
    @param verbose whether to report verbose logging output
    @return whether a rule was successfully loaded

    this function will parse out a single rule from a snakemake file.
    it is designed to be called until it returns false.
   */
  bool load_snakemake_rule(std::ifstream &input, const std::string &filename,
                           bool verbose);

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

 private:
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
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_RULE_BLOCK_H_
