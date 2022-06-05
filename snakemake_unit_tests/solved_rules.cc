/*!
  @file solved_rules.cc
  @brief implementation of solved_rules class
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/solved_rules.h"

void snakemake_unit_tests::solved_rules::load_file(const std::string &filename) {
  std::ifstream input;
  std::string line = "";
  std::vector<std::string> input_filenames, output_filenames;
  std::map<std::string, bool> affected_by_checkpoint;
  const boost::regex standard_rule_declaration("^rule ([^ ]+):.*$");
  const boost::regex checkpoint_declaration("^checkpoint ([^ ]+):.*$");
  const boost::regex checkpoint_impact_flag("^Updating job ([^\\.]+)\\.");
  boost::smatch regex_result;
  try {
    // open log file
    input.open(filename.c_str());
    if (!input.is_open()) throw std::runtime_error("cannot open snakemake log file \"" + filename + "\"");
    // while log entries remain
    while (input.peek() != EOF) {
      getline(input, line);
      // if the line is a valid rule declaration
      if (boost::regex_match(line, regex_result, standard_rule_declaration) ||
          boost::regex_match(line, regex_result, checkpoint_declaration)) {
        // set apparent rule name
        // for magical reasons, this regex seems to be working where
        // the include directive one wasn't; has to do with '/'?
        boost::shared_ptr<recipe> rep(new recipe);
        rep->set_rule_name(regex_result[1]);
        if (line.find("checkpoint") == 0) {
          rep->set_checkpoint(true);
        }
        if (affected_by_checkpoint.find(regex_result[1]) != affected_by_checkpoint.end()) {
          rep->set_checkpoint_update(true);
        }
        // scan for remaining rule content lines
        while (input.peek() != EOF) {
          getline(input, line);
          if (line.empty() || line.at(0) != ' ') break;
          if (line.find("    input:") == 0) {
            // special handler for solved input files
            // new: detect unresolved checkpoint inputs
            if (line.find("<TBD>") != std::string::npos) {
              throw std::logic_error("in log entry \"" + regex_result[1] +
                                     "\": "
                                     "apparent unresolved checkpoint input; "
                                     "logs for pipelines with checkpoints *cannot* "
                                     "be created with --dryrun active");
            }
            split_comma_list(line.substr(11), &input_filenames);
            for (std::vector<std::string>::const_iterator iter = input_filenames.begin(); iter != input_filenames.end();
                 ++iter) {
              rep->add_input(*iter);
            }
          } else if (line.find("    output:") == 0) {
            // special handler for solved output files
            split_comma_list(line.substr(12), &output_filenames);
            for (std::vector<std::string>::const_iterator iter = output_filenames.begin();
                 iter != output_filenames.end(); ++iter) {
              rep->add_output(*iter);
            }
          } else if (line.find("    log:") == 0) {
            // track log file but not 100% sure what to do with it.
            // snakemake --generate-unit-tests tends to fail when
            // log files get created. may need to add this to
            // an exclusion list.
            rep->set_log(line.substr(9));
          } else if (line.find("    jobid:") == 0 || line.find("    wildcards:") == 0 ||
                     line.find("    benchmark:") == 0 || line.find("    resources:") == 0 ||
                     line.find("    threads:") == 0 || line.find("    priority:") == 0 ||
                     line.find("    reason:") == 0) {
            // other recognized solution annotations;
            // for the moment, do nothing with them
          } else {
            // flag solution annotations that aren't present
            // in the example snakemake run, in case they
            // need to be specially handled
            throw std::logic_error("unrecognized snakemake log block: \"" + line + "\"; please file bug report");
          }
        }
        _recipes.push_back(rep);
        // link each output to its recipe
        for (std::vector<std::string>::const_iterator iter = output_filenames.begin(); iter != output_filenames.end();
             ++iter) {
          if (_output_lookup.find(*iter) != _output_lookup.end()) {
            throw std::logic_error("same output file \"" + *iter +
                                   "\" appears multiple "
                                   "times in log file? this should be impossible");
          }
          _output_lookup[*iter] = rep;
        }
      } else if (boost::regex_match(line, regex_result, checkpoint_impact_flag)) {
        affected_by_checkpoint[regex_result[1]] = true;
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
    const boost::filesystem::path &pipeline_top_dir, const boost::filesystem::path &pipeline_run_dir,
    const boost::filesystem::path &inst_dir, const std::map<std::string, bool> &exclude_rules,
    const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles, bool update_added_content,
    bool update_inputs, bool update_outputs, bool update_pytest, bool include_entire_dag) const {
  // create unit test output directory
  // by default, this looks like `.tests/unit`
  // but will be overridden as `output_test_dir/unit`
  // for compatibility with pytest
  boost::filesystem::path test_parent_path = output_test_dir / "unit";
  boost::filesystem::create_directories(test_parent_path);

  // repo files for creating pytest infrastructure
  boost::filesystem::path inst_test_py = inst_dir / "test.py";
  boost::filesystem::path inst_common_py = inst_dir / "common.py";
  boost::filesystem::path inst_launcher_bash = inst_dir / "pytest_runner.bash";
  if (!boost::filesystem::is_regular_file(inst_test_py) || !boost::filesystem::is_regular_file(inst_common_py) ||
      !boost::filesystem::is_regular_file(inst_launcher_bash)) {
    throw std::runtime_error(
        "cannot locate required files test.py or common.py "
        "in inst directory \"" +
        inst_dir.string() + "\"");
  }

  // iterate across loaded recipes, creating tests as you go
  std::map<std::string, bool> test_history;
  for (std::vector<boost::shared_ptr<recipe>>::const_iterator iter = _recipes.begin(); iter != _recipes.end(); ++iter) {
    if (test_history.find((*iter)->get_rule_name()) == test_history.end()) {
      bool deployment_successful = false;
      std::map<std::string, bool> missing_rules;
      std::map<boost::shared_ptr<recipe>, bool> missing_recipes;
      do {
        create_workspace(*iter, sf, output_test_dir, test_parent_path, pipeline_top_dir, pipeline_run_dir, inst_test_py,
                         missing_recipes, exclude_rules, added_files, added_directories, update_snakefiles,
                         update_added_content, update_inputs, update_outputs, update_pytest, include_entire_dag);
        // new: deal with the fact that certain kinds of rule relationships (e.g. rulesdot) cannot be
        // reliably detected with this program's approach to querying snakefiles
        std::vector<std::string> snakemake_exec;
        std::cout << "executing snakemake command: \"cd "
                  << (test_parent_path / (*iter)->get_rule_name() / "workspace").string() << " && snakemake -nFs "
                  << sf.get_snakefile_relative_path().string() << "\"" << std::endl;
        snakemake_exec = sf.exec("cd " + (test_parent_path / (*iter)->get_rule_name() / "workspace").string() +
                                     " && snakemake -nFs " + sf.get_snakefile_relative_path().string(),
                                 false);
        // try to find snakemake errors that report rules missing from dag
        unsigned initial_missing_count = missing_rules.size();
        find_missing_rules(snakemake_exec, &missing_rules);
        if (missing_rules.size() == initial_missing_count) {
          deployment_successful = true;
        } else {
          for (std::vector<boost::shared_ptr<recipe>>::const_iterator rec_iter = _recipes.begin();
               rec_iter != _recipes.end(); ++rec_iter) {
            if (missing_rules.find((*rec_iter)->get_rule_name()) != missing_rules.end()) {
              missing_recipes[*rec_iter] = true;
            }
          }
        }
      } while (!deployment_successful);
      test_history[(*iter)->get_rule_name()] = true;
      // new: remove evidence of having run snakemake in-place
      boost::filesystem::remove_all(test_parent_path / (*iter)->get_rule_name() / "workspace/.snakemake");
    }
  }
  // emit common.py in the test_parent_path; no modifications needed
  if (update_pytest) {
    boost::filesystem::copy(
        inst_common_py, test_parent_path,
        boost::filesystem::copy_options::overwrite_existing | boost::filesystem::copy_options::recursive);
    report_modified_launcher_script(test_parent_path, output_test_dir, inst_launcher_bash);
  }
}

void snakemake_unit_tests::solved_rules::find_missing_rules(const std::vector<std::string> &snakemake_exec,
                                                            std::map<std::string, bool> *target) const {
  if (!target) throw std::runtime_error("null pointer to solved_rules::find_missing_rules");
  // target error pattern is: "'Rules' object has no attribute 'RULENAME'"
  const boost::regex rule_missing("^.*'Rules' object has no attribute '([^']+)'.*\n$");
  for (std::vector<std::string>::const_iterator iter = snakemake_exec.begin(); iter != snakemake_exec.end(); ++iter) {
    boost::smatch regex_result;
    if (boost::regex_match(*iter, regex_result, rule_missing)) {
      target->insert(std::make_pair(regex_result[1].str(), true));
    }
  }
}

void snakemake_unit_tests::solved_rules::add_dag_from_leaf(const boost::shared_ptr<recipe> &rec,
                                                           bool include_entire_dag,
                                                           std::map<boost::shared_ptr<recipe>, bool> *target) const {
  if (!target) throw std::runtime_error("null pointer to add_dag_from_leaf");
  std::map<boost::shared_ptr<recipe>, bool>::const_iterator dependency_finder;
  for (std::vector<boost::filesystem::path>::const_iterator iter = rec->get_inputs().begin();
       iter != rec->get_inputs().end(); ++iter) {
    std::map<boost::filesystem::path, boost::shared_ptr<recipe>>::const_iterator finder;
    if ((finder = _output_lookup.find(*iter)) != _output_lookup.end()) {
      (*target)[finder->second] = true;
      if (include_entire_dag) {
        add_dag_from_leaf(finder->second, include_entire_dag, target);
      }
    }
  }
}

void snakemake_unit_tests::solved_rules::create_workspace(
    const boost::shared_ptr<recipe> &rec, const snakemake_file &sf, const boost::filesystem::path &output_test_dir,
    const boost::filesystem::path &test_parent_path, const boost::filesystem::path &pipeline_top_dir,
    const boost::filesystem::path &pipeline_run_dir, const boost::filesystem::path &inst_test_py,
    const std::map<boost::shared_ptr<recipe>, bool> &extra_required_recipes,
    const std::map<std::string, bool> &exclude_rules, const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles, bool update_added_content,
    bool update_inputs, bool update_outputs, bool update_pytest, bool include_entire_dag) const {
  // new: deal with rule structures that drag a certain number of upstream
  // recipes with them:
  //  - rules.
  //  - checkpoints
  //  - scattergather
  std::map<boost::shared_ptr<recipe>, bool> dependent_recipes = extra_required_recipes, orphaned_checkpoints;
  std::map<std::string, bool> dependent_rulenames;
  std::vector<boost::filesystem::path> extra_comparison_exclusions;
  dependent_recipes[rec] = true;
  if (include_entire_dag) {
    add_dag_from_leaf(rec, include_entire_dag, &dependent_recipes);
  }
  for (std::vector<boost::shared_ptr<recipe>>::const_iterator iter = _recipes.begin(); iter != _recipes.end(); ++iter) {
    if (rec->is_checkpoint_update() && (*iter)->is_checkpoint() &&
        dependent_recipes.find(*iter) == dependent_recipes.end()) {
      // add checkpoint rule itself to dependent_rulenames
      orphaned_checkpoints[*iter] = true;
      // copy all of this checkpoint's output files to the
      // comparison exclusion set
      for (std::vector<boost::filesystem::path>::const_iterator out_iter = (*iter)->get_outputs().begin();
           out_iter != (*iter)->get_outputs().end(); ++out_iter) {
        extra_comparison_exclusions.push_back(*out_iter);
      }
    }
  }
  for (std::map<boost::shared_ptr<recipe>, bool>::const_iterator iter = orphaned_checkpoints.begin();
       iter != orphaned_checkpoints.end(); ++iter) {
    dependent_recipes[iter->first] = iter->second;
  }
  for (std::map<boost::shared_ptr<recipe>, bool>::const_iterator iter = dependent_recipes.begin();
       iter != dependent_recipes.end(); ++iter) {
    dependent_rulenames[iter->first->get_rule_name()] = true;
  }
  // only create output if the rule has not already been hit,
  // and if the user didn't want this rule disabled
  if (exclude_rules.find(rec->get_rule_name()) == exclude_rules.end()) {
    std::cout << "emitting test for rule \"" << rec->get_rule_name() << "\"" << std::endl;

    bool update_any = update_snakefiles || update_added_content || update_inputs || update_outputs || update_pytest;
    // create a test output directory that is unique for this rule
    boost::filesystem::path rule_parent_path = test_parent_path / rec->get_rule_name();
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
      copy_contents(rec->get_outputs(), pipeline_top_dir / pipeline_run_dir, rule_expected_path / pipeline_run_dir,
                    rec->get_rule_name());
    }
    if (update_inputs) {
      // copy *input* to workspace
      // new: respect inputs to all dependent rules (for checkpoints)
      for (std::map<boost::shared_ptr<recipe>, bool>::const_iterator iter = dependent_recipes.begin();
           iter != dependent_recipes.end(); ++iter) {
        copy_contents(iter->first->get_inputs(), pipeline_top_dir / pipeline_run_dir, workspace_path / pipeline_run_dir,
                      rec->get_rule_name());
      }
    }
    if (update_added_content) {
      // copy extra files and directories, if provided, to workspace
      copy_contents(added_files, pipeline_top_dir, workspace_path, "added files");
      copy_contents(added_directories, pipeline_top_dir, workspace_path, "added directories");
    }
    if (update_snakefiles) {
      // new: aggregate all possible parent rules to required derived rules
      std::deque<std::string> possible_children;
      for (std::map<std::string, bool>::const_iterator iter = dependent_rulenames.begin();
           iter != dependent_rulenames.end(); ++iter) {
        possible_children.push_back(iter->first);
      }
      std::string parent_candidate = "";
      while (!possible_children.empty()) {
        if (sf.get_base_rule_name(possible_children.front(), &parent_candidate)) {
          if (!parent_candidate.empty()) {
            possible_children.push_back(parent_candidate);
            dependent_rulenames[parent_candidate] = true;
          }
          possible_children.pop_front();
        } else {
          throw std::runtime_error("unable to locate required rule \"" + possible_children.front() + "\"");
        }
      }
      // enforce success across possibly many files by checking the sum
      // of found rules. logic only works because the postflight checker
      // enforces lack of redundant rulenames.
      if (emit_snakefile(sf, workspace_path, rec, dependent_rulenames, true) != dependent_rulenames.size()) {
        throw std::runtime_error("cannot find rule for requested log content \"" + rec->get_rule_name() + "\"");
      }
    }
    // modify repo inst/test.py into a test runner for this rule
    if (update_pytest) {
      report_modified_test_script(test_parent_path, output_test_dir, rec->get_rule_name(),
                                  sf.get_snakefile_relative_path(), pipeline_run_dir, extra_comparison_exclusions,
                                  inst_test_py);
    }
  }
}

unsigned snakemake_unit_tests::solved_rules::emit_snakefile(const snakemake_file &sf,
                                                            const boost::filesystem::path &workspace_path,
                                                            const boost::shared_ptr<recipe> &rec,
                                                            const std::map<std::string, bool> &dependent_rulenames,
                                                            bool requires_phony_all) const {
  // create parent directories for synthetic snakefile
  boost::filesystem::create_directories((workspace_path / sf.get_snakefile_relative_path()).parent_path());
  // create the synthetic snakefile in workspace
  std::string output_filename = (workspace_path / sf.get_snakefile_relative_path()).string();
  std::ofstream output;
  output.open(output_filename.c_str());
  if (!output.is_open()) throw std::runtime_error("cannot create synthetic snakemake file \"" + output_filename + "\"");
  // before adding anything else: add a single 'all' rule that points at
  // solved rule output files
  // note: only do this at top level
  if (requires_phony_all) report_phony_all_target(output, rec->get_outputs());
  // find the rule from the parsed snakefile(s) and report it to file
  unsigned res = sf.report_single_rule(dependent_rulenames, output);
  output.close();
  for (std::map<boost::filesystem::path, boost::shared_ptr<snakemake_file>>::const_iterator mapper =
           sf.loaded_files().begin();
       mapper != sf.loaded_files().end(); ++mapper) {
    res += emit_snakefile(*mapper->second, workspace_path, rec, dependent_rulenames, false);
  }
  return res;
}

void snakemake_unit_tests::solved_rules::create_empty_workspace(
    const boost::filesystem::path &output_test_dir, const boost::filesystem::path &pipeline_dir,
    const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories) const {
  // create test directory, for output from test run
  boost::filesystem::path workspace_path = output_test_dir / ".snakemake_unit_tests";
  boost::filesystem::create_directories(workspace_path);

  // copy extra files and directories, if provided, to workspace
  copy_contents(added_files, pipeline_dir, workspace_path, "added files");
  copy_contents(added_directories, pipeline_dir, workspace_path, "added directories");
}

void snakemake_unit_tests::solved_rules::remove_empty_workspace(const boost::filesystem::path &output_test_dir) const {
  boost::filesystem::remove_all(output_test_dir / ".snakemake_unit_tests");
}

void snakemake_unit_tests::solved_rules::copy_contents(const std::vector<boost::filesystem::path> &contents,
                                                       const boost::filesystem::path &source_prefix,
                                                       const boost::filesystem::path &target_prefix,
                                                       const std::string &rule_name) const {
  std::map<boost::filesystem::path, bool> copied_sources;
  for (std::vector<boost::filesystem::path>::const_iterator iter = contents.begin(); iter != contents.end(); ++iter) {
    boost::filesystem::path source_file = source_prefix / *iter;
    boost::filesystem::path target_file = target_prefix / *iter;
    // deal with the fact that source prefix might be an absolute path :(
    if (boost::filesystem::absolute(*iter) == *iter) {
      // corner case: for some reason, snakemake is tracking absolute path of
      // something that is still actually in the pipeline directory
      if (boost::filesystem::canonical(boost::filesystem::absolute(*iter))
              .string()
              .find(boost::filesystem::canonical(boost::filesystem::absolute(source_prefix)).string()) == 0) {
        source_file = *iter;
        target_file = target_prefix / boost::filesystem::relative(
                                          boost::filesystem::canonical(boost::filesystem::absolute(*iter)),
                                          boost::filesystem::canonical(boost::filesystem::absolute(source_prefix)));
      } else {
        std::cout << "warning: absolute path input file detected: \"" << iter->string()
                  << "\". for consistency, this file will *not* be copied. your unit tests "
                  << "will function, but they will not be modular in the sense that you cannot "
                  << "in most cases move them off your filesystem. to avoid this problem, "
                  << "configure your pipeline to only take inputs inside the pipeline directory itself; "
                  << "or add the impacted rule to your excluded ruleset in your configuration." << std::endl;
        continue;
      }
    }
    // check source exists
    if (!boost::filesystem::is_regular_file(source_file) && !boost::filesystem::is_directory(source_file)) {
      throw std::runtime_error("cannot find file/directory \"" + source_file.string() + "\" for " + rule_name);
    }
    // prohibit multiple copies of the same file
    // don't know why some files are multiply tracked, but
    // it's seemingly harmless
    if (copied_sources.find(source_file) == copied_sources.end()) {
      copied_sources[source_file] = true;
      // create parent directories as needed
      boost::filesystem::create_directories(target_file.parent_path());
      // recursive copy
      boost::filesystem::copy(
          source_file, target_file,
          boost::filesystem::copy_options::overwrite_existing | boost::filesystem::copy_options::recursive);
    }
  }
}

void snakemake_unit_tests::solved_rules::report_phony_all_target(
    std::ostream &out, const std::vector<boost::filesystem::path> &targets) const {
  if (!(out << "rule all:\n    input:" << std::endl))
    throw std::runtime_error("cannot write phony 'all' rule to synthetic snakefile");
  // 'all' rule triggers build by listing desired files as inputs
  for (std::vector<boost::filesystem::path>::const_iterator output_iter = targets.begin(); output_iter != targets.end();
       ++output_iter) {
    if (!(out << "        \"" << output_iter->string() << "\"," << std::endl))
      throw std::runtime_error("cannot write phony 'all' outputs to synthetic snakefile");
  }
  if (!(out << std::endl << std::endl))
    throw std::runtime_error(
        "cannot write phony 'all' rule trailing whitespace to synthetic "
        "snakefile");
}

void snakemake_unit_tests::solved_rules::report_modified_test_script(
    const boost::filesystem::path &parent_dir, const boost::filesystem::path &test_dir, const std::string &rule_name,
    const boost::filesystem::path &snakefile_relative_path, const boost::filesystem::path &pipeline_run_dir,
    const std::vector<boost::filesystem::path> &extra_comparison_exclusions,
    const boost::filesystem::path &inst_test_py) const {
  std::ifstream input;
  std::ofstream output;
  std::string test_python_file = (parent_dir / ("test_" + rule_name + ".py")).string();
  output.open(test_python_file.c_str());
  if (!output.is_open()) throw std::runtime_error("cannot write test python file \"" + test_python_file + "\"");
  if (!(output << "#!/usr/bin/env python3\ntestdir='" << test_dir.string() << "'" << std::endl
               << "rulename='" << rule_name << '\'' << std::endl
               << "snakefile_relative_path='" << snakefile_relative_path.string() << "'" << std::endl
               << "snakemake_exec_path='" << pipeline_run_dir.string() << "'" << std::endl
               << "extra_comparison_exclusions=["))
    throw std::runtime_error("cannot write rulename variable to test python file \"" + test_python_file + "\"");
  for (std::vector<boost::filesystem::path>::const_iterator iter = extra_comparison_exclusions.begin();
       iter != extra_comparison_exclusions.end(); ++iter) {
    if (!(output << "'" << iter->string() << "', "))
      throw std::runtime_error("cannot write extra comparison exclusions to test python file \"" + test_python_file +
                               "\"");
  }
  if (!(output << "]" << std::endl))
    throw std::runtime_error("cannot close extra comparison exclusions in test python file \"" + test_python_file +
                             "\"");
  input.open(inst_test_py.string().c_str());
  if (!input.is_open()) throw std::runtime_error("cannot read installed file \"" + inst_test_py.string() + "\"");
  if (!(output << input.rdbuf()))
    throw std::runtime_error("cannot dump \"" + inst_test_py.string() + "\" to output \"" + test_python_file + "\"");
  input.close();
  output.close();
}

void snakemake_unit_tests::solved_rules::report_modified_launcher_script(
    const boost::filesystem::path &parent_dir, const boost::filesystem::path &test_dir,
    const boost::filesystem::path &inst_launcher_script) const {
  std::ifstream input;
  std::ofstream output;
  std::string launcher_file = (parent_dir / "pytest_runner.bash").string();
  output.open(launcher_file.c_str());
  if (!output.is_open()) throw std::runtime_error("cannot write launcher file \"" + launcher_file + "\"");
  if (!(output << "#!/usr/bin/env bash\nSNAKEMAKE_UNIT_TESTS_DIR=" << test_dir.string() << std::endl)) {
    throw std::runtime_error("cannot write bash header to \"" + launcher_file + "\"");
  }
  input.open(inst_launcher_script.string().c_str());
  if (!input.is_open())
    throw std::runtime_error("cannot read installed file \"" + inst_launcher_script.string() + "\"");
  if (!(output << input.rdbuf()))
    throw std::runtime_error("cannot dump \"" + inst_launcher_script.string() + "\" to output \"" + launcher_file +
                             "\"");
  input.close();
  output.close();
}
