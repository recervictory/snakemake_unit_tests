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
  b1._named_blocks.push_back(std::make_pair("thing1", "thing2"));
  b1._named_blocks.push_back(std::make_pair("thing2", "thing3"));
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
  CPPUNIT_ASSERT(!b2._named_blocks.at(0).first.compare("thing1"));
  CPPUNIT_ASSERT(!b2._named_blocks.at(0).second.compare("thing2"));
  CPPUNIT_ASSERT(!b2._named_blocks.at(1).first.compare("thing2"));
  CPPUNIT_ASSERT(!b2._named_blocks.at(1).second.compare("thing3"));
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
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_rule_name() {
  rule_block b;
  b.set_rule_name("dothething");
  CPPUNIT_ASSERT(!b._rule_name.compare("dothething"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_rule_name() {
  rule_block b;
  b._rule_name = "myfunrule";
  CPPUNIT_ASSERT(!b.get_rule_name().compare("myfunrule"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_base_rule_name() {
  rule_block b;
  b.set_base_rule_name("mybaserule");
  CPPUNIT_ASSERT(!b._base_rule_name.compare("mybaserule"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_base_rule_name() {
  rule_block b;
  b._base_rule_name = "starter";
  CPPUNIT_ASSERT(!b.get_base_rule_name().compare("starter"));
}
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
void snakemake_unit_tests::rule_blockTest::test_rule_block_print_contents() {
  /*
    print_contents probes the content of the rule_block and decides what to emit
    depending on what it finds

    - if there's a code block, print the code block directly
    - else if there's a rule name:
    -- if there's a base rule name, make a derived rule declaration
    -- else make a standard rule declaration
    -- if there's a docstring, print the docstring
    -- report any rule block sections (e.g. input, output, shell) in order they were encountered
    - else emit something that is treated as a snakemake metacontent block
    -- e.g. global wildcard_constraints

    behavior of emitting in order detected in input is new. this will hopefully make the
    code substantially more resilient to upstream changes in snakemake behavior, as the order
    should generally be guaranteed valid for whatever version of snakemake was used to run
    the test pipeline.
   */
  rule_block b;
  // test logical order of determining what to report
  b._code_chunk.push_back("code chunk line 1");
  b._code_chunk.push_back("   code chunk line 2");
  b._rule_name = "myrulename";
  b._base_rule_name = "parentrulename";
  b._docstring = "      '''here's some commentary'''";
  b._named_blocks.push_back(std::make_pair("input", "\n          'filename1',\n          'filename2',"));
  b._named_blocks.push_back(std::make_pair("output", "\n          'filename3',"));
  b._named_blocks.push_back(std::make_pair("shell", "\n          'cat {input} > {output}'"));
  b._local_indentation = 2u;
  // since a code chunk is present, it should prefer to report the code chunk
  std::ostringstream o1, o2, o3, o4;
  std::string expected = "";
  b.print_contents(o1);
  CPPUNIT_ASSERT(!o1.str().compare("code chunk line 1\n   code chunk line 2\n"));
  // remove code chunk; should turn it into a derived rule
  b._code_chunk.clear();
  b.print_contents(o2);
  expected =
      "  use rule parentrulename as myrulename with:\n"
      "      '''here's some commentary'''\n"
      "      input:\n          'filename1',\n"
      "          'filename2',\n"
      "      output:\n          'filename3',\n"
      "      shell:\n          'cat {input} > {output}'\n\n\n";
  CPPUNIT_ASSERT(!o2.str().compare(expected));
  // remove base rule name; should turn it into a standard rule
  b._base_rule_name = "";
  b.print_contents(o3);
  expected =
      "  rule myrulename:\n"
      "      '''here's some commentary'''\n"
      "      input:\n          'filename1',\n"
      "          'filename2',\n"
      "      output:\n          'filename3',\n"
      "      shell:\n          'cat {input} > {output}'\n\n\n";
  CPPUNIT_ASSERT(!o3.str().compare(expected));
  // remove rule name; should turn it into floating snakemake content directives;
  //   obviously these are not actually global directives, but the point remains.
  //   note that this is expected to preserve the indentation of lines after the
  //   block name tag exactly; so since this is reusing the example above, the
  //   resulting indentation is not pythonic.
  b._rule_name = "";
  b.print_contents(o4);
  expected =
      "  input:\n          'filename1',\n"
      "          'filename2',\n"
      "  output:\n          'filename3',\n"
      "  shell:\n          'cat {input} > {output}'\n";
  CPPUNIT_ASSERT(!o4.str().compare(expected));
}

void snakemake_unit_tests::rule_blockTest::test_rule_block_get_code_chunk() {
  rule_block b;
  std::vector<std::string> data, result;
  data.push_back("line1");
  data.push_back("line2");
  data.push_back("line3");
  b._code_chunk = data;
  result = b.get_code_chunk();
  CPPUNIT_ASSERT(result.size() == 3u);
  CPPUNIT_ASSERT(!result.at(0).compare("line1"));
  CPPUNIT_ASSERT(!result.at(1).compare("line2"));
  CPPUNIT_ASSERT(!result.at(2).compare("line3"));
  b._code_chunk.clear();
  result = b.get_code_chunk();
  CPPUNIT_ASSERT(result.empty());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_named_blocks() {
  rule_block b;
  std::vector<std::pair<std::string, std::string> > result;
  b._named_blocks.push_back(std::make_pair("input", "  name1\n  name2"));
  b._named_blocks.push_back(std::make_pair("output", "  name3"));
  result = b.get_named_blocks();
  CPPUNIT_ASSERT(result.size() == 2u);
  CPPUNIT_ASSERT(!result.at(0).first.compare("input"));
  CPPUNIT_ASSERT(!result.at(0).second.compare("  name1\n  name2"));
  CPPUNIT_ASSERT(!result.at(1).first.compare("output"));
  CPPUNIT_ASSERT(!result.at(1).second.compare("  name3"));
  b._named_blocks.clear();
  result = b.get_named_blocks();
  CPPUNIT_ASSERT(result.empty());
}
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
