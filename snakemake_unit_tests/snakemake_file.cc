/*!
 @file snakemake_file.cc
 @brief implementation of snakemake_file class
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/snakemake_file.h"

/*
  The parser reimplmentation will be structured as follows:

  - consider every piece of content of the file as *either*:
  -- a snakemake rule
  -- a (possibly incomplete) python instruction, which can further be classified
  as:
  --- unrelated to file parse
  --- an include directive (eventually split into: on strings or on
  variables/expressions

  The complete parse operation is as follows:

  1) add the top-level snakefile as a pseudo-include directive to the
  snakemake_file 2) parse the file, non-recursively 3) while unresolved
  snakemake rule and include directives remain:
  -- emit a dummy workspace containing a simplified representation of the
  currently loaded data
  ---- include directives and snakemake rules are assigned unique identifiers
  ---- their locations in file are replaced with tracking statements
  ---- python code unrelated to snakemake or include directives is included
  as-is
  -- run the python code, capture the screen output
  -- inspect queue of unresolved rules and include directives
  ---- if the next entry is a rule, determine whether the rule was evaluated and
  resolve, pop, continue
  ---- if the next entry is an include directive, determine whether the
  directive was evaluated
  ------ if not, flag it as such, do not process, pop, continue
  ------ if so, set it to resolved, pop it, stop processing resolutions
  -- parse the next include directive, without recursing. flag new rules and
  includes as potentially problematic


  Assorted comments:
  - the above logic is incredibly conservative, and designed to handle some
  additional issues not enumerated above (include directives on variables).
  - the logic can be loosened for some types of includes that can be handled in
  one pass, avoiding additional iterations of python evaluation. most notably,
  include directives at the same depth with no intervening python code, and
  0-depth includes in the currently parsed file.

 */

void snakemake_unit_tests::snakemake_file::load_everything(
    const boost::filesystem::path &filename,
    const boost::filesystem::path &base_dir, bool verbose) {
  // create a dummy rule block with just a single include directive for this
  // file
  boost::shared_ptr<rule_block> ptr(new rule_block);
  ptr->add_code_chunk("include: \"" + filename.string() + "\"");
  _blocks.push_back(ptr);
  std::vector<std::string> loaded_lines;
  // while any unresolved code chunk is present
  // TODO(cpalmer718): handle hackjob python interface (not at all trivial lol)
  bool unresolved = true;
  while (unresolved) {
    unresolved = false;
    for (std::list<boost::shared_ptr<rule_block> >::iterator iter =
             _blocks.begin();
         iter != _blocks.end(); ++iter) {
      if ((*iter)->is_include_directive()) {
        if (verbose)
          std::cout << "found include directive, adding \""
                    << (*iter)->get_recursive_filename() << "\"" << std::endl;
        // load the included file
        boost::filesystem::path recursive_path =
            base_dir / (*iter)->get_recursive_filename();
        load_lines(recursive_path, &loaded_lines);
        parse_file(loaded_lines, iter, recursive_path,
                   (*iter)->get_include_depth(), verbose);
        unresolved = true;
        // and now that the include has been performed, do not add the include
        // statement
        iter = _blocks.erase(iter);
      }
    }
  }

  // deal with derived rules
  resolve_derived_rules();
}

void snakemake_unit_tests::snakemake_file::load_lines(
    const boost::filesystem::path &filename,
    std::vector<std::string> *target) const {
  if (!target) throw std::runtime_error("null pointer to load_lines");
  target->clear();
  std::ifstream input;
  std::string line = "";
  try {
    input.open(filename.string().c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake file \"" +
                               filename.string() + "\"");
    // update: load all lines to memory first
    while (input.peek() != EOF) {
      getline(input, line);
      target->push_back(line);
    }
    input.close();
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
}

void snakemake_unit_tests::snakemake_file::resolve_derived_rules() {
  /*
    for snakemake 6.0 support: handle derived rules

    basically, for each rule, probe it to see if it has a base rule.
    if so, scan the rule set for that base rule, and then load any missing
    rule block contents from the base rule into the derived one.

    TODO(cpalmer718): support multiple layers of derived rules
  */
  // for each loaded rule
  for (std::list<boost::shared_ptr<rule_block> >::iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    // if it has a base class
    if (!(*iter)->get_base_rule_name().empty()) {
      // locate the base class
      std::list<boost::shared_ptr<rule_block> >::const_iterator
          base_rule_finder;
      for (base_rule_finder = _blocks.begin();
           base_rule_finder != _blocks.end(); ++base_rule_finder) {
        if (!(*base_rule_finder)
                 ->get_rule_name()
                 .compare((*iter)->get_base_rule_name())) {
          break;
        }
      }
      // flag if the base rule is not present in loaded data
      if (base_rule_finder == _blocks.end()) {
        throw std::runtime_error(
            "derived rule \"" + (*iter)->get_rule_name() +
            "\" requested base rule \"" + (*iter)->get_base_rule_name() +
            "\", which could not be found in available snakefiles");
      }
      // for each of the arbitrarily many blocks in the base rule, push the
      // contents to the derived rule if the derived rule does not already
      // have a definition
      for (std::map<std::string, std::string>::const_iterator block_iter =
               (*base_rule_finder)->get_named_blocks().begin();
           block_iter != (*base_rule_finder)->get_named_blocks().end();
           ++block_iter) {
        (*iter)->offer_base_rule_contents((*base_rule_finder)->get_rule_name(),
                                          block_iter->first,
                                          block_iter->second);
      }
    }
  }
}

void snakemake_unit_tests::snakemake_file::parse_file(
    const std::vector<std::string> &loaded_lines,
    std::list<boost::shared_ptr<rule_block> >::iterator insertion_point,
    const boost::filesystem::path &filename, unsigned global_indentation,
    bool verbose) {
  // track current line
  unsigned current_line = 0;
  while (current_line < loaded_lines.size()) {
    boost::shared_ptr<rule_block> rb(new rule_block);
    if (rb->load_content_block(loaded_lines, filename, global_indentation,
                               verbose, &current_line)) {
      _blocks.insert(insertion_point, rb);
    }
  }
}

void snakemake_unit_tests::snakemake_file::print_blocks(
    std::ostream &out) const {
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           get_blocks().begin();
       iter != get_blocks().end(); ++iter) {
    (*iter)->print_contents(out);
  }
}

void snakemake_unit_tests::snakemake_file::report_single_rule(
    const std::string &rule_name, std::ostream &out) const {
  // find the requested rule
  bool found_rule = false;
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           get_blocks().begin();
       iter != get_blocks().end(); ++iter) {
    // if this is the rule, that's great
    if (!(*iter)->get_rule_name().compare(rule_name)) {
      found_rule = true;
    }
    // if this is the rule or if it's not a rule at all,
    // report it to the synthetic snakefile
    if (!(*iter)->get_rule_name().compare(rule_name) ||
        (*iter)->get_rule_name().empty()) {
      (*iter)->print_contents(out);
    }
  }
  // if the correct rule was never found, complain
  if (!found_rule)
    throw std::runtime_error(
        "unable to locate log requested rule in scanned snakefiles: \"" +
        rule_name + "\"");
}
