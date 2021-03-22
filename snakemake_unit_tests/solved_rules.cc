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
    input.open(filename.c_str());
    if (!input.is_open())
      throw std::runtime_error("cannot open snakemake log file \"" + filename +
                               "\"");
    while (input.peek() != EOF) {
      getline(input, line);
      // if the line is a valid rule declaration
      if (boost::regex_match(line, regex_result, standard_rule_declaration)) {
        recipe rep;
        rep.set_rule_name(regex_result[1]);
        while (input.peek() != EOF) {
          getline(input, line);
          if (line.empty()) break;
          if (line.find("    input:") == 0) {
            split_comma_list(line.substr(11), &input_filenames);
            for (std::vector<std::string>::const_iterator iter =
                     input_filenames.begin();
                 iter != input_filenames.end(); ++iter) {
              rep.add_input(*iter);
            }
          } else if (line.find("    output:") == 0) {
            split_comma_list(line.substr(12), &output_filenames);
            for (std::vector<std::string>::const_iterator iter =
                     output_filenames.begin();
                 iter != output_filenames.end(); ++iter) {
              rep.add_output(*iter);
            }
          } else if (line.find("    log:") == 0) {
            rep.set_log(line.substr(9));
          } else if (line.find("    jobid:") == 0 ||
                     line.find("    wildcards:") == 0) {
            // for the moment, do nothing
          } else {
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
    const snakemake_file &sf) const {
  // iterate across loaded recipes, creating tests as you go
  std::map<std::string, bool> test_history;
  for (std::vector<recipe>::const_iterator iter = _recipes.begin();
       iter != _recipes.end(); ++iter) {
    // only create output if the rule has not already been hit
    if (test_history.find(iter->get_rule_name()) == test_history.end()) {
      test_history[iter->get_rule_name()] = true;
      // create an output file that is hopefully unique for the rule
      std::string output_filename = "Snakefile." + iter->get_rule_name();
      std::ofstream output;
      output.open(output_filename.c_str());
      if (!output.is_open())
        throw std::runtime_error("cannot create working snakemake file \"" +
                                 output_filename + "\"");
      // find the rule from the parsed snakefile(s) and report it to file
      sf.report_single_rule(iter->get_rule_name(), output);
      // for now, just report what you think the command should be
      output << "## snakemake -n";
      for (std::vector<std::string>::const_iterator output_iter =
               iter->get_outputs().begin();
           output_iter != iter->get_outputs().end(); ++output_iter) {
        output << ' ' << *output_iter;
      }
      output << std::endl;
      output.close();
      output.clear();
    }
  }
}
