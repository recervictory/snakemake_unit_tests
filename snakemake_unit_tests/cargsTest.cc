/*!
  \file cargsTest.cc
  \brief implementation of command line parsing unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/cargsTest.h"

void snakemake_unit_tests::cargsTest::setUp() {}

void snakemake_unit_tests::cargsTest::tearDown() {
  // nothing to do at the moment
}

void snakemake_unit_tests::cargsTest::test_params_default_constructor() {}
void snakemake_unit_tests::cargsTest::test_params_copy_constructor() {}
void snakemake_unit_tests::cargsTest::test_params_report_settings() {}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_map() {}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_vector() {}
void snakemake_unit_tests::cargsTest::test_cargs_default_constructor() { cargs ap; }
void snakemake_unit_tests::cargsTest::test_cargs_standard_constructor() {}
void snakemake_unit_tests::cargsTest::test_cargs_copy_constructor() {}
void snakemake_unit_tests::cargsTest::test_cargs_initialize_options() {}
void snakemake_unit_tests::cargsTest::test_cargs_set_parameters() {}
void snakemake_unit_tests::cargsTest::test_cargs_help() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_config_yaml() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakefile() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakemake_log() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_output_test_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_top_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_run_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_inst_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_files() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_directories() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_exclude_rules() {}
void snakemake_unit_tests::cargsTest::test_cargs_include_entire_dag() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_all() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_snakefiles() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_added_content() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_config() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_inputs() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_outputs() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_pytest() {}
void snakemake_unit_tests::cargsTest::test_cargs_verbose() {}
void snakemake_unit_tests::cargsTest::test_cargs_compute_flag() {}
void snakemake_unit_tests::cargsTest::test_cargs_compute_parameter() {}
void snakemake_unit_tests::cargsTest::test_cargs_print_help() {}
void snakemake_unit_tests::cargsTest::test_cargs_override_if_specified() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_vector() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_map() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_nonempty() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_regular_file() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_vector_convert() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::cargsTest);
