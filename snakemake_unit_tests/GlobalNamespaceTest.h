/*!
  \file GlobalNamespaceTest.h
  \brief global namespace test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_GLOBALNAMESPACETEST_H_
#define SNAKEMAKE_UNIT_TESTS_GLOBALNAMESPACETEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <map>
#include <string>
#include <vector>

#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
class GlobalNamespaceTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(GlobalNamespaceTest);
  CPPUNIT_TEST(test_vector_to_map);
  CPPUNIT_TEST(test_split_comma_list_1);
  CPPUNIT_TEST(test_split_comma_list_2);
  CPPUNIT_TEST(test_split_comma_list_3);
  CPPUNIT_TEST(test_split_comma_list_4);
  CPPUNIT_TEST(test_split_comma_list_5);
  CPPUNIT_TEST(test_split_comma_list_6);
  CPPUNIT_TEST(test_split_comma_list_7);
  CPPUNIT_TEST(test_split_comma_list_8);
  CPPUNIT_TEST(test_append_resolved_line_1);
  CPPUNIT_TEST(test_append_resolved_line_2);
  CPPUNIT_TEST(test_append_resolved_line_3);
  CPPUNIT_TEST(test_append_resolved_line_4);
  CPPUNIT_TEST(test_append_resolved_line_5);
  CPPUNIT_TEST(test_append_resolved_line_13);
  CPPUNIT_TEST(test_append_resolved_line_14);
  CPPUNIT_TEST(test_append_resolved_line_15);
  CPPUNIT_TEST(test_append_resolved_line_16);
  CPPUNIT_TEST_EXCEPTION(test_append_resolved_line_null_arg1, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_append_resolved_line_null_arg2, std::runtime_error);
  CPPUNIT_TEST(test_resolve_string_delimiter_1);
  CPPUNIT_TEST(test_resolve_string_delimiter_2);
  CPPUNIT_TEST(test_resolve_string_delimiter_3);
  CPPUNIT_TEST(test_resolve_string_delimiter_4);
  CPPUNIT_TEST(test_resolve_string_delimiter_5);
  CPPUNIT_TEST(test_resolve_string_delimiter_6);
  CPPUNIT_TEST(test_resolve_string_delimiter_7);
  CPPUNIT_TEST(test_resolve_string_delimiter_8);
  CPPUNIT_TEST(test_resolve_string_delimiter_9);
  CPPUNIT_TEST(test_resolve_string_delimiter_10);
  CPPUNIT_TEST(test_resolve_string_delimiter_11);
  CPPUNIT_TEST(test_resolve_string_delimiter_12);
  CPPUNIT_TEST(test_resolve_string_delimiter_13);
  CPPUNIT_TEST(test_resolve_string_delimiter_14);
  CPPUNIT_TEST(test_resolve_string_delimiter_15);
  CPPUNIT_TEST(test_resolve_string_delimiter_16);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg1, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg2, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg3, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg4, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_index_oob, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_index_not_mark, std::runtime_error);
  CPPUNIT_TEST(test_lexical_parse);
  CPPUNIT_TEST(test_exec);
  CPPUNIT_TEST_EXCEPTION(test_exec_fail_on_error, std::runtime_error);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_vector_to_map();
  void test_split_comma_list_1();
  void test_split_comma_list_2();
  void test_split_comma_list_3();
  void test_split_comma_list_4();
  void test_split_comma_list_5();
  void test_split_comma_list_6();
  void test_split_comma_list_7();
  void test_split_comma_list_8();
  void test_append_resolved_line_1();
  void test_append_resolved_line_2();
  void test_append_resolved_line_3();
  void test_append_resolved_line_4();
  void test_append_resolved_line_5();
  void test_append_resolved_line_13();
  void test_append_resolved_line_14();
  void test_append_resolved_line_15();
  void test_append_resolved_line_16();
  void test_append_resolved_line_null_arg1();
  void test_append_resolved_line_null_arg2();
  void test_resolve_string_delimiter_1();
  void test_resolve_string_delimiter_2();
  void test_resolve_string_delimiter_3();
  void test_resolve_string_delimiter_4();
  void test_resolve_string_delimiter_5();
  void test_resolve_string_delimiter_6();
  void test_resolve_string_delimiter_7();
  void test_resolve_string_delimiter_8();
  void test_resolve_string_delimiter_9();
  void test_resolve_string_delimiter_10();
  void test_resolve_string_delimiter_11();
  void test_resolve_string_delimiter_12();
  void test_resolve_string_delimiter_13();
  void test_resolve_string_delimiter_14();
  void test_resolve_string_delimiter_15();
  void test_resolve_string_delimiter_16();
  void test_resolve_string_delimiter_null_arg1();
  void test_resolve_string_delimiter_null_arg2();
  void test_resolve_string_delimiter_null_arg3();
  void test_resolve_string_delimiter_null_arg4();
  void test_resolve_string_delimiter_index_oob();
  void test_resolve_string_delimiter_index_not_mark();
  void test_lexical_parse();
  void test_exec();
  void test_exec_fail_on_error();

 private:
  std::map<std::string, bool> _test_map;
  std::vector<std::string> _test_vec;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_GLOBALNAMESPACETEST_H_
