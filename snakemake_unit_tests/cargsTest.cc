/*!
  \file cargsTest.cc
  \brief implementation of command line parsing unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/cargsTest.h"

void snakemake_unit_tests::cargsTest::setUp() {
  _arg_vec_long.push_back("./snakemake_unit_tests.out");
  _arg_vec_long.push_back("--config,-c");
  _arg_vec_long.push_back("configname.yaml");
  _arg_vec_long.push_back("--added-directories,-d");
  _arg_vec_long.push_back("added_dir");
  _arg_vec_long.push_back("--exclude-rules,-e");
  _arg_vec_long.push_back("rulename");
  _arg_vec_long.push_back("--added-files,-f");
  _arg_vec_long.push_back("added_file");
  _arg_vec_long.push_back("--help,-h");
  _arg_vec_long.push_back("--inst-dir,-i");
  _arg_vec_long.push_back("inst");
  _arg_vec_long.push_back("--snakemake-log,-l");
  _arg_vec_long.push_back("logfile");
  _arg_vec_long.push_back("--output-test-dir,-o");
  _arg_vec_long.push_back("outdir");
  _arg_vec_long.push_back("--pipeline-top-dir,-p");
  _arg_vec_long.push_back("project");
  _arg_vec_long.push_back("--pipeline-run-dir,-r");
  _arg_vec_long.push_back("rundir");
  _arg_vec_long.push_back("--snakefile,-s");
  _arg_vec_long.push_back("Snakefile");
  _arg_vec_long.push_back("--verbose,-v");
  _arg_vec_long.push_back("--update-all");
  _arg_vec_long.push_back("--update-snakefiles");
  _arg_vec_long.push_back("--update-added-content");
  _arg_vec_long.push_back("--update-config");
  _arg_vec_long.push_back("--update-inputs");
  _arg_vec_long.push_back("--update-outputs");
  _arg_vec_long.push_back("--update-pytest");
  _arg_vec_long.push_back("--include-entire-dag");
  _argv_long = new const char *[_arg_vec_long.size()];
  _argv_short = new const char *[_arg_vec_long.size()];
  _arg_vec_short.reserve(_arg_vec_long.size());
  for (unsigned i = 0; i < _arg_vec_long.size(); ++i) {
    if (_arg_vec_long.at(i).find(",-")) {
      _arg_vec_short.push_back(_arg_vec_long.at(i).substr(_arg_vec_long.at(i).find(",-") + 1));
      _arg_vec_long.at(i) = _arg_vec_long.at(i).substr(0, _arg_vec_long.at(i).find(",-"));
    } else {
      _arg_vec_short.push_back(_arg_vec_long.at(i));
    }
  }
  for (unsigned i = 0; i < _arg_vec_long.size(); ++i) {
    _argv_long[i] = _arg_vec_long.at(i).c_str();
    _argv_short[i] = _arg_vec_short.at(i).c_str();
  }
  _argc = _arg_vec_long.size();
}

void snakemake_unit_tests::cargsTest::tearDown() {
  if (_argv_long) {
    delete[] _argv_long;
    _argv_long = 0;
  }
  if (_argv_short) {
    delete[] _argv_short;
    _argv_short = 0;
  }
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
void snakemake_unit_tests::cargsTest::test_cargs_standard_constructor() {
  cargs ap(_argc, _argv_long);
  // individual accessors will be tested elsewhere, so this really
  // just tests that the standard constructor functions at all
}
void snakemake_unit_tests::cargsTest::test_cargs_copy_constructor() {
  cargs ap(_argc, _argv_long);
  cargs ap2(ap);
  // check that output descriptions are identical
  std::ostringstream o1, o2;
  o1 << ap._desc;
  o2 << ap2._desc;
  CPPUNIT_ASSERT(!o1.str().compare(o2.str()));
  // check that each flag is present, and that flags with associated values are paired
  for (unsigned i = 0; i < _arg_vec_long.size(); ++i) {
    std::string current = _arg_vec_long.at(i);
    if (current.find("--") == 0) {
      current = current.substr(2);
      CPPUNIT_ASSERT_MESSAGE("cargs copy constructor: " + current, ap2._vm.count(current));
    } else if (i > 0) {
      std::string prev = _arg_vec_long.at(i - 1).substr(2);
      CPPUNIT_ASSERT_MESSAGE("cargs copy constructor, parameters sane: " + prev + " -> " + current,
                             ap2._vm.count(prev));
      // handle multiple value parameters separately
      if (!prev.compare("added-directories") || !prev.compare("added-files") || !prev.compare("exclude-rules")) {
        std::vector<std::string> result = ap2._vm[prev].as<std::vector<std::string> >();
        CPPUNIT_ASSERT_MESSAGE("cargs copy constructor key->value: " + prev + " -> " + current,
                               result.size() == 1 && !result.at(0).compare(current));
      } else {
        std::string result = ap2._vm[prev].as<std::string>();
        CPPUNIT_ASSERT_MESSAGE("cargs copy constructor key->value: " + prev + " -> " + current,
                               !result.compare(current));
      }
    }
  }
}
void snakemake_unit_tests::cargsTest::test_cargs_initialize_options() {
  cargs ap(_argc, _argv_long);
  // initialize_options is called by standard constructor; test that
  // standard constructor has reasonably called it. we're going to
  // compromise with feasibility and only test that the flags are present,
  // not that the descriptions are anything in particular; it seems like
  // the descriptions are the bit that are most subject to change.
  std::ostringstream o;
  o << ap._desc;
  CPPUNIT_ASSERT(o.str().find("-c [ --config ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-d [ --added-directories ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-e [ --exclude-rules ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-f [ --added-files ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-h [ --help ]") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-i [ --inst-dir ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-l [ --snakemake-log ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-o [ --output-test-dir ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-p [ --pipeline-top-dir ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-r [ --pipeline-run-dir ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-s [ --snakefile ] arg") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("-v [ --verbose ]") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-all") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-snakefiles") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-added-content") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-config") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-inputs") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-outputs") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--update-pytest") != std::string::npos);
  CPPUNIT_ASSERT(o.str().find("--include-entire-dag") != std::string::npos);
}
void snakemake_unit_tests::cargsTest::test_cargs_set_parameters() {}
void snakemake_unit_tests::cargsTest::test_cargs_help() {
  cargs ap(_argc, _argv_long);
  CPPUNIT_ASSERT_MESSAGE("cargs help request detected", ap.help());
}
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
void snakemake_unit_tests::cargsTest::test_cargs_print_help() {
  cargs ap(_argc, _argv_long);
  std::ostringstream o1, o2;
  ap.print_help(o1);
  o2 << ap._desc;
  CPPUNIT_ASSERT_MESSAGE("cargs help string printed", !o1.str().compare(o2.str()));
}
void snakemake_unit_tests::cargsTest::test_cargs_override_if_specified() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_vector() {}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_map() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_nonempty() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_regular_file() {}
void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir() {}
void snakemake_unit_tests::cargsTest::test_cargs_vector_convert() {}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::cargsTest);
