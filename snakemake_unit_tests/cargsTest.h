/*!
  \file cargsTest.h
  \brief command line parser test fixture for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_CARGSTEST_H_
#define SNAKEMAKE_UNIT_TESTS_CARGSTEST_H_

#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/utilities.h"

namespace snakemake_unit_tests {
class cargsTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(cargsTest);
  CPPUNIT_TEST(test_params_default_constructor);
  CPPUNIT_TEST(test_params_copy_constructor);
  CPPUNIT_TEST(test_params_report_settings);
  CPPUNIT_TEST(test_params_emit_yaml_map);
  CPPUNIT_TEST(test_params_emit_yaml_vector);
  CPPUNIT_TEST_EXCEPTION(test_cargs_default_constructor, std::domain_error);
  CPPUNIT_TEST(test_cargs_standard_constructor);
  CPPUNIT_TEST(test_cargs_copy_constructor);
  CPPUNIT_TEST(test_cargs_initialize_options);
  CPPUNIT_TEST(test_cargs_set_parameters);
  CPPUNIT_TEST(test_cargs_help);
  CPPUNIT_TEST(test_cargs_get_config_yaml);
  CPPUNIT_TEST(test_cargs_get_snakefile);
  CPPUNIT_TEST(test_cargs_get_snakemake_log);
  CPPUNIT_TEST(test_cargs_get_output_test_dir);
  CPPUNIT_TEST(test_cargs_get_pipeline_top_dir);
  CPPUNIT_TEST(test_cargs_get_pipeline_run_dir);
  CPPUNIT_TEST(test_cargs_get_inst_dir);
  CPPUNIT_TEST(test_cargs_get_added_files);
  CPPUNIT_TEST(test_cargs_get_added_directories);
  CPPUNIT_TEST(test_cargs_get_exclude_rules);
  CPPUNIT_TEST(test_cargs_include_entire_dag);
  CPPUNIT_TEST(test_cargs_update_all);
  CPPUNIT_TEST(test_cargs_update_snakefiles);
  CPPUNIT_TEST(test_cargs_update_added_content);
  CPPUNIT_TEST(test_cargs_update_config);
  CPPUNIT_TEST(test_cargs_update_inputs);
  CPPUNIT_TEST(test_cargs_update_outputs);
  CPPUNIT_TEST(test_cargs_update_pytest);
  CPPUNIT_TEST(test_cargs_verbose);
  CPPUNIT_TEST(test_cargs_compute_flag);
  CPPUNIT_TEST(test_cargs_compute_parameter);
  CPPUNIT_TEST(test_cargs_print_help);
  CPPUNIT_TEST(test_cargs_override_if_specified);
  CPPUNIT_TEST(test_cargs_add_contents_to_vector);
  CPPUNIT_TEST(test_cargs_add_contents_to_map);
  CPPUNIT_TEST(test_cargs_check_nonempty);
  CPPUNIT_TEST(test_cargs_check_regular_file);
  CPPUNIT_TEST(test_cargs_check_and_fix_dir);
  CPPUNIT_TEST(test_cargs_vector_convert);
  CPPUNIT_TEST_SUITE_END();

 public:
  // setup/teardown
  void setUp();
  void tearDown();
  // test case methods
  void test_params_default_constructor();
  void test_params_copy_constructor();
  void test_params_report_settings();
  void test_params_emit_yaml_map();
  void test_params_emit_yaml_vector();
  void test_cargs_default_constructor();
  void test_cargs_standard_constructor();
  void test_cargs_copy_constructor();
  void test_cargs_initialize_options();
  void test_cargs_set_parameters();
  void test_cargs_help();
  void test_cargs_get_config_yaml();
  void test_cargs_get_snakefile();
  void test_cargs_get_snakemake_log();
  void test_cargs_get_output_test_dir();
  void test_cargs_get_pipeline_top_dir();
  void test_cargs_get_pipeline_run_dir();
  void test_cargs_get_inst_dir();
  void test_cargs_get_added_files();
  void test_cargs_get_added_directories();
  void test_cargs_get_exclude_rules();
  void test_cargs_include_entire_dag();
  void test_cargs_update_all();
  void test_cargs_update_snakefiles();
  void test_cargs_update_added_content();
  void test_cargs_update_config();
  void test_cargs_update_inputs();
  void test_cargs_update_outputs();
  void test_cargs_update_pytest();
  void test_cargs_verbose();
  void test_cargs_compute_flag();
  void test_cargs_compute_parameter();
  void test_cargs_print_help();
  void test_cargs_override_if_specified();
  void test_cargs_add_contents_to_vector();
  void test_cargs_add_contents_to_map();
  void test_cargs_check_nonempty();
  void test_cargs_check_regular_file();
  void test_cargs_check_and_fix_dir();
  void test_cargs_vector_convert();

 private:
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_CARGSTEST_H_
