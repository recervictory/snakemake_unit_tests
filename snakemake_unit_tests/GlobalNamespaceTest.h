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

#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
class GlobalNamespaceTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(GlobalNamespaceTest);
  CPPUNIT_TEST(test_vector_to_map);
  CPPUNIT_TEST(test_remove_comments_and_docstrings);
  CPPUNIT_TEST(test_split_comma_list);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_vector_to_map();
  void test_remove_comments_and_docstrings();
  void test_split_comma_list();

 private:
  std::map<std::string, bool> _test_map;
  std::vector<std::string> _test_vec;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_GLOBALNAMESPACETEST_H_
