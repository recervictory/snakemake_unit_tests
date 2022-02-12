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
  CPPUNIT_TEST(test_split_comma_list);
  CPPUNIT_TEST(test_concatenate_string_literals);
  CPPUNIT_TEST_EXCEPTION(test_concatenate_string_literals_null_arg1,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_concatenate_string_literals_null_arg2,
                         std::runtime_error);
  CPPUNIT_TEST(test_resolve_string_delimiter);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg1,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg2,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg3,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_null_arg4,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_index_oob,
                         std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_resolve_string_delimiter_index_not_mark,
                         std::runtime_error);
  CPPUNIT_TEST(test_lexical_parse);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_vector_to_map();
  void test_split_comma_list();
  void test_concatenate_string_literals();
  void test_concatenate_string_literals_null_arg1();
  void test_concatenate_string_literals_null_arg2();
  void test_resolve_string_delimiter();
  void test_resolve_string_delimiter_null_arg1();
  void test_resolve_string_delimiter_null_arg2();
  void test_resolve_string_delimiter_null_arg3();
  void test_resolve_string_delimiter_null_arg4();
  void test_resolve_string_delimiter_index_oob();
  void test_resolve_string_delimiter_index_not_mark();
  void test_lexical_parse();

 private:
  std::map<std::string, bool> _test_map;
  std::vector<std::string> _test_vec;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_GLOBALNAMESPACETEST_H_
