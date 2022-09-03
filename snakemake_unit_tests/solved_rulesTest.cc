/*!
  \file solved_rulesTest.cc
  \brief implementation of solved rules unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/solved_rulesTest.h"

void snakemake_unit_tests::solved_rulesTest::setUp() {
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutSRTXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("solved_rulesTest mkdtemp failed");
  }
}

void snakemake_unit_tests::solved_rulesTest::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void snakemake_unit_tests::solved_rulesTest::test_recipe_default_constructor() {
  recipe r;
  CPPUNIT_ASSERT(r._rule_name.empty());
  CPPUNIT_ASSERT(r._inputs.empty());
  CPPUNIT_ASSERT(r._outputs.empty());
  CPPUNIT_ASSERT(r._log.empty());
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_copy_constructor() {
  recipe r;
  r._rule_name = "rulename";
  r._inputs.push_back("input1");
  r._inputs.push_back("input2");
  r._outputs.push_back("output1");
  r._outputs.push_back("output2");
  r._log = "logname";
  recipe s(r);
  CPPUNIT_ASSERT(!s._rule_name.compare("rulename"));
  CPPUNIT_ASSERT(s._inputs.size() == 2);
  CPPUNIT_ASSERT(!s._inputs.at(0).string().compare("input1"));
  CPPUNIT_ASSERT(!s._inputs.at(1).string().compare("input2"));
  CPPUNIT_ASSERT(!s._outputs.at(0).string().compare("output1"));
  CPPUNIT_ASSERT(!s._outputs.at(1).string().compare("output2"));
  CPPUNIT_ASSERT(!s._log.compare("logname"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_rule_name() {
  recipe r;
  CPPUNIT_ASSERT(r.get_rule_name().empty());
  r._rule_name = "rulename";
  CPPUNIT_ASSERT(!r.get_rule_name().compare("rulename"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_set_rule_name() {
  recipe r;
  r.set_rule_name("rulename1");
  CPPUNIT_ASSERT(!r._rule_name.compare("rulename1"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_inputs() {
  recipe r;
  r._inputs.push_back("input1");
  r._inputs.push_back("input2");
  std::vector<boost::filesystem::path> inputs;
  inputs = r.get_inputs();
  CPPUNIT_ASSERT(inputs.size() == 2);
  CPPUNIT_ASSERT(!inputs.at(0).string().compare("input1"));
  CPPUNIT_ASSERT(!inputs.at(1).string().compare("input2"));
  CPPUNIT_ASSERT(r._inputs.size() == 2);
  CPPUNIT_ASSERT(!r._inputs.at(0).string().compare("input1"));
  CPPUNIT_ASSERT(!r._inputs.at(1).string().compare("input2"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_add_input() {
  recipe r;
  r.add_input("input1");
  CPPUNIT_ASSERT(r._inputs.size() == 1);
  CPPUNIT_ASSERT(!r._inputs.at(0).string().compare("input1"));
  r.add_input("input2");
  CPPUNIT_ASSERT(r._inputs.size() == 2);
  CPPUNIT_ASSERT(!r._inputs.at(0).string().compare("input1"));
  CPPUNIT_ASSERT(!r._inputs.at(1).string().compare("input2"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_outputs() {
  recipe r;
  r._outputs.push_back("output1");
  r._outputs.push_back("output2");
  std::vector<boost::filesystem::path> outputs;
  outputs = r.get_outputs();
  CPPUNIT_ASSERT(outputs.size() == 2);
  CPPUNIT_ASSERT(!outputs.at(0).string().compare("output1"));
  CPPUNIT_ASSERT(!outputs.at(1).string().compare("output2"));
  CPPUNIT_ASSERT(r._outputs.size() == 2);
  CPPUNIT_ASSERT(!r._outputs.at(0).string().compare("output1"));
  CPPUNIT_ASSERT(!r._outputs.at(1).string().compare("output2"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_add_output() {
  recipe r;
  r.add_output("output1");
  CPPUNIT_ASSERT(r._outputs.size() == 1);
  CPPUNIT_ASSERT(!r._outputs.at(0).string().compare("output1"));
  r.add_output("output2");
  CPPUNIT_ASSERT(r._outputs.size() == 2);
  CPPUNIT_ASSERT(!r._outputs.at(0).string().compare("output1"));
  CPPUNIT_ASSERT(!r._outputs.at(1).string().compare("output2"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_log() {
  recipe r;
  r._log = "logname";
  CPPUNIT_ASSERT(!r.get_log().compare("logname"));
  r._log = "othername";
  CPPUNIT_ASSERT(!r.get_log().compare("othername"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_set_log() {
  recipe r;
  r.set_log("logname");
  CPPUNIT_ASSERT(!r._log.compare("logname"));
  r.set_log("othername");
  CPPUNIT_ASSERT(!r._log.compare("othername"));
}
void snakemake_unit_tests::solved_rulesTest::test_recipe_clear() {
  recipe r;
  r._rule_name = "rulename";
  r._inputs.push_back("input1");
  r._inputs.push_back("input2");
  r._outputs.push_back("output1");
  r._outputs.push_back("output2");
  r._log = "logname";
  r.clear();
  CPPUNIT_ASSERT(r._rule_name.empty());
  CPPUNIT_ASSERT(r._inputs.empty());
  CPPUNIT_ASSERT(r._outputs.empty());
  CPPUNIT_ASSERT(r._log.empty());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_default_constructor() {
  solved_rules sr;
  CPPUNIT_ASSERT(sr._recipes.empty());
  CPPUNIT_ASSERT(sr._output_lookup.empty());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_copy_constructor() {
  solved_rules sr;
  boost::shared_ptr<recipe> rec(new recipe);
  sr._recipes.push_back(rec);
  sr._output_lookup["my/path"] = rec;
  solved_rules ss(sr);
  CPPUNIT_ASSERT(ss._recipes.size() == 1);
  CPPUNIT_ASSERT(ss._recipes.at(0) == rec);
  CPPUNIT_ASSERT(ss._output_lookup.size() == 1);
  CPPUNIT_ASSERT(!ss._output_lookup.begin()->first.string().compare("my/path"));
  CPPUNIT_ASSERT(ss._output_lookup.begin()->second == rec);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_load_file() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  std::string log_contents =
      "[Mon Jun 50 14:65:00 2022]\n"
      "rule rulename1:\n"
      "    input: input1, input2\n"
      "    output: output.tsv\n"
      "    log: logfile\n"
      "[Mon Jun 50 14:65:01 2022]\n"
      "checkpoint checkpointname:\n"
      "    input: input3\n"
      "    output: output2.tsv\n"
      "    jobid: whatever\n"
      "    wildcards: whatever\n"
      "    benchmark: whatever\n"
      "    resources: whatever\n"
      "    threads: whatever\n"
      "    priority: whatever\n"
      "    reason: whatever\n"
      "This was a dry-run (flag -n)";
  boost::filesystem::path output_filename = tmp_parent / "logfile.txt";
  std::ofstream output;
  output.open(output_filename.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write solved rules test logfile");
  }
  if (!(output << log_contents << std::endl)) {
    throw std::runtime_error("cannot write solved rules test logfile contents");
  }
  output.close();

  solved_rules sr;
  sr.load_file(output_filename.string());

  CPPUNIT_ASSERT(sr._recipes.size() == 2);
  CPPUNIT_ASSERT(!sr._recipes.at(0)->_rule_name.compare("rulename1"));
  CPPUNIT_ASSERT(sr._recipes.at(0)->_inputs.size() == 2);
  CPPUNIT_ASSERT(!sr._recipes.at(0)->_inputs.at(0).string().compare("input1"));
  CPPUNIT_ASSERT(!sr._recipes.at(0)->_inputs.at(1).string().compare("input2"));
  CPPUNIT_ASSERT(sr._recipes.at(0)->_outputs.size() == 1);
  CPPUNIT_ASSERT(!sr._recipes.at(0)->_outputs.at(0).string().compare("output.tsv"));
  CPPUNIT_ASSERT(!sr._recipes.at(0)->_log.compare("logfile"));
  CPPUNIT_ASSERT(!sr._recipes.at(1)->_rule_name.compare("checkpointname"));
  CPPUNIT_ASSERT(sr._recipes.at(1)->_inputs.size() == 1);
  CPPUNIT_ASSERT(!sr._recipes.at(1)->_inputs.at(0).string().compare("input3"));
  CPPUNIT_ASSERT(sr._recipes.at(1)->_outputs.size() == 1);
  CPPUNIT_ASSERT(!sr._recipes.at(1)->_outputs.at(0).string().compare("output2.tsv"));
  CPPUNIT_ASSERT(sr._recipes.at(1)->_log.empty());
  CPPUNIT_ASSERT(sr._output_lookup.size() == 2);
  CPPUNIT_ASSERT(sr._output_lookup.find("output.tsv") != sr._output_lookup.end());
  CPPUNIT_ASSERT(sr._output_lookup["output.tsv"] == sr._recipes.at(0));
  CPPUNIT_ASSERT(sr._output_lookup.find("output2.tsv") != sr._output_lookup.end());
  CPPUNIT_ASSERT(sr._output_lookup["output2.tsv"] == sr._recipes.at(1));
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_load_file_unresolved_checkpoint() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  std::string log_contents =
      "[Mon Jun 50 14:65:00 2022]\n"
      "rule rulename1:\n"
      "    input: input1, input2\n"
      "    output: output.tsv\n"
      "    log: logfile\n"
      "[Mon Jun 50 14:65:01 2022]\n"
      "checkpoint checkpointname:\n"
      "    input: <TBD>\n"
      "    output: output2.tsv\n"
      "    jobid: whatever\n"
      "    wildcards: whatever\n"
      "    benchmark: whatever\n"
      "    resources: whatever\n"
      "    threads: whatever\n"
      "    priority: whatever\n"
      "    reason: whatever\n"
      "This was a dry-run (flag -n)";
  boost::filesystem::path output_filename = tmp_parent / "logfile.txt";
  std::ofstream output;
  output.open(output_filename.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write solved rules unresolved checkpoint test logfile");
  }
  if (!(output << log_contents << std::endl)) {
    throw std::runtime_error("cannot write solved rules unresolved checkpoint test logfile contents");
  }
  output.close();

  solved_rules sr;
  sr.load_file(output_filename.string());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_load_file_toxic_output_files() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  std::string log_contents =
      "[Mon Jun 50 14:65:00 2022]\n"
      "rule rulename1:\n"
      "    input: input1, input2\n"
      "    output: output.tsv\n"
      "    log: logfile\n"
      "[Mon Jun 50 14:65:01 2022]\n"
      "checkpoint checkpointname:\n"
      "    input: input3\n"
      "    output: output.tsv\n"
      "    jobid: whatever\n"
      "    wildcards: whatever\n"
      "    benchmark: whatever\n"
      "    resources: whatever\n"
      "    threads: whatever\n"
      "    priority: whatever\n"
      "    reason: whatever\n"
      "This was a dry-run (flag -n)";
  boost::filesystem::path output_filename = tmp_parent / "logfile.txt";
  std::ofstream output;
  output.open(output_filename.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write solved rules test logfile");
  }
  if (!(output << log_contents << std::endl)) {
    throw std::runtime_error("cannot write solved rules test logfile contents");
  }
  output.close();

  // capture std::cout
  std::ostringstream observed;
  std::streambuf *previous_buffer(std::cout.rdbuf(observed.rdbuf()));
  solved_rules sr;

  try {
    sr.load_file(output_filename.string());
  } catch (...) {
    std::cout.rdbuf(previous_buffer);
    throw;
  }

  // reset std::cout
  std::cout.rdbuf(previous_buffer);

  // toxic outputs overwrite predecessors in the output tracking map
  CPPUNIT_ASSERT(sr._output_lookup.size() == 1);
  CPPUNIT_ASSERT(sr._output_lookup.find("output.tsv") != sr._output_lookup.end());
  CPPUNIT_ASSERT(sr._output_lookup["output.tsv"] == sr._recipes.at(1));

  // there should be a rather verbose message warning the user about this behavior
  CPPUNIT_ASSERT(observed.str().find("warning: at least one output file appears multiple times") != std::string::npos);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_load_file_unrecognized_block() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  std::string log_contents =
      "[Mon Jun 50 14:65:00 2022]\n"
      "rule rulename1:\n"
      "    input: input1, input2\n"
      "    output: output.tsv\n"
      "    log: logfile\n"
      "[Mon Jun 50 14:65:01 2022]\n"
      "checkpoint checkpointname:\n"
      "    input: <TBD>\n"
      "    output: output2.tsv\n"
      "    johannes: whatever\n"
      "This was a dry-run (flag -n)";
  boost::filesystem::path output_filename = tmp_parent / "logfile.txt";
  std::ofstream output;
  output.open(output_filename.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write solved rules unresolved checkpoint test logfile");
  }
  if (!(output << log_contents << std::endl)) {
    throw std::runtime_error("cannot write solved rules unresolved checkpoint test logfile contents");
  }
  output.close();

  solved_rules sr;
  sr.load_file(output_filename.string());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_emit_tests() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_emit_snakefile() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path workspace = tmp_parent / "workspace";

  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<recipe> rec(new recipe);
  rec->_rule_name = "myrule1";
  rec->_inputs.push_back("input1.tsv");
  rec->_outputs.push_back("output1.tsv");

  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block);
  rb1->_rule_name = "myrule1";
  rb1->_named_blocks.push_back(std::make_pair("input", " \"input1.tsv\","));
  rb1->_named_blocks.push_back(std::make_pair("output", " \"output1.tsv\","));
  rb1->_queried_by_python = true;
  rb1->_resolution = RESOLVED_INCLUDED;
  rb2->_code_chunk.push_back("include: \"rules/file2.smk\"");
  rb2->_queried_by_python = true;
  rb2->_resolution = RESOLVED_INCLUDED;
  rb3->_rule_name = "myrule2";
  rb3->_named_blocks.push_back(std::make_pair("output", " \"output2.tsv\","));
  rb3->_queried_by_python = true;
  rb3->_resolution = RESOLVED_INCLUDED;
  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf2->_blocks.push_back(rb3);

  sf1->_snakefile_relative_path = "workflow/file1.smk";
  sf2->_snakefile_relative_path = "workflow/rules/file2.smk";

  sf1->_included_files["workflow/rules/file2.smk"] = sf2;

  std::map<std::string, bool> dependent_rulenames;
  dependent_rulenames["myrule1"] = true;
  dependent_rulenames["myrule2"] = true;

  solved_rules sr;
  sr.emit_snakefile(*sf1, workspace, rec, dependent_rulenames, true);

  CPPUNIT_ASSERT(boost::filesystem::is_directory(workspace));
  CPPUNIT_ASSERT(boost::filesystem::is_directory(workspace / "workflow"));
  CPPUNIT_ASSERT(boost::filesystem::is_directory(workspace / "workflow" / "rules"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(workspace / "workflow" / "file1.smk"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(workspace / "workflow" / "rules" / "file2.smk"));

  std::ifstream input;
  std::string line = "";
  input.open((workspace / "workflow" / "file1.smk").string().c_str());
  CPPUNIT_ASSERT(input.is_open());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("rule all:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("    input:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("        \"output1.tsv\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("rule myrule1:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("    input: \"input1.tsv\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("    output: \"output1.tsv\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("include: \"rules/file2.smk\""));
  CPPUNIT_ASSERT(input.peek() == EOF);

  input.close();
  input.clear();
  input.open((workspace / "workflow" / "rules" / "file2.smk").string().c_str());
  CPPUNIT_ASSERT(input.is_open());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("rule myrule2:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("    output: \"output2.tsv\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() == EOF);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_create_workspace() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_create_empty_workspace() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path target = tmp_parent / "target";
  boost::filesystem::path inputs = tmp_parent / "workspace/inputs";
  boost::filesystem::path input_dir = inputs / "input_dir";
  boost::filesystem::path input_file_dir = inputs / "input_files";
  boost::filesystem::path input_file1 = input_file_dir / "file1.tsv";
  boost::filesystem::path input_file2 = input_file_dir / "file2.tsv";
  boost::filesystem::path excluded_file = input_file_dir / "file3.tsv";
  boost::filesystem::path dir_file = input_dir / "file4.tsv";
  boost::filesystem::path external_file = tmp_parent / "file5.tsv";

  boost::filesystem::create_directories(target);
  boost::filesystem::create_directories(input_dir);
  boost::filesystem::create_directories(input_file_dir);

  std::ofstream output;
  output.open(input_file1.string().c_str());
  output.close();
  output.clear();
  output.open(input_file2.string().c_str());
  output.close();
  output.clear();
  output.open(excluded_file.string().c_str());
  output.close();
  output.clear();
  output.open(dir_file.string().c_str());
  output.close();
  output.clear();
  output.open(external_file.string().c_str());
  output.close();
  output.clear();

  std::vector<boost::filesystem::path> added_files, added_directories;
  added_files.push_back(input_file1);
  added_files.push_back(input_file2);
  added_files.push_back(external_file);
  added_directories.push_back(input_dir);

  std::map<std::string, std::vector<std::string> > files_outside_workspace;

  solved_rules sr;
  sr.create_empty_workspace(target, inputs, added_files, added_directories, &files_outside_workspace);

  CPPUNIT_ASSERT(boost::filesystem::is_directory(target / ".snakemake_unit_tests"));
  CPPUNIT_ASSERT(boost::filesystem::is_directory(target / ".snakemake_unit_tests" / "input_dir"));
  CPPUNIT_ASSERT(boost::filesystem::is_directory(target / ".snakemake_unit_tests" / "input_files"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target / ".snakemake_unit_tests" / "input_dir" / "file4.tsv"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target / ".snakemake_unit_tests" / "input_files" / "file1.tsv"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target / ".snakemake_unit_tests" / "input_files" / "file2.tsv"));
  CPPUNIT_ASSERT(!boost::filesystem::is_regular_file(target / ".snakemake_unit_tests" / "input_files" / "file3.tsv"));
  CPPUNIT_ASSERT(files_outside_workspace.size() == 1);
  CPPUNIT_ASSERT(files_outside_workspace.find(external_file.string()) != files_outside_workspace.end());
  CPPUNIT_ASSERT(files_outside_workspace[external_file.string()].size() == 1);
  CPPUNIT_ASSERT(!files_outside_workspace[external_file.string()].at(0).compare("added files"));
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_remove_empty_workspace() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path workspace = tmp_parent / ".snakemake_unit_tests";
  boost::filesystem::path embedded_content = workspace / "subdir";
  boost::filesystem::create_directories(embedded_content);
  CPPUNIT_ASSERT(boost::filesystem::is_directory(workspace));

  solved_rules sr;
  sr.remove_empty_workspace(tmp_parent);
  CPPUNIT_ASSERT(!boost::filesystem::is_directory(workspace));
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_copy_contents() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path workspace = tmp_parent / "workspace";
  boost::filesystem::path outside = tmp_parent / "outside";
  boost::filesystem::path target = tmp_parent / "destination";
  boost::filesystem::create_directories(workspace);
  boost::filesystem::create_directories(outside);
  boost::filesystem::create_directories(target);
  // create some files for copying
  boost::filesystem::path file1 = workspace / "test1.tsv";
  boost::filesystem::path file2 = workspace / "test2.tsv";
  boost::filesystem::path file3 = outside / "test3.tsv";
  std::ofstream output;
  output.open(file1.string().c_str());
  output.close();
  output.clear();
  output.open(file2.string().c_str());
  output.close();
  output.clear();
  output.open(file3.string().c_str());
  output.close();
  output.clear();
  // pretend a file already exists in target and is permission locked
  output.open((target / "test2.tsv").string().c_str());
  output.close();
  output.clear();
  boost::filesystem::permissions(target / "test2.tsv",
                                 boost::filesystem::owner_write | boost::filesystem::remove_perms);

  std::vector<boost::filesystem::path> contents;
  contents.push_back(boost::filesystem::path("test1.tsv"));
  contents.push_back(file2);
  contents.push_back(boost::filesystem::path(file3));
  std::map<std::string, std::vector<std::string> > files_outside_workspace;

  solved_rules sr;
  sr.copy_contents(contents, workspace, target, "myrule", &files_outside_workspace);

  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target / "test1.tsv"));
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target / "test2.tsv"));
  CPPUNIT_ASSERT(!boost::filesystem::is_regular_file(target / "test3.tsv"));
  CPPUNIT_ASSERT(files_outside_workspace.size() == 1);
  CPPUNIT_ASSERT(files_outside_workspace.find(file3.string()) != files_outside_workspace.end());
  CPPUNIT_ASSERT(files_outside_workspace[file3.string()].size() == 1);
  CPPUNIT_ASSERT(!files_outside_workspace[file3.string()].at(0).compare("myrule"));
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_phony_all_target() {
  std::ofstream output;
  std::vector<boost::filesystem::path> targets;
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path output_file = tmp_parent / "output.smk";
  output.open(output_file.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write phony_all_target output file");
  }
  if (!(output << "preserved content" << std::endl)) {
    throw std::runtime_error("cannot write contents of phony_all_target output file");
  }
  targets.push_back(tmp_parent / "target2.tsv");
  targets.push_back(tmp_parent / "target1.tsv");
  solved_rules sr;
  sr.report_phony_all_target(output, targets);
  output.close();

  std::ifstream input;
  std::string line = "";
  input.open(output_file.string().c_str());
  CPPUNIT_ASSERT(input.is_open());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("preserved content"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("rule all:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("    input:"));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("        \"" + targets.at(0).string() + "\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(!line.compare("        \"" + targets.at(1).string() + "\","));
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() != EOF);
  getline(input, line);
  CPPUNIT_ASSERT(line.empty());
  CPPUNIT_ASSERT(input.peek() == EOF);
  input.close();
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_test_script() {
  /*
    need the following:
    - parent directory for output script (e.g. tests/unit/)
    - directory for relative calls to pytest (e.g. tests/)
    - rule name
    - snakefile path relative to workflow execution directory
    - workflow execution directory
    - bonus patterns to add to comparison ignore list
    - full path to inst/test.py schematic file
   */
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path testdir = tmp_parent / "tests";
  boost::filesystem::path unitdir = testdir / "unit";
  std::string rulename = "myrule";
  boost::filesystem::path snakefile_relative_path = "workflow/Snakefile";
  boost::filesystem::path rundir = ".";
  std::vector<boost::filesystem::path> extra_exclusions;
  extra_exclusions.push_back(".docx");
  extra_exclusions.push_back(".eps");
  boost::filesystem::path inst_test_py = tmp_parent / "inst" / "test.py";
  boost::filesystem::create_directories(unitdir);
  boost::filesystem::create_directories(tmp_parent / "inst");

  std::ofstream output;
  output.open(inst_test_py.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write modified_test_script inst.py");
  }
  if (!(output << "interesting stuff goes here" << std::endl)) {
    throw std::runtime_error("cannot write contents of modified_test_script inst.py");
  }
  output.close();

  solved_rules sr;
  sr.report_modified_test_script(unitdir, testdir, rulename, snakefile_relative_path, rundir, extra_exclusions,
                                 inst_test_py);

  boost::filesystem::path expected = unitdir / ("test_" + rulename + ".py");
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(expected));
  std::ifstream input;
  input.open(expected.string().c_str());
  bool found_shebang = false, found_testdir = false, found_rulename = false, found_relative_path = false,
       found_exec_path = false, found_extra_exclusions = false, found_inst_contents = false, firstline = true;
  std::string line = "";
  while (input.peek() != EOF) {
    getline(input, line);
    if (!line.compare("#!/usr/bin/env python3")) {
      CPPUNIT_ASSERT(firstline);
      firstline = false;
      found_shebang = true;
    } else if (!line.compare("testdir='" + testdir.string() + "'")) {
      CPPUNIT_ASSERT(!found_testdir);
      found_testdir = true;
    } else if (!line.compare("rulename='" + rulename + "'")) {
      CPPUNIT_ASSERT(!found_rulename);
      found_rulename = true;
    } else if (!line.compare("snakefile_relative_path='" + snakefile_relative_path.string() + "'")) {
      CPPUNIT_ASSERT(!found_relative_path);
      found_relative_path = true;
    } else if (!line.compare("snakemake_exec_path='" + rundir.string() + "'")) {
      CPPUNIT_ASSERT(!found_exec_path);
      found_exec_path = true;
    } else if (!line.compare("extra_comparison_exclusions=['.docx', '.eps', ]")) {
      CPPUNIT_ASSERT(!found_extra_exclusions);
      found_extra_exclusions = true;
    } else if (!line.compare("interesting stuff goes here")) {
      CPPUNIT_ASSERT(!found_inst_contents);
      found_inst_contents = true;
    }
  }
  input.close();

  CPPUNIT_ASSERT(found_shebang);
  CPPUNIT_ASSERT(found_testdir);
  CPPUNIT_ASSERT(found_rulename);
  CPPUNIT_ASSERT(found_relative_path);
  CPPUNIT_ASSERT(found_exec_path);
  CPPUNIT_ASSERT(found_extra_exclusions);
  CPPUNIT_ASSERT(found_inst_contents);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_launcher_script() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path inst_dir = tmp_parent / "inst";
  boost::filesystem::path target_dir = tmp_parent / "target";
  boost::filesystem::create_directories(inst_dir);
  boost::filesystem::create_directories(target_dir);
  boost::filesystem::path input_script = inst_dir / "scriptname.bash";
  boost::filesystem::path test_dir = target_dir / "all_the_tests";
  boost::filesystem::path target_script = target_dir / "pytest_runner.bash";
  std::ifstream input;
  std::ofstream output;
  output.open(input_script.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write modified launcher script file");
  }
  if (!(output << "script\ncontents" << std::endl)) {
    throw std::runtime_error("cannot write modified launcher script contents");
  }
  output.close();

  CPPUNIT_ASSERT(!boost::filesystem::is_regular_file(target_script));
  solved_rules sr;
  sr.report_modified_launcher_script(target_dir, test_dir, input_script);
  CPPUNIT_ASSERT(boost::filesystem::is_regular_file(target_script));
  input.open(target_script.string().c_str());
  if (!input.is_open()) {
    throw std::runtime_error("mysteriously cannot open modified launcher script output");
  }
  std::vector<std::string> expected;
  expected.push_back("#!/usr/bin/env bash");
  expected.push_back("SNAKEMAKE_UNIT_TESTS_DIR=" + test_dir.string());
  expected.push_back("script");
  expected.push_back("contents");
  unsigned line_count = 0;
  std::string line = "";
  for (; line_count < expected.size(); ++line_count) {
    CPPUNIT_ASSERT(input.peek() != EOF);
    getline(input, line);
    CPPUNIT_ASSERT(!line.compare(expected.at(line_count)));
  }
  CPPUNIT_ASSERT(input.peek() == EOF);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_launcher_script_bad_target_directory() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path inst_dir = tmp_parent / "inst";
  boost::filesystem::path target_dir = tmp_parent / "target";
  boost::filesystem::create_directories(inst_dir);
  boost::filesystem::path input_script = inst_dir / "scriptname.bash";
  boost::filesystem::path test_dir = target_dir / "all_the_tests";
  boost::filesystem::path target_script = target_dir / "pytest_runner.bash";
  std::ofstream output;
  output.open(input_script.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write modified launcher script file");
  }
  if (!(output << "script\ncontents" << std::endl)) {
    throw std::runtime_error("cannot write modified launcher script contents");
  }
  output.close();

  CPPUNIT_ASSERT(!boost::filesystem::is_regular_file(target_script));
  solved_rules sr;
  sr.report_modified_launcher_script(target_dir, test_dir, input_script);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_launcher_script_missing_script() {
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path inst_dir = tmp_parent / "inst";
  boost::filesystem::path target_dir = tmp_parent / "target";
  boost::filesystem::create_directories(inst_dir);
  boost::filesystem::create_directories(target_dir);
  boost::filesystem::path input_script = inst_dir / "scriptname.bash";
  boost::filesystem::path test_dir = target_dir / "all_the_tests";
  boost::filesystem::path target_script = target_dir / "pytest_runner.bash";

  CPPUNIT_ASSERT(!boost::filesystem::is_regular_file(target_script));
  solved_rules sr;
  sr.report_modified_launcher_script(target_dir, test_dir, input_script);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_find_missing_rules() {
  std::vector<std::string> exec_log;
  std::map<std::string, bool> missing_rules;
  exec_log.push_back("Exception: 'Rules' object has no attribute 'rulename1' so that's a bummer\n");
  exec_log.push_back("Other exception: 'Rules' object has no attribute 'rulename2' which makes me sad\n");
  exec_log.push_back("'Rules' object has attribute 'rulename3', so let's not just focus on the negative\n");
  exec_log.push_back("Exception: 'Checkpoints' object has no attribute 'check1', which again stinks\n");
  exec_log.push_back("Other exception: 'Checkpoints' object has no attribute 'check2', I give up\n");
  solved_rules sr;
  sr.find_missing_rules(exec_log, &missing_rules);
  CPPUNIT_ASSERT(missing_rules.size() == 4);
  CPPUNIT_ASSERT(missing_rules.find("rulename1") != missing_rules.end());
  CPPUNIT_ASSERT(missing_rules.find("rulename2") != missing_rules.end());
  CPPUNIT_ASSERT(missing_rules.find("check1") != missing_rules.end());
  CPPUNIT_ASSERT(missing_rules.find("check2") != missing_rules.end());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_find_missing_rules_null_pointer() {
  std::vector<std::string> exec_log;
  solved_rules sr;
  sr.find_missing_rules(exec_log, NULL);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_find_missing_rules_unexpected_error() {
  std::vector<std::string> exec_log;
  std::map<std::string, bool> missing_rules;
  exec_log.push_back("'Rules' object has attribute 'rulename3', so let's not just focus on the negative\n");
  exec_log.push_back("Exception: damnable portal of antediluvian evil\n");

  // capture std::cerr
  std::ostringstream observed;
  std::streambuf *previous_buffer(std::cerr.rdbuf(observed.rdbuf()));
  solved_rules sr;

  try {
    sr.find_missing_rules(exec_log, &missing_rules);
  } catch (...) {
    std::cerr.rdbuf(previous_buffer);
    CPPUNIT_ASSERT(observed.str().find("damnable portal of antediluvian evil") != std::string::npos);
    throw;
  }

  // reset std::cerr
  std::cout.rdbuf(previous_buffer);
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_add_dag_from_leaf() {
  std::map<boost::shared_ptr<recipe>, bool> included_rules;
  boost::shared_ptr<recipe> rec1(new recipe), rec2(new recipe), rec3(new recipe);
  rec1->_inputs.push_back("input1.tsv");
  rec1->_inputs.push_back("input2.tsv");
  rec1->_outputs.push_back("output1.tsv");
  rec2->_inputs.push_back("input3.tsv");
  rec2->_inputs.push_back("output1.tsv");
  rec2->_outputs.push_back("output2.tsv");
  rec3->_inputs.push_back("input4.tsv");
  rec3->_inputs.push_back("output2.tsv");
  rec3->_outputs.push_back("output3.tsv");
  solved_rules sr;
  sr._recipes.push_back(rec1);
  sr._recipes.push_back(rec2);
  sr._recipes.push_back(rec3);
  sr._output_lookup["output1.tsv"] = rec1;
  sr._output_lookup["output2.tsv"] = rec2;
  sr._output_lookup["output3.tsv"] = rec3;
  sr.add_dag_from_leaf(rec3, false, &included_rules);
  CPPUNIT_ASSERT(included_rules.size() == 1);
  CPPUNIT_ASSERT(included_rules.find(rec2) != included_rules.end());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_add_dag_from_leaf_entire() {
  std::map<boost::shared_ptr<recipe>, bool> included_rules;
  boost::shared_ptr<recipe> rec1(new recipe), rec2(new recipe), rec3(new recipe);
  rec1->_inputs.push_back("input1.tsv");
  rec1->_inputs.push_back("input2.tsv");
  rec1->_outputs.push_back("output1.tsv");
  rec2->_inputs.push_back("input3.tsv");
  rec2->_inputs.push_back("output1.tsv");
  rec2->_outputs.push_back("output2.tsv");
  rec3->_inputs.push_back("input4.tsv");
  rec3->_inputs.push_back("output2.tsv");
  rec3->_outputs.push_back("output3.tsv");
  solved_rules sr;
  sr._recipes.push_back(rec1);
  sr._recipes.push_back(rec2);
  sr._recipes.push_back(rec3);
  sr._output_lookup["output1.tsv"] = rec1;
  sr._output_lookup["output2.tsv"] = rec2;
  sr._output_lookup["output3.tsv"] = rec3;
  sr.add_dag_from_leaf(rec3, true, &included_rules);
  CPPUNIT_ASSERT(included_rules.size() == 2);
  CPPUNIT_ASSERT(included_rules.find(rec2) != included_rules.end());
  CPPUNIT_ASSERT(included_rules.find(rec1) != included_rules.end());
}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_add_dag_from_leaf_null_pointer() {
  boost::shared_ptr<recipe> rec(new recipe);
  solved_rules sr;
  sr.add_dag_from_leaf(rec, true, NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::solved_rulesTest);
