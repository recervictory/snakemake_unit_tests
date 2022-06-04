/*!
  \file rule_blockTest.h
  \brief rule block test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_RULE_BLOCKTEST_H_
#define SNAKEMAKE_UNIT_TESTS_RULE_BLOCKTEST_H_

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

#include "boost/filesystem.hpp"
#include "snakemake_unit_tests/rule_block.h"

namespace snakemake_unit_tests {
class rule_blockTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(rule_blockTest);
  CPPUNIT_TEST(test_rule_block_default_constructor);
  CPPUNIT_TEST(test_rule_block_copy_constructor);
  CPPUNIT_TEST(test_rule_block_load_content_block);
  CPPUNIT_TEST(test_rule_block_consume_rule_contents);
  CPPUNIT_TEST(test_rule_block_set_rule_name);
  CPPUNIT_TEST(test_rule_block_get_rule_name);
  CPPUNIT_TEST(test_rule_block_set_base_rule_name);
  CPPUNIT_TEST(test_rule_block_get_base_rule_name);
  CPPUNIT_TEST(test_rule_block_is_simple_include_directive);
  CPPUNIT_TEST(test_rule_block_contains_include_directive);
  CPPUNIT_TEST_EXCEPTION(test_rule_block_get_filename_expression_invalid_statement, std::runtime_error);
  CPPUNIT_TEST(test_rule_block_get_filename_expression);
  CPPUNIT_TEST(test_rule_block_get_include_depth);
  CPPUNIT_TEST(test_rule_block_get_standard_filename);
  CPPUNIT_TEST(test_rule_block_print_contents);
  CPPUNIT_TEST(test_rule_block_get_code_chunk);
  CPPUNIT_TEST(test_rule_block_get_named_blocks);
  CPPUNIT_TEST(test_rule_block_get_local_indentation);
  CPPUNIT_TEST(test_rule_block_add_code_chunk);
  CPPUNIT_TEST(test_rule_block_equality_operator);
  CPPUNIT_TEST(test_rule_block_inequality_operator);
  CPPUNIT_TEST(test_rule_block_resolved);
  CPPUNIT_TEST(test_rule_block_included);
  CPPUNIT_TEST(test_rule_block_set_resolution);
  CPPUNIT_TEST(test_rule_block_get_resolution_status);
  CPPUNIT_TEST(test_rule_block_set_interpreter_tag);
  CPPUNIT_TEST(test_rule_block_get_interpreter_tag);
  CPPUNIT_TEST(test_rule_block_report_python_logging_code);
  CPPUNIT_TEST(test_rule_block_update_resolution);
  CPPUNIT_TEST(test_rule_block_get_resolved_included_filename);
  CPPUNIT_TEST(test_rule_block_is_checkpoint);
  CPPUNIT_TEST(test_rule_block_set_checkpoint);
  CPPUNIT_TEST(test_rule_block_report_rulesdot_rules);
  CPPUNIT_TEST(test_rule_block_indentation);
  CPPUNIT_TEST(test_rule_block_apply_indentation);
  CPPUNIT_TEST(test_rule_block_clear);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_rule_block_default_constructor();
  void test_rule_block_copy_constructor();
  void test_rule_block_load_content_block();
  void test_rule_block_consume_rule_contents();
  void test_rule_block_set_rule_name();
  void test_rule_block_get_rule_name();
  void test_rule_block_set_base_rule_name();
  void test_rule_block_get_base_rule_name();
  void test_rule_block_is_simple_include_directive();
  void test_rule_block_contains_include_directive();
  void test_rule_block_get_filename_expression();
  void test_rule_block_get_filename_expression_invalid_statement();
  void test_rule_block_get_include_depth();
  void test_rule_block_get_standard_filename();
  void test_rule_block_print_contents();
  void test_rule_block_get_code_chunk();
  void test_rule_block_get_named_blocks();
  void test_rule_block_get_local_indentation();
  void test_rule_block_add_code_chunk();
  void test_rule_block_equality_operator();
  void test_rule_block_inequality_operator();
  void test_rule_block_resolved();
  void test_rule_block_included();
  void test_rule_block_set_resolution();
  void test_rule_block_get_resolution_status();
  void test_rule_block_set_interpreter_tag();
  void test_rule_block_get_interpreter_tag();
  void test_rule_block_report_python_logging_code();
  void test_rule_block_update_resolution();
  void test_rule_block_get_resolved_included_filename();
  void test_rule_block_is_checkpoint();
  void test_rule_block_set_checkpoint();
  void test_rule_block_report_rulesdot_rules();
  void test_rule_block_indentation();
  void test_rule_block_apply_indentation();
  void test_rule_block_clear();

 private:
  char *_tmp_dir;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_RULE_BLOCKTEST_H_
