/*!
  \file cargsTest.cc
  \brief implementation of command line parsing unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/cargsTest.h"

void snakemake_unit_tests::cargsTest::populate_arguments(const std::string &cmd, std::vector<std::string> *vec,
                                                         const char ***arr) const {
  if (!vec || !arr) {
    throw std::runtime_error("null pointer(s) to populate_arguments");
  }
  vec->clear();
  std::istringstream strm1(cmd);
  std::string token = "";
  while (strm1 >> token) {
    vec->push_back(token);
  }
  *arr = new const char *[vec->size()];
  for (unsigned i = 0; i < vec->size(); ++i) {
    (*arr)[i] = vec->at(i).c_str();
  }
}

void snakemake_unit_tests::cargsTest::setUp() {
  std::string longform =
      "./snakemake_unit_tests.out --config configname.yaml "
      "--added-directories added_dir --exclude-rules rulename --added-files added_file "
      "--help --inst-dir inst --snakemake-log logfile --output-test-dir outdir "
      "--pipeline-top-dir project --pipeline-run-dir rundir --snakefile Snakefile "
      "--verbose --update-all --update-snakefiles --update-added-content "
      "--update-config --update-inputs --update-outputs --update-pytest --include-entire-dag";
  std::string shortform =
      "./snakemake_unit_tests.out -c configname.yaml "
      "-d added_dir -e rulename -f added_file "
      "-h -i inst -l logfile -o outdir "
      "-p project -r rundir -s Snakefile -v";
  populate_arguments(longform, &_arg_vec_long, &_argv_long);
  populate_arguments(shortform, &_arg_vec_short, &_argv_short);
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
  cargs ap(_arg_vec_long.size(), _argv_long);
  // individual accessors will be tested elsewhere, so this really
  // just tests that the standard constructor functions at all
}
void snakemake_unit_tests::cargsTest::test_cargs_copy_constructor() {
  cargs ap(_arg_vec_long.size(), _argv_long);
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
  cargs ap(_arg_vec_long.size(), _argv_long);
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
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT_MESSAGE("cargs help request detected", ap.help());
}
void snakemake_unit_tests::cargsTest::test_cargs_get_config_yaml() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_config_yaml().compare("configname.yaml"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakefile() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_snakefile().compare("Snakefile"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_snakemake_log() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_snakemake_log().compare("logfile"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_output_test_dir() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_output_test_dir().compare("outdir"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_top_dir() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_pipeline_top_dir().compare("project"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_pipeline_run_dir() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_pipeline_run_dir().compare("rundir"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_inst_dir() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(!ap.get_inst_dir().compare("inst"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_files() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  std::vector<std::string> res = ap.get_added_files();
  CPPUNIT_ASSERT(res.size() == 1 && !res.at(0).compare("added_file"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_added_directories() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  std::vector<std::string> res = ap.get_added_directories();
  CPPUNIT_ASSERT(res.size() == 1 && !res.at(0).compare("added_dir"));
}
void snakemake_unit_tests::cargsTest::test_cargs_get_exclude_rules() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  std::vector<std::string> res = ap.get_exclude_rules();
  CPPUNIT_ASSERT(res.size() == 1 && !res.at(0).compare("rulename"));
}
void snakemake_unit_tests::cargsTest::test_cargs_include_entire_dag() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.include_entire_dag());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_all() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_all());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_snakefiles() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_snakefiles());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_added_content() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_added_content());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_config() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_config());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_inputs() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_inputs());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_outputs() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_outputs());
}
void snakemake_unit_tests::cargsTest::test_cargs_update_pytest() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.update_pytest());
}
void snakemake_unit_tests::cargsTest::test_cargs_verbose() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.verbose());
}
void snakemake_unit_tests::cargsTest::test_cargs_compute_flag() {
  // note that a desired behavior might be for this to not behave
  // gracefully but rather crash when an unsupported flag is queried
  cargs ap(_arg_vec_long.size(), _argv_long);
  CPPUNIT_ASSERT(ap.compute_flag("help"));
  CPPUNIT_ASSERT_MESSAGE("cargs compute_flag gracefully handles absent tags", !ap.compute_flag("othertag"));
}
void snakemake_unit_tests::cargsTest::test_cargs_compute_parameter() {
  // note that as with compute_flag, this doesn't actually check that
  // the queried parameter is part of the accepted parameter set
  cargs ap(_arg_vec_long.size(), _argv_long);
  // a recognized parameter should have its value returned
  CPPUNIT_ASSERT_MESSAGE("cargs compute_parameter returns parameter value",
                         !ap.compute_parameter<std::string>("config").compare("configname.yaml"));
  // an optional parameter is allowed to not be present
  CPPUNIT_ASSERT_MESSAGE("cargs compute_parameter tolerates missing optional parameters",
                         ap.compute_parameter<std::string>("weirdname", true).empty());
}
void snakemake_unit_tests::cargsTest::test_cargs_compute_missing_required_parameter() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.compute_parameter<std::string>("weirdname", false);
}
void snakemake_unit_tests::cargsTest::test_cargs_print_help() {
  cargs ap(_arg_vec_long.size(), _argv_long);
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
