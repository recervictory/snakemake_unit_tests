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

#include <cstdlib>
#include <filesystem>
#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "snakemake_unit_tests/cargs.h"
#include "snakemake_unit_tests/utilities.h"
#include "snakemake_unit_tests/yaml_reader.h"

namespace snakemake_unit_tests {
class cargsTest : public CppUnit::TestFixture {
  // macros to declare suite
  CPPUNIT_TEST_SUITE(cargsTest);
  CPPUNIT_TEST(test_params_default_constructor);
  CPPUNIT_TEST(test_params_copy_constructor);
  CPPUNIT_TEST(test_params_report_settings);
  CPPUNIT_TEST(test_params_emit_yaml_map_multiple_entries);
  CPPUNIT_TEST(test_params_emit_yaml_map_single_entry);
  CPPUNIT_TEST(test_params_emit_yaml_map_no_entries);
  CPPUNIT_TEST_EXCEPTION(test_params_emit_yaml_map_null_pointer, std::runtime_error);
  CPPUNIT_TEST(test_params_emit_yaml_vector_multiple_entries);
  CPPUNIT_TEST(test_params_emit_yaml_vector_single_entry);
  CPPUNIT_TEST(test_params_emit_yaml_vector_no_entries);
  CPPUNIT_TEST_EXCEPTION(test_params_emit_yaml_vector_null_pointer, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_default_constructor, std::domain_error);
  CPPUNIT_TEST(test_cargs_standard_constructor);
  CPPUNIT_TEST(test_cargs_copy_constructor);
  CPPUNIT_TEST(test_cargs_initialize_options);
  CPPUNIT_TEST(test_cargs_set_parameters);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_output_dir_missing, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_snakefile_invalid, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_top_dir_invalid, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_run_dir_invalid, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_inst_dir_invalid, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_inst_dir_missing_test, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_inst_dir_missing_common, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_snakemake_log_missing, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_added_files_invalid, std::logic_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_set_parameters_added_directories_invalid, std::logic_error);
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
  CPPUNIT_TEST_EXCEPTION(test_cargs_compute_missing_required_parameter, std::domain_error);
  CPPUNIT_TEST(test_cargs_print_help);
  CPPUNIT_TEST(test_cargs_override_if_specified);
  CPPUNIT_TEST(test_cargs_add_contents_to_vector);
  CPPUNIT_TEST_EXCEPTION(test_cargs_add_contents_to_vector_null_pointer, std::runtime_error);
  CPPUNIT_TEST(test_cargs_add_contents_to_map);
  CPPUNIT_TEST_EXCEPTION(test_cargs_add_contents_to_map_null_pointer, std::runtime_error);
  CPPUNIT_TEST(test_cargs_check_nonempty);
  CPPUNIT_TEST_EXCEPTION(test_cargs_check_nonempty_invalid_path, std::logic_error);
  CPPUNIT_TEST(test_cargs_check_regular_file);
  CPPUNIT_TEST_EXCEPTION(test_cargs_check_regular_file_not_file, std::logic_error);
  CPPUNIT_TEST(test_cargs_check_and_fix_dir);
  CPPUNIT_TEST_EXCEPTION(test_cargs_check_and_fix_dir_null_pointer, std::runtime_error);
  CPPUNIT_TEST_EXCEPTION(test_cargs_check_and_fix_dir_not_directory, std::logic_error);
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
  void test_params_emit_yaml_map_multiple_entries();
  void test_params_emit_yaml_map_single_entry();
  void test_params_emit_yaml_map_no_entries();
  void test_params_emit_yaml_map_null_pointer();
  void test_params_emit_yaml_vector_multiple_entries();
  void test_params_emit_yaml_vector_single_entry();
  void test_params_emit_yaml_vector_no_entries();
  void test_params_emit_yaml_vector_null_pointer();
  void test_cargs_default_constructor();
  void test_cargs_standard_constructor();
  void test_cargs_copy_constructor();
  void test_cargs_initialize_options();
  void test_cargs_set_parameters();
  void test_cargs_set_parameters_output_dir_missing();
  void test_cargs_set_parameters_snakefile_invalid();
  void test_cargs_set_parameters_top_dir_invalid();
  void test_cargs_set_parameters_run_dir_invalid();
  void test_cargs_set_parameters_inst_dir_invalid();
  void test_cargs_set_parameters_inst_dir_missing_test();
  void test_cargs_set_parameters_inst_dir_missing_common();
  void test_cargs_set_parameters_snakemake_log_missing();
  void test_cargs_set_parameters_added_files_invalid();
  void test_cargs_set_parameters_added_directories_invalid();
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
  void test_cargs_compute_missing_required_parameter();
  void test_cargs_print_help();
  void test_cargs_override_if_specified();
  void test_cargs_add_contents_to_vector();
  void test_cargs_add_contents_to_vector_null_pointer();
  void test_cargs_add_contents_to_map();
  void test_cargs_add_contents_to_map_null_pointer();
  void test_cargs_check_nonempty();
  void test_cargs_check_nonempty_invalid_path();
  void test_cargs_check_regular_file();
  void test_cargs_check_regular_file_not_file();
  void test_cargs_check_and_fix_dir();
  void test_cargs_check_and_fix_dir_not_directory();
  void test_cargs_check_and_fix_dir_null_pointer();
  void test_cargs_vector_convert();

 private:
  void populate_arguments(const std::string &cmd, std::vector<std::string> *vec, const char ***arr) const;
  void create_empty_file(const boost::filesystem::path &p) const;

  const char **_argv_long;
  const char **_argv_short;
  const char **_argv_adhoc;
  std::vector<std::string> _arg_vec_long;
  std::vector<std::string> _arg_vec_short;
  std::vector<std::string> _arg_vec_adhoc;
  char *_tmp_dir;
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_CARGSTEST_H_
