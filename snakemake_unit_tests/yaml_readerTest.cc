/*!
  \file yaml_readerTest.cc
  \brief implementation of yaml reader unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/yaml_readerTest.h"

void snakemake_unit_tests::yaml_readerTest::setUp() {
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutYRTXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("yaml_readerTest mkdtemp failed");
  }

  /*
    create two dummy yaml files for testing purposes

    one of them actually needs features:

    the other can just be anything, as long as it's different
   */
  std::ofstream output;
  _yaml_file_1 = boost::filesystem::path(std::string(_tmp_dir)) / "file1.yaml";
  _yaml_file_2 = boost::filesystem::path(std::string(_tmp_dir)) / "file2.yaml";
  std::string f1 =
      "tag1: val1\n"
      "tag2:\n"
      "  - val21\n  - val22\n  - val23\n"
      "tag3: ~\n"
      "tag4:\n  tag5:\n    - val6\n"
      "tag7:\n  tag8: val8\n  tag9: val9\n"
      "tag10:\n  tag11:\n    tag12: val12\n";
  output.open(_yaml_file_1.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write first example yaml file");
  }
  output << f1;
  output.close();
  output.clear();
  output.open(_yaml_file_2.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write second example yaml file");
  }
  output << "tag1: val1\n";
  output.close();
  output.clear();
}

void snakemake_unit_tests::yaml_readerTest::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_default_constructor() { yaml_reader yr; }
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_copy_constructor() {
  yaml_reader yr1(_yaml_file_1.string());
  yaml_reader yr2(yr1);
  CPPUNIT_ASSERT(yr1 == yr2);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_string_constructor() {
  yaml_reader yr1(_yaml_file_1.string()), yr2;
  yr2._data = YAML::LoadFile(_yaml_file_1.string().c_str());
  CPPUNIT_ASSERT(yr1 == yr2);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_load_file() {
  yaml_reader yr1, yr2;
  yr1.load_file(_yaml_file_1.string());
  yr2._data = YAML::LoadFile(_yaml_file_1.string().c_str());
  CPPUNIT_ASSERT(yr1 == yr2);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_load_file_bad_file() {
  yaml_reader yr1;
  yr1.load_file((_yaml_file_1 / "fakename").string());
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_entry_query() {
  yaml_reader yr(_yaml_file_1.string());
  std::string query = "tag1";
  std::string result = yr.get_entry(query);
  CPPUNIT_ASSERT(!result.compare("val1"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_entry_queries() {
  yaml_reader yr(_yaml_file_1.string());
  std::vector<std::string> queries;
  queries.push_back("tag4");
  queries.push_back("tag5");
  std::string result = yr.get_entry(queries);
  CPPUNIT_ASSERT(!result.compare("val6"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_entry_multiple_values() {
  std::string result = "";
  yaml_reader yr(_yaml_file_1.string());
  result = yr.get_entry("tag2");
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_sequence_query() {
  std::vector<std::string> result;
  yaml_reader yr(_yaml_file_1.string());
  // query scalar
  result = yr.get_sequence("tag1");
  CPPUNIT_ASSERT(result.size() == 1);
  CPPUNIT_ASSERT(!result.at(0).compare("val1"));
  // query sequence
  result = yr.get_sequence("tag2");
  CPPUNIT_ASSERT(result.size() == 3);
  CPPUNIT_ASSERT(!result.at(0).compare("val21"));
  CPPUNIT_ASSERT(!result.at(1).compare("val22"));
  CPPUNIT_ASSERT(!result.at(2).compare("val23"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_sequence_queries() {
  std::vector<std::string> result, queries;
  yaml_reader yr(_yaml_file_1.string());
  queries.push_back("tag4");
  queries.push_back("tag5");
  result = yr.get_sequence(queries);
  CPPUNIT_ASSERT(result.size() == 1);
  CPPUNIT_ASSERT(!result.at(0).compare("val6"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_sequence_non_conformable() {
  std::vector<std::string> result, queries;
  yaml_reader yr(_yaml_file_1.string());
  queries.push_back("tag7");
  result = yr.get_sequence(queries);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_map_query() {
  std::vector<std::pair<std::string, std::string> > result;
  yaml_reader yr(_yaml_file_1.string());
  result = yr.get_map("tag7");
  CPPUNIT_ASSERT(result.size() == 2);
  CPPUNIT_ASSERT(!result.at(0).first.compare("tag8"));
  CPPUNIT_ASSERT(!result.at(0).second.compare("val8"));
  CPPUNIT_ASSERT(!result.at(1).first.compare("tag9"));
  CPPUNIT_ASSERT(!result.at(1).second.compare("val9"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_map_queries() {
  std::vector<std::pair<std::string, std::string> > result;
  std::vector<std::string> queries;
  queries.push_back("tag10");
  queries.push_back("tag11");
  yaml_reader yr(_yaml_file_1.string());
  result = yr.get_map(queries);
  CPPUNIT_ASSERT(result.size() == 1);
  CPPUNIT_ASSERT(!result.at(0).first.compare("tag12"));
  CPPUNIT_ASSERT(!result.at(0).second.compare("val12"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_map_non_conformable() {
  std::vector<std::string> queries;
  yaml_reader yr(_yaml_file_1.string());
  queries.push_back("tag2");
  std::vector<std::pair<std::string, std::string> > result = yr.get_map(queries);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_node_query() {
  yaml_reader yr(_yaml_file_1.string());
  YAML::Node result = yr._data["tag1"];
  YAML::Node n = yr.get_node("tag1");
  // since the nodes are copied, direct identity isn't on offer
  // so hack the equality operator
  yaml_reader yr1, yr2;
  yr1._data = result;
  yr2._data = n;
  CPPUNIT_ASSERT(yr1 == yr2);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_get_node_queries() {
  yaml_reader yr(_yaml_file_1.string());
  YAML::Node result = yr._data["tag4"]["tag5"];
  std::vector<std::string> queries;
  queries.push_back("tag4");
  queries.push_back("tag5");
  YAML::Node n = yr.get_node(queries);
  // since the nodes are copied, direct identity isn't on offer
  // so hack the equality operator
  yaml_reader yr1, yr2;
  yr1._data = result;
  yr2._data = n;
  CPPUNIT_ASSERT(yr1 == yr2);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_query_valid_query() {
  yaml_reader yr(_yaml_file_1.string());
  CPPUNIT_ASSERT(yr.query_valid("tag1"));
  CPPUNIT_ASSERT(!yr.query_valid("tag222"));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_query_valid_queries() {
  yaml_reader yr(_yaml_file_1.string());
  std::vector<std::string> queries;
  queries.push_back("tag4");
  queries.push_back("tag5");
  CPPUNIT_ASSERT(yr.query_valid(queries));
  queries.push_back("tag6");
  CPPUNIT_ASSERT(!yr.query_valid(queries));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_equality() {
  yaml_reader yr1(_yaml_file_1.string()), yr2(_yaml_file_2.string()), yr3(_yaml_file_1.string());
  CPPUNIT_ASSERT(yr1 == yr1);
  CPPUNIT_ASSERT(yr2 == yr2);
  CPPUNIT_ASSERT(yr1 == yr3);
  CPPUNIT_ASSERT(!(yr1 == yr2));
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries() {
  yaml_reader yr(_yaml_file_1.string());
  YAML::Node n = YAML::Clone(yr._data), next;
  YAML::Node result = n["tag4"]["tag5"];
  std::vector<std::string> queries;
  queries.push_back("tag4");
  queries.push_back("tag5");
  yr.apply_queries(queries, &n, &next);
  CPPUNIT_ASSERT(result == n);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries_null_pointer() {
  yaml_reader yr(_yaml_file_1.string());
  std::vector<std::string> queries;
  queries.push_back("tag1");
  yr.apply_queries(queries, NULL, NULL);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries_no_queries() {
  yaml_reader yr(_yaml_file_1.string());
  std::vector<std::string> queries;
  YAML::Node n = YAML::Clone(yr._data), next;
  yr.apply_queries(queries, &n, &next);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries_wrong_type() {
  yaml_reader yr(_yaml_file_2.string());
  std::vector<std::string> queries;
  queries.push_back("tag1");
  queries.push_back("tag2");
  YAML::Node n = YAML::Clone(yr._data), next;
  yr.apply_queries(queries, &n, &next);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries_query_absent() {
  yaml_reader yr(_yaml_file_1.string());
  YAML::Node n = YAML::Clone(yr._data), next;
  std::vector<std::string> queries;
  queries.push_back("tag222");
  yr.apply_queries(queries, &n, &next);
}
void snakemake_unit_tests::yaml_readerTest::test_yaml_reader_apply_queries_scalar_terminator() {
  yaml_reader yr(_yaml_file_1.string());
  YAML::Node n = YAML::Clone(yr._data), next;
  std::vector<std::string> queries;
  queries.push_back("tag1");
  queries.push_back("tag2");
  yr.apply_queries(queries, &n, &next);
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::yaml_readerTest);
