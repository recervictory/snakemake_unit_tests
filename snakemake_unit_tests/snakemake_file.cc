/*!
 @file snakemake_file.cc
 @brief implementation of snakemake_file class
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/snakemake_file.h"

void snakemake_unit_tests::snakemake_file::load_file(
    const std::string &filename, const std::string &base_dir) {
  std::ifstream input;
  bool verbose = true;
  try {
    input.open((base_dir + "/" + filename).c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake file \"" + filename +
                               "\"");
    rule_block rb;
    // while valid content is detected in the file
    std::cout << "starting file \"" << filename << "\" load" << std::endl;
    while (rb.load_snakemake_rule(input, filename)) {
      if (verbose) std::cout << "found a chunk" << std::endl;
      // determine if this block was actually an include directive
      if (rb.is_include_directive()) {
        if (verbose)
          std::cout << "found include directive, recursing to \""
                    << rb.get_recursive_filename() << "\"" << std::endl;
        // load the included file
        boost::filesystem::path recursive_path =
            base_dir + "/" + rb.get_recursive_filename();
        load_file(recursive_path.leaf().string(),
                  recursive_path.branch_path().string());
        // and now that the include has been performed, do not add the include
        // statement
      } else {
        // add the rule block to accumulation
        _blocks.push_back(rb);
      }
    }
    input.close();
    /*
      for snakemake 6.0 support: handle derived rules

      basically, for each rule, probe it to see if it has a base rule.
      if so, scan the rule set for that base rule, and then load any missing
      rule block contents from the base rule into the derived one.

      TODO(cpalmer718): support multiple layers of derived rules
     */
    // for each loaded rule
    for (std::vector<rule_block>::iterator iter = _blocks.begin();
         iter != _blocks.end(); ++iter) {
      // if it has a base class
      if (!iter->get_base_rule_name().empty()) {
        // locate the base class
        std::vector<rule_block>::const_iterator base_rule_finder;
        for (base_rule_finder = _blocks.begin();
             base_rule_finder != _blocks.end(); ++base_rule_finder) {
          if (!base_rule_finder->get_rule_name().compare(
                  iter->get_base_rule_name())) {
            break;
          }
        }
        // flag if the base rule is not present in loaded data
        if (base_rule_finder == _blocks.end()) {
          throw std::runtime_error(
              "derived rule \"" + iter->get_rule_name() +
              "\" requested base rule \"" + iter->get_base_rule_name() +
              "\", which could not be found in available snakefiles");
        }
        // for each of the arbitrarily many blocks in the base rule, push the
        // contents to the derived rule if the derived rule does not already
        // have a definition
        for (std::map<std::string, std::string>::const_iterator block_iter =
                 base_rule_finder->get_named_blocks().begin();
             block_iter != base_rule_finder->get_named_blocks().end();
             ++block_iter) {
          iter->offer_base_rule_contents(base_rule_finder->get_rule_name(),
                                         block_iter->first, block_iter->second);
        }
      }
    }
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
}

void snakemake_unit_tests::snakemake_file::print_blocks(
    std::ostream &out) const {
  for (std::vector<rule_block>::const_iterator iter = get_blocks().begin();
       iter != get_blocks().end(); ++iter) {
    iter->print_contents(out);
  }
}
