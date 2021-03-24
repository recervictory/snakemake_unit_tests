/*!
  @file solved_rules.cc
  @brief implementation of solved_rules class
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/solved_rules.h"

void snakemake_unit_tests::solved_rules::load_file(
    const std::string &filename) {
  std::ifstream input;
  std::string line = "";
  std::vector<std::string> input_filenames, output_filenames;
  const boost::regex standard_rule_declaration("^rule ([^ ]+):.*$");
  boost::smatch regex_result;
  try {
    // open log file
    input.open(filename.c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake log file \"" + filename +
                               "\"");
    // while log entries remain
    while (input.peek() != EOF) {
      getline(input, line);
      // if the line is a valid rule declaration
      if (boost::regex_match(line, regex_result, standard_rule_declaration)) {
        // set apparent rule name
        // for magical reasons, this regex seems to be working where
        // the include directive one wasn't; has to do with '/'?
        recipe rep;
        rep.set_rule_name(regex_result[1]);
        // scan for remaining rule content lines
        while (input.peek() != EOF) {
          getline(input, line);
          if (line.empty()) break;
          if (line.find("    input:") == 0) {
            // special handler for solved input files
            split_comma_list(line.substr(11), &input_filenames);
            for (std::vector<std::string>::const_iterator iter =
                     input_filenames.begin();
                 iter != input_filenames.end(); ++iter) {
              rep.add_input(*iter);
            }
          } else if (line.find("    output:") == 0) {
            // special handler for solved output files
            split_comma_list(line.substr(12), &output_filenames);
            for (std::vector<std::string>::const_iterator iter =
                     output_filenames.begin();
                 iter != output_filenames.end(); ++iter) {
              rep.add_output(*iter);
            }
          } else if (line.find("    log:") == 0) {
            // track log file but not 100% sure what to do with it.
            // snakemake --generate-unit-tests tends to fail when
            // log files get created. may need to add this to
            // an exclusion list.
            rep.set_log(line.substr(9));
          } else if (line.find("    jobid:") == 0 ||
                     line.find("    wildcards:") == 0) {
            // other recognized solution annotations;
            // for the moment, do nothing with them
          } else {
            // flag solution annotations that aren't present
            // in the example snakemake run, in case they
            // need to be specially handled
            throw std::logic_error("unrecognized snakemake log block: \"" +
                                   line + "\"; please file bug report");
          }
        }
        _recipes.push_back(rep);
      }
    }
    input.close();
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
}

void snakemake_unit_tests::solved_rules::emit_tests(
    const snakemake_file &sf, const boost::filesystem::path &output_test_dir,
    const boost::filesystem::path &pipeline_dir,
    const boost::filesystem::path &inst_dir,
    const std::vector<std::string> &exclude_rules,
    const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories) const {
  // create unit test output directory
  // by default, this looks like `.tests/unit`
  // but will be overridden as `output_test_dir/unit`
  // for compatibility with pytest
  boost::filesystem::path test_parent_path = output_test_dir.string() + "/unit";
  boost::filesystem::create_directories(test_parent_path);

  // repo files for creating pytest infrastructure
  boost::filesystem::path inst_test_py = inst_dir.string() + "/test.py";
  boost::filesystem::path inst_common_py = inst_dir.string() + "/common.py";
  if (!boost::filesystem::is_regular_file(inst_test_py) ||
      !boost::filesystem::is_regular_file(inst_common_py))
    throw std::runtime_error(
        "cannot locate required files test.py or common.py "
        "in inst directory \"" +
        inst_dir.string() + "\"");

  // create an excluded rule lookup, for filtering things the user
  // wants skipped
  std::map<std::string, bool> exclude_rule_lookup;
  for (std::vector<std::string>::const_iterator iter = exclude_rules.begin();
       iter != exclude_rules.end(); ++iter) {
    exclude_rule_lookup[*iter] = true;
  }

  // iterate across loaded recipes, creating tests as you go
  std::map<std::string, bool> test_history;
  for (std::vector<recipe>::const_iterator iter = _recipes.begin();
       iter != _recipes.end(); ++iter) {
    // only create output if the rule has not already been hit,
    // and if the user didn't want this rule disabled
    if (test_history.find(iter->get_rule_name()) == test_history.end() &&
        exclude_rule_lookup.find(iter->get_rule_name()) ==
            exclude_rule_lookup.end()) {
      test_history[iter->get_rule_name()] = true;
      // create a test output directory that is unique for this rule
      boost::filesystem::path rule_parent_path =
          test_parent_path.string() + "/" + iter->get_rule_name();
      boost::filesystem::create_directories(rule_parent_path);
      // create test directory, for output from test run
      boost::filesystem::path rule_expected_path =
          rule_parent_path.string() + "/expected";
      boost::filesystem::create_directories(rule_expected_path);
      // new to this program: create a workspace with all input directories
      boost::filesystem::path workspace_path =
          rule_parent_path.string() + "/workspace";
      boost::filesystem::create_directories(workspace_path);
      // copy *output* to expected path
      for (std::vector<std::string>::const_iterator output_iter =
               iter->get_outputs().begin();
           output_iter != iter->get_outputs().end(); ++output_iter) {
        // source file is: actual_snakemake_run/relative_path_to_file
        boost::filesystem::path source_file =
            pipeline_dir.string() + "/" + *output_iter;
        // target file is: rule_expected_path/relative_path_to_file
        boost::filesystem::path target_file =
            rule_expected_path.string() + "/" + *output_iter;
        // check source exists
        if (!boost::filesystem::is_regular_file(source_file) &&
            !boost::filesystem::is_directory(source_file)) {
          throw std::runtime_error("cannot find output file/directory \"" +
                                   source_file.string() + "\" for rule \"" +
                                   iter->get_rule_name() + "\"");
        }
        // create parent directories as needed
        boost::filesystem::create_directories(target_file.branch_path());
        // recursive copy
        boost::filesystem::copy(
            source_file, target_file,
            boost::filesystem::copy_options::overwrite_existing |
                boost::filesystem::copy_options::recursive);
      }
      // copy *input* to workspace
      for (std::vector<std::string>::const_iterator input_iter =
               iter->get_inputs().begin();
           input_iter != iter->get_inputs().end(); ++input_iter) {
        // source file is: actual_snakemake_run/relative_path_to_file
        boost::filesystem::path source_file =
            pipeline_dir.string() + "/" + *input_iter;
        // target file is: workspace_path/relative_path_to_file
        boost::filesystem::path target_file =
            workspace_path.string() + "/" + *input_iter;
        // check source exists
        if (!boost::filesystem::is_regular_file(source_file) &&
            !boost::filesystem::is_directory(source_file)) {
          throw std::runtime_error("cannot find input file \"" +
                                   source_file.string() + "\" for rule \"" +
                                   iter->get_rule_name() + "\"");
        }
        // create parent directories as needed
        boost::filesystem::create_directories(target_file.branch_path());
        // recursive copy
        boost::filesystem::copy(
            source_file, target_file,
            boost::filesystem::copy_options::overwrite_existing |
                boost::filesystem::copy_options::recursive);
      }
      // copy extra files and directories, if provided, to workspace
      for (std::vector<boost::filesystem::path>::const_iterator
               added_file_iter = added_files.begin();
           added_file_iter != added_files.end(); ++added_file_iter) {
        // source file is: actual_snakemake_run/relative_path_to_file
        boost::filesystem::path source_file = pipeline_dir / *added_file_iter;
        // target file is: workspace_path/relative_path_to_file
        boost::filesystem::path target_file = workspace_path / *added_file_iter;
        // check source exists
        if (!boost::filesystem::is_regular_file(source_file)) {
          throw std::runtime_error("cannot find added file \"" +
                                   source_file.string() + "\"");
        }
        // create parent directories as needed
        boost::filesystem::create_directories(target_file.branch_path());
        // copy
        boost::filesystem::copy(
            source_file, target_file,
            boost::filesystem::copy_options::overwrite_existing);
      }
      for (std::vector<boost::filesystem::path>::const_iterator
               added_directory_iter = added_directories.begin();
           added_directory_iter != added_directories.end();
           ++added_directory_iter) {
        // source file is: actual_snakemake_run/relative_path_to_directory
        boost::filesystem::path source_directory =
            pipeline_dir / *added_directory_iter;
        // target file is: workspace_path/relative_path_to_directory
        boost::filesystem::path target_directory =
            workspace_path / *added_directory_iter;
        // check source *directory* exists
        if (!boost::filesystem::is_directory(source_directory)) {
          throw std::runtime_error("cannot find added directory \"" +
                                   source_directory.string() + "\"");
        }
        // create parent directories as needed
        boost::filesystem::create_directories(target_directory.branch_path());
        // recursive copy
        boost::filesystem::copy(
            source_directory, target_directory,
            boost::filesystem::copy_options::overwrite_existing |
                boost::filesystem::copy_options::recursive);
      }
      // create parent directories for synthetic snakefile
      boost::filesystem::create_directories(workspace_path / "workflow");
      // create the synthetic snakefile in workspace/workflow/Snakefile
      std::string output_filename =
          (workspace_path / "workflow" / "Snakefile").string();
      std::ofstream output;
      output.open(output_filename.c_str());
      if (!output.is_open())
        throw std::runtime_error("cannot create synthetic snakemake file \"" +
                                 output_filename + "\"");
      // before adding anything else: add a single 'all' rule that points at
      // solved rule output files
      if (!(output << "rule all:\n    input:" << std::endl))
        throw std::runtime_error(
            "cannot write phony 'all' rule to synthetic snakefile");
      for (std::vector<std::string>::const_iterator output_iter =
               iter->get_outputs().begin();
           output_iter != iter->get_outputs().end(); ++output_iter) {
        if (!(output << "        \"" << *output_iter << "\"," << std::endl))
          throw std::runtime_error(
              "cannot write phony 'all' outputs to synthetic snakefile");
      }
      if (!(output << std::endl))
        throw std::runtime_error(
            "cannot write phony 'all' rule trailing whitespace to synthetic "
            "snakefile");
      // find the rule from the parsed snakefile(s) and report it to file
      sf.report_single_rule(iter->get_rule_name(), output);
      output.close();
      output.clear();
      // modify repo inst/test.py into a test runner for this rule
      std::string test_python_file =
          (test_parent_path / ("test_" + iter->get_rule_name() + ".py"))
              .string();
      output.open(test_python_file.c_str());
      if (!output.is_open())
        throw std::runtime_error("cannot write test python file \"" +
                                 test_python_file + "\"");
      if (!(output << "#!/usr/bin/env python3\ntestdir='"
                   << output_test_dir.string() << "'\nrulename='"
                   << iter->get_rule_name() << '\'' << std::endl))
        throw std::runtime_error(
            "cannot write rulename variable to test python file \"" +
            test_python_file + "\"");
      std::ifstream input;
      input.open(inst_test_py.string().c_str());
      if (!input.is_open())
        throw std::runtime_error("cannot read installed file \"" +
                                 inst_test_py.string() + "\"");
      if (!(output << input.rdbuf()))
        throw std::runtime_error("cannot dump \"" + inst_test_py.string() +
                                 "\" to output \"" + test_python_file + "\"");
      input.close();
      input.clear();
      output.close();
      output.clear();
    }
  }
  // emit common.py in the test_parent_path; no modifications needed
  boost::filesystem::copy(inst_common_py, test_parent_path);
}
