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
                     line.find("    wildcards:") == 0 ||
                     line.find("    benchmark:") == 0 ||
                     line.find("    resources:") == 0) {
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
    const std::vector<boost::filesystem::path> &added_directories,
    bool update_snakefiles, bool update_added_content, bool update_inputs,
    bool update_outputs, bool update_pytest) const {
  // create unit test output directory
  // by default, this looks like `.tests/unit`
  // but will be overridden as `output_test_dir/unit`
  // for compatibility with pytest
  boost::filesystem::path test_parent_path = output_test_dir / "unit";
  boost::filesystem::create_directories(test_parent_path);

  // repo files for creating pytest infrastructure
  boost::filesystem::path inst_test_py = inst_dir / "test.py";
  boost::filesystem::path inst_common_py = inst_dir / "common.py";
  if (!boost::filesystem::is_regular_file(inst_test_py) ||
      !boost::filesystem::is_regular_file(inst_common_py))
    throw std::runtime_error(
        "cannot locate required files test.py or common.py "
        "in inst directory \"" +
        inst_dir.string() + "\"");

  // iterate across loaded recipes, creating tests as you go
  std::map<std::string, bool> test_history;
  for (std::vector<recipe>::const_iterator iter = _recipes.begin();
       iter != _recipes.end(); ++iter) {
    if (test_history.find(iter->get_rule_name()) == test_history.end()) {
      create_workspace(*iter, sf, output_test_dir, test_parent_path,
                       pipeline_dir, inst_test_py, exclude_rules, added_files,
                       added_directories, update_snakefiles,
                       update_added_content, update_inputs, update_outputs,
                       update_pytest);
      test_history[iter->get_rule_name()] = true;
    }
  }
  // emit common.py in the test_parent_path; no modifications needed
  if (update_pytest) {
    boost::filesystem::copy(
        inst_common_py, test_parent_path,
        boost::filesystem::copy_options::overwrite_existing |
            boost::filesystem::copy_options::recursive);
  }
}

void snakemake_unit_tests::solved_rules::create_workspace(
    const recipe &rec, const snakemake_file &sf,
    const boost::filesystem::path &output_test_dir,
    const boost::filesystem::path &test_parent_path,
    const boost::filesystem::path &pipeline_dir,
    const boost::filesystem::path &inst_test_py,
    const std::vector<std::string> &exclude_rules,
    const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories,
    bool update_snakefiles, bool update_added_content, bool update_inputs,
    bool update_outputs, bool update_pytest) const {
  // create an excluded rule lookup, for filtering things the user
  // wants skipped
  std::map<std::string, bool> exclude_rule_lookup;
  for (std::vector<std::string>::const_iterator iter = exclude_rules.begin();
       iter != exclude_rules.end(); ++iter) {
    exclude_rule_lookup[*iter] = true;
  }
  // only create output if the rule has not already been hit,
  // and if the user didn't want this rule disabled
  if (exclude_rule_lookup.find(rec.get_rule_name()) ==
      exclude_rule_lookup.end()) {
    std::cout << "emitting test for rule \"" << rec.get_rule_name() << "\""
              << std::endl;

    bool update_any = update_snakefiles || update_added_content ||
                      update_inputs || update_outputs || update_pytest;
    // create a test output directory that is unique for this rule
    boost::filesystem::path rule_parent_path =
        test_parent_path / rec.get_rule_name();
    // create test directory, for output from test run
    boost::filesystem::path rule_expected_path = rule_parent_path / "expected";
    // new to this program: create a workspace with all input directories
    boost::filesystem::path workspace_path = rule_parent_path / "workspace";
    if (update_any) {
      boost::filesystem::create_directories(rule_expected_path);
      boost::filesystem::create_directories(workspace_path);
    }
    if (update_outputs) {
      // copy *output* to expected path
      copy_contents(rec.get_outputs(), pipeline_dir, rule_expected_path,
                    rec.get_rule_name());
    }
    if (update_inputs) {
      // copy *input* to workspace
      copy_contents(rec.get_inputs(), pipeline_dir, workspace_path,
                    rec.get_rule_name());
    }
    if (update_added_content) {
      // copy extra files and directories, if provided, to workspace
      copy_contents(added_files, pipeline_dir, workspace_path, "added files");
      copy_contents(added_directories, pipeline_dir, workspace_path,
                    "added directories");
    }
    if (update_snakefiles) {
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
      report_phony_all_target(output, rec.get_outputs());
      // find the rule from the parsed snakefile(s) and report it to file
      sf.report_single_rule(rec.get_rule_name(), output);
      output.close();
    }
    // modify repo inst/test.py into a test runner for this rule
    if (update_pytest) {
      report_modified_test_script(test_parent_path, output_test_dir,
                                  rec.get_rule_name(), inst_test_py);
    }
  }
}

void snakemake_unit_tests::solved_rules::copy_contents(
    const std::vector<boost::filesystem::path> &contents,
    const boost::filesystem::path &source_prefix,
    const boost::filesystem::path &target_prefix,
    const std::string &rule_name) const {
  for (std::vector<boost::filesystem::path>::const_iterator iter =
           contents.begin();
       iter != contents.end(); ++iter) {
    boost::filesystem::path source_file = source_prefix / *iter;
    boost::filesystem::path target_file = target_prefix / *iter;
    // check source exists
    if (!boost::filesystem::is_regular_file(source_file) &&
        !boost::filesystem::is_directory(source_file)) {
      throw std::runtime_error("cannot find file/directory \"" +
                               source_file.string() + "\" for " + rule_name);
    }
    // create parent directories as needed
    boost::filesystem::create_directories(target_file.parent_path());
    // recursive copy
    boost::filesystem::copy(
        source_file, target_file,
        boost::filesystem::copy_options::overwrite_existing |
            boost::filesystem::copy_options::recursive);
  }
}

void snakemake_unit_tests::solved_rules::report_phony_all_target(
    std::ostream &out,
    const std::vector<boost::filesystem::path> &targets) const {
  if (!(out << "rule all:\n    input:" << std::endl))
    throw std::runtime_error(
        "cannot write phony 'all' rule to synthetic snakefile");
  // 'all' rule triggers build by listing desired files as inputs
  for (std::vector<boost::filesystem::path>::const_iterator output_iter =
           targets.begin();
       output_iter != targets.end(); ++output_iter) {
    if (!(out << "        \"" << output_iter->string() << "\"," << std::endl))
      throw std::runtime_error(
          "cannot write phony 'all' outputs to synthetic snakefile");
  }
  if (!(out << std::endl << std::endl))
    throw std::runtime_error(
        "cannot write phony 'all' rule trailing whitespace to synthetic "
        "snakefile");
}

void snakemake_unit_tests::solved_rules::report_modified_test_script(
    const boost::filesystem::path &parent_dir,
    const boost::filesystem::path &test_dir, const std::string &rule_name,
    const boost::filesystem::path &inst_test_py) const {
  std::ifstream input;
  std::ofstream output;
  std::string test_python_file =
      (parent_dir / ("test_" + rule_name + ".py")).string();
  output.open(test_python_file.c_str());
  if (!output.is_open())
    throw std::runtime_error("cannot write test python file \"" +
                             test_python_file + "\"");
  if (!(output << "#!/usr/bin/env python3\ntestdir='" << test_dir.string()
               << "'\nrulename='" << rule_name << '\'' << std::endl))
    throw std::runtime_error(
        "cannot write rulename variable to test python file \"" +
        test_python_file + "\"");
  input.open(inst_test_py.string().c_str());
  if (!input.is_open())
    throw std::runtime_error("cannot read installed file \"" +
                             inst_test_py.string() + "\"");
  if (!(output << input.rdbuf()))
    throw std::runtime_error("cannot dump \"" + inst_test_py.string() +
                             "\" to output \"" + test_python_file + "\"");
  input.close();
  output.close();
}
