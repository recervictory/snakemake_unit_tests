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
#include <utility>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/regex.hpp"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
/*!
  @brief python resolution status of a rule block
 */
typedef enum { UNRESOLVED, RESOLVED_INCLUDED, RESOLVED_EXCLUDED } block_status;
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
        _rule_is_checkpoint(false),
        _docstring(""),
        _local_indentation(0),
        _resolution(UNRESOLVED),
        _queried_by_python(false),
        _python_tag(0) {}
  /*!
    @brief copy constructor
    @param obj existing rule block
   */
  rule_block(const rule_block &obj)
      : _rule_name(obj._rule_name),
        _base_rule_name(obj._base_rule_name),
        _rule_is_checkpoint(obj._rule_is_checkpoint),
        _docstring(obj._docstring),
        _named_blocks(obj._named_blocks),
        _code_chunk(obj._code_chunk),
        _local_indentation(obj._local_indentation),
        _resolution(obj._resolution),
        _queried_by_python(obj._queried_by_python),
        _python_tag(obj._python_tag),
        _resolved_included_filename(obj._resolved_included_filename) {}
  /*!
    @brief destructor
   */
  ~rule_block() throw() {}

  /*!
    @brief load a rule block or python chunk from a snakemake file vector
    @param loaded_lines vector of snakemake file lines to process
    @param verbose whether to report verbose logging output
    @param current_line currently probed line tracker
    @return whether a rule was successfully loaded

    this function will parse out a single rule from a snakemake file.
    it is designed to be called until it returns false.
   */
  bool load_content_block(const std::vector<std::string> &loaded_lines, bool verbose, unsigned *current_line);

  /*!
    @brief having found a rule declaration, load its blocks
    @param loaded_lines vector of snakemake lines to process
    @param verbose whether to report verbose logging output
    @param current_line currently probed line tracker
    @return whether a rule was successfully loaded
   */
  bool consume_rule_contents(const std::vector<std::string> &loaded_lines, bool verbose, unsigned *current_line);

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
    @brief determine whether this block contains something that looks like
    an include directive but that doesn't conform to basic include syntax
    @return whether a non-standard include directive is in effect
   */
  bool contains_include_directive() const;

  /*!
    @brief extract the filename expression from an include statement
    @return the filename expression from an include statement

    the hope is to extract a python expression that the interpreter
    can report back on
   */
  std::string get_filename_expression() const;

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
    @return named blocks of rule body, or empty vector
   */
  const std::vector<std::pair<std::string, std::string> > &get_named_blocks() const { return _named_blocks; }

  /*!
    @brief get local indentation of rule block
    @return local indentation of rule block
   */
  unsigned get_local_indentation() const { return _local_indentation; }

  /*!
    @brief manually inject code into this block
    @param s code to inject

    required for the redesign of the parser
   */
  void add_code_chunk(const std::string &s) { _code_chunk.push_back(s); }

  /*!
    @brief test equality
    @param obj other rule_block object to compare to
    @return whether *this and obj contain the same contents

    note that indentation level is irrelevant for this comparison
   */
  bool operator==(const rule_block &obj) const {
    // local indentation *does not need to be equal*
    if (get_rule_name().compare(obj.get_rule_name())) return false;
    if (get_base_rule_name().compare(obj.get_base_rule_name())) return false;
    if (get_named_blocks().size() != obj.get_named_blocks().size()) return false;
    if (!std::equal(get_named_blocks().begin(), get_named_blocks().end(), obj.get_named_blocks().begin())) return false;
    if (!std::equal(get_code_chunk().begin(), get_code_chunk().end(), obj.get_code_chunk().begin())) return false;
    return true;
  }
  /*!
    @brief test inequality
    @param obj other rule_block object to compare to
    @return whether *this and obj contain the same contents

    note that indentation level is irrelevant for this comparison
   */
  bool operator!=(const rule_block &obj) const { return !(*this == obj); }

  /*!
    @brief whether the object's content needs a python pass to resolve
    @return whether the object's content needs a python pass to resolve
   */
  bool resolved() const { return _resolution != UNRESOLVED && _queried_by_python; }

  /*!
    @brief whether the object's contents should be included in the output
    @return whether the object's contents should be included in the output
   */
  bool included() const { return _resolution == RESOLVED_INCLUDED; }
  /*!
    @brief override the current resolution status based on external logic
    @param s new value for resolution status
   */
  void set_resolution(block_status s) { _resolution = s; }
  /*!
    @brief get the current resolution status
    @return the current resolution status
  */
  block_status get_resolution_status() const { return _resolution; }
  /*!
    @brief set the unique python tag for inclusion tracking
    @param u new value for unique python tag
   */
  void set_interpreter_tag(unsigned u) { _python_tag = u; }
  /*!
    @brief get the unique python tag for inclusion tracking
    @return the unique python tag for inclusion tracking
   */
  unsigned get_interpreter_tag() const { return _python_tag; }
  /*!
    @brief emit a python syntax reporting block to dummy file,
    until just after the first unresolved include directive
    is encountered (if ever)
    @param out open output stream for reporting
    @return whether the reporting terminated upon first instance
    of an unresolved include directive
   */
  bool report_python_logging_code(std::ostream &out);
  /*!
    @brief using python tag output, update resolution status
    @param tag_values loaded key(:value) pairs from python output
    @return whether the current rule allows downstream logic to continue
   */
  bool update_resolution(const std::map<std::string, std::string> &tag_values);
  /*!
    @brief for include statements, get actual filename for inclusion
    @return resolved filename
   */
  const boost::filesystem::path &get_resolved_included_filename() const { return _resolved_included_filename; }
  /*!
    @brief if this is a rule, is it a checkpoint
    @return whether, if a rule, this is a checkpoint

    uninterpretable if not a rule
  */
  bool is_checkpoint() const { return _rule_is_checkpoint; }
  /*!
    @brief set checkpoint status
    @param b new setting for checkpoint indicator

    uninterpretable if not a rule
   */
  void set_checkpoint(bool b) { _rule_is_checkpoint = b; }

 private:
  friend class rule_blockTest;
  friend class snakemake_fileTest;
  /*!
    @brief return a string containing some number of whitespaces
    @param count total whitespace indentation to apply
    @return formatted indentation whitespace
   */
  std::string indentation(unsigned count) const;

  /*!
    @brief replace embedded newlines in a string with newlines plus indentation
    @param s input string, possibly with embedded newlines
    @param count total whitespace indentation to apply
    @return reformatted input string

    this is intended for use with rule block contents, and as such does
    not interact with the implied newline at the end of the string
   */
  std::string apply_indentation(const std::string &s, unsigned count) const;

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
    @brief if the chunk is a rule, whether the rule is a checkpoint

    note that there are indications (28 Apr 2021) of checkpoints not functioning
    on cluster systems, so their use is somewhat dubious
   */
  bool _rule_is_checkpoint;
  /*!
    @brief optional docstring

    allow the rule to have an arbitrary docstring (or series of docstrings)
   */
  std::string _docstring;
  /*!
    @brief arbitrary named blocks and their contents

    ordering of blocks in rules is respected to prevent complications
    with intrinsic ordering requirements in snakemake and new snakemake
    features upstream
   */
  std::vector<std::pair<std::string, std::string> > _named_blocks;
  /*!
    @brief arbitrary python code chunk that can exist between rules

    this wasn't the original intention of this class, but rather than
    deal with derived classes and other nonsense, since arbitrary code blocks
    follow the same logic as rule blocks, they can just be stored in their
    own copy of this class
   */
  std::vector<std::string> _code_chunk;
  /*!
    @brief allow for local indentation of conditionally included rules

    rules included within python blocks should temporarily have escalated
    indentation, which determines the end of the rule block later in the file
   */
  unsigned _local_indentation;
  /*!
    @brief resolution status of block

    this can be: UNRESOLVED, RESOLVED_INCLUDED, RESOLVED_EXCLUDED
   */
  block_status _resolution;
  /*!
    @brief whether the block has been queried by python at least once

    the idea is: resolution status can only be known for certain if the
    tag has been emitted at least one time
  */
  bool _queried_by_python;
  /*!
    @brief tag for python interpreter tracking

    this is just a unique number that will be emitted to the python
    tracking pass
   */
  unsigned _python_tag;
  /*!
    @brief for include directives: resolved name of included file
   */
  boost::filesystem::path _resolved_included_filename;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_RULE_BLOCK_H_
