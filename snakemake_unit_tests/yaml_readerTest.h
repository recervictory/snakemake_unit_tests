/*!
  \file yaml_readerTest.h
  \brief yaml reader test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_YAML_READERTEST_H_
#define SNAKEMAKE_UNIT_TESTS_YAML_READERTEST_H_

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
#include "snakemake_unit_tests/yaml_reader.h"
#include "yaml-cpp/yaml.h"

namespace snakemake_unit_tests {
class yaml_readerTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(yaml_readerTest);
  CPPUNIT_TEST(test_yaml_reader_default_constructor);
  CPPUNIT_TEST(test_yaml_reader_copy_constructor);
  CPPUNIT_TEST(test_yaml_reader_default_constructor);
  CPPUNIT_TEST(test_yaml_reader_string_constructor);
  CPPUNIT_TEST(test_yaml_reader_copy_constructor);
  CPPUNIT_TEST(test_yaml_reader_load_file);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_load_file_bad_file, std::runtime_error);
  CPPUNIT_TEST(test_yaml_reader_get_entry_query);
  CPPUNIT_TEST(test_yaml_reader_get_entry_queries);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_get_entry_multiple_values, std::runtime_error);
  CPPUNIT_TEST(test_yaml_reader_get_sequence_query);
  CPPUNIT_TEST(test_yaml_reader_get_sequence_queries);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_get_sequence_non_conformable, std::runtime_error);
  CPPUNIT_TEST(test_yaml_reader_get_map_query);
  CPPUNIT_TEST(test_yaml_reader_get_map_queries);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_get_map_non_conformable, std::runtime_error);
  CPPUNIT_TEST(test_yaml_reader_get_node_query);
  CPPUNIT_TEST(test_yaml_reader_get_node_queries);
  CPPUNIT_TEST(test_yaml_reader_query_valid_query);
  CPPUNIT_TEST(test_yaml_reader_query_valid_queries);
  CPPUNIT_TEST(test_yaml_reader_equality);
  CPPUNIT_TEST(test_yaml_reader_apply_queries);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_apply_queries_null_pointer, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_apply_queries_no_queries, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_apply_queries_wrong_type, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_apply_queries_query_absent, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_yaml_reader_apply_queries_scalar_terminator, std::runtime_error);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_yaml_reader_default_constructor();
  void test_yaml_reader_string_constructor();
  void test_yaml_reader_copy_constructor();
  void test_yaml_reader_load_file();
  void test_yaml_reader_load_file_bad_file();
  void test_yaml_reader_get_entry_query();
  void test_yaml_reader_get_entry_queries();
  void test_yaml_reader_get_entry_multiple_values();
  void test_yaml_reader_get_sequence_query();
  void test_yaml_reader_get_sequence_queries();
  void test_yaml_reader_get_sequence_non_conformable();
  void test_yaml_reader_get_map_query();
  void test_yaml_reader_get_map_queries();
  void test_yaml_reader_get_map_non_conformable();
  void test_yaml_reader_get_node_query();
  void test_yaml_reader_get_node_queries();
  void test_yaml_reader_query_valid_query();
  void test_yaml_reader_query_valid_queries();
  void test_yaml_reader_equality();
  void test_yaml_reader_apply_queries();
  void test_yaml_reader_apply_queries_null_pointer();
  void test_yaml_reader_apply_queries_no_queries();
  void test_yaml_reader_apply_queries_wrong_type();
  void test_yaml_reader_apply_queries_query_absent();
  void test_yaml_reader_apply_queries_scalar_terminator();

 private:
  char *_tmp_dir;
  boost::filesystem::path _yaml_file_1;
  boost::filesystem::path _yaml_file_2;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_YAML_READERTEST_H_
