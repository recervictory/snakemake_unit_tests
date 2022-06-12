/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()("config,c", boost::program_options::value<std::string>(),
                      "config yaml file specifying default options for other flags")(
      "added-directories,d", boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative directory paths that will be installed "
      "alongside tests")("include-rules,n", boost::program_options::value<std::vector<std::string> >(),
                         "optional set of rules to include in testing")(
      "exclude-rules,e", boost::program_options::value<std::vector<std::string> >(),
      "optional set of rules to skip for testing")(
      "added-files,f", boost::program_options::value<std::vector<std::string> >(),
      "optional set of relative file paths that will be installed alongside "
      "tests")("help,h", "emit this help message")("inst-dir,i", boost::program_options::value<std::string>(),
                                                   "snakemake_unit_tests inst directory")(
      "snakemake-log,l", boost::program_options::value<std::string>(),
      "snakemake log file for run that needs unit tests")(
      "output-test-dir,o", boost::program_options::value<std::string>(), "top-level output directory for all tests")(
      "pipeline-top-dir,p", boost::program_options::value<std::string>(),
      "top-level pipeline directory for actual instance of pipeline (if not "
      "specified, "
      "will be computed as * assuming --snakefile is */workflow/Snakefile)")(
      "pipeline-run-dir,r", boost::program_options::value<std::string>(),
      "directory from which the pipeline was actually run, relative to "
      "pipeline-top-dir; defaults to '.'")("snakefile,s", boost::program_options::value<std::string>(),
                                           "snakefile used to run target pipeline")(
      "verbose,v", "emit verbose logging content; useful for debugging")(
      "update-all",
      "update all test content: snakefiles, inputs, outputs, added files and "
      "directories")("update-snakefiles", "update snakefiles in unit tests")(
      "update-added-content", "update added files and directories in unit tests")(
      "update-config", "update configuration report in unit test output")(
      "update-inputs", "update rule inputs in unit tests")("update-outputs", "update rule outputs in unit test")(
      "update-pytest", "update pytest infrastructure in output directories")(
      "include-entire-dag",
      "add entire DAG to test snakefiles, instead of choosing target rules "
      "only (not recommended)");
}

snakemake_unit_tests::params snakemake_unit_tests::cargs::set_parameters(bool use_schema_validation) const {
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
      /*
        new: handle inst as quickly as possible

        - the idea here is that we want to enable schema validation,
          to provide the user some sanity checks; but the schema is present
          in inst, so to actually trigger the validator, we need to do
          at least some interface processing first
       */
      if (p.config.query_valid("inst-dir")) {
        p.inst_dir = p.config.get_entry("inst-dir");
      }
      p.inst_dir = override_if_specified(get_inst_dir(), p.inst_dir);
      // now: validate the config with a json schema spec.
      // note that, due to the override behavior of the program,
      // this doesn't enforce all of what it might; rather, it
      // primarily detects config files with unsupported features
      if (use_schema_validation) {
        validate_config(p.config_filename, p.inst_dir);
      }

      if (p.config.query_valid("output-test-dir")) {
        p.output_test_dir = p.config.get_entry("output-test-dir");
      }
      if (p.config.query_valid("snakefile")) {
        p.snakefile = p.config.get_entry("snakefile");
      }
      if (p.config.query_valid("pipeline-top-dir")) {
        p.pipeline_top_dir = p.config.get_entry("pipeline-top-dir");
      }
      if (p.config.query_valid("pipeline-run-dir")) {
        p.pipeline_run_dir = p.config.get_entry("pipeline-run-dir");
      }
      if (p.config.query_valid("inst-dir")) {
        p.inst_dir = p.config.get_entry("inst-dir");
      }
      if (p.config.query_valid("snakemake-log")) {
        p.snakemake_log = p.config.get_entry("snakemake-log");
      }
      if (p.config.query_valid("added-files")) {
        p.added_files = vector_convert<boost::filesystem::path>(p.config.get_sequence("added-files"));
      }
      if (p.config.query_valid("added-directories")) {
        p.added_directories = vector_convert<boost::filesystem::path>(p.config.get_sequence("added-directories"));
      }
      if (p.config.query_valid("include-rules")) {
        p.include_rules = vector_to_map<std::string>(p.config.get_sequence("include-rules"));
      }
      if (p.config.query_valid("exclude-rules")) {
        p.exclude_rules = vector_to_map<std::string>(p.config.get_sequence("exclude-rules"));
      }
      if (p.config.query_valid("exclude-patterns")) {
        p.exclude_patterns = vector_to_map<std::string>(p.config.get_sequence("exclude-patterns"));
      }
      if (p.config.query_valid("comparators")) {
        p.comparators = p.config.get_node("comparators");
      }
    } else {
      throw std::runtime_error("configuration file \"" + p.config_filename.string() + "\" is not a regular file");
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
  p.include_entire_dag = include_entire_dag();

  // output_test_dir: override if specified
  p.output_test_dir = override_if_specified(get_output_test_dir(), p.output_test_dir);
  // snakefile: override if specified
  p.snakefile = override_if_specified(get_snakefile(), p.snakefile);
  // pipeline_run_dir: override if specified, but then handle differently
  p.pipeline_top_dir = override_if_specified(get_pipeline_top_dir(), p.pipeline_top_dir);
  if (p.pipeline_top_dir.string().empty()) {
    // behavior: if not specified, select it as the directory above
    // wherever the snakefile is installed
    // so we're assuming Snakefile is "something/workflow/Snakefile",
    // and we're setting pipeline_run_dir to "something"
    p.pipeline_top_dir = p.snakefile.remove_trailing_separator().parent_path().parent_path();
  }
  p.pipeline_run_dir = override_if_specified(get_pipeline_run_dir(), p.pipeline_run_dir);
  if (p.pipeline_run_dir.string().empty()) {
    // behavior: if not specified, select it as '.', that is,
    // the same directory as pipeline-top-dir
    p.pipeline_run_dir = boost::filesystem::path(".");
  }
  // inst_dir: override if specified
  // this is sort of handled above; but if no config file is provided,
  // that code above won't be executed, and this directive still works
  p.inst_dir = override_if_specified(get_inst_dir(), p.inst_dir);
  // snakemake_log: override if specified
  p.snakemake_log = override_if_specified(get_snakemake_log(), p.snakemake_log);
  // added_files: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_files(), &p.added_files);
  // added_directories: augment whatever is present in config.yaml
  add_contents<boost::filesystem::path>(get_added_directories(), &p.added_directories);
  // include_rules: augment whatever is present in config.yaml
  // note that I'm not sure that this is best? what is the intuitive behavior?
  // override with command line? I guess we'll have to wait and see
  add_contents<std::string>(get_include_rules(), &p.include_rules);
  // exclude_rules: augment whatever is present in config.yaml
  add_contents<std::string>(get_exclude_rules(), &p.exclude_rules);
  // add "all" to exclusion list, always
  // it's ok if it dups with user specification, it's uniqued later
  p.exclude_rules["all"] = true;

  // consistency checks
  // verbose is fine regardless
  // output_test_dir: doesn't have trailing separator, but doesn't have to exist
  p.output_test_dir = p.output_test_dir.remove_trailing_separator();
  // but it should at least be nonempty
  check_nonempty(p.output_test_dir, "output-test-dir");
  // snakefile: should exist, be regular file
  check_nonempty(p.snakefile, "snakefile");
  check_regular_file(p.snakefile, "", "snakefile");
  // pipeline_top_dir: should exist, be directory, no trailing separator
  check_nonempty(p.pipeline_top_dir, "pipeline-top-dir");
  check_and_fix_dir(&p.pipeline_top_dir, "", "pipeline-topdir");
  // pipeline_run_dir: should exist, be directory relative to pipeline-top-dir
  check_nonempty(p.pipeline_run_dir, "pipeline-run-dir");
  p.pipeline_run_dir = p.pipeline_run_dir.remove_trailing_separator();
  if (!boost::filesystem::is_directory(p.pipeline_top_dir / p.pipeline_run_dir))
    throw std::runtime_error("pipeline run directory \"" + p.pipeline_run_dir.string() +
                             "\" should be a valid path relative to pipeline top directory");
  // inst_dir: should exist, be directory
  check_nonempty(p.inst_dir, "inst-dir");
  check_and_fix_dir(&p.inst_dir, "", "inst-dir");
  //     should also contain two files: test.py and common.py
  try {
    check_regular_file("test.py", p.inst_dir, "inst-dir/test.py");
    check_regular_file("common.py", p.inst_dir, "inst-dir/common.py");
  } catch (...) {
    throw std::runtime_error("inst directory \"" + p.inst_dir.string() +
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
  // added_files: should be regular files, relative to pipeline top dir
  // doesn't have to be specified at all though
  for (std::vector<boost::filesystem::path>::iterator iter = p.added_files.begin(); iter != p.added_files.end();
       ++iter) {
    check_regular_file(*iter, p.pipeline_top_dir, "added-files");
  }
  // added_directories: should be directories, relative to pipeline top dir
  // doesn't have to be specified at all though
  for (std::vector<boost::filesystem::path>::iterator iter = p.added_directories.begin();
       iter != p.added_directories.end(); ++iter) {
    check_and_fix_dir(&(*iter), p.pipeline_top_dir, "added-directories");
  }

  // in theory, if they've made it this far, parameters are ready to go
  return p;
}

boost::filesystem::path snakemake_unit_tests::cargs::override_if_specified(
    const std::string &cli_entry, const boost::filesystem::path &params_entry) const {
  return cli_entry.empty() ? params_entry : boost::filesystem::path(cli_entry);
}

void snakemake_unit_tests::cargs::check_nonempty(const boost::filesystem::path &p, const std::string &msg) const {
  if (p.string().empty())
    throw std::logic_error("parameter \"" + msg +
                           "\" does not have a default value "
                           "and must be specified");
}

void snakemake_unit_tests::cargs::check_regular_file(const boost::filesystem::path &p,
                                                     const boost::filesystem::path &prefix,
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
    throw std::logic_error("for \"" + msg + "\", provided path \"" + combined.string() + "\" is not a regular file");
  }
}

void snakemake_unit_tests::cargs::check_and_fix_dir(boost::filesystem::path *p, const boost::filesystem::path &prefix,
                                                    const std::string &msg) const {
  if (!p) throw std::runtime_error("null pointer provided to check_and_fix_dir");
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
    throw std::logic_error("for \"" + msg + "\", provided path \"" + combined.string() + "\" is not a directory");
  }
}

void snakemake_unit_tests::params::report_settings(const boost::filesystem::path &filename) const {
  // aggregate settings into a YAML stream, then report it in one shot
  YAML::Emitter out;
  out << YAML::BeginMap;
  // output-test-dir
  out << YAML::Key << "output-test-dir" << YAML::Value << boost::filesystem::absolute(output_test_dir).string();
  // snakefile
  out << YAML::Key << "snakefile" << YAML::Value << boost::filesystem::absolute(snakefile).string();
  // pipeline-top-dir
  out << YAML::Key << "pipeline-top-dir" << YAML::Value << boost::filesystem::absolute(pipeline_top_dir).string();
  // pipeline-run-dir
  out << YAML::Key << "pipeline-run-dir" << YAML::Value << boost::filesystem::absolute(pipeline_run_dir).string();
  // inst-dir
  out << YAML::Key << "inst-dir" << YAML::Value << boost::filesystem::absolute(inst_dir).string();
  // snakemake-log
  out << YAML::Key << "snakemake-log" << YAML::Value << boost::filesystem::absolute(snakemake_log).string();
  // added-files
  emit_yaml_vector(&out, added_files, "added-files");
  // added-directories
  emit_yaml_vector(&out, added_directories, "added-directories");
  // include-rules
  emit_yaml_map(&out, include_rules, "include-rules");
  // exclude-rules
  emit_yaml_map(&out, exclude_rules, "exclude-rules");
  // exclude-patterns
  emit_yaml_map(&out, exclude_patterns, "exclude-patterns");
  // comparators
  if (comparators.size()) {
    out << YAML::Key << "comparators" << YAML::Value << comparators;
  }
  // end the content
  out << YAML::EndMap;
  // write to output file
  std::ofstream output;
  output.open(filename.string().c_str());
  if (!output.is_open()) throw std::runtime_error("cannot write to output config file \"" + filename.string() + "\"");
  try {
    output << out.c_str() << std::endl;
    output.close();
  } catch (...) {
    if (output.is_open()) output.close();
    throw;
  }
}

void snakemake_unit_tests::params::emit_yaml_map(YAML::Emitter *out, const std::map<std::string, bool> &data,
                                                 const std::string &key) const {
  if (!out) throw std::runtime_error("null pointer to emit_yaml_map");
  *out << YAML::Key << key << YAML::Value;
  if (!data.empty()) {
    *out << YAML::BeginSeq;
    for (std::map<std::string, bool>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
      *out << iter->first;
    }
    *out << YAML::EndSeq;
  } else {
    *out << YAML::Null;
  }
}

void snakemake_unit_tests::params::emit_yaml_vector(YAML::Emitter *out,
                                                    const std::vector<boost::filesystem::path> &data,
                                                    const std::string &key) const {
  if (!out) throw std::runtime_error("null pointer to emit_yaml_vector");
  *out << YAML::Key << key << YAML::Value;
  if (!data.empty()) {
    *out << YAML::BeginSeq;
    for (std::vector<boost::filesystem::path>::const_iterator iter = data.begin(); iter != data.end(); ++iter) {
      *out << iter->string();
    }
    *out << YAML::EndSeq;
  } else {
    *out << YAML::Null;
  }
}

void snakemake_unit_tests::cargs::validate_config(const boost::filesystem::path &config_filename,
                                                  const boost::filesystem::path &inst_directory) const {
  boost::filesystem::path schema = inst_directory / "user_config_schema.yaml";
  if (!boost::filesystem::exists(schema)) {
    throw std::runtime_error("expected json schema file \"" + schema.string() + "\" could not be located");
  }
  std::string command =
      "python3 -c \"from snakemake.utils import validate ; import yaml ; "
      "validate(yaml.safe_load(open(\\\"" +
      config_filename.string() + "\\\", \\\"r\\\")), schema=\\\"" + schema.string() + "\\\")\"";
  try {
    std::vector<std::string> result = exec(command, true, true);
  } catch (const std::runtime_error &e) {
    std::string message =
        "validation of --config/-c file \"" + config_filename.string() +
        "\" has failed. "
        "the schema for this validation is at \"" +
        schema.string() +
        "\" for review. due to how the "
        "command line and config files interact with snakemake_unit_tests, this schema does not strictly "
        "enforce most properties. however, it does detect configuration features that are not part of "
        "the currently supported feature set. examples of likely candidates for this detection are:\n\n"
        "  - 'pipeline_dir', deprecated in favor of 'pipeline-top-dir' and 'pipeline-run-dir';\n"
        "  - 'exclude-extensions' and 'exclude-paths', deprecated in favor of 'exclude-patterns';\n"
        "  - 'byte-comparisons', deprecated in favor of 'comparators'\n\n"
        "the newest feature set options are available with examples in config.example.yaml.";
    throw std::logic_error(message);
  }
}
