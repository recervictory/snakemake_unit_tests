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
