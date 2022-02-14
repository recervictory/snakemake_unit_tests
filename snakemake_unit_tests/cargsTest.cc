/*!
  \file cargsTest.cc
  \brief implementation of command line parsing unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/cargsTest.h"

void snakemake_unit_tests::cargsTest::setUp() {}

void snakemake_unit_tests::cargsTest::tearDown() {
  // nothing to do at the moment
}

void snakemake_unit_tests::cargsTest::test_params_default_constructor() {
  params p;
  CPPUNIT_ASSERT(!p.verbose);
  CPPUNIT_ASSERT(!p.update_all);
  CPPUNIT_ASSERT(!p.update_snakefiles);
  CPPUNIT_ASSERT(!p.update_added_content);
  CPPUNIT_ASSERT(!p.update_config);
  CPPUNIT_ASSERT(!p.update_inputs);
  CPPUNIT_ASSERT(!p.update_outputs);
  CPPUNIT_ASSERT(!p.update_pytest);
  CPPUNIT_ASSERT(!p.include_entire_dag);
  CPPUNIT_ASSERT(p.config_filename.string().empty());
  CPPUNIT_ASSERT(p.config == yaml_reader());
  CPPUNIT_ASSERT(p.output_test_dir.string().empty());
  CPPUNIT_ASSERT(p.snakefile.string().empty());
  CPPUNIT_ASSERT(p.pipeline_top_dir.string().empty());
  CPPUNIT_ASSERT(p.pipeline_run_dir.string().empty());
  CPPUNIT_ASSERT(p.inst_dir.string().empty());
  CPPUNIT_ASSERT(p.snakemake_log.string().empty());
  CPPUNIT_ASSERT(p.added_files.empty());
  CPPUNIT_ASSERT(p.added_directories.empty());
  CPPUNIT_ASSERT(p.exclude_rules.empty());
  CPPUNIT_ASSERT(p.exclude_extensions.empty());
  CPPUNIT_ASSERT(p.exclude_paths.empty());
  CPPUNIT_ASSERT(p.byte_comparisons.empty());
}

void snakemake_unit_tests::cargsTest::test_params_copy_constructor() {
  params p;
  p.verbose = p.update_all = p.update_snakefiles = p.update_added_content = true;
  p.update_config = p.update_inputs = p.update_outputs = p.update_pytest = p.include_entire_dag = true;
  p.config_filename = "thing1";
  p.config._data = YAML::Load("[1, 2, 3]");
  p.output_test_dir = "thing2";
  p.snakefile = "thing3";
  p.pipeline_top_dir = "thing4";
  p.pipeline_run_dir = "thing5";
  p.inst_dir = "thing6";
  p.snakemake_log = "thing7";
  p.added_files.push_back("thing8");
  p.added_directories.push_back("thing9");
  p.exclude_rules["thing10"] = true;
  p.exclude_extensions["thing11"] = true;
  p.exclude_paths["thing12"] = true;
  p.byte_comparisons["thing13"] = true;
  params q(p);
  CPPUNIT_ASSERT(p.verbose == q.verbose);
  CPPUNIT_ASSERT(p.update_all = q.update_all);
  CPPUNIT_ASSERT(p.update_snakefiles == q.update_snakefiles);
  CPPUNIT_ASSERT(p.update_added_content == q.update_added_content);
  CPPUNIT_ASSERT(p.update_config == q.update_config);
  CPPUNIT_ASSERT(p.update_inputs == q.update_inputs);
  CPPUNIT_ASSERT(p.update_outputs == q.update_outputs);
  CPPUNIT_ASSERT(p.update_pytest == q.update_pytest);
  CPPUNIT_ASSERT(p.include_entire_dag == q.include_entire_dag);
  CPPUNIT_ASSERT(p.config_filename == q.config_filename);
  CPPUNIT_ASSERT(p.config == q.config);
  CPPUNIT_ASSERT(p.output_test_dir == q.output_test_dir);
  CPPUNIT_ASSERT(p.snakefile == q.snakefile);
  CPPUNIT_ASSERT(p.pipeline_top_dir == q.pipeline_top_dir);
  CPPUNIT_ASSERT(p.inst_dir == q.inst_dir);
  CPPUNIT_ASSERT(p.snakemake_log == q.snakemake_log);
  CPPUNIT_ASSERT(p.added_files == q.added_files);
  CPPUNIT_ASSERT(p.added_directories == q.added_directories);
  CPPUNIT_ASSERT(p.exclude_rules == q.exclude_rules);
  CPPUNIT_ASSERT(p.exclude_extensions == q.exclude_extensions);
  CPPUNIT_ASSERT(p.exclude_paths == q.exclude_paths);
  CPPUNIT_ASSERT(p.byte_comparisons == q.byte_comparisons);
}
void snakemake_unit_tests::cargsTest::test_params_report_settings() {}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_map_multiple_entries() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::map<std::string, bool> data;
  data["mykey1"] = true;
  data["mykey2"] = true;
  data["mykey3"] = true;
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_map(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey:\n  - mykey1\n  - mykey2\n  - mykey3";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_map_single_entry() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::map<std::string, bool> data;
  data["mykey1"] = true;
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_map(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey:\n  - mykey1";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_map_no_entries() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::map<std::string, bool> data;
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_map(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey: ~";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_map_null_pointer() {
  std::string key = "mykey";
  std::map<std::string, bool> data;
  params p;
  p.emit_yaml_map(NULL, data, key);
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_vector_multiple_entries() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::vector<boost::filesystem::path> data;
  data.push_back(boost::filesystem::path("path1"));
  data.push_back(boost::filesystem::path("path2"));
  data.push_back(boost::filesystem::path("path3"));
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_vector(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey:\n  - path1\n  - path2\n  - path3";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_vector_single_entry() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::vector<boost::filesystem::path> data;
  data.push_back(boost::filesystem::path("path1"));
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_vector(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey:\n  - path1";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_vector_no_entries() {
  YAML::Emitter out;
  std::string key = "mykey";
  std::vector<boost::filesystem::path> data;
  params p;
  out << YAML::BeginMap;
  p.emit_yaml_vector(&out, data, key);
  out << YAML::EndMap;
  std::string expected = "mykey: ~";
  std::string observed = std::string(out.c_str());
  CPPUNIT_ASSERT(!expected.compare(observed));
}
void snakemake_unit_tests::cargsTest::test_params_emit_yaml_vector_null_pointer() {
  std::string key = "mykey";
  std::vector<boost::filesystem::path> data;
  params p;
  p.emit_yaml_vector(NULL, data, key);
}
void snakemake_unit_tests::cargsTest::test_cargs_default_constructor() { cargs ap; }
void snakemake_unit_tests::cargsTest::test_cargs_standard_constructor() {}
void snakemake_unit_tests::cargsTest::test_cargs_copy_constructor() {}
void snakemake_unit_tests::cargsTest::test_cargs_initialize_options() {}
void snakemake_unit_tests::cargsTest::test_cargs_set_parameters() {}
void snakemake_unit_tests::cargsTest::test_cargs_help() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_config_yaml() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakefile() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakemake_log() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_output_test_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_top_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_run_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_inst_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_files() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_directories() {}
void snakemake_unit_tests::cargsTest::test_cargs_get_exclude_rules() {}
void snakemake_unit_tests::cargsTest::test_cargs_include_entire_dag() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_all() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_snakefiles() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_added_content() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_config() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_inputs() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_outputs() {}
void snakemake_unit_tests::cargsTest::test_cargs_update_pytest() {}
void snakemake_unit_tests::cargsTest::test_cargs_verbose() {}
void snakemake_unit_tests::cargsTest::test_cargs_compute_flag() {}
void snakemake_unit_tests::cargsTest::test_cargs_compute_parameter() {}
void snakemake_unit_tests::cargsTest::test_cargs_print_help() {}
void snakemake_unit_tests::cargsTest::test_cargs_override_if_specified() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_vector() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_map() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_nonempty() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_regular_file() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_vector_convert() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::cargsTest);
