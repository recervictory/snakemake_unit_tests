/*!
 @file snakemake_file.cc
 @brief implementation of snakemake_file class
 @author Cameron Palmer
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/snakemake_file.h"

/*
  The parser reimplementation is structured as follows:

  - consider every piece of content of the file as *either*:
  -- a snakemake rule
  -- a (possibly incomplete) python instruction, which can further be classified
  as:
  --- unrelated to file parse
  --- an include directive

  The complete parse operation is as follows:

  1) add the top-level snakefile as a pseudo-include directive to the
  snakemake_file

  2) parse the file, non-recursively

  3) while unresolved snakemake rule and include directives remain:
  -- emit a dummy workspace containing a simplified representation of the
  currently loaded data
  ---- include directives and snakemake rules are assigned unique identifiers
  ---- their locations in file are replaced with tracking statements
  ---- python code unrelated to snakemake or include directives is included
  as-is
  -- run the python code, capture the screen output
  -- inspect queue of unresolved rules and include directives
  ---- if the next entry is a rule, determine whether the rule was evaluated and
  resolve, pop, continue
  ---- if the next entry is an include directive, determine whether the
  directive was evaluated
  ------ if not, flag it as such, do not process, pop, continue
  ------ if so, set it to resolved, pop it, stop processing resolutions
  -- parse the next include directive, without recursing. flag new rules and
  includes as potentially problematic


  Assorted comments:
  - the above logic is incredibly conservative, and designed to handle some
  additional issues not enumerated above (include directives on variables).
  - the logic can be loosened for some types of includes that can be handled in
  one pass, avoiding additional iterations of python evaluation. most notably,
  include directives at the same depth with no intervening python code, and
  0-depth includes in the currently parsed file.

 */

void snakemake_unit_tests::snakemake_file::load_everything(
    const boost::filesystem::path &filename,
    const boost::filesystem::path &base_dir,
    std::map<std::string, bool> *exclude_rules, bool verbose) {
  if (!exclude_rules)
    throw std::runtime_error(
        "null exclude_rules provided to "
        "load_everything");
  _snakefile_relative_path = filename;
  std::vector<std::string> loaded_lines;
  boost::filesystem::path recursive_path = base_dir / filename;
  load_lines(recursive_path, &loaded_lines);
  // new: preprocess all lines with the improved lexical parser
  loaded_lines = lexical_parse(loaded_lines);
  parse_file(loaded_lines, _blocks.begin(), filename, verbose);
}

void snakemake_unit_tests::snakemake_file::postflight_checks(
    std::map<std::string, bool> *exclude_rules) {
  // placeholder: add screening step to detect known issues/unsupported features
  detect_known_issues(exclude_rules);
  // resolved rules are being dealt with differently, and in solved_rules
}

void snakemake_unit_tests::snakemake_file::report_rules(
    std::map<std::string, std::vector<boost::shared_ptr<rule_block> > >
        *aggregated_rules) const {
  if (!aggregated_rules)
    throw std::runtime_error("null pointer provided to report_rules");
  std::map<std::string, std::vector<boost::shared_ptr<rule_block> > >::iterator
      finder;
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    // new: respect blocks' reports of inclusion status
    if (!(*iter)->included()) continue;
    // python code. scan for remaining include directives
    if (!(*iter)->get_rule_name().empty()) {
      // disable reporting of "all" phony target
      if (!(*iter)->get_rule_name().compare("all")) continue;
      // allow global snakemake directives with no rulename
      // otherwise: rule. aggregate for duplication
      if ((finder = aggregated_rules->find((*iter)->get_rule_name())) ==
          aggregated_rules->end()) {
        finder = aggregated_rules
                     ->insert(std::make_pair(
                         (*iter)->get_rule_name(),
                         std::vector<boost::shared_ptr<rule_block> >()))
                     .first;
      }
      finder->second.push_back(*iter);
    }
  }
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator iter =
           _included_files.begin();
       iter != _included_files.end(); ++iter) {
    iter->second->report_rules(aggregated_rules);
  }
}

void snakemake_unit_tests::snakemake_file::detect_known_issues(
    std::map<std::string, bool> *exclude_rules) {
  /*
    Known issues as implemented here

    1) include directives on variables or in more complicated one line logic
    statements

    2) conditional rules causing duplicate rules with the same name but
    different contents to be loaded; snakemake wouldn't care, but this code
    isn't smart enough to resolve yet

    3) derived rules where the base rule is not detected for some reason
    (detected during that scan)
   */
  if (!exclude_rules)
    throw std::runtime_error("null pointer provided to exclude_rules");
  std::map<std::string, std::vector<boost::shared_ptr<rule_block> > >
      aggregated_rules;
  std::map<std::string, std::vector<boost::shared_ptr<rule_block> > >::iterator
      finder;
  unsigned duplicated_rules = 0;
  std::vector<std::string> unresolvable_duplicated_rules;
  report_rules(&aggregated_rules);

  for (finder = aggregated_rules.begin(); finder != aggregated_rules.end();
       ++finder) {
    if (finder->second.size() > 1) ++duplicated_rules;
    bool problematic = false;
    for (unsigned i = 1; i < finder->second.size() && !problematic; ++i) {
      if (*finder->second.at(i) != *finder->second.at(0)) {
        bool already_excluded = false;
        for (std::map<std::string, bool>::const_iterator miter =
                 exclude_rules->begin();
             miter != exclude_rules->end() && !already_excluded; ++miter) {
          if (!miter->first.compare(finder->first)) {
            already_excluded = true;
          }
        }
        if (!already_excluded) {
          problematic = true;
          exclude_rules->insert(std::make_pair(finder->first, false));
          unresolvable_duplicated_rules.push_back(finder->first);
        }
      }
    }
  }
  // report results
  // new: suppress messages about multiple definitions and include directives
  // new format and support means that multiple definitions should be handled,
  // and include directives are not squashed and thus expected in output.
  std::cout << "snakefile load summary" << std::endl;
  std::cout << "----------------------" << std::endl;
  std::cout << "total loaded candidate rules: " << aggregated_rules.size()
            << std::endl;
  std::cout << "  of those rules, " << duplicated_rules
            << " had multiple entries in unconditional logic" << std::endl;
  if (!unresolvable_duplicated_rules.empty()) {
    std::cout << "***of these duplicate rules, "
              << unresolvable_duplicated_rules.size()
              << " had incompatible duplicate content:" << std::endl;
    for (std::vector<std::string>::const_iterator iter =
             unresolvable_duplicated_rules.begin();
         iter != unresolvable_duplicated_rules.end(); ++iter) {
      std::cout << "     " << *iter << std::endl;
    }
    std::cout
        << std::endl
        << "sorry, "
        << (unresolvable_duplicated_rules.size() == 1 ? "this rule is"
                                                      : "these rules are")
        << " unsupported in the current software build. "
        << "this information will be automatically added to exclude-rules "
        << "to prevent inconsistent behavior" << std::endl
        << std::endl;
  }
}

void snakemake_unit_tests::snakemake_file::load_lines(
    const boost::filesystem::path &filename,
    std::vector<std::string> *target) const {
  if (!target) throw std::runtime_error("null pointer to load_lines");
  target->clear();
  std::ifstream input;
  std::string line = "";
  try {
    input.open(filename.string().c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake file \"" +
                               filename.string() + "\"");
    // update: load all lines to memory first
    while (input.peek() != EOF) {
      getline(input, line);
      target->push_back(line);
    }
    input.close();
  } catch (...) {
    if (input.is_open()) input.close();
    throw;
  }
}

void snakemake_unit_tests::snakemake_file::parse_file(
    const std::vector<std::string> &loaded_lines,
    std::list<boost::shared_ptr<rule_block> >::iterator insertion_point,
    const boost::filesystem::path &filename, bool verbose) {
  _snakefile_relative_path = filename;
  // track current line
  unsigned current_line = 0;
  while (current_line < loaded_lines.size()) {
    boost::shared_ptr<rule_block> rb(new rule_block);
    if (rb->load_content_block(loaded_lines, filename, verbose,
                               &current_line)) {
      // set python interpreter resolution status
      // rules should all be set to unresolved before first pass
      if (!rb->get_rule_name().empty()) {
        rb->set_resolution(UNRESOLVED);
        rb->set_interpreter_tag(*_tag_counter);
        ++*_tag_counter;
      } else if (rb->contains_include_directive()) {
        // ambiguous include directives need a complicated resolution pass
        rb->set_resolution(UNRESOLVED);
        rb->set_interpreter_tag(*_tag_counter);
        ++*_tag_counter;
      } else {
        // all other contents are good to go, to be handled by interpreter later
        rb->set_resolution(RESOLVED_INCLUDED);
      }
      _blocks.insert(insertion_point, rb);
    }
  }
}

void snakemake_unit_tests::snakemake_file::print_blocks(
    std::ostream &out) const {
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           get_blocks().begin();
       iter != get_blocks().end(); ++iter) {
    (*iter)->print_contents(out);
  }
}

unsigned snakemake_unit_tests::snakemake_file::report_single_rule(
    const std::map<std::string, bool> &rule_names, std::ostream &out) const {
  // find the requested rule
  unsigned found_rule_count = 0;
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           get_blocks().begin();
       iter != get_blocks().end(); ++iter) {
    // if this is the rule, that's great
    bool is_target = false;
    // allow multiple targets
    if (rule_names.find((*iter)->get_rule_name()) != rule_names.end() &&
        (*iter)->included()) {
      is_target = true;
      ++found_rule_count;
    }
    // if this is the rule or if it's not a rule at all,
    // report it to the synthetic snakefile
    // new: respect rule's inclusion status
    if ((is_target && (*iter)->included()) ||
        (*iter)->get_rule_name().empty()) {
      (*iter)->print_contents(out);
    } else {
      for (unsigned i = 0; i < (*iter)->get_local_indentation(); ++i)
        out << ' ';
      out << "pass" << std::endl << std::endl << std::endl;
    }
  }
  // return number of the target rules found
  return found_rule_count;
}

bool snakemake_unit_tests::snakemake_file::fully_resolved() const {
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    if (!(*iter)->resolved()) return false;
  }
  return true;
}

bool snakemake_unit_tests::snakemake_file::contains_blockers() const {
  bool res = _updated_last_round;
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator iter =
           _included_files.begin();
       iter != _included_files.end(); ++iter) {
    res |= iter->second->contains_blockers();
  }
  return res;
}

bool snakemake_unit_tests::snakemake_file::resolve_with_python(
    const boost::filesystem::path &workspace,
    const boost::filesystem::path &pipeline_top_dir,
    const boost::filesystem::path &pipeline_run_dir, bool verbose,
    bool disable_resolution) {
  // if this is the top-level call
  if (!disable_resolution) {
    // set this file and all its dependencies to no update
    set_update_status(false);
  }
  // within a workspace, open a snakefile
  std::ofstream output;
  boost::filesystem::path output_name = workspace / _snakefile_relative_path;
  if (verbose) {
    std::cout << "\toutput workspace: \"" << workspace.string() << "\""
              << std::endl
              << "\tsnakefile relative path: \""
              << _snakefile_relative_path.string() << "\"" << std::endl
              << "\toutput name: \"" << output_name.string() << "\""
              << std::endl;
  }
  // create directory if needed
  boost::filesystem::create_directories(output_name.parent_path());
  if (verbose) {
    std::cout << "\twriting interpreter snakefile " << output_name.string()
              << std::endl;
  }
  output.open(output_name.string().c_str());
  if (!output.is_open())
    throw std::runtime_error(
        "cannot write interpreter snakefile "
        "to file \"" +
        output_name.string() + "\"");
  // write python reporting code
  bool reporting_terminated = false;
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           get_blocks().begin();
       iter != get_blocks().end() && !reporting_terminated; ++iter) {
    // ask the rule to report the python equivalent of its contents
    /* new: disable logging reporting after the first include statement

       in some cases, downstream python logic may depend on what was loaded
       from the first (unresolved) include statement. that will cause
       the interpreter to crash, and make for all kinds of problems.
       so for the moment, be conservative and only report until
       immediately after the first unresolved include statement
       is reported.
    */
    // true return value means the reporter hit an unresolved include
    if ((*iter)->report_python_logging_code(output)) {
      reporting_terminated = true;
    }
  }
  // handle recursive reporters
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::iterator iter =
           _included_files.begin();
       iter != _included_files.end() && !reporting_terminated; ++iter) {
    if (verbose) {
      std::cout << "\trecursing in python resolution" << std::endl;
    }
    if (iter->second->resolve_with_python(workspace, pipeline_top_dir,
                                          pipeline_run_dir, verbose, true)) {
      reporting_terminated = true;
    }
  }
  // only from the top-level call, so not during recursion
  if (!disable_resolution) {
    if (!(output << "rule tmp:" << std::endl
                 << "    output: \"tmp.txt\"," << std::endl))
      throw std::runtime_error(
          "cannot write tmp output rule to python reporter");
    // adjust snakefile such that it is relative to the run directory
    boost::filesystem::path complete_run_directory =
        boost::filesystem::canonical(pipeline_top_dir / pipeline_run_dir);
    boost::filesystem::path complete_snakefile_loc =
        boost::filesystem::canonical(pipeline_top_dir /
                                     _snakefile_relative_path);
    std::string adjusted_snakefile = complete_snakefile_loc.string().substr(
        complete_run_directory.string().size() + 1);
    // execute python script and capture output
    std::vector<std::string> results =
        exec("cd " + (workspace / pipeline_run_dir).string() +
             " && snakemake -nFs " + adjusted_snakefile);
    // capture the resulting tags for updating completion status
    std::map<std::string, std::string> tag_values;
    capture_python_tag_values(results, &tag_values);
    process_python_results(workspace, pipeline_top_dir, verbose, tag_values,
                           output_name);
    for (std::map<boost::filesystem::path,
                  boost::shared_ptr<snakemake_file> >::iterator mapper =
             _included_files.begin();
         mapper != _included_files.end(); ++mapper) {
      mapper->second->process_python_results(workspace, pipeline_top_dir,
                                             verbose, tag_values, output_name);
    }
  }
  output.close();
  return reporting_terminated;
}

bool snakemake_unit_tests::snakemake_file::process_python_results(
    const boost::filesystem::path &workspace,
    const boost::filesystem::path &pipeline_top_dir, bool verbose,
    const std::map<std::string, std::string> &tag_values,
    const boost::filesystem::path &output_name) {
  std::vector<std::string> loaded_lines;
  // update rule block status based on python report
  for (std::list<boost::shared_ptr<rule_block> >::iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    // if the block reports that it was an include directive
    (*iter)->update_resolution(tag_values);
    if ((*iter)->contains_include_directive() &&
        !(*iter)->get_resolved_included_filename().empty()) {
      if (verbose) {
        std::cout << "\tfound an include directive during python processing"
                  << std::endl;
      }
      boost::filesystem::path recursive_path =
          output_name.parent_path() / (*iter)->get_resolved_included_filename();
      std::string recursive_str = recursive_path.string();
      recursive_str =
          recursive_str.substr(recursive_str.find(workspace.string()) +
                               workspace.string().size() + 1);
      // determine if this was included already
      std::map<boost::filesystem::path,
               boost::shared_ptr<snakemake_file> >::iterator file_finder;
      std::string input_name = output_name.string();
      boost::filesystem::path computed_relative_suffix =
          boost::filesystem::path(
              input_name.substr(input_name.find(workspace.string()) +
                                workspace.size() + 1))
              .parent_path() /
          (*iter)->get_resolved_included_filename();
      input_name = (pipeline_top_dir / computed_relative_suffix).string();

      if ((file_finder = _included_files.find(
               boost::filesystem::path(input_name))) != _included_files.end()) {
        // it was already loaded. recurse into that loaded file
        if (verbose)
          std::cout << "\t\tthe file \"" << file_finder->first.string()
                    << "\" was already loaded, passing python "
                       "results along to it"
                    << std::endl;
        file_finder->second->process_python_results(
            workspace, pipeline_top_dir, verbose, tag_values, recursive_path);
      } else {
        if (verbose) {
          std::cout << "cannot find tag " << boost::filesystem::path(input_name)
                    << " in already included files" << std::endl;
          for (std::map<boost::filesystem::path,
                        boost::shared_ptr<snakemake_file> >::const_iterator
                   mapper = _included_files.begin();
               mapper != _included_files.end(); ++mapper) {
            std::cout << "\tcandidate: " << mapper->first << std::endl;
          }
        }
        // it updated itself to an unambiguous include directive. include it.
        // need the location in input
        if (verbose) {
          std::cout << "\t\toutput name \"" << output_name << "\"" << std::endl
                    << "\t\tinput name: \"" << input_name << "\"" << std::endl
                    << "\t\tworkspace: \"" << workspace << "\"" << std::endl
                    << "\t\tsuffix relative to pipeline dir: \""
                    << computed_relative_suffix.string() << "\"" << std::endl
                    << "\t\tresolved inclusion: \""
                    << (*iter)->get_resolved_included_filename() << "\""
                    << std::endl;
        }
        load_lines(input_name, &loaded_lines);
        if (verbose)
          std::cout
              << "\t\tthe file has not been loaded before, loading it now: "
              << input_name << std::endl;

        boost::shared_ptr<snakemake_file> ptr(new snakemake_file(_tag_counter));
        ptr->parse_file(loaded_lines, ptr->get_blocks().begin(),
                        computed_relative_suffix, verbose);
        _included_files[boost::filesystem::path(input_name)] = ptr;
        // always flag as updated when new file is loaded
        _updated_last_round = true;
      }
    }
  }
  return true;
}

std::vector<std::string> snakemake_unit_tests::snakemake_file::exec(
    const std::string &cmd) const {
  // https://stackoverflow.com/questions/478898/how-do-i-execute-a-command-and-get-the-output-of-the-command-within-c-using-po
  std::array<char, 128> buffer;
  std::vector<std::string> result;
  FILE *pipe = 0;
  pipe = popen(cmd.c_str(), "r");
  if (!pipe) {
    throw std::runtime_error("popen() failed!");
  }
  try {
    while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
      result.push_back(std::string(buffer.data()));
    }
    int status = pclose(pipe);
    if (status == -1) {
      for (std::vector<std::string>::const_iterator iter = result.begin();
           iter != result.end(); ++iter) {
        std::cerr << *iter;
      }
      throw std::runtime_error(
          "exec pipe close failed. this exit status is conceptually possible, "
          "but most likely "
          "due to system inconsistency or instability, or killing a remote job "
          "on a cluster "
          "mid-run. you might consider rerunning with more "
          "RAM or processes free. otherwise, please consider posting any log "
          "output from python3 "
          "to the snakemake_unit_tests repository for feedback.");
    }
    if (!WIFEXITED(status)) {
      for (std::vector<std::string>::const_iterator iter = result.begin();
           iter != result.end(); ++iter) {
        std::cerr << *iter;
      }
      throw std::runtime_error(
          "python subprocess terminated abnormally. this is probably a system "
          "configuration "
          "issue, but may be due to a logic failure in snakemake_unit_tests. "
          "please post "
          "the preceding log output from python3 to an issue in the "
          "snakemake_unit_tests "
          "repository.");
    }
    if (WEXITSTATUS(status)) {
      for (std::vector<std::string>::const_iterator iter = result.begin();
           iter != result.end(); ++iter) {
        std::cerr << *iter;
      }
      throw std::runtime_error(
          "python subprocess returned error exit status. this is most likely "
          "due to "
          "a logic error or snakemake feature in your pipeline that is not "
          "currently "
          "supported by snakemake_unit_tests. please post the preceding log "
          "output from "
          "python3 to an issue in the snakemake_unit_tests repository.");
    }
    return result;
  } catch (...) {
    if (pipe) pclose(pipe);
    throw;
  }
}

void snakemake_unit_tests::snakemake_file::capture_python_tag_values(
    const std::vector<std::string> &vec,
    std::map<std::string, std::string> *target) const {
  const boost::regex tag_value_pair("^(tag[0-9]+): *(.*) *[\r\n]+$");
  const boost::regex tag_alone("^(tag[0-9]+) *[\r\n]+$");
  boost::smatch tag_match;
  if (!target)
    throw std::runtime_error(
        "null pointer provided to capture_python_tag_values");
  // for each line of output
  for (std::vector<std::string>::const_iterator iter = vec.begin();
       iter != vec.end(); ++iter) {
    // match format "tag#: value"
    if (boost::regex_match(*iter, tag_match, tag_value_pair)) {
      (*target)[tag_match[1].str()] = tag_match[2].str();
    } else if (boost::regex_match(*iter, tag_match, tag_alone)) {
      // match format "tag#"
      (*target)[tag_match[1].str()] = "";
    } else {
      // in snakemake interpreter mode, just ignore this
      continue;
    }
  }
}

bool snakemake_unit_tests::snakemake_file::query_rule_checkpoint(
    const std::string &rule_name, bool *target) const {
  if (!target)
    throw std::runtime_error("null pointer to query_rule_checkpoint");
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    // new: respect blocks' reports of inclusion status
    if (!(*iter)->included()) continue;
    // python code. scan for remaining include directives
    if (!(*iter)->get_rule_name().compare(rule_name)) {
      *target = (*iter)->is_checkpoint();
      return true;
    }
  }
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator mapper =
           loaded_files().begin();
       mapper != loaded_files().end(); ++mapper) {
    if (mapper->second->query_rule_checkpoint(rule_name, target)) return true;
  }
  return false;
}

void snakemake_unit_tests::snakemake_file::aggregate_rulesdot() {
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    // new: respect blocks' reports of inclusion status
    if (!(*iter)->included()) {
      continue;
    }
    std::map<std::string, bool> target;
    (*iter)->report_rulesdot_rules(&target);
    std::vector<std::string> vec;
    for (std::map<std::string, bool>::const_iterator miter = target.begin();
         miter != target.end(); ++miter) {
      vec.push_back(miter->first);
    }
    _rulesdot[(*iter)->get_rule_name()] = vec;
  }
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator mapper =
           loaded_files().begin();
       mapper != loaded_files().end(); ++mapper) {
    mapper->second->aggregate_rulesdot();
  }
}

void snakemake_unit_tests::snakemake_file::recursively_query_rulesdot(
    const std::string &rule_name, std::map<std::string, bool> *target) const {
  if (!target) throw std::runtime_error("null pointer to query_rule_rulesdot");
  std::map<std::string, bool> already_found;
  std::deque<std::string> next_up;
  std::string current_query = "";
  std::vector<std::string> res;
  next_up.push_back(rule_name);
  while (!next_up.empty()) {
    current_query = next_up.front();
    next_up.pop_front();
    if (already_found.find(current_query) != already_found.end()) {
      continue;
    }
    already_found[current_query] = true;
    if (!get_rulesdot(current_query, &res))
      throw std::runtime_error(
          "no aggregated rules. data available for rule \"" + rule_name +
          "\"; "
          "did you forget to call aggregate_rulesdot?");
    for (std::vector<std::string>::const_iterator iter = res.begin();
         iter != res.end(); ++iter) {
      if (already_found.find(*iter) == already_found.end()) {
        (*target)[*iter] = true;
        next_up.push_back(*iter);
      }
    }
  }
}

bool snakemake_unit_tests::snakemake_file::get_rulesdot(
    const std::string &name, std::vector<std::string> *target) const {
  if (!target) throw std::runtime_error("null pointer to get_rulesdot");
  std::map<std::string, std::vector<std::string> >::const_iterator finder;
  if ((finder = _rulesdot.find(name)) != _rulesdot.end()) {
    *target = finder->second;
    return true;
  }
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator iter =
           _included_files.begin();
       iter != _included_files.end(); ++iter) {
    if (iter->second->get_rulesdot(name, target)) return true;
  }
  return false;
}

bool snakemake_unit_tests::snakemake_file::get_base_rule_name(
    const std::string &name, std::string *target) const {
  if (!target) throw std::runtime_error("null pointer to get_base_rule_name");
  for (std::list<boost::shared_ptr<rule_block> >::const_iterator iter =
           _blocks.begin();
       iter != _blocks.end(); ++iter) {
    if (!(*iter)->included()) {
      continue;
    }
    if (!(*iter)->get_rule_name().compare(name)) {
      *target = (*iter)->get_base_rule_name();
      return true;
    }
  }
  for (std::map<boost::filesystem::path,
                boost::shared_ptr<snakemake_file> >::const_iterator iter =
           _included_files.begin();
       iter != _included_files.end(); ++iter) {
    if (iter->second->get_base_rule_name(name, target)) return true;
  }
  return false;
}
