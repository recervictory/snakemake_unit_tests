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
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_load_file() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_emit_tests() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_emit_snakefile() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_create_workspace() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_create_empty_workspace() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_remove_empty_workspace() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_copy_contents() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_phony_all_target() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_test_script() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_report_modified_launcher_script() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_find_missing_rules() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_add_dag_from_leaf() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::solved_rulesTest);
