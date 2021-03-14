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
  try {
    input.open((base_dir + "/" + filename).c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake file \"" + filename +
                               "\"");
    rule_block rb;
    // while valid content is detected in the file
    while (rb.load_snakemake_rule(input, filename)) {
      // determine if this block was actually an include directive
      if (rb.is_include_directive()) {
        // load the included file
        std::string recursive_filename = rb.get_recursive_filename();
        load_file(recursive_filename, base_dir);
        // and now that the include has been performed, do not add the include
        // statement
      } else {
        // add the rule block to accumulation
        _blocks.push_back(rb);
      }
    }
    input.close();
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
}
