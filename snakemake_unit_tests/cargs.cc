/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()(
      "added-directories,d",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative directory paths that will be installed "
      "alongside tests")(
      "exclude-rules,e",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of rules to skip for testing")(
      "added-files,f",
      boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative file paths that will be installed alongside "
      "tests")("help,h", "emit this help message")(
      "snakemake-log,l", boost::program_options::value<std::string>(),
      "snakemake log file for run that needs unit tests")(
      "output-test-dir,o",
      boost::program_options::value<std::string>()->default_value(".tests"),
      "top-level output directory for all tests")(
      "pipeline-dir,p",
      boost::program_options::value<std::string>()->default_value(""),
      "top-level run directory for actual instance of pipeline (if not "
      "specified, "
      "will be computed as * assuming --snakefile is */workflow/Snakefile)")(
      "snakefile,s",
      boost::program_options::value<std::string>()->default_value(
          "workflow/Snakefile"),
      "snakefile used to run target pipeline")(
      "verbose,v", "emit verbose logging content; useful for debugging");
}
