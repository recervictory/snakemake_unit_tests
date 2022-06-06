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

void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_default_constructor() {
  snakemake_file sf;
  CPPUNIT_ASSERT(sf._blocks.empty());
  CPPUNIT_ASSERT(sf._snakefile_relative_path.string().empty());
  CPPUNIT_ASSERT(sf._included_files.empty());
  CPPUNIT_ASSERT(sf._tag_counter.get());
  CPPUNIT_ASSERT_EQUAL(1u, *sf._tag_counter);
  CPPUNIT_ASSERT(sf._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_pointer_constructor() {
  boost::shared_ptr<unsigned> ptr(new unsigned);
  *ptr = 20u;
  snakemake_file sf(ptr);
  CPPUNIT_ASSERT(sf._blocks.empty());
  CPPUNIT_ASSERT(sf._snakefile_relative_path.string().empty());
  CPPUNIT_ASSERT(sf._included_files.empty());
  CPPUNIT_ASSERT(sf._tag_counter.get());
  CPPUNIT_ASSERT_EQUAL(20u, *sf._tag_counter);
  CPPUNIT_ASSERT(sf._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_copy_constructor() {
  snakemake_file sf1;
  boost::shared_ptr<rule_block> ptr_rb(new rule_block);
  boost::shared_ptr<snakemake_file> ptr_sf(new snakemake_file);
  sf1._blocks.push_back(ptr_rb);
  sf1._snakefile_relative_path = "/my/path";
  sf1._included_files["/other/path"] = ptr_sf;
  *sf1._tag_counter = 55u;
  sf1._updated_last_round = false;
  snakemake_file sf2(sf1);
  CPPUNIT_ASSERT(sf2._blocks.size() == 1u);
  CPPUNIT_ASSERT(*(sf2._blocks.begin()) == ptr_rb);
  CPPUNIT_ASSERT(!sf2._snakefile_relative_path.string().compare("/my/path"));
  CPPUNIT_ASSERT(sf2._included_files.size() == 1u);
  CPPUNIT_ASSERT(sf2._included_files.find("/other/path") != sf2._included_files.end());
  CPPUNIT_ASSERT(sf2._included_files["/other/path"] == ptr_sf);
  CPPUNIT_ASSERT_EQUAL(55u, *sf2._tag_counter);
  CPPUNIT_ASSERT(!sf2._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_everything() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_parse_file() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_lines() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_detect_known_issues() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_blocks() {
  std::list<boost::shared_ptr<rule_block> > result;
  boost::shared_ptr<rule_block> b1(new rule_block), b2(new rule_block);
  b1->_rule_name = "myrule";
  b1->_named_blocks.push_back(std::make_pair("input", " 'myinfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("output", " 'myoutfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("shell", " 'echo thing > {output}'"));
  b2->_code_chunk.push_back("localrules: myrule");
  snakemake_file sf;
  sf._blocks.push_back(b1);
  sf._blocks.push_back(b2);
  result = sf.get_blocks();
  CPPUNIT_ASSERT(result.size() == 2u);
  CPPUNIT_ASSERT(*(result.begin()) == b1);
  CPPUNIT_ASSERT(*(result.rbegin()) == b2);
}
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
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::snakemake_fileTest);
