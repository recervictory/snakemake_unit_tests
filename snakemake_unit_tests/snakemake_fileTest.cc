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
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_single_rule() {
  /*
    construct some dummy infrastructure to test the ability to distinguish between:
    - a rule that's included
    - a rule that's excluded
    - python supporting code
  */
  snakemake_file sf;
  boost::shared_ptr<rule_block> b1(new rule_block), b2(new rule_block), b3(new rule_block), b4(new rule_block),
      b5(new rule_block);
  b1->_code_chunk.push_back("if True:");
  b1->_resolution = RESOLVED_INCLUDED;
  b1->_queried_by_python = true;
  b2->_rule_name = "myrule";
  b2->_local_indentation = 4;
  b2->_named_blocks.push_back(std::make_pair("input", "\n            file1,"));
  b2->_resolution = RESOLVED_INCLUDED;
  b2->_queried_by_python = true;
  b3->_code_chunk.push_back("else:");
  b3->_resolution = RESOLVED_INCLUDED;
  b3->_queried_by_python = true;
  b4->_rule_name = "myrule";
  b4->_local_indentation = 4;
  b4->_resolution = RESOLVED_EXCLUDED;
  b4->_queried_by_python = true;
  b5->_rule_name = "otherrule";
  b5->_local_indentation = 0;
  b5->_named_blocks.push_back(std::make_pair("input", "\n        file2,"));
  b5->_resolution = RESOLVED_INCLUDED;
  b5->_queried_by_python = true;

  sf._blocks.push_back(b1);
  sf._blocks.push_back(b2);
  sf._blocks.push_back(b3);
  sf._blocks.push_back(b4);
  sf._blocks.push_back(b5);

  std::map<std::string, bool> ruleset;
  ruleset["myrule"] = true;
  ruleset["otherrule"] = true;
  std::ostringstream out;
  unsigned result = sf.report_single_rule(ruleset, out);
  CPPUNIT_ASSERT(result == 2);
  std::string expected =
      "if True:\n    rule myrule:\n        input:\n            file1,\n\n\n"
      "else:\n    pass\n\n\nrule otherrule:\n    input:\n        file2,\n\n\n";
  CPPUNIT_ASSERT(!out.str().compare(expected));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_fully_resolved() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_contains_blockers() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_resolve_with_python() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_process_python_results() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_capture_python_tag_values() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_postflight_checks() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_snakefile_relative_path() {
  snakemake_file sf;
  sf._snakefile_relative_path = boost::filesystem::path("mypath");
  CPPUNIT_ASSERT(!sf.get_snakefile_relative_path().string().compare("mypath"));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_loaded_files() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_set_update_status() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_rules() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name() {
  snakemake_file sf1;
  boost::shared_ptr<snakemake_file> sf2(new snakemake_file);
  boost::shared_ptr<rule_block> b1(new rule_block), b2(new rule_block), b3(new rule_block);
  b1->_rule_name = "myrule";
  b1->_base_rule_name = "top_level_rule";
  b1->_named_blocks.push_back(std::make_pair("input", " 'myinfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("output", " 'myoutfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("shell", " 'echo thing > {output}'"));
  b1->_resolution = RESOLVED_INCLUDED;
  b2->_code_chunk.push_back("localrules: myrule");
  b2->_resolution = RESOLVED_INCLUDED;
  b3->_rule_name = "hidden_rule";
  b3->_base_rule_name = "magical_treasure";
  b3->_named_blocks.push_back(std::make_pair("output", " 'nextoutfile.txt',"));
  b3->_named_blocks.push_back(std::make_pair("shell", " 'echo otherthing > {output}'"));
  b3->_resolution = RESOLVED_INCLUDED;
  sf1._blocks.push_back(b1);
  sf1._blocks.push_back(b2);
  sf2->_blocks.push_back(b3);
  sf1._included_files["/path/to/file"] = sf2;
  std::string result = "";
  CPPUNIT_ASSERT(sf1.get_base_rule_name("myrule", &result));
  CPPUNIT_ASSERT(!result.compare("top_level_rule"));
  CPPUNIT_ASSERT(sf1.get_base_rule_name("hidden_rule", &result));
  CPPUNIT_ASSERT(!result.compare("magical_treasure"));
  CPPUNIT_ASSERT(!sf1.get_base_rule_name("fake_rule", &result));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name_null_pointer() {
  snakemake_file sf;
  sf.get_base_rule_name("fake_rule", NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::snakemake_fileTest);
