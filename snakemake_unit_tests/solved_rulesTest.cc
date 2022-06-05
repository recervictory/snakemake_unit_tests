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

void snakemake_unit_tests::solved_rulesTest::test_recipe_default_constructor() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_copy_constructor() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_rule_name() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_set_rule_name() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_inputs() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_add_input() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_outputs() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_add_output() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_get_log() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_set_log() {}
void snakemake_unit_tests::solved_rulesTest::test_recipe_clear() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_default_constructor() {}
void snakemake_unit_tests::solved_rulesTest::test_solved_rules_copy_constructor() {}
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
