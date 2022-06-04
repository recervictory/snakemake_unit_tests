/*!
  \file rule_blockTest.cc
  \brief implementation of rule block unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/rule_blockTest.h"

void snakemake_unit_tests::rule_blockTest::setUp() {
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutRBTXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("rule_blockTest mkdtemp failed");
  }
}

void snakemake_unit_tests::rule_blockTest::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void snakemake_unit_tests::rule_blockTest::test_rule_block_default_constructor() {
  rule_block b;
  CPPUNIT_ASSERT(b._rule_name.empty());
  CPPUNIT_ASSERT(b._base_rule_name.empty());
  CPPUNIT_ASSERT(!b._rule_is_checkpoint);
  CPPUNIT_ASSERT(b._docstring.empty());
  CPPUNIT_ASSERT(b._named_blocks.empty());
  CPPUNIT_ASSERT(b._code_chunk.empty());
  CPPUNIT_ASSERT_EQUAL(0u, b._local_indentation);
  CPPUNIT_ASSERT_EQUAL(UNRESOLVED, b._resolution);
  CPPUNIT_ASSERT(!b._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, b._python_tag);
  CPPUNIT_ASSERT(b._resolved_included_filename.string().empty());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_copy_constructor() {
  rule_block b1;
  b1._rule_name = "rulename";
  b1._base_rule_name = "baserulename";
  b1._rule_is_checkpoint = true;
  b1._docstring = "text goes here";
  b1._named_blocks["thing1"] = "thing2";
  b1._named_blocks["thing2"] = "thing3";
  b1._code_chunk.push_back("code goes here;");
  b1._code_chunk.push_back("{some other thing;}");
  b1._local_indentation = 42;
  b1._resolution = RESOLVED_INCLUDED;
  b1._queried_by_python = true;
  b1._python_tag = 333;
  b1._resolved_included_filename = "thing1/thing2/thing3";
  rule_block b2(b1);
  CPPUNIT_ASSERT(!b2._rule_name.compare("rulename"));
  CPPUNIT_ASSERT(!b2._base_rule_name.compare("baserulename"));
  CPPUNIT_ASSERT(b2._rule_is_checkpoint);
  CPPUNIT_ASSERT(!b2._docstring.compare("text goes here"));
  CPPUNIT_ASSERT(b2._named_blocks.size() == 2u);
  CPPUNIT_ASSERT(b2._named_blocks.find("thing1") != b2._named_blocks.end());
  CPPUNIT_ASSERT(!b2._named_blocks["thing1"].compare("thing2"));
  CPPUNIT_ASSERT(b2._named_blocks.find("thing2") != b2._named_blocks.end());
  CPPUNIT_ASSERT(!b2._named_blocks["thing2"].compare("thing3"));
  CPPUNIT_ASSERT(b2._code_chunk.size() == 2u);
  CPPUNIT_ASSERT(!b2._code_chunk.at(0).compare("code goes here;"));
  CPPUNIT_ASSERT(!b2._code_chunk.at(1).compare("{some other thing;}"));
  CPPUNIT_ASSERT_EQUAL(42u, b2._local_indentation);
  CPPUNIT_ASSERT_EQUAL(RESOLVED_INCLUDED, b2._resolution);
  CPPUNIT_ASSERT(b2._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(333u, b2._python_tag);
  CPPUNIT_ASSERT(!b2._resolved_included_filename.compare("thing1/thing2/thing3"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_load_content_block() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_consume_rule_contents() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_rule_name() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_rule_name() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_base_rule_name() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_base_rule_name() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_is_simple_include_directive() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_contains_include_directive() {
  rule_block b;
  b._code_chunk.push_back("include: stuff");
  CPPUNIT_ASSERT(b.contains_include_directive());
  b._code_chunk.at(0) = "include: \"stuff\"";
  CPPUNIT_ASSERT(b.contains_include_directive());
  b._code_chunk.at(0) = "   include: thing";
  CPPUNIT_ASSERT(b.contains_include_directive());
  b._code_chunk.at(0) = "include: \"thing\"   ";
  CPPUNIT_ASSERT(b.contains_include_directive());
  b._code_chunk.at(0) = "include thing";
  CPPUNIT_ASSERT(!b.contains_include_directive());
  b._code_chunk.at(0) = "sinclude: thing";
  CPPUNIT_ASSERT(!b.contains_include_directive());
  // note that this code chunk is technically impossible,
  // as individual python statements should be stored individually
  b._code_chunk.at(0) = "include: thing";
  b._code_chunk.push_back("include: otherthing");
  CPPUNIT_ASSERT(!b.contains_include_directive());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_filename_expression() {
  rule_block b;
  b._code_chunk.push_back("include: stuff");
  CPPUNIT_ASSERT(!b.get_filename_expression().compare("stuff"));
  b._code_chunk.at(0) = "include: \"stuff\"";
  CPPUNIT_ASSERT(!b.get_filename_expression().compare("\"stuff\""));
  b._code_chunk.at(0) = "   include: thing";
  CPPUNIT_ASSERT(!b.get_filename_expression().compare("thing"));
  b._code_chunk.at(0) = "include: \"thing\"   ";
  CPPUNIT_ASSERT(!b.get_filename_expression().compare("\"thing\""));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_filename_expression_invalid_statement() {
  rule_block b;
  b._code_chunk.push_back("here's some weird statement that isn't an include");
  b.get_filename_expression();
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_include_depth() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_standard_filename() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_print_contents() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_code_chunk() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_named_blocks() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_local_indentation() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_add_code_chunk() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_equality_operator() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_inequality_operator() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_resolved() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_included() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_resolution() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_resolution_status() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_interpreter_tag() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_interpreter_tag() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_report_python_logging_code() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_update_resolution() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_resolved_included_filename() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_is_checkpoint() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_checkpoint() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_report_rulesdot_rules() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_indentation() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_apply_indentation() {}
void snakemake_unit_tests::rule_blockTest::test_rule_block_clear() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::rule_blockTest);
