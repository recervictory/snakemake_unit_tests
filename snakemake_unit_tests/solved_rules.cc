/*!
  @file solved_rules.cc
  @brief implementation of solved_rules class
  @author Cameron Palmer
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/solved_rules.h"

snakemake_unit_tests::recipe::recipe() : _rule_name(""), _log("") {}
snakemake_unit_tests::recipe::recipe(const recipe &obj)
    : _rule_name(obj._rule_name), _inputs(obj._inputs), _outputs(obj._outputs), _log(obj._log) {}
snakemake_unit_tests::recipe::~recipe() throw() {}
const std::string &snakemake_unit_tests::recipe::get_rule_name() const { return _rule_name; }
void snakemake_unit_tests::recipe::set_rule_name(const std::string &s) { _rule_name = s; }
const std::vector<boost::filesystem::path> &snakemake_unit_tests::recipe::get_inputs() const { return _inputs; }
void snakemake_unit_tests::recipe::add_input(const std::string &s) { _inputs.push_back(s); }
const std::vector<boost::filesystem::path> &snakemake_unit_tests::recipe::get_outputs() const { return _outputs; }
void snakemake_unit_tests::recipe::add_output(const std::string &s) { _outputs.push_back(s); }
const std::string &snakemake_unit_tests::recipe::get_log() const { return _log; }
void snakemake_unit_tests::recipe::set_log(const std::string &s) { _log = s; }
void snakemake_unit_tests::recipe::clear() {
  _rule_name = _log = "";
  _inputs.clear();
  _outputs.clear();
}

void snakemake_unit_tests::solved_rules::load_file(const std::string &filename) {
  std::ifstream input;
  std::string line = "";
  std::vector<std::string> input_filenames, output_filenames;
  const boost::regex standard_rule_declaration("^rule ([^ ]+):.*$");
  const boost::regex checkpoint_declaration("^checkpoint ([^ ]+):.*$");
  boost::smatch regex_result;
  std::map<std::string, std::vector<std::string>> toxic_output_files;
  try {
    // open log file
    input.open(filename.c_str());
    if (!input.is_open()) throw std::runtime_error("cannot open snakemake log file \"" + filename + "\"");
    // while log entries remain
    while (input.peek() != EOF) {
      getline(input, line);
      // if the line is a valid rule declaration
      if (boost::regex_match(line, regex_result, standard_rule_declaration) ||
          boost::regex_match(line, regex_result, checkpoint_declaration)) {  // set apparent rule name
        // for magical reasons, this regex seems to be working where
        // the include directive one wasn't; has to do with '/'?
        boost::shared_ptr<recipe> rep(new recipe);
        rep->set_rule_name(regex_result[1]);
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
            std::map<std::string, std::vector<std::string>>::iterator toxic_finder;
            if ((toxic_finder = toxic_output_files.find(*iter)) == toxic_output_files.end()) {
              toxic_finder = toxic_output_files.insert(std::make_pair(*iter, std::vector<std::string>())).first;
              toxic_finder->second.push_back(_output_lookup[*iter]->get_rule_name());
            }
            toxic_finder->second.push_back(rep->get_rule_name());
          }
          _output_lookup[*iter] = rep;
        }
      }
    }
    input.close();
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
  if (!toxic_output_files.empty()) {
    std::cout << "warning: at least one output file appears multiple times in the run log file."
              << " in theory, this behavior should be impossible; in practice, it seems like snakemake "
              << "does not enforce unambiguous output targets if the output targets themselves are not "
              << "requested as part of the dependency graph. with this assumption in mind, this redundant"
              << " content is currently tolerated, in anticipation of the duplicated output files not "
              << "actually being used for anything. however, this is very strange behavior, and should "
              << "be considered a bug in a pipeline, and should be resolved; and until it is resolved, "
              << "snakemake_unit_tests cannot unambiguously resolve the relationship between certain "
              << "theoretical combinations of log entries." << std::endl;
    std::cout << "affected duplicate output files:" << std::endl;
    for (std::map<std::string, std::vector<std::string>>::const_iterator iter = toxic_output_files.begin();
         iter != toxic_output_files.end(); ++iter) {
      std::cout << " - '" << iter->first << "'";
      for (std::vector<std::string>::const_iterator riter = iter->second.begin(); riter != iter->second.end();
           ++riter) {
        if (riter == iter->second.begin()) {
          std::cout << ": impacted rules: ";
        } else {
          std::cout << ", ";
        }
        std::cout << *riter;
      }
      std::cout << std::endl;
    }
  }
}

void snakemake_unit_tests::solved_rules::emit_tests(
    const snakemake_file &sf, const boost::filesystem::path &output_test_dir,
    const boost::filesystem::path &pipeline_top_dir, const boost::filesystem::path &pipeline_run_dir,
    const boost::filesystem::path &inst_dir, const std::map<std::string, bool> &include_rules,
    const std::map<std::string, bool> &exclude_rules, const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles, bool update_added_content,
    bool update_inputs, bool update_outputs, bool update_pytest, bool include_entire_dag,
    std::map<std::string, std::vector<std::string>> *files_outside_workspace) const {
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
                         missing_recipes, include_rules, exclude_rules, added_files, added_directories,
                         update_snakefiles, update_added_content, update_inputs, update_outputs, update_pytest,
                         include_entire_dag, files_outside_workspace);
        // new: deal with the fact that certain kinds of rule relationships (e.g. rulesdot) cannot be
        // reliably detected with this program's approach to querying snakefiles
        if (exclude_rules.find((*iter)->get_rule_name()) == exclude_rules.end() &&
            (include_rules.empty() || include_rules.find((*iter)->get_rule_name()) != include_rules.end()) &&
            (update_snakefiles || update_added_content || update_inputs || update_outputs)) {
          std::vector<std::string> snakemake_exec;
          snakemake_exec =
              exec("cd " + (test_parent_path / (*iter)->get_rule_name() / "workspace").string() + " && snakemake -nFs" +
                       sf.get_snakefile_relative_path().string() + " --directory " + pipeline_run_dir.string(),
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
        } else {
          // the rule was manually excluded in config; for evaluation purposes, that means we're done
          deployment_successful = true;
        }
        if (!deployment_successful) {
          std::cout << "\truleset has been adjusted for rules./checkpoint features; trying again..." << std::endl;
        }
      } while (!deployment_successful);
      test_history[(*iter)->get_rule_name()] = true;
      // remove evidence of having run snakemake in-place
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
  // target error pattern is: "'(Rules|Checkpoints)' object has no attribute 'RULENAME'"
  const boost::regex rule_missing("^.*'Rules' object has no attribute '([^']+)'.*\n$");
  const boost::regex checkpoint_missing("^.*'Checkpoints' object has no attribute '([^']+)'.*\n$");
  const boost::regex any_error("^.*[eE][xX][cC][eE][pP][tT][iI][oO][nN].*\n?$");
  bool found_error = false, found_permitted_error = false;
  for (std::vector<std::string>::const_iterator iter = snakemake_exec.begin(); iter != snakemake_exec.end(); ++iter) {
    boost::smatch regex_result;
    if (boost::regex_match(*iter, regex_result, rule_missing) ||
        boost::regex_match(*iter, regex_result, checkpoint_missing)) {
      target->insert(std::make_pair(regex_result[1].str(), true));
      found_permitted_error = true;
    }
    if (boost::regex_match(*iter, regex_result, any_error)) {
      found_error = true;
    }
  }
  if (found_error && !found_permitted_error) {
    for (std::vector<std::string>::const_iterator iter = snakemake_exec.begin(); iter != snakemake_exec.end(); ++iter) {
      std::cerr << *iter;
    }
    throw std::runtime_error(
        "snakemake dryrun found unhandled error, indicating something wrong with either "
        "the configuration of this run or the internal logic of snakemake_unit_tests; "
        "please inspect the logging information above to determine which. in particular, "
        "any message about missing infrastructure files from this workflow (e.g. config.yaml) "
        "may indicate that you need to add more things to added-files or added-directories, "
        "for files that are necessary for pipeline functionality but that exist outside "
        "of the DAG.");
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
    const std::map<std::string, bool> &include_rules, const std::map<std::string, bool> &exclude_rules,
    const std::vector<boost::filesystem::path> &added_files,
    const std::vector<boost::filesystem::path> &added_directories, bool update_snakefiles, bool update_added_content,
    bool update_inputs, bool update_outputs, bool update_pytest, bool include_entire_dag,
    std::map<std::string, std::vector<std::string>> *files_outside_workspace) const {
  // new: deal with rule structures that drag a certain number of upstream
  // recipes with them:
  //  - scattergather
  // formerly, this was supposed to handle rules. and checkpoints; that has been migrated elsewhere
  std::map<boost::shared_ptr<recipe>, bool> dependent_recipes = extra_required_recipes;
  std::map<std::string, bool> dependent_rulenames;
  std::vector<boost::filesystem::path> extra_comparison_exclusions;
  dependent_recipes[rec] = true;
  if (include_entire_dag) {
    add_dag_from_leaf(rec, include_entire_dag, &dependent_recipes);
  }
  for (std::map<boost::shared_ptr<recipe>, bool>::const_iterator iter = dependent_recipes.begin();
       iter != dependent_recipes.end(); ++iter) {
    dependent_rulenames[iter->first->get_rule_name()] = true;
  }
  // only create output if the rule has not already been hit,
  // and if the user didn't want this rule disabled
  if (exclude_rules.find(rec->get_rule_name()) == exclude_rules.end() &&
      (include_rules.empty() || include_rules.find(rec->get_rule_name()) != include_rules.end())) {
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
                    rec->get_rule_name(), files_outside_workspace);
    }
    if (update_inputs) {
      // copy *input* to workspace
      // new: respect outputs to all dependent rules (e.g. for checkpoints)
      for (std::map<boost::shared_ptr<recipe>, bool>::const_iterator iter = dependent_recipes.begin();
           iter != dependent_recipes.end(); ++iter) {
        if (!iter->first->get_rule_name().compare(rec->get_rule_name())) {
          copy_contents(iter->first->get_inputs(), pipeline_top_dir / pipeline_run_dir,
                        workspace_path / pipeline_run_dir, rec->get_rule_name(), files_outside_workspace);
        } else {
          // upstream rules should have their *outputs* emitted as *input* to the unit test
          copy_contents(iter->first->get_outputs(), pipeline_top_dir / pipeline_run_dir,
                        workspace_path / pipeline_run_dir, rec->get_rule_name(), files_outside_workspace);
        }
      }
    }
    if (update_added_content) {
      // copy extra files and directories, if provided, to workspace
      copy_contents(added_files, pipeline_top_dir, workspace_path, "added files", files_outside_workspace);
      copy_contents(added_directories, pipeline_top_dir, workspace_path, "added directories", files_outside_workspace);
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
    const std::vector<boost::filesystem::path> &added_directories,
    std::map<std::string, std::vector<std::string>> *files_outside_workspace) const {
  // create test directory, for output from test run
  boost::filesystem::path workspace_path = output_test_dir / ".snakemake_unit_tests";
  boost::filesystem::create_directories(workspace_path);

  // copy extra files and directories, if provided, to workspace
  copy_contents(added_files, pipeline_dir, workspace_path, "added files", files_outside_workspace);
  copy_contents(added_directories, pipeline_dir, workspace_path, "added directories", files_outside_workspace);
}

void snakemake_unit_tests::solved_rules::remove_empty_workspace(const boost::filesystem::path &output_test_dir) const {
  boost::filesystem::remove_all(output_test_dir / ".snakemake_unit_tests");
}

void snakemake_unit_tests::solved_rules::copy_contents(
    const std::vector<boost::filesystem::path> &contents, const boost::filesystem::path &source_prefix,
    const boost::filesystem::path &target_prefix, const std::string &rule_name,
    std::map<std::string, std::vector<std::string>> *files_outside_workspace) const {
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
      } else if (files_outside_workspace) {
        std::map<std::string, std::vector<std::string>>::iterator file_finder;
        if ((file_finder = files_outside_workspace->find(iter->string())) == files_outside_workspace->end()) {
          file_finder =
              files_outside_workspace->insert(std::make_pair(iter->string(), std::vector<std::string>())).first;
        }
        file_finder->second.push_back(rule_name);
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
      // for compatibility with other applications: if the target exists, change its permissions and remove it
      if (boost::filesystem::exists(target_file)) {
        if (boost::filesystem::is_directory(target_file)) {
          boost::filesystem::recursive_directory_iterator rec_iter(target_file), rec_end;
          for (; rec_iter != rec_end; ++rec_iter) {
            boost::filesystem::permissions(*rec_iter, boost::filesystem::owner_write | boost::filesystem::add_perms);
          }
        } else {
          boost::filesystem::permissions(target_file, boost::filesystem::owner_write | boost::filesystem::add_perms);
        }
        boost::filesystem::remove_all(target_file);
      }
      // then copy
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
