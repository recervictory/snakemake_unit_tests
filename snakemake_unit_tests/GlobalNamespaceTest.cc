/*!
  \file GlobalNamespaceTest.cc
  \brief implementation of global namespace unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/GlobalNamespaceTest.h"

void snakemake_unit_tests::GlobalNamespaceTest::setUp() {
  _test_map["a"] = true;
  _test_map["b"] = true;
  _test_vec.push_back("a");
  _test_vec.push_back("a");
  _test_vec.push_back("b");
}

void snakemake_unit_tests::GlobalNamespaceTest::tearDown() {
  // nothing to do at the moment
}

void snakemake_unit_tests::GlobalNamespaceTest::test_vector_to_map() {
  std::map<std::string, bool> observed;
  observed = vector_to_map<std::string>(_test_vec);
  CPPUNIT_ASSERT_MESSAGE("map contains at most as many entries as vector",
                         observed.size() <= _test_vec.size());
  std::map<std::string, bool>::iterator finder;
  std::map<std::string, bool> vector_entries_found;
  for (std::vector<std::string>::const_iterator iter = _test_vec.begin();
       iter != _test_vec.end(); ++iter) {
    CPPUNIT_ASSERT_MESSAGE("map contains every vector entry",
                           (finder = observed.find(*iter)) != observed.end());
    CPPUNIT_ASSERT_MESSAGE("vector_to_map sets placeholder booleans to true",
                           finder->second);
    vector_entries_found[*iter] = true;
  }
  CPPUNIT_ASSERT_MESSAGE("map contains only vector entries",
                         observed.size() == vector_entries_found.size());
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_remove_comments_and_docstrings() {
  std::vector<std::string> content;
  content.push_back("## here's a comment");
  content.push_back("");
  content.push_back("");
  content.push_back("");
  content.push_back("");
  content.push_back("\"\"\" here is some ");
  content.push_back("multiline content \"\"\"");
  content.push_back("def thing_to_do:   ");
  content.push_back("   var=\"\"\" string literal stored to variable \"\"\"");
  content.push_back("   return \"{}\".format(varname)");
  content.push_back("");
  content.push_back("rule myrule:\t");
  content.push_back("   \"\"\" this rule does something \"\"\"\"\"\"");
  content.push_back("  some extra stuff\"\"\"");
  content.push_back("   input: \"file.txt\",");
  content.push_back("   output: 'file2.txt',");
  content.push_back("   shell:");
  content.push_back(
      "    \"here is my command with an embedded #comment "
      "that should be maintained\"");
  content.push_back("");
  content.push_back("");
  unsigned counter = 0, starting_counter = 0;
  std::string input = "";
  std::string output = "";
  while (counter < content.size()) {
    if (counter == 12) {
      // this example is currently broken; skip
      counter = 14;
      continue;
    }
    starting_counter = counter;
    input = content.at(counter);
    output = remove_comments_and_docstrings(input, content, &counter);
    if (counter < 5 || counter == 10 || counter == 18 || counter == 19) {
      CPPUNIT_ASSERT_MESSAGE("return empty line", output.empty());
      CPPUNIT_ASSERT_MESSAGE("counter unchanged", counter == starting_counter);
    } else if (counter == 6 || counter == 13) {
      CPPUNIT_ASSERT_MESSAGE("return empty line", output.empty());
      CPPUNIT_ASSERT_MESSAGE("counter incremented",
                             counter == starting_counter + 1);
    } else if (counter == 7) {
      CPPUNIT_ASSERT_MESSAGE("trailing whitespace trimmed from df",
                             !output.compare("def thing_to_do:"));
      CPPUNIT_ASSERT_MESSAGE("counter unchanged", counter == starting_counter);

    } else if (counter == 8 || counter == 9 ||
               (counter >= 14 && counter <= 17)) {
      CPPUNIT_ASSERT_MESSAGE("line unchanged",
                             !output.compare(content.at(counter)));
      CPPUNIT_ASSERT_MESSAGE("counter unchanged", counter == starting_counter);
    } else if (counter == 11) {
      CPPUNIT_ASSERT_MESSAGE("strip trailing tab",
                             !output.compare("rule myrule:"));
      CPPUNIT_ASSERT_MESSAGE("counter unchanged", counter == starting_counter);
    } else if (counter == 5 || counter == 12) {
      CPPUNIT_FAIL("encountered returned line that should have been skipped");
    }
    ++counter;
  }
}

void snakemake_unit_tests::GlobalNamespaceTest::test_split_comma_list() {
  // test: null target pointer throws exception instead of segfault
  CPPUNIT_ASSERT_THROW(split_comma_list("", NULL), std::runtime_error);
  // test: empty string returns empty string
  std::vector<std::string> split_vec, expected_vec;
  expected_vec.push_back("");
  split_comma_list("", &split_vec);
  CPPUNIT_ASSERT_MESSAGE("split_comma_list: empty string returns empty string",
                         split_vec == expected_vec);
  // test: single entry with no commas returns single entry in vector
  expected_vec.clear();
  expected_vec.push_back("aaa");
  split_comma_list("aaa", &split_vec);
  CPPUNIT_ASSERT_MESSAGE(
      "split_comma_list: single entry with no commas returns single entry in "
      "vector",
      split_vec == expected_vec);
  // test: double entry with single comma splits into two tokens
  expected_vec.clear();
  expected_vec.push_back("abc");
  expected_vec.push_back("def");
  split_comma_list("abc, def", &split_vec);
  CPPUNIT_ASSERT_MESSAGE(
      "split_comma_list: double entry with single comma splits into two tokens",
      split_vec == expected_vec);
  // test: triple entry with two commas splits into three tokens
  expected_vec.clear();
  expected_vec.push_back("1");
  expected_vec.push_back("22");
  expected_vec.push_back("333");
  split_comma_list("1, 22, 333", &split_vec);
  CPPUNIT_ASSERT_MESSAGE(
      "split_comma_list: triple entry with two commas splits into three tokens",
      split_vec == expected_vec);
  // test: a bunch of consecutive commas returns empty strings
  expected_vec.clear();
  expected_vec.push_back("");
  expected_vec.push_back("");
  expected_vec.push_back("");
  expected_vec.push_back("");
  expected_vec.push_back("");
  split_comma_list(", , , , ", &split_vec);
  CPPUNIT_ASSERT_MESSAGE(
      "split_comma_list: multiple consecutive commas returns empty strings",
      split_vec == expected_vec);
  // test: function distinguishes between commas with trailing whitespace and
  // those without
  expected_vec.clear();
  expected_vec.push_back("aaa");
  expected_vec.push_back("bbb,ccc");
  expected_vec.push_back("ddd");
  split_comma_list("aaa, bbb,ccc, ddd", &split_vec);
  CPPUNIT_ASSERT_MESSAGE(
      "split_comma_list: distinguishes between commas with trailing whitespace "
      "and those without",
      split_vec == expected_vec);
  // test: combined test
  expected_vec.clear();
  expected_vec.push_back("a1");
  expected_vec.push_back("");
  expected_vec.push_back("2");
  expected_vec.push_back("3");
  expected_vec.push_back("44,44");
  expected_vec.push_back("");
  split_comma_list("a1, , 2, 3, 44,44, ", &split_vec);
  CPPUNIT_ASSERT_MESSAGE("split_comma_list: combined test",
                         split_vec == expected_vec);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_concatenate_string_literals() {
  std::string resolved_line = "";
  std::string aggregated_line = "";
  std::vector<std::string> results;

  // single line added to empty vector with no aggregated content
  resolved_line = " here is my line";
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 1);
  CPPUNIT_ASSERT(!results.at(0).compare(" here is my line"));
  // single line added to empty vector with aggregated content
  resolved_line = " here is my line";
  aggregated_line = " stuff at the beginning";
  results.clear();
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 1);
  CPPUNIT_ASSERT(
      !results.at(0).compare(" stuff at the beginning here is my line"));
  // single line added to full vector with no merge required and no aggregated
  // content
  resolved_line = " no merge here";
  results.clear();
  results.push_back("stuff that's already' present");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("stuff that's already' present"));
  CPPUNIT_ASSERT(!results.at(1).compare(" no merge here"));
  // single line added to full vector with no merge required and aggregated
  // content
  resolved_line = " no merge here";
  aggregated_line = " why was this here before";
  results.clear();
  results.push_back("starting content");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("starting content"));
  CPPUNIT_ASSERT(
      !results.at(1).compare(" why was this here before no merge here"));
  // vector ends in ", newline starts in '
  // single line added to full vector with merge required and no aggregated
  // content
  resolved_line = "'here is a thing'";
  results.clear();
  results.push_back("thing1");
  results.push_back("\"thing 2\"");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(!results.at(1).compare("\"thing 2\"\n'here is a thing'"));
  // single line added to full vector with merge required and aggregated content
  resolved_line = "here is a thing";
  aggregated_line = "'here is a thing'";
  results.clear();
  results.push_back("thing1");
  results.push_back("\"thing 2\"");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(
      !results.at(1).compare("\"thing 2\"\n'here is a thing'here is a thing"));
  // vector ends in ", newline starts in "
  // single line added to full vector with merge required and no aggregated
  // content
  resolved_line = "\"here is a thing\"";
  results.clear();
  results.push_back("thing1");
  results.push_back("\"thing 2\"");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(!results.at(1).compare("\"thing 2\"\n\"here is a thing\""));
  // single line added to full vector with merge required and aggregated content
  resolved_line = "here is a thing";
  aggregated_line = "\"here is a thing\"";
  results.clear();
  results.push_back("thing1");
  results.push_back("\"thing 2\"");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(!results.at(1).compare(
      "\"thing 2\"\n\"here is a thing\"here is a thing"));
  // vector ends in ', newline starts in "
  // single line added to full vector with merge required and no aggregated
  // content
  resolved_line = "\"here is a thing\"";
  results.clear();
  results.push_back("thing1");
  results.push_back("'thing 2'");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(!results.at(1).compare("'thing 2'\n\"here is a thing\""));
  // single line added to full vector with merge required and aggregated content
  resolved_line = "here is a thing";
  aggregated_line = "\"here is a thing\"";
  results.clear();
  results.push_back("thing1");
  results.push_back("'thing 2'");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(
      !results.at(1).compare("'thing 2'\n\"here is a thing\"here is a thing"));
  // vector ends in ', newline starts in '
  // single line added to full vector with merge required and no aggregated
  // content
  resolved_line = "'here is a thing'";
  results.clear();
  results.push_back("thing1");
  results.push_back("'thing 2'");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(!results.at(1).compare("'thing 2'\n'here is a thing'"));
  // single line added to full vector with merge required and aggregated content
  resolved_line = "here is a thing";
  aggregated_line = "'here is a thing'";
  results.clear();
  results.push_back("thing1");
  results.push_back("'thing 2'");
  concatenate_string_literals(resolved_line, &aggregated_line, &results);
  CPPUNIT_ASSERT(aggregated_line.empty());
  CPPUNIT_ASSERT(results.size() == 2);
  CPPUNIT_ASSERT(!results.at(0).compare("thing1"));
  CPPUNIT_ASSERT(
      !results.at(1).compare("'thing 2'\n'here is a thing'here is a thing"));
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_concatenate_string_literals_null_arg1() {
  std::string resolved_line = "";
  std::vector<std::string> results;
  concatenate_string_literals(resolved_line, NULL, &results);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_concatenate_string_literals_null_arg2() {
  std::string resolved_line = "", aggregated_line = "";
  concatenate_string_literals(resolved_line, &aggregated_line, NULL);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter() {
  quote_type active_quote_type = none;
  unsigned parse_index = 0;
  bool string_open = false;
  bool literal_open = false;
  std::string example_simple_triple_quote =
      "here is \"\"\"a simple literal\"\"\"";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_simple_triple_quote, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 27;
  resolve_string_delimiter(example_simple_triple_quote, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 30);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_simple_triple_tick = "here is '''a simple literal'''";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_simple_triple_tick, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 27;
  resolve_string_delimiter(example_simple_triple_tick, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 30);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_simple_single_quote = "here is \"a quoted thing\"";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_simple_single_quote, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(active_quote_type == single_quote);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  parse_index = 23;
  resolve_string_delimiter(example_simple_single_quote, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 24);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_simple_single_tick = "here is 'a quoted thing'";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_simple_single_tick, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(active_quote_type == single_tick);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  parse_index = 23;
  resolve_string_delimiter(example_simple_single_tick, &active_quote_type,
                           &parse_index, &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 24);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_embedded1 =
      "here is \"\"\"something that should 'be skipped'\"\"\"";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_embedded1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 33;
  resolve_string_delimiter(example_embedded1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 34);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 44;
  resolve_string_delimiter(example_embedded1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 45);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  resolve_string_delimiter(example_embedded1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 48);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_embedded2 =
      "here is '''something that should \"be skipped\"'''";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_embedded2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 33;
  resolve_string_delimiter(example_embedded2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 34);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 44;
  resolve_string_delimiter(example_embedded2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 45);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  resolve_string_delimiter(example_embedded2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 48);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_embedded3 =
      "here is '''something that should 'be skipped'''";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_embedded3, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 33;
  resolve_string_delimiter(example_embedded3, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 34);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 44;
  resolve_string_delimiter(example_embedded3, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 47);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_embedded4 =
      "here is \"\"\"something that should \"be skipped\"\"\"";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_embedded4, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 11);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 33;
  resolve_string_delimiter(example_embedded4, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 34);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 44;
  resolve_string_delimiter(example_embedded4, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 47);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_greedy1 = "here is 'a strange example'''";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_greedy1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(active_quote_type == single_tick);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  parse_index = 26;
  resolve_string_delimiter(example_greedy1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 27);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  resolve_string_delimiter(example_greedy1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 28);
  CPPUNIT_ASSERT(active_quote_type == single_tick);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  resolve_string_delimiter(example_greedy1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 29);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string example_greedy2 = "here is \"a strange example\"\"\"";
  parse_index = 8;
  string_open = literal_open = false;
  active_quote_type = none;
  resolve_string_delimiter(example_greedy2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(active_quote_type == single_quote);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  parse_index = 26;
  resolve_string_delimiter(example_greedy2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 27);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  resolve_string_delimiter(example_greedy2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 28);
  CPPUNIT_ASSERT(active_quote_type == single_quote);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  resolve_string_delimiter(example_greedy2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 29);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape1 = "ignore \\\"escaped marks";
  parse_index = 8;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape1, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(active_quote_type == none);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape2 = "ignore \\'escaped marks";
  parse_index = 8;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape2, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 9);
  CPPUNIT_ASSERT(active_quote_type == none);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape3 = "accept \\\\\"valid marks";
  parse_index = 9;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape3, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 10);
  CPPUNIT_ASSERT(active_quote_type == single_quote);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape4 = "accept \\\\'valid marks";
  parse_index = 9;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape4, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 10);
  CPPUNIT_ASSERT(active_quote_type == single_tick);
  CPPUNIT_ASSERT(string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape5 = "\"\"\" keep going \\\\\"\\\\\"\"\"";
  parse_index = 0;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape5, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 3);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 17;
  resolve_string_delimiter(escape5, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 18);
  CPPUNIT_ASSERT(active_quote_type == triple_quote);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 20;
  resolve_string_delimiter(escape5, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 23);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
  std::string escape6 = "''' keep going \\\\'\\\\'''";
  parse_index = 0;
  active_quote_type = none;
  string_open = literal_open = false;
  resolve_string_delimiter(escape6, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 3);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 17;
  resolve_string_delimiter(escape6, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 18);
  CPPUNIT_ASSERT(active_quote_type == triple_tick);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(literal_open);
  parse_index = 20;
  resolve_string_delimiter(escape6, &active_quote_type, &parse_index,
                           &string_open, &literal_open);
  CPPUNIT_ASSERT(parse_index == 23);
  CPPUNIT_ASSERT(!string_open);
  CPPUNIT_ASSERT(!literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_null_arg1() {
  std::string line = "";
  unsigned index = 0;
  bool string_open = false, literal_open = false;
  resolve_string_delimiter(line, NULL, &index, &string_open, &literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_null_arg2() {
  std::string line = "";
  quote_type qt = none;
  bool string_open = false, literal_open = false;
  resolve_string_delimiter(line, &qt, NULL, &string_open, &literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_null_arg3() {
  std::string line = "";
  quote_type qt = none;
  unsigned index = 0;
  bool literal_open = false;
  resolve_string_delimiter(line, &qt, &index, NULL, &literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_null_arg4() {
  std::string line = "";
  quote_type qt = none;
  unsigned index = 0;
  bool string_open = false;
  resolve_string_delimiter(line, &qt, &index, &string_open, NULL);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_index_oob() {
  std::string line = "here is a line";
  quote_type qt = none;
  unsigned index = 100;
  bool string_open = false, literal_open = false;
  resolve_string_delimiter(line, &qt, &index, &string_open, &literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_resolve_string_delimiter_index_not_mark() {
  std::string line = "here is a line";
  quote_type qt = none;
  unsigned index = 0;
  bool string_open = false, literal_open = false;
  resolve_string_delimiter(line, &qt, &index, &string_open, &literal_open);
}

void snakemake_unit_tests::GlobalNamespaceTest::test_lexical_parse() {
  std::vector<std::string> input, output, expected;
  input.push_back("   standard lines are preserved");
  input.push_back("   comments are pruned # like me");
  input.push_back("   quotes in comments are irrelevant # like this '");
  input.push_back(
      "   trailing slash in comments are irrelevant # like this \\");
  input.push_back("   trailing slashes merge lines \\");
  input.push_back(" with their following line");
  input.push_back("example: \"quotes on the same line are preserved\"");
  input.push_back(
      "example: \"comment characters within quotes like # are preserved\"");
  input.push_back("example: \"\"\" literals crossing lines are ");
  input.push_back("       merged together sensibly \"\"\"");
  input.push_back(
      "example: \"\"\" multiple literals on the same \"\"\" line are \"\"\"");
  input.push_back(" handled identically \"\"\"");
  input.push_back("example: \"aggregated strings are glued together\"  ");
  input.push_back(" 'in a way that preserves python syntax' ");

  expected.push_back("   standard lines are preserved");
  expected.push_back("   comments are pruned ");
  expected.push_back("   quotes in comments are irrelevant ");
  expected.push_back("   trailing slash in comments are irrelevant ");
  expected.push_back(
      "   trailing slashes merge lines  with their following line");
  expected.push_back("example: \"quotes on the same line are preserved\"");
  expected.push_back(
      "example: \"comment characters within quotes like # are preserved\"");
  expected.push_back(
      "example: \"\"\" literals crossing lines are        merged together "
      "sensibly \"\"\"");
  expected.push_back(
      "example: \"\"\" multiple literals on the same \"\"\" line are \"\"\" "
      "handled identically \"\"\"");
  expected.push_back(
      "example: \"aggregated strings are glued together\"  \n 'in a way that "
      "preserves python syntax' ");

  output = lexical_parse(input);

  CPPUNIT_ASSERT(output.size() == expected.size());
  for (unsigned i = 0; i < output.size(); ++i) {
    CPPUNIT_ASSERT(!output.at(i).compare(expected.at(i)));
  }
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::GlobalNamespaceTest);
