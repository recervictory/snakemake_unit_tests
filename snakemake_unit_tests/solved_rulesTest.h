/*!
  \file solved_rulesTest.h
  \brief solved rules test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SOLVED_RULESTEST_H_
#define SNAKEMAKE_UNIT_TESTS_SOLVED_RULESTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cstdlib>
#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "snakemake_unit_tests/solved_rules.h"

namespace snakemake_unit_tests {
class solved_rulesTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(solved_rulesTest);
  CPPUNIT_TEST(test_recipe_default_constructor);
  CPPUNIT_TEST(test_recipe_copy_constructor);
  CPPUNIT_TEST(test_recipe_get_rule_name);
  CPPUNIT_TEST(test_recipe_set_rule_name);
  CPPUNIT_TEST(test_recipe_get_inputs);
  CPPUNIT_TEST(test_recipe_add_input);
  CPPUNIT_TEST(test_recipe_get_outputs);
  CPPUNIT_TEST(test_recipe_add_output);
  CPPUNIT_TEST(test_recipe_get_log);
  CPPUNIT_TEST(test_recipe_set_log);
  CPPUNIT_TEST(test_recipe_clear);
  CPPUNIT_TEST(test_solved_rules_default_constructor);
  CPPUNIT_TEST(test_solved_rules_copy_constructor);
  CPPUNIT_TEST(test_solved_rules_load_file);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_load_file_unresolved_checkpoint, std::logic_error);
  CPPUNIT_TEST(test_solved_rules_load_file_toxic_output_files);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_load_file_unrecognized_block, std::logic_error);
  CPPUNIT_TEST(test_solved_rules_emit_tests);
  CPPUNIT_TEST(test_solved_rules_emit_snakefile);
  CPPUNIT_TEST(test_solved_rules_create_workspace);
  CPPUNIT_TEST(test_solved_rules_create_empty_workspace);
  CPPUNIT_TEST(test_solved_rules_remove_empty_workspace);
  CPPUNIT_TEST(test_solved_rules_copy_contents);
  CPPUNIT_TEST(test_solved_rules_report_phony_all_target);
  CPPUNIT_TEST(test_solved_rules_report_modified_test_script);
  CPPUNIT_TEST(test_solved_rules_report_modified_launcher_script);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_report_modified_launcher_script_bad_target_directory, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_report_modified_launcher_script_missing_script, std::runtime_error);
  CPPUNIT_TEST(test_solved_rules_find_missing_rules);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_find_missing_rules_null_pointer, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_find_missing_rules_unexpected_error, std::runtime_error);
  CPPUNIT_TEST(test_solved_rules_add_dag_from_leaf);
  CPPUNIT_TEST(test_solved_rules_add_dag_from_leaf_entire);
  CPPUNIT_TEST_EXCEPTION(test_solved_rules_add_dag_from_leaf_null_pointer, std::runtime_error);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_recipe_default_constructor();
  void test_recipe_copy_constructor();
  void test_recipe_get_rule_name();
  void test_recipe_set_rule_name();
  void test_recipe_get_inputs();
  void test_recipe_add_input();
  void test_recipe_get_outputs();
  void test_recipe_add_output();
  void test_recipe_get_log();
  void test_recipe_set_log();
  void test_recipe_clear();
  void test_solved_rules_default_constructor();
  void test_solved_rules_copy_constructor();
  void test_solved_rules_load_file();
  void test_solved_rules_load_file_unresolved_checkpoint();
  void test_solved_rules_load_file_toxic_output_files();
  void test_solved_rules_load_file_unrecognized_block();
  void test_solved_rules_emit_tests();
  void test_solved_rules_emit_snakefile();
  void test_solved_rules_create_workspace();
  void test_solved_rules_create_empty_workspace();
  void test_solved_rules_remove_empty_workspace();
  void test_solved_rules_copy_contents();
  void test_solved_rules_report_phony_all_target();
  void test_solved_rules_report_modified_test_script();
  void test_solved_rules_report_modified_launcher_script();
  void test_solved_rules_report_modified_launcher_script_bad_target_directory();
  void test_solved_rules_report_modified_launcher_script_missing_script();
  void test_solved_rules_find_missing_rules();
  void test_solved_rules_find_missing_rules_null_pointer();
  void test_solved_rules_find_missing_rules_unexpected_error();
  void test_solved_rules_add_dag_from_leaf();
  void test_solved_rules_add_dag_from_leaf_entire();
  void test_solved_rules_add_dag_from_leaf_null_pointer();

 private:
  char *_tmp_dir;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SOLVED_RULESTEST_H_
