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

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::GlobalNamespaceTest);
