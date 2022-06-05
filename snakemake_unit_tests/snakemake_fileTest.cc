/*!
  \file snakemake_fileTest.cc
  \brief implementation of snakemake file unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/snakemake_fileTest.h"

void snakemake_unit_tests::snakemake_fileTest::setUp() {
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutSFTXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("snakemake_fileTest mkdtemp failed");
  }
}

void snakemake_unit_tests::snakemake_fileTest::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_default_constructor() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_pointer_constructor() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_copy_constructor() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_everything() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_parse_file() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_lines() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_detect_known_issues() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_print_blocks() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_blocks() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_single_rule() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_fully_resolved() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_contains_blockers() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_resolve_with_python() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_process_python_results() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_exec() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_capture_python_tag_values() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_postflight_checks() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_snakefile_relative_path() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_loaded_files() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_set_update_status() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_rules() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_query_rule_checkpoint() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_aggregate_rulesdot() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_recursively_query_rulesdot() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_rulesdot() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::snakemake_fileTest);
