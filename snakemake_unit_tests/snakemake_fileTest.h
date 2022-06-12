/*!
  \file snakemake_fileTest.h
  \brief snakemake file test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILETEST_H_
#define SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILETEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <cstdlib>
#include <filesystem>
#include <list>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "snakemake_unit_tests/snakemake_file.h"

namespace snakemake_unit_tests {
class snakemake_fileTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(snakemake_fileTest);
  CPPUNIT_TEST(test_snakemake_file_default_constructor);
  CPPUNIT_TEST(test_snakemake_file_pointer_constructor);
  CPPUNIT_TEST(test_snakemake_file_copy_constructor);
  CPPUNIT_TEST(test_snakemake_file_load_everything);
  CPPUNIT_TEST(test_snakemake_file_parse_file);
  CPPUNIT_TEST(test_snakemake_file_load_lines);
  CPPUNIT_TEST(test_snakemake_file_detect_known_issues);
  CPPUNIT_TEST(test_snakemake_file_get_blocks);
  CPPUNIT_TEST(test_snakemake_file_report_single_rule);
  CPPUNIT_TEST(test_snakemake_file_fully_resolved);
  CPPUNIT_TEST(test_snakemake_file_contains_blockers);
  CPPUNIT_TEST(test_snakemake_file_resolve_with_python);
  CPPUNIT_TEST(test_snakemake_file_process_python_results);
  CPPUNIT_TEST(test_snakemake_file_capture_python_tag_values);
  CPPUNIT_TEST(test_snakemake_file_postflight_checks);
  CPPUNIT_TEST(test_snakemake_file_get_snakefile_relative_path);
  CPPUNIT_TEST(test_snakemake_file_loaded_files);
  CPPUNIT_TEST(test_snakemake_file_set_update_status);
  CPPUNIT_TEST(test_snakemake_file_report_rules);
  CPPUNIT_TEST(test_snakemake_file_get_base_rule_name);
  CPPUNIT_TEST_EXCEPTION(test_snakemake_file_get_base_rule_name_null_pointer, std::runtime_error);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_snakemake_file_default_constructor();
  void test_snakemake_file_pointer_constructor();
  void test_snakemake_file_copy_constructor();
  void test_snakemake_file_load_everything();
  void test_snakemake_file_parse_file();
  void test_snakemake_file_load_lines();
  void test_snakemake_file_detect_known_issues();
  void test_snakemake_file_get_blocks();
  void test_snakemake_file_report_single_rule();
  void test_snakemake_file_fully_resolved();
  void test_snakemake_file_contains_blockers();
  void test_snakemake_file_resolve_with_python();
  void test_snakemake_file_process_python_results();
  void test_snakemake_file_capture_python_tag_values();
  void test_snakemake_file_postflight_checks();
  void test_snakemake_file_get_snakefile_relative_path();
  void test_snakemake_file_loaded_files();
  void test_snakemake_file_set_update_status();
  void test_snakemake_file_report_rules();
  void test_snakemake_file_get_base_rule_name();
  void test_snakemake_file_get_base_rule_name_null_pointer();

 private:
  char *_tmp_dir;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_SNAKEMAKE_FILETEST_H_
