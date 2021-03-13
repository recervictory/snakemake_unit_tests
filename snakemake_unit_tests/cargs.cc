/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()("help,h", "emit this help message")(
      "output-test-dir,o",
      boost::program_options::value<std::string>()->default_value(".tests"),
      "top-level output directory for all tests")(
      "snakemake-log,s", boost::program_options::value<std::string>(),
      "snakemake log file for run that needs unit tests")(
      "added-files,a",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative file paths that will be installed alongside "
      "tests");
}
