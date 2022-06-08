/*!
  \file rule_blockTest.cc
  \brief implementation of rule block unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/rule_blockTest.h"

void snakemake_unit_tests::rule_blockTest::setUp() {
  _snakefile_lines.push_back("rule myrule:");
  _snakefile_lines.push_back("    ''' here is my docstring '''");
  _snakefile_lines.push_back("    input:");
  _snakefile_lines.push_back("       'input1.txt',");
  _snakefile_lines.push_back("       'input2.txt',");
  _snakefile_lines.push_back("    output:");
  _snakefile_lines.push_back("       'output1.txt',");
  _snakefile_lines.push_back("    shell:");
  _snakefile_lines.push_back("       'echo \"do the thing\" > {output}");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("use rule myrule as myderivedrule with:");
  _snakefile_lines.push_back("    output:");
  _snakefile_lines.push_back("       'output2.txt',");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("localrules: mylocalrule");
  _snakefile_lines.push_back("if True:");
  _snakefile_lines.push_back("    checkpoint mycheckpoint:");
  _snakefile_lines.push_back("        ''' here's another docstring\nwith multiple lines '''");
  _snakefile_lines.push_back("        input: 'input3.txt',");
  _snakefile_lines.push_back("        output: 'output3.txt',");
  _snakefile_lines.push_back("        shell: 'touch {output}'");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("");
  _snakefile_lines.push_back("configfile: \"myconfigfile.yaml\"");
}

void snakemake_unit_tests::rule_blockTest::tearDown() {}

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
void snakemake_unit_tests::rule_blockTest::test_rule_block_load_content_block() {
  rule_block standard_rule, derived_rule, localrules, python_if, checkpoint, configfile;
  unsigned current_line = 0u;
  standard_rule.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(!standard_rule._rule_name.compare("myrule"));
  CPPUNIT_ASSERT(standard_rule._base_rule_name.empty());
  CPPUNIT_ASSERT(!standard_rule._rule_is_checkpoint);
  CPPUNIT_ASSERT(!standard_rule._docstring.compare("    ''' here is my docstring '''"));
  CPPUNIT_ASSERT(standard_rule._named_blocks.size() == 3u);
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(0).first.compare("input"));
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(0).second.compare("\n       'input1.txt',\n       'input2.txt',"));
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(1).first.compare("output"));
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(1).second.compare("\n       'output1.txt',"));
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(2).first.compare("shell"));
  CPPUNIT_ASSERT(!standard_rule._named_blocks.at(2).second.compare("\n       'echo \"do the thing\" > {output}"));
  CPPUNIT_ASSERT(standard_rule._code_chunk.empty());
  CPPUNIT_ASSERT_EQUAL(0u, standard_rule._local_indentation);
  CPPUNIT_ASSERT(standard_rule._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!standard_rule._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, standard_rule._python_tag);
  CPPUNIT_ASSERT(standard_rule._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(11u, current_line);
  derived_rule.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(!derived_rule._rule_name.compare("myderivedrule"));
  CPPUNIT_ASSERT(!derived_rule._base_rule_name.compare("myrule"));
  CPPUNIT_ASSERT(!derived_rule._rule_is_checkpoint);
  CPPUNIT_ASSERT(derived_rule._docstring.empty());
  CPPUNIT_ASSERT(derived_rule._named_blocks.size() == 1u);
  CPPUNIT_ASSERT(!derived_rule._named_blocks.at(0).first.compare("output"));
  CPPUNIT_ASSERT(!derived_rule._named_blocks.at(0).second.compare("\n       'output2.txt',"));
  CPPUNIT_ASSERT(derived_rule._code_chunk.empty());
  CPPUNIT_ASSERT_EQUAL(0u, derived_rule._local_indentation);
  CPPUNIT_ASSERT(derived_rule._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!derived_rule._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, derived_rule._python_tag);
  CPPUNIT_ASSERT(derived_rule._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(16u, current_line);
  localrules.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(localrules._rule_name.empty());
  CPPUNIT_ASSERT(localrules._base_rule_name.empty());
  CPPUNIT_ASSERT(!localrules._rule_is_checkpoint);
  CPPUNIT_ASSERT(localrules._docstring.empty());
  CPPUNIT_ASSERT(localrules._named_blocks.empty());
  CPPUNIT_ASSERT(localrules._code_chunk.size() == 1u);
  CPPUNIT_ASSERT(!localrules._code_chunk.at(0).compare("localrules: mylocalrule"));
  CPPUNIT_ASSERT_EQUAL(0u, localrules._local_indentation);
  CPPUNIT_ASSERT(localrules._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!localrules._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, localrules._python_tag);
  CPPUNIT_ASSERT(localrules._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(17u, current_line);
  python_if.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(python_if._rule_name.empty());
  CPPUNIT_ASSERT(python_if._base_rule_name.empty());
  CPPUNIT_ASSERT(!python_if._rule_is_checkpoint);
  CPPUNIT_ASSERT(python_if._docstring.empty());
  CPPUNIT_ASSERT(python_if._named_blocks.empty());
  CPPUNIT_ASSERT(python_if._code_chunk.size() == 1u);
  CPPUNIT_ASSERT(!python_if._code_chunk.at(0).compare("if True:"));
  CPPUNIT_ASSERT_EQUAL(0u, python_if._local_indentation);
  CPPUNIT_ASSERT(python_if._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!python_if._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, python_if._python_tag);
  CPPUNIT_ASSERT(python_if._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(18u, current_line);
  checkpoint.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(!checkpoint._rule_name.compare("mycheckpoint"));
  CPPUNIT_ASSERT(checkpoint._base_rule_name.empty());
  CPPUNIT_ASSERT(checkpoint._rule_is_checkpoint);
  CPPUNIT_ASSERT(!checkpoint._docstring.compare("        ''' here's another docstring\nwith multiple lines '''"));
  CPPUNIT_ASSERT(checkpoint._named_blocks.size() == 3u);
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(0).first.compare("input"));
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(0).second.compare(" 'input3.txt',"));
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(1).first.compare("output"));
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(1).second.compare(" 'output3.txt',"));
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(2).first.compare("shell"));
  CPPUNIT_ASSERT(!checkpoint._named_blocks.at(2).second.compare(" 'touch {output}'"));
  CPPUNIT_ASSERT(checkpoint._code_chunk.empty());
  CPPUNIT_ASSERT_EQUAL(4u, checkpoint._local_indentation);
  CPPUNIT_ASSERT(checkpoint._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!checkpoint._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, checkpoint._python_tag);
  CPPUNIT_ASSERT(checkpoint._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(25u, current_line);
  configfile.load_content_block(_snakefile_lines, false, &current_line);
  CPPUNIT_ASSERT(configfile._rule_name.empty());
  CPPUNIT_ASSERT(configfile._base_rule_name.empty());
  CPPUNIT_ASSERT(!configfile._rule_is_checkpoint);
  CPPUNIT_ASSERT(configfile._docstring.empty());
  CPPUNIT_ASSERT(configfile._named_blocks.empty());
  CPPUNIT_ASSERT(configfile._code_chunk.size() == 1);
  CPPUNIT_ASSERT(!configfile._code_chunk.at(0).compare("configfile: \"myconfigfile.yaml\""));
  CPPUNIT_ASSERT_EQUAL(0u, configfile._local_indentation);
  CPPUNIT_ASSERT(configfile._resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!configfile._queried_by_python);
  CPPUNIT_ASSERT_EQUAL(0u, configfile._python_tag);
  CPPUNIT_ASSERT(configfile._resolved_included_filename.string().empty());
  CPPUNIT_ASSERT_EQUAL(26u, current_line);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_load_content_block_null_pointer() {
  rule_block b;
  b.load_content_block(_snakefile_lines, false, NULL);
}
// consume_rule_contents is only ever called from within load_content_block, so for
// simplicity let's assume that it will be covered from within the tests for the calling function
void snakemake_unit_tests::rule_blockTest::test_rule_block_consume_rule_contents_null_pointer() {
  rule_block b;
  b.consume_rule_contents(_snakefile_lines, false, NULL);
}
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
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_local_indentation() {
  rule_block b;
  b._local_indentation = 4u;
  CPPUNIT_ASSERT_EQUAL(4u, b.get_local_indentation());
  b._local_indentation = 0u;
  CPPUNIT_ASSERT_EQUAL(0u, b.get_local_indentation());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_add_code_chunk() {
  rule_block b;
  b.add_code_chunk("  thing1;\n");
  CPPUNIT_ASSERT(b._code_chunk.size() == 1);
  CPPUNIT_ASSERT(!b._code_chunk.at(0).compare("  thing1;\n"));
  b.add_code_chunk("  thing2;\n");
  CPPUNIT_ASSERT(b._code_chunk.size() == 2);
  CPPUNIT_ASSERT(!b._code_chunk.at(0).compare("  thing1;\n"));
  CPPUNIT_ASSERT(!b._code_chunk.at(1).compare("  thing2;\n"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_equality_operator() {
  rule_block b1, b2;
  CPPUNIT_ASSERT(b1 == b2);
  CPPUNIT_ASSERT(b1 == b1);
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
  CPPUNIT_ASSERT(!(b1 == b2));
  b2._rule_name = "rulename";
  b2._base_rule_name = "baserulename";
  b2._rule_is_checkpoint = true;
  b2._docstring = "text goes here";
  b2._named_blocks.push_back(std::make_pair("thing1", "thing2"));
  b2._named_blocks.push_back(std::make_pair("thing2", "thing3"));
  b2._code_chunk.push_back("code goes here;");
  b2._code_chunk.push_back("{some other thing;}");
  b2._local_indentation = 42;
  b2._resolution = RESOLVED_INCLUDED;
  b2._queried_by_python = true;
  b2._python_tag = 333;
  b2._resolved_included_filename = "thing1/thing2/thing3";
  CPPUNIT_ASSERT(b1 == b2);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_inequality_operator() {
  rule_block b1, b2;
  CPPUNIT_ASSERT(!(b1 != b2));
  CPPUNIT_ASSERT(!(b1 != b1));
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
  CPPUNIT_ASSERT(b1 != b2);
  b2._rule_name = "rulename";
  b2._base_rule_name = "baserulename";
  b2._rule_is_checkpoint = true;
  b2._docstring = "text goes here";
  b2._named_blocks.push_back(std::make_pair("thing1", "thing2"));
  b2._named_blocks.push_back(std::make_pair("thing2", "thing3"));
  b2._code_chunk.push_back("code goes here;");
  b2._code_chunk.push_back("{some other thing;}");
  b2._local_indentation = 42;
  b2._resolution = RESOLVED_INCLUDED;
  b2._queried_by_python = true;
  b2._python_tag = 333;
  b2._resolved_included_filename = "thing1/thing2/thing3";
  CPPUNIT_ASSERT(!(b1 != b2));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_resolved() {
  rule_block b;
  CPPUNIT_ASSERT(!b.resolved());
  b._resolution = RESOLVED_INCLUDED;
  CPPUNIT_ASSERT(!b.resolved());
  b._resolution = RESOLVED_EXCLUDED;
  CPPUNIT_ASSERT(!b.resolved());
  b._queried_by_python = true;
  CPPUNIT_ASSERT(b.resolved());
  b._resolution = RESOLVED_INCLUDED;
  CPPUNIT_ASSERT(b.resolved());
  b._resolution = UNRESOLVED;
  CPPUNIT_ASSERT(!b.resolved());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_included() {
  rule_block b;
  CPPUNIT_ASSERT(!b.included());
  b._resolution = RESOLVED_EXCLUDED;
  CPPUNIT_ASSERT(!b.included());
  b._resolution = RESOLVED_INCLUDED;
  CPPUNIT_ASSERT(b.included());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_resolution() {
  rule_block b;
  b.set_resolution(RESOLVED_INCLUDED);
  CPPUNIT_ASSERT(b._resolution == RESOLVED_INCLUDED);
  b.set_resolution(RESOLVED_EXCLUDED);
  CPPUNIT_ASSERT(b._resolution == RESOLVED_EXCLUDED);
  b.set_resolution(UNRESOLVED);
  CPPUNIT_ASSERT(b._resolution == UNRESOLVED);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_resolution_status() {
  rule_block b;
  CPPUNIT_ASSERT(b.get_resolution_status() == UNRESOLVED);
  b._resolution = RESOLVED_EXCLUDED;
  CPPUNIT_ASSERT(b.get_resolution_status() == RESOLVED_EXCLUDED);
  b._resolution = RESOLVED_INCLUDED;
  CPPUNIT_ASSERT(b.get_resolution_status() == RESOLVED_INCLUDED);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_interpreter_tag() {
  rule_block b;
  b.set_interpreter_tag(10u);
  CPPUNIT_ASSERT_EQUAL(10u, b._python_tag);
  b.set_interpreter_tag(20u);
  CPPUNIT_ASSERT_EQUAL(20u, b._python_tag);
  b.set_interpreter_tag(0u);
  CPPUNIT_ASSERT_EQUAL(0u, b._python_tag);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_interpreter_tag() {
  rule_block b;
  CPPUNIT_ASSERT_EQUAL(0u, b.get_interpreter_tag());
  b._python_tag = 10u;
  CPPUNIT_ASSERT_EQUAL(10u, b.get_interpreter_tag());
  b._python_tag = 20u;
  CPPUNIT_ASSERT_EQUAL(20u, b.get_interpreter_tag());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_report_python_logging_code() {
  /*
    the logic for determining which type of information is emitted
    follows similar logic as in print_contents. only major difference
    is that the presence of an include statement causes different behaviors
    for a code chunk.
   */
  rule_block b;
  // test logical order of determining what to report
  b._code_chunk.push_back("code chunk line 1");
  b._code_chunk.push_back("   code chunk line 2");
  b._rule_name = "myrulename";
  // base rule name doesn't control anything, unlike in print_contents
  b._named_blocks.push_back(std::make_pair("input", "\n          'filename1',\n          'filename2',"));
  b._named_blocks.push_back(std::make_pair("output", "\n          'filename3',"));
  b._named_blocks.push_back(std::make_pair("shell", "\n          'cat {input} > {output}'"));
  b._local_indentation = 2u;
  b._python_tag = 22u;
  std::ostringstream o1, o2, o3, o4, o5;
  std::string expected = "";
  CPPUNIT_ASSERT(!b.report_python_logging_code(o1));
  CPPUNIT_ASSERT(b._queried_by_python);
  expected = "code chunk line 1\n   code chunk line 2\n";
  CPPUNIT_ASSERT(!o1.str().compare(expected));
  b._code_chunk.clear();
  b._code_chunk.push_back("    include: \"myname.smk\"");
  CPPUNIT_ASSERT(b.report_python_logging_code(o2));
  expected = "    print(\"tag22: {}\".format(\"myname.smk\"))\n";
  CPPUNIT_ASSERT(!o2.str().compare(expected));
  b._resolution = RESOLVED_INCLUDED;
  CPPUNIT_ASSERT(!b.report_python_logging_code(o3));
  expected = "    include: \"myname.smk\"\n    print(\"tag22: {}\".format(\"myname.smk\"))\n";
  CPPUNIT_ASSERT(!o3.str().compare(expected));
  b._code_chunk.clear();
  CPPUNIT_ASSERT(!b.report_python_logging_code(o4));
  expected = "  print(\"tag22\")\n\n\n";
  CPPUNIT_ASSERT(!o4.str().compare(expected));
  b._rule_name = "";
  CPPUNIT_ASSERT(!b.report_python_logging_code(o5));
  expected =
      "  input:\n          'filename1',\n          'filename2',\n"
      "  output:\n          'filename3',\n"
      "  shell:\n          'cat {input} > {output}'\n";
  CPPUNIT_ASSERT(!o5.str().compare(expected));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_update_resolution() {
  /*
    rule_block objects scan the results of a python logging pass and
    look to see if there's any informative information in the results
    regarding their unique python tag. code elements that don't include
    interesting things (snakemake rules and include directives) have tag
    identity 0 and are always flagged as included after the first python pass.
   */
  std::map<std::string, std::string> tag_results;
  tag_results["tag1"] = "filename1";
  tag_results["tag2"] = "filename2";
  tag_results["tag3"] = "";
  rule_block b;
  b._python_tag = 1u;
  CPPUNIT_ASSERT(!b.update_resolution(tag_results));
  CPPUNIT_ASSERT(!b._resolved_included_filename.compare("filename1"));
  CPPUNIT_ASSERT(b._resolution == RESOLVED_INCLUDED);
  CPPUNIT_ASSERT(b.update_resolution(tag_results));
  CPPUNIT_ASSERT(!b._resolved_included_filename.compare("filename1"));
  CPPUNIT_ASSERT(b._resolution == RESOLVED_INCLUDED);
  b._python_tag = 2u;
  CPPUNIT_ASSERT(!b.update_resolution(tag_results));
  CPPUNIT_ASSERT(!b._resolved_included_filename.compare("filename2"));
  CPPUNIT_ASSERT(b._resolution == RESOLVED_INCLUDED);
  b._python_tag = 3u;
  CPPUNIT_ASSERT(b.update_resolution(tag_results));
  CPPUNIT_ASSERT(b._resolution == RESOLVED_INCLUDED);
  b._python_tag = 4u;
  CPPUNIT_ASSERT(b.update_resolution(tag_results));
  CPPUNIT_ASSERT(b._resolution == RESOLVED_EXCLUDED);
  b._python_tag = 0u;
  CPPUNIT_ASSERT(b.update_resolution(tag_results));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_get_resolved_included_filename() {
  rule_block b;
  CPPUNIT_ASSERT(b.get_resolved_included_filename().empty());
  b._resolved_included_filename = "filename1";
  CPPUNIT_ASSERT(!b.get_resolved_included_filename().compare("filename1"));
  b._resolved_included_filename = "filename2";
  CPPUNIT_ASSERT(!b.get_resolved_included_filename().compare("filename2"));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_is_checkpoint() {
  rule_block b;
  CPPUNIT_ASSERT(!b.is_checkpoint());
  b._rule_is_checkpoint = true;
  CPPUNIT_ASSERT(b.is_checkpoint());
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_set_checkpoint() {
  rule_block b;
  b.set_checkpoint(true);
  CPPUNIT_ASSERT(b._rule_is_checkpoint);
  b.set_checkpoint(false);
  CPPUNIT_ASSERT(!b._rule_is_checkpoint);
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_indentation() {
  rule_block b;
  CPPUNIT_ASSERT(!b.indentation(10u).compare("          "));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_apply_indentation() {
  rule_block b;
  std::string test_str = "here is some content:\n  there's some stuff here\n    ok?";
  std::string result = b.apply_indentation(test_str, 5);
  std::string expected = "here is some content:\n       there's some stuff here\n         ok?";
  CPPUNIT_ASSERT(!result.compare(expected));
}
void snakemake_unit_tests::rule_blockTest::test_rule_block_clear() {
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
  rule_block b2;
  b1.clear();
  CPPUNIT_ASSERT(b1 == b2);
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::rule_blockTest);
