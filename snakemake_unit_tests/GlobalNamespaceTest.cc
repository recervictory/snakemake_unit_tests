/*!
  \file GlobalNamespaceTest.cc
  \brief implementation of global namespace unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/GlobalNamespaceTest.h"

void snakemake_unit_tests::GlobalNamespaceTest::setUp() {
  _test_map["a"] = true;
  _test_vec.push_back("a");
}

void snakemake_unit_tests::GlobalNamespaceTest::tearDown() {
  // nothing to do at the moment
}

void snakemake_unit_tests::GlobalNamespaceTest::test_vector_to_map() {
  CPPUNIT_FAIL("test_vector_to_map not implemented");
}

void snakemake_unit_tests::GlobalNamespaceTest::
    test_remove_comments_and_docstrings() {
  CPPUNIT_FAIL("test_remove_comments_and_docstrings not implemented");
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
