/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()(
      "config,c", boost::program_options::value<std::string>(),
      "config yaml file specifying default options for other flags")(
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
      "inst-dir,i", boost::program_options::value<std::string>(),
      "snakemake_unit_tests inst directory")(
      "snakemake-log,l", boost::program_options::value<std::string>(),
      "snakemake log file for run that needs unit tests")(
      "output-test-dir,o", boost::program_options::value<std::string>(),
      "top-level output directory for all tests")(
      "pipeline-dir,p", boost::program_options::value<std::string>(),
      "top-level run directory for actual instance of pipeline (if not "
      "specified, "
      "will be computed as * assuming --snakefile is */workflow/Snakefile)")(
      "snakefile,s", boost::program_options::value<std::string>(),
      "snakefile used to run target pipeline")(
      "verbose,v", "emit verbose logging content; useful for debugging")(
      "update-all",
      "update all test content: snakefiles, inputs, outputs, added files and "
      "directories")("update-snakefiles", "update snakefiles in unit tests")(
      "update-added-content",
      "update added files and directories in unit tests")(
      "update-config", "update configuration report in unit test output")(
      "update-inputs", "update rule inputs in unit tests")(
      "update-outputs", "update rule outputs in unit test")(
      "update-pytest", "update pytest infrastructure in output directories");
}

snakemake_unit_tests::params snakemake_unit_tests::cargs::set_parameters()
    const {
  params p;
  // start with config yaml
  p.config_filename = get_config_yaml();
  // if the user specified a configuration file
  if (!p.config_filename.string().empty()) {
    // if the file exists at all
    if (boost::filesystem::is_regular_file(p.config_filename)) {
      // attempt to load it into yaml-cpp Node
      // the exception from yaml-cpp is reasonably informative,
      // so let that propagate upward
      p.config.load_file(p.config_filename.string());
      // do NOT accept help from config file
      // do NOT accept verbose from config file
      if (p.config.query_valid("output-test-dir")) {
        p.output_test_dir = p.config.get_entry("output-test-dir");
      }
      if (p.config.query_valid("snakefile")) {
        p.snakefile = p.config.get_entry("snakefile");
      }
      if (p.config.query_valid("pipeline-dir")) {
        p.pipeline_run_dir = p.config.get_entry("pipeline-dir");
      }
      if (p.config.query_valid("inst-dir")) {
        p.inst_dir = p.config.get_entry("inst-dir");
      }
      if (p.config.query_valid("snakemake-log")) {
        p.snakemake_log = p.config.get_entry("snakemake-log");
      }
      if (p.config.query_valid("added-files")) {
        p.added_files = vector_convert<boost::filesystem::path>(
            p.config.get_sequence("added-files"));
      }
      if (p.config.query_valid("added-directories")) {
        p.added_directories = vector_convert<boost::filesystem::path>(
            p.config.get_sequence("added-directories"));
      }
      if (p.config.query_valid("exclude-rules")) {
        p.exclude_rules = p.config.get_sequence("exclude-rules");
      }
    }
  }
  // load command line options
  // deal with overriding options from the config yaml
  // verbosity: just accept CLI, this shouldn't be a config yaml option
  p.verbose = verbose();
  // control which content gets updated: just accept CLI version
  p.update_all = update_all();
  p.update_snakefiles = update_snakefiles();
  p.update_added_content = update_added_content();
  p.update_config = update_config();
  p.update_inputs = update_inputs();
  p.update_outputs = update_outputs();
  p.update_pytest = update_pytest();

  // output_test_dir: override if specified
  p.output_test_dir =
      override_if_specified(get_output_test_dir(), p.output_test_dir);
  // snakefile: override if specified
  p.snakefile = override_if_specified(get_snakefile(), p.snakefile);
  // pipeline_run_dir: override if specified, but then handle differently
  p.pipeline_run_dir =
      override_if_specified(get_pipeline_dir(), p.pipeline_run_dir);
  if (p.pipeline_run_dir.string().empty()) {
    // behavior: if not specified, select it as the directory above
    // wherever the snakefile is installed
    // so we're assuming Snakefile is "something/workflow/Snakefile",
    // and we're setting pipeline_run_dir to "something"
    p.pipeline_run_dir =
        p.snakefile.remove_trailing_separator().parent_path().parent_path();
  }
  // inst_dir: override if specified
  p.inst_dir = override_if_specified(get_inst_dir(), p.inst_dir);
  // snakemake_log: override if specified
  p.snakemake_log = override_if_specified(get_snakemake_log(), p.snakemake_log);
  // added_files: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_files(), &p.added_files);
  // added_directories: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_directories(),
                                        &p.added_directories);
  // exclude_rules: augment whatever is present in config.yaml
  add_contents<std::string>(get_exclude_rules(), &p.exclude_rules);
  // add "all" to exclusion list, always
  // it's ok if it dups with user specification, it's uniqued later
  p.exclude_rules.push_back("all");

  // consistency checks
  // verbose is fine regardless
  // output_test_dir: doesn't have trailing separator, but doesn't have to exist
  p.output_test_dir = p.output_test_dir.remove_trailing_separator();
  // but it should at least be nonempty
  check_nonempty(p.output_test_dir, "output-test-dir");
  // snakefile: should exist, be regular file
  check_nonempty(p.snakefile, "snakefile");
  check_regular_file(p.snakefile, "", "snakefile");
  // pipeline_run_dir: should exist, be directory, no trailing separator
  check_nonempty(p.pipeline_run_dir, "pipeline-dir");
  check_and_fix_dir(&p.pipeline_run_dir, "", "pipeline-dir");
  // inst_dir: should exist, be directory
  check_nonempty(p.inst_dir, "inst-dir");
  check_and_fix_dir(&p.inst_dir, "", "inst-dir");
  //     should also contain two files: test.py and common.py
  try {
    check_regular_file("test.py", p.inst_dir, "inst-dir/test.py");
    check_regular_file("common.py", p.inst_dir, "inst-dir/common.py");
  } catch (...) {
    throw std::runtime_error(
        "inst directory \"" + p.inst_dir.string() +
        "\" exists, but"
        " doesn't appear to contain either 'common.py' or 'test.py',"
        " required infrastructure files from snakemake_unit_tests. "
        "If you've cloned and built snakemake_unit_tests_locally, "
        "you should provide /path/to/snakemake_unit_tests/inst for "
        "this option; otherwise, if using conda, you can provide "
        "$CONDA_PREFIX/share/snakemake_unit_tests/inst");
  }
  // snakemake_log: should exist, be a regular file
  check_nonempty(p.snakemake_log, "snakemake-log");
  check_regular_file(p.snakemake_log, "", "snakemake-log");
  // added_files: should be regular files, relative to pipeline run dir
  // doesn't have to be specified at all though
  for (std::vector<boost::filesystem::path>::iterator iter =
           p.added_files.begin();
       iter != p.added_files.end(); ++iter) {
    check_regular_file(*iter, p.pipeline_run_dir, "added-files");
  }
  // added_directories: should be directories, relative to pipeline run dir
  // doesn't have to be specified at all though
  for (std::vector<boost::filesystem::path>::iterator iter =
           p.added_directories.begin();
       iter != p.added_directories.end(); ++iter) {
    check_and_fix_dir(&(*iter), p.pipeline_run_dir, "added-directories");
  }

  // in theory, if they've made it this far, parameters are ready to go
  return p;
}

boost::filesystem::path snakemake_unit_tests::cargs::override_if_specified(
    const std::string &cli_entry,
    const boost::filesystem::path &params_entry) const {
  return cli_entry.empty() ? params_entry : boost::filesystem::path(cli_entry);
}

void snakemake_unit_tests::cargs::check_nonempty(
    const boost::filesystem::path &p, const std::string &msg) const {
  if (p.string().empty())
    throw std::logic_error("parameter \"" + msg +
                           "\" does not have a default value "
                           "and must be specified");
}

void snakemake_unit_tests::cargs::check_regular_file(
    const boost::filesystem::path &p, const boost::filesystem::path &prefix,
    const std::string &msg) const {
  boost::filesystem::path combined;
  // if the prefix is empty
  if (prefix.string().empty()) {
    // the candidate is ready for testing as-is
    combined = p;
  } else {
    // add it to the file candidate before testing
    combined = prefix / p;
  }
  // enforce it being a file
  // TODO(cpalmer718): add an option for warning instead of exception?
  if (!boost::filesystem::is_regular_file(combined)) {
    throw std::logic_error("for \"" + msg + "\", provided path \"" +
                           combined.string() + "\" is not a regular file");
  }
}

void snakemake_unit_tests::cargs::check_and_fix_dir(
    boost::filesystem::path *p, const boost::filesystem::path &prefix,
    const std::string &msg) const {
  if (!p)
    throw std::runtime_error("null pointer provided to check_and_fix_dir");
  // remove trailing separator if needed
  *p = p->remove_trailing_separator();
  boost::filesystem::path combined;
  // if the prefix is empty
  if (prefix.string().empty()) {
    // the candidate is ready for testing as-is
    combined = *p;
  } else {
    // add it to the file candidate before testing
    combined = prefix / *p;
  }
  // enforce it being a directory
  // TODO(cpalmer718): add an option for warning instead of exception?
  if (!boost::filesystem::is_directory(combined)) {
    throw std::logic_error("for \"" + msg + "\", provided path \"" +
                           combined.string() + "\" is not a directory");
  }
}

void snakemake_unit_tests::params::report_settings(
    const boost::filesystem::path &filename) const {
  // aggregate settings into a YAML stream, then report it in one shot
  YAML::Emitter out;
  out << YAML::BeginMap;
  // output-test-dir
  out << YAML::Key << "output-test-dir" << YAML::Value
      << output_test_dir.string();
  // snakefile
  out << YAML::Key << "snakefile" << YAML::Value << snakefile.string();
  // pipeline-dir
  out << YAML::Key << "pipeline-dir" << YAML::Value
      << pipeline_run_dir.string();
  // inst-dir
  out << YAML::Key << "inst-dir" << YAML::Value << inst_dir.string();
  // snakemake-log
  out << YAML::Key << "snakemake-log" << YAML::Value << snakemake_log.string();
  // added-files
  out << YAML::Key << "added-files" << YAML::Value << YAML::BeginSeq;
  for (std::vector<boost::filesystem::path>::const_iterator iter =
           added_files.begin();
       iter != added_files.end(); ++iter) {
    out << iter->string();
  }
  out << YAML::EndSeq;
  // added-directories
  out << YAML::Key << "added-directories" << YAML::Value << YAML::BeginSeq;
  for (std::vector<boost::filesystem::path>::const_iterator iter =
           added_directories.begin();
       iter != added_directories.end(); ++iter) {
    out << iter->string();
  }
  out << YAML::EndSeq;
  // exclude-rules
  out << YAML::Key << "exclude-rules" << YAML::Value << YAML::BeginSeq;
  for (std::vector<std::string>::const_iterator iter = exclude_rules.begin();
       iter != exclude_rules.end(); ++iter) {
    out << *iter;
  }
  out << YAML::EndSeq;
  out << YAML::EndMap;

  // write to output file
  std::ofstream output;
  output.open(filename.string().c_str());
  if (!output.is_open())
    throw std::runtime_error("cannot write to output config file \"" +
                             filename.string() + "\"");
  try {
    output << out.c_str();
    output.close();
  } catch (...) {
    if (output.is_open()) output.close();
    throw;
  }
}
