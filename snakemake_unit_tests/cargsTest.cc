/*!
  \file cargsTest.cc
  \brief implementation of command line parsing unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include "snakemake_unit_tests/cargsTest.h"

void snakemake_unit_tests::cargsTest::create_empty_file(const boost::filesystem::path &p) const {
  std::ofstream output;
  output.open(p.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write empty file \"" + p.string() + "\"");
  }
  output.close();
}

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
  if (*arr) {
    delete[] * arr;
    *arr = 0;
  }
  *arr = new const char *[vec->size()];
  for (unsigned i = 0; i < vec->size(); ++i) {
    (*arr)[i] = vec->at(i).c_str();
  }
}

void snakemake_unit_tests::cargsTest::setUp() {
  std::string longform =
      "./snakemake_unit_tests.out --config configname.yaml "
      "--added-directories added_dir --include-rules keepme --exclude-rules rulename --added-files added_file "
      "--help --inst-dir inst --snakemake-log logfile --output-test-dir outdir "
      "--pipeline-top-dir project --pipeline-run-dir rundir --snakefile Snakefile "
      "--verbose --update-all --update-snakefiles --update-added-content "
      "--update-config --update-inputs --update-outputs --update-pytest --include-entire-dag";
  std::string shortform =
      "./snakemake_unit_tests.out -c configname.yaml "
      "-d added_dir -n keepme -e rulename -f added_file "
      "-h -i inst -l logfile -o outdir "
      "-p project -r rundir -s Snakefile -v";
  populate_arguments(longform, &_arg_vec_long, &_argv_long);
  populate_arguments(shortform, &_arg_vec_short, &_argv_short);
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutCATXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("cargsTest mkdtemp failed");
  }
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
  if (_argv_adhoc) {
    delete[] _argv_adhoc;
    _argv_adhoc = 0;
  }
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
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
  CPPUNIT_ASSERT(p.include_rules.empty());
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
  p.include_rules["thing9a"] = true;
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
  CPPUNIT_ASSERT(p.include_rules == q.include_rules);
  CPPUNIT_ASSERT(p.exclude_rules == q.exclude_rules);
  CPPUNIT_ASSERT(p.exclude_extensions == q.exclude_extensions);
  CPPUNIT_ASSERT(p.exclude_paths == q.exclude_paths);
  CPPUNIT_ASSERT(p.byte_comparisons == q.byte_comparisons);
}
void snakemake_unit_tests::cargsTest::test_params_report_settings() {
  boost::filesystem::path output_filename =
      boost::filesystem::path(std::string(_tmp_dir)) / "params_report_settings.yaml";
  params p;
  p.output_test_dir = "outdir";
  p.snakefile = "Snakefile";
  p.pipeline_top_dir = "ptop";
  p.pipeline_run_dir = "prun";
  p.inst_dir = "inst";
  p.snakemake_log = "slog";
  std::vector<boost::filesystem::path> files, directories;
  files.push_back("fname1");
  files.push_back("fname2");
  p.added_files = files;
  directories.push_back("dname1");
  directories.push_back("dname2");
  p.added_directories = directories;
  p.include_rules["keepme1"] = true;
  p.include_rules["keepme2"] = true;
  p.exclude_rules["rulename1"] = true;
  p.exclude_rules["rulename2"] = true;
  p.exclude_extensions.clear();
  p.exclude_paths["path1"] = true;
  p.byte_comparisons[".ext1"] = true;
  p.byte_comparisons[".ext2"] = true;
  p.byte_comparisons[".ext3"] = true;
  p.report_settings(output_filename);
  std::string pwd = boost::filesystem::current_path().string();
  std::string expected_contents = "output-test-dir: " + pwd +
                                  "/outdir\n"
                                  "snakefile: " +
                                  pwd +
                                  "/Snakefile\n"
                                  "pipeline-top-dir: " +
                                  pwd +
                                  "/ptop\n"
                                  "pipeline-run-dir: " +
                                  pwd +
                                  "/prun\n"
                                  "inst-dir: " +
                                  pwd +
                                  "/inst\n"
                                  "snakemake-log: " +
                                  pwd +
                                  "/slog\n"
                                  "added-files:\n  - fname1\n  - fname2\n"
                                  "added-directories:\n  - dname1\n  - dname2\n"
                                  "include-rules:\n  - keepme1\n  - keepme2\n"
                                  "exclude-rules:\n  - rulename1\n  - rulename2\n"
                                  "exclude-extensions: ~\n"
                                  "exclude-paths:\n  - path1\n"
                                  "byte-comparisons:\n  - .ext1\n  - .ext2\n  - .ext3\n";
  std::ifstream input;
  std::string line = "";
  std::ostringstream observed_contents;
  input.open(output_filename.string().c_str());
  CPPUNIT_ASSERT(input.is_open());
  while (input.peek() != EOF) {
    getline(input, line);
    observed_contents << line << '\n';
  }
  input.close();
  CPPUNIT_ASSERT(!observed_contents.str().compare(expected_contents));
}
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
      if (!prev.compare("added-directories") || !prev.compare("added-files") || !prev.compare("include-rules") ||
          !prev.compare("exclude-rules")) {
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
  // enforce that permitted flags are copied to new object
  CPPUNIT_ASSERT(ap._permitted_flags.size() == ap2._permitted_flags.size());
  std::map<std::string, bool>::const_iterator iter1, iter2;
  for (iter1 = ap._permitted_flags.begin(), iter2 = ap2._permitted_flags.begin(); iter1 != ap._permitted_flags.end();
       ++iter1, ++iter2) {
    CPPUNIT_ASSERT(!iter1->first.compare(iter2->first));
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
  CPPUNIT_ASSERT(o.str().find("-n [ --include-rules ] arg") != std::string::npos);
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
void snakemake_unit_tests::cargsTest::test_cargs_set_parameters() {
  /*
    set_parameters handles resolution of conflicting run settings provided
    in the optional config yaml and on the command line. the logic is somewhat
    convoluted, as the desired behavior varies depending on the type of
    parameter in question. additionally, not all of the options can be
    specified in both places; specifically, the run mode flags are only
    accepted on the command line, to encourage some level of user interaction;
    and some multiply specified parameters override, while others add on top.

    the settings are as follows (CLI tag, yaml tag, params internal variable):

    flags only allowed on the command line:
    - (update-all, NA, update_all)
    - (update-snakefiles, NA, update_snakefiles)
    - (update-added-content, NA, update_added_content)
    - (update-config, NA, update_config)
    - (update-inputs, NA, update_inputs)
    - (update-outputs, NA, update_outputs)
    - (update-pytest, NA, update_pytest)
    - (include-entire-dag, NA, include_entire_dag)

    parameters that override when present on the CLI:
    - (output-test-dir, output-test-dir, output_test_dir)
    - (snakefile, snakefile, snakefile)
    - (pipeline-top-dir, pipeline-top-dir, pipeline_top_dir)
    -   if this is NOT specified, it should be set to the parent of the Snakefile directory
    - (pipeline-run-dir, pipeline-run-dir, pipeline_run_dir)
    -   if this is NOT specified, it should be set to '.'
    - (inst-dir, inst-dir, inst_dir)
    - (snakemake-log, snakemake-log, snakemake_log)

    parameters that augment when present on the CLI:
    - (added-files, added-files, added_files)
    - (added-directories, added-directories, added_directories)
    - (include-rules, include-rules, include_rules)
    - (exclude-rules, exclude-rules, exclude_rules)
    -   exclude_rules unconditionally gets 'all' added to it (flagged for deprecation)
   */
  // mandatory input files and directories need to be present for this test
  boost::filesystem::path filename, prefix = std::string(_tmp_dir);
  std::ofstream output;
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  boost::filesystem::path top_dir_config = prefix / "set_parameters_config";
  std::filesystem::create_directory(top_dir.string().c_str());
  std::filesystem::create_directory(top_dir_config.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  boost::filesystem::path run_dir_config = "workflow_config";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  std::filesystem::create_directory((top_dir_config / run_dir_config).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  boost::filesystem::path inst_dir_config = prefix / "inst_config";
  std::filesystem::create_directory(inst_dir.string().c_str());
  std::filesystem::create_directory(inst_dir_config.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  boost::filesystem::path run_log_config = top_dir_config / "set_parameters_config.log";
  create_empty_file(run_log);
  create_empty_file(run_log_config);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  boost::filesystem::path snakefile_config = top_dir_config / run_dir_config / "Snakefile_config";
  create_empty_file(snakefile);
  create_empty_file(snakefile_config);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  boost::filesystem::path common_py_config = inst_dir_config / "common.py";
  create_empty_file(common_py);
  create_empty_file(common_py_config);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  boost::filesystem::path test_py_config = inst_dir_config / "test.py";
  create_empty_file(test_py);
  create_empty_file(test_py_config);
  // added files, directories
  std::map<boost::filesystem::path, bool> added_files, added_files_config, added_dirs, added_dirs_config;
  added_files["file1"] = true;
  added_files["file2"] = true;
  added_files_config["file1_config"] = true;
  added_files_config["file2_config"] = true;
  added_dirs["dir1"] = true;
  added_dirs["dir2"] = true;
  added_dirs_config["dir1_config"] = true;
  added_dirs_config["dir2_config"] = true;
  // included rules
  std::map<std::string, bool> include_rules, include_rules_config;
  include_rules["keepme1"] = true;
  include_rules["keepme2"] = true;
  include_rules["keepme1_config"] = true;
  include_rules["keepme2_config"] = true;
  // excluded rules
  std::map<std::string, bool> exclude_rules, exclude_rules_config;
  exclude_rules["rule1"] = true;
  exclude_rules["rule2"] = true;
  exclude_rules["rule1_config"] = true;
  exclude_rules["rule2_config"] = true;
  // excluded paths, extensions; byte comparisons. these are only accepted from the config
  std::map<std::string, bool> exclude_paths, exclude_extensions, byte_comparisons;
  exclude_paths["path1"] = true;
  exclude_paths["path2"] = true;
  exclude_extensions["ext1"] = true;
  exclude_extensions["ext2"] = true;
  byte_comparisons["ext3"] = true;
  byte_comparisons["ext4"] = true;
  // added directories
  // doesn't need to be created, but worth keeping this stored
  boost::filesystem::path outdir = prefix / "outdir";
  boost::filesystem::path outdir_config = prefix / "outdir_config";
  // config yaml, for testing conflict resolution
  boost::filesystem::path config_yaml = prefix / "config.yaml";
  std::string config_data = "output-test-dir: " + outdir_config.string() +
                            "\n"
                            "snakefile: " +
                            snakefile_config.string() +
                            "\n"
                            "pipeline-top-dir: " +
                            top_dir_config.string() +
                            "\n"
                            "pipeline-run-dir: " +
                            run_dir_config.string() +
                            "\n"
                            "inst-dir: " +
                            inst_dir_config.string() +
                            "\n"
                            "snakemake-log: " +
                            run_log_config.string() +
                            "\n"
                            "added-files:\n";
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_files_config.begin();
       iter != added_files_config.end(); ++iter) {
    config_data += "  - " + iter->first.string() + "\n";
    create_empty_file(top_dir_config / iter->first);
    // for later tests, this file must also be installed under the non-config top directory
    create_empty_file(top_dir / iter->first);
  }
  config_data += "added-directories:\n";
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_dirs_config.begin();
       iter != added_dirs_config.end(); ++iter) {
    config_data += "  - " + iter->first.string() + "\n";
    std::filesystem::create_directory((top_dir_config / iter->first).string().c_str());
    // for later tests, this directory must also be installed under the non-config top directory
    std::filesystem::create_directory((top_dir / iter->first).string().c_str());
  }
  config_data += "include-rules:\n";
  for (std::map<std::string, bool>::const_iterator iter = include_rules_config.begin();
       iter != include_rules_config.end(); ++iter) {
    config_data += "  - " + iter->first + "\n";
  }
  config_data += "exclude-rules:\n";
  for (std::map<std::string, bool>::const_iterator iter = exclude_rules_config.begin();
       iter != exclude_rules_config.end(); ++iter) {
    config_data += "  - " + iter->first + "\n";
  }
  config_data += "exclude-paths:\n";
  for (std::map<std::string, bool>::const_iterator iter = exclude_paths.begin(); iter != exclude_paths.end(); ++iter) {
    config_data += "  - " + iter->first + "\n";
  }
  config_data += "exclude-extensions:\n";
  for (std::map<std::string, bool>::const_iterator iter = exclude_extensions.begin(); iter != exclude_extensions.end();
       ++iter) {
    config_data += "  - " + iter->first + "\n";
  }
  config_data += "byte-comparisons:\n";
  for (std::map<std::string, bool>::const_iterator iter = byte_comparisons.begin(); iter != byte_comparisons.end();
       ++iter) {
    config_data += "  - " + iter->first + "\n";
  }
  output.open(config_yaml.string().c_str());
  if (!output.is_open()) throw std::runtime_error("cargs set_parameters: cannot write config yaml");
  output << config_data;
  output.close();
  output.clear();
  // ok
  // a run with the flag that should turn on all other run modes
  std::string command =
      "./snakemake_unit_tests.out --update-all "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " --output-test-dir " + outdir.string() +
      " --pipeline-top-dir " + top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " +
      snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap1(_arg_vec_adhoc.size(), _argv_adhoc);
  params p1 = ap1.set_parameters();
  CPPUNIT_ASSERT(p1.update_all);
  // a run with every other state flag;
  // also check the propagated state of the mandatory arguments
  command =
      "./snakemake_unit_tests.out "
      "--update-snakefiles --update-added-content --update-inputs "
      "--update-outputs --update-config --update-pytest "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " --output-test-dir " + outdir.string() +
      " --pipeline-top-dir " + top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " +
      snakefile.string() +
      " --include-rules keepme1 --include-rules keepme2 --exclude-rules rule1 --exclude-rules rule2";
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_dirs.begin(); iter != added_dirs.end();
       ++iter) {
    command += " -d " + iter->first.string();
    std::filesystem::create_directory((top_dir / iter->first).string().c_str());
  }
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_files.begin(); iter != added_files.end();
       ++iter) {
    command += " -f " + iter->first.string();
    create_empty_file(top_dir / iter->first);
  }
  for (std::map<std::string, bool>::const_iterator iter = include_rules.begin(); iter != include_rules.end(); ++iter) {
    command += " -n " + iter->first;
  }
  for (std::map<std::string, bool>::const_iterator iter = exclude_rules.begin(); iter != exclude_rules.end(); ++iter) {
    command += " -e " + iter->first;
  }
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap2(_arg_vec_adhoc.size(), _argv_adhoc);
  params p2 = ap2.set_parameters();
  CPPUNIT_ASSERT(p2.update_snakefiles);
  CPPUNIT_ASSERT(p2.update_config);
  CPPUNIT_ASSERT(p2.update_pytest);
  CPPUNIT_ASSERT(p2.update_inputs);
  CPPUNIT_ASSERT(p2.update_outputs);
  CPPUNIT_ASSERT(p2.update_added_content);
  CPPUNIT_ASSERT(!p2.snakefile.string().compare(snakefile.string()));
  CPPUNIT_ASSERT(!p2.pipeline_top_dir.string().compare(top_dir.string()));
  CPPUNIT_ASSERT(!p2.pipeline_run_dir.string().compare(run_dir.string()));
  CPPUNIT_ASSERT(!p2.output_test_dir.string().compare(outdir.string()));
  CPPUNIT_ASSERT(!p2.inst_dir.string().compare(inst_dir.string()));
  CPPUNIT_ASSERT(!p2.snakemake_log.string().compare(run_log.string()));
  CPPUNIT_ASSERT(p2.added_files.size() == added_files.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p2.added_files.begin(); iter != p2.added_files.end();
       ++iter) {
    CPPUNIT_ASSERT(added_files.find(*iter) != added_files.end());
  }
  CPPUNIT_ASSERT(p2.added_directories.size() == added_dirs.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p2.added_directories.begin();
       iter != p2.added_directories.end(); ++iter) {
    CPPUNIT_ASSERT(added_dirs.find(*iter) != added_dirs.end());
  }
  CPPUNIT_ASSERT(p2.include_rules.size() == include_rules.size());
  for (std::map<std::string, bool>::const_iterator iter = p2.include_rules.begin(); iter != p2.include_rules.end();
       ++iter) {
    CPPUNIT_ASSERT(include_rules.find(iter->first) != include_rules.end());
  }
  CPPUNIT_ASSERT(p2.exclude_rules.size() == exclude_rules.size() + 1);
  for (std::map<std::string, bool>::const_iterator iter = p2.exclude_rules.begin(); iter != p2.exclude_rules.end();
       ++iter) {
    if (iter->first.compare("all")) {
      CPPUNIT_ASSERT(exclude_rules.find(iter->first) != exclude_rules.end());
    }
  }
  CPPUNIT_ASSERT(p2.exclude_rules.find("all") != p2.exclude_rules.end());
  // a run with config yaml input only
  command = "./snakemake_unit_tests.out -c " + config_yaml.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap3(_arg_vec_adhoc.size(), _argv_adhoc);
  params p3 = ap3.set_parameters();
  CPPUNIT_ASSERT(!p3.snakefile.string().compare(snakefile_config.string()));
  CPPUNIT_ASSERT(!p3.pipeline_top_dir.string().compare(top_dir_config.string()));
  CPPUNIT_ASSERT(!p3.pipeline_run_dir.string().compare(run_dir_config.string()));
  CPPUNIT_ASSERT(!p3.output_test_dir.string().compare(outdir_config.string()));
  CPPUNIT_ASSERT(!p3.inst_dir.string().compare(inst_dir_config.string()));
  CPPUNIT_ASSERT(!p3.snakemake_log.string().compare(run_log_config.string()));
  CPPUNIT_ASSERT(p3.added_files.size() == added_files_config.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p3.added_files.begin(); iter != p3.added_files.end();
       ++iter) {
    CPPUNIT_ASSERT(added_files_config.find(*iter) != added_files_config.end());
  }
  CPPUNIT_ASSERT(p3.added_directories.size() == added_dirs_config.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p3.added_directories.begin();
       iter != p3.added_directories.end(); ++iter) {
    CPPUNIT_ASSERT(added_dirs_config.find(*iter) != added_dirs_config.end());
  }
  CPPUNIT_ASSERT(p3.include_rules.size() == include_rules_config.size());
  for (std::map<std::string, bool>::const_iterator iter = p3.include_rules.begin(); iter != p3.include_rules.end();
       ++iter) {
    CPPUNIT_ASSERT(include_rules.find(iter->first) != include_rules.end());
  }
  CPPUNIT_ASSERT(p3.exclude_rules.size() == exclude_rules_config.size() + 1);
  for (std::map<std::string, bool>::const_iterator iter = p3.exclude_rules.begin(); iter != p3.exclude_rules.end();
       ++iter) {
    if (iter->first.compare("all")) {
      CPPUNIT_ASSERT(exclude_rules_config.find(iter->first) != exclude_rules_config.end());
    }
  }
  CPPUNIT_ASSERT(p3.exclude_rules.find("all") != p3.exclude_rules.end());
  CPPUNIT_ASSERT(p3.exclude_paths.size() == exclude_paths.size());
  for (std::map<std::string, bool>::const_iterator iter = p3.exclude_paths.begin(); iter != p3.exclude_paths.end();
       ++iter) {
    CPPUNIT_ASSERT(exclude_paths.find(iter->first) != exclude_paths.end());
  }
  CPPUNIT_ASSERT(p3.exclude_extensions.size() == exclude_extensions.size());
  for (std::map<std::string, bool>::const_iterator iter = p3.exclude_extensions.begin();
       iter != p3.exclude_extensions.end(); ++iter) {
    CPPUNIT_ASSERT(exclude_extensions.find(iter->first) != exclude_extensions.end());
  }
  CPPUNIT_ASSERT(p3.byte_comparisons.size() == byte_comparisons.size());
  for (std::map<std::string, bool>::const_iterator iter = p3.byte_comparisons.begin();
       iter != p3.byte_comparisons.end(); ++iter) {
    CPPUNIT_ASSERT(byte_comparisons.find(iter->first) != byte_comparisons.end());
  }

  // a run with both config yaml input and CLI input, to test resolution
  command =
      "./snakemake_unit_tests.out "
      "--update-snakefiles --update-added-content --update-inputs "
      "--update-outputs --update-config --update-pytest "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " --output-test-dir " + outdir.string() +
      " --pipeline-top-dir " + top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " +
      snakefile.string() + " --exclude-rules rule1 --exclude-rules rule2";
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_dirs.begin(); iter != added_dirs.end();
       ++iter) {
    command += " -d " + iter->first.string();
    std::filesystem::create_directory((top_dir / iter->first).string().c_str());
  }
  for (std::map<boost::filesystem::path, bool>::const_iterator iter = added_files.begin(); iter != added_files.end();
       ++iter) {
    command += " -f " + iter->first.string();
    create_empty_file(top_dir / iter->first);
  }
  for (std::map<std::string, bool>::const_iterator iter = exclude_rules.begin(); iter != exclude_rules.end(); ++iter) {
    command += " -e " + iter->first;
  }
  command += " -c " + config_yaml.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap4(_arg_vec_adhoc.size(), _argv_adhoc);
  params p4 = ap4.set_parameters();
  // almost everything should be what was found on the command line, not in the config yaml
  CPPUNIT_ASSERT(!p4.snakefile.string().compare(snakefile.string()));
  CPPUNIT_ASSERT(!p4.pipeline_top_dir.string().compare(top_dir.string()));
  CPPUNIT_ASSERT(!p4.pipeline_run_dir.string().compare(run_dir.string()));
  CPPUNIT_ASSERT(!p4.output_test_dir.string().compare(outdir.string()));
  CPPUNIT_ASSERT(!p4.inst_dir.string().compare(inst_dir.string()));
  CPPUNIT_ASSERT(!p4.snakemake_log.string().compare(run_log.string()));
  // files, directories, rules are cumulative
  CPPUNIT_ASSERT(p4.added_files.size() == added_files.size() + added_files_config.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p4.added_files.begin(); iter != p4.added_files.end();
       ++iter) {
    CPPUNIT_ASSERT(added_files_config.find(*iter) != added_files_config.end() ||
                   added_files.find(*iter) != added_files.end());
  }
  CPPUNIT_ASSERT(p4.added_directories.size() == added_dirs.size() + added_dirs_config.size());
  for (std::vector<boost::filesystem::path>::const_iterator iter = p4.added_directories.begin();
       iter != p4.added_directories.end(); ++iter) {
    CPPUNIT_ASSERT(added_dirs_config.find(*iter) != added_dirs_config.end() ||
                   added_dirs.find(*iter) != added_dirs.end());
  }
  CPPUNIT_ASSERT(p4.include_rules.size() == include_rules_config.size());
  for (std::map<std::string, bool>::const_iterator iter = p4.include_rules.begin(); iter != p4.include_rules.end();
       ++iter) {
    CPPUNIT_ASSERT(include_rules.find(iter->first) != include_rules.end());
  }
  CPPUNIT_ASSERT(p4.exclude_rules.size() == exclude_rules.size() + exclude_rules_config.size() + 1);
  for (std::map<std::string, bool>::const_iterator iter = p4.exclude_rules.begin(); iter != p4.exclude_rules.end();
       ++iter) {
    if (iter->first.compare("all")) {
      CPPUNIT_ASSERT(exclude_rules_config.find(iter->first) != exclude_rules_config.end() ||
                     exclude_rules.find(iter->first) != exclude_rules.end());
    }
  }
  CPPUNIT_ASSERT(p4.exclude_rules.find("all") != p4.exclude_rules.end());
  CPPUNIT_ASSERT(p4.exclude_paths.size() == exclude_paths.size());
  for (std::map<std::string, bool>::const_iterator iter = p4.exclude_paths.begin(); iter != p4.exclude_paths.end();
       ++iter) {
    CPPUNIT_ASSERT(exclude_paths.find(iter->first) != exclude_paths.end());
  }
  CPPUNIT_ASSERT(p4.exclude_extensions.size() == exclude_extensions.size());
  for (std::map<std::string, bool>::const_iterator iter = p4.exclude_extensions.begin();
       iter != p4.exclude_extensions.end(); ++iter) {
    CPPUNIT_ASSERT(exclude_extensions.find(iter->first) != exclude_extensions.end());
  }
  CPPUNIT_ASSERT(p4.byte_comparisons.size() == byte_comparisons.size());
  for (std::map<std::string, bool>::const_iterator iter = p4.byte_comparisons.begin();
       iter != p4.byte_comparisons.end(); ++iter) {
    CPPUNIT_ASSERT(byte_comparisons.find(iter->first) != byte_comparisons.end());
  }
}
void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_output_dir_missing() {
  // construct an otherwise valid command, but leave out output directory
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " --pipeline-top-dir " + top_dir.string() +
      " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_snakefile_invalid() {
  // construct an otherwise valid command, but snakefile is invalid
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_top_dir_invalid() {
  // construct an otherwise valid command, but snakemake top directory is invalid
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      (top_dir / "bad").string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_run_dir_invalid() {
  // construct an otherwise valid command, but snakemake run directory is malformed
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = prefix / "workflow";
  std::filesystem::create_directory(run_dir.string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_inst_dir_invalid() {
  // construct an otherwise valid command, but inst is bad
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_inst_dir_missing_test() {
  // construct an otherwise valid command, but test.py isn't present under inst
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_inst_dir_missing_common() {
  // construct an otherwise valid command, but common.py isn't present under inst
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_snakemake_log_missing() {
  // construct an otherwise valid command, but leave out snakemake log
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_added_files_invalid() {
  // construct an otherwise valid command, but provide bad added file
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string() + " -f " +
      (top_dir / "weird_file").string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

void snakemake_unit_tests::cargsTest::test_cargs_set_parameters_added_directories_invalid() {
  // construct an otherwise valid command, but provide bad added directory
  boost::filesystem::path prefix = std::string(_tmp_dir);
  // pipeline top level directory
  boost::filesystem::path top_dir = prefix / "set_parameters";
  std::filesystem::create_directory(top_dir.string().c_str());
  // pipeline run directory
  boost::filesystem::path run_dir = "workflow";
  std::filesystem::create_directory((top_dir / run_dir).string().c_str());
  // inst directory
  boost::filesystem::path inst_dir = prefix / "inst";
  std::filesystem::create_directory(inst_dir.string().c_str());
  // snakemake run log
  boost::filesystem::path run_log = top_dir / "set_parameters.log";
  create_empty_file(run_log);
  // snakefile
  boost::filesystem::path snakefile = top_dir / run_dir / "Snakefile";
  create_empty_file(snakefile);
  // inst common.py
  boost::filesystem::path common_py = inst_dir / "common.py";
  create_empty_file(common_py);
  // inst test.py
  boost::filesystem::path test_py = inst_dir / "test.py";
  create_empty_file(test_py);
  // output directory
  boost::filesystem::path outdir = prefix / "outdir";
  std::string command =
      "./snakemake_unit_tests.out "
      "--inst-dir " +
      inst_dir.string() + " --snakemake-log " + run_log.string() + " -o " + outdir.string() + " --pipeline-top-dir " +
      top_dir.string() + " --pipeline-run-dir " + run_dir.string() + " --snakefile " + snakefile.string() + " -d " +
      (top_dir / "weird_dir").string();
  populate_arguments(command, &_arg_vec_adhoc, &_argv_adhoc);
  cargs ap(_arg_vec_adhoc.size(), _argv_adhoc);
  params p = ap.set_parameters();
}

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
void snakemake_unit_tests::cargsTest::test_cargs_get_include_rules() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  std::vector<std::string> res = ap.get_include_rules();
  CPPUNIT_ASSERT(res.size() == 1 && !res.at(0).compare("keepme"));
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
  cargs ap(_arg_vec_short.size(), _argv_short);
  CPPUNIT_ASSERT(ap.compute_flag("help"));
  CPPUNIT_ASSERT(ap.compute_flag("verbose"));
  CPPUNIT_ASSERT_MESSAGE("cargs compute_flag gracefully handles absent tags", !ap.compute_flag("update-all"));
  // make sure all permitted flags are in fact permitted
  CPPUNIT_ASSERT(!ap.compute_flag("include-entire-dag"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-all"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-snakefiles"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-added-content"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-inputs"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-outputs"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-pytest"));
  CPPUNIT_ASSERT(!ap.compute_flag("update-config"));
}
void snakemake_unit_tests::cargsTest::test_cargs_compute_flag_invalid_flag() {
  cargs ap(_arg_vec_short.size(), _argv_short);
  ap.compute_flag("othertag");
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
void snakemake_unit_tests::cargsTest::test_cargs_override_if_specified() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  boost::filesystem::path result;
  result = ap.override_if_specified("cli-entry", boost::filesystem::path());
  CPPUNIT_ASSERT_MESSAGE("cargs override_if_specified chooses CLI with NULL alternative",
                         !result.string().compare("cli-entry"));
  result = ap.override_if_specified("", boost::filesystem::path("yaml-entry"));
  CPPUNIT_ASSERT_MESSAGE("cargs override_if_specified chooses yaml with NULL alternative",
                         !result.string().compare("yaml-entry"));
  result = ap.override_if_specified("", boost::filesystem::path());
  CPPUNIT_ASSERT_MESSAGE("cargs override_if_specified quietly returns NULL if both entries are NULL",
                         result.string().empty());
  result = ap.override_if_specified("cli-entry", boost::filesystem::path("yaml-entry"));
  CPPUNIT_ASSERT_MESSAGE("cargs override_if_specified prefers CLI entry", !result.string().compare("cli-entry"));
}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_vector() {
  std::vector<std::string> vec1, vec2;
  vec1.push_back("oldarg");
  vec2.push_back("newarg1");
  vec2.push_back("newarg2");
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.add_contents<std::string>(vec2, &vec1);
  CPPUNIT_ASSERT_MESSAGE("add_contents to vector results in correct size", vec1.size() == 3);
  CPPUNIT_ASSERT(!vec1.at(0).compare("oldarg"));
  CPPUNIT_ASSERT(!vec1.at(1).compare("newarg1"));
  CPPUNIT_ASSERT(!vec1.at(2).compare("newarg2"));
}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_vector_null_pointer() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.add_contents<std::string>(std::vector<std::string>(), reinterpret_cast<std::vector<std::string> *>(NULL));
}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_map() {
  std::map<std::string, bool> map1;
  std::vector<std::string> vec2;
  map1["oldarg"] = false;
  vec2.push_back("newarg1");
  vec2.push_back("newarg2");
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.add_contents<std::string>(vec2, &map1);
  CPPUNIT_ASSERT_MESSAGE("add_contents to map results in correct size", map1.size() == 3);
  std::map<std::string, bool>::const_iterator finder;
  CPPUNIT_ASSERT((finder = map1.find("oldarg")) != map1.end() && !finder->second);
  CPPUNIT_ASSERT((finder = map1.find("newarg1")) != map1.end() && finder->second);
  CPPUNIT_ASSERT((finder = map1.find("newarg2")) != map1.end() && finder->second);
}
void snakemake_unit_tests::cargsTest::test_cargs_add_contents_to_map_null_pointer() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.add_contents<std::string>(std::vector<std::string>(), reinterpret_cast<std::map<std::string, bool> *>(NULL));
}
void snakemake_unit_tests::cargsTest::test_cargs_check_nonempty() {
  // this currently throws exception if it is empty, so this is just... check that it runs
  cargs ap(_arg_vec_long.size(), _argv_long);
  boost::filesystem::path p("placeholder");
  ap.check_nonempty(p, "my parameter goes here");
}
void snakemake_unit_tests::cargsTest::test_cargs_check_nonempty_invalid_path() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  boost::filesystem::path p;
  ap.check_nonempty(p, "empty parameter");
}
void snakemake_unit_tests::cargsTest::test_cargs_check_regular_file() {
  // create a regular file under allocated tmp directory
  std::string file_suffix = "check_regular_file.txt";
  std::ofstream output;
  output.open((_tmp_dir + std::string("/") + file_suffix).c_str());
  if (!output.is_open()) throw std::runtime_error("cargs check_regular_file: unable to write file");
  output.close();
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.check_regular_file(boost::filesystem::path(std::string(_tmp_dir)) / file_suffix, boost::filesystem::path(),
                        "file with null prefix");
  ap.check_regular_file(boost::filesystem::path(file_suffix), boost::filesystem::path(std::string(_tmp_dir)),
                        "file with prefix");
}
void snakemake_unit_tests::cargsTest::test_cargs_check_regular_file_not_file() {
  // can pass it whatever?
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.check_regular_file(boost::filesystem::path(std::string(_tmp_dir)), boost::filesystem::path(""),
                        "actually a directory");
}
void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  boost::filesystem::path p(std::string(_tmp_dir) + "/");
  // check that it strips trailing separator
  ap.check_and_fix_dir(&p, boost::filesystem::path(), "simple directory with trailing suffix");
  CPPUNIT_ASSERT_MESSAGE("cargs check_and_fix_dir strips trailing separator",
                         !p.string().compare(std::string(_tmp_dir)));
  // check that it preserves directory without trailing separator
  ap.check_and_fix_dir(&p, boost::filesystem::path(), "simple directory without trailing suffix");
  CPPUNIT_ASSERT_MESSAGE("cargs check_and_fix_dir preserves directory without trailing suffix",
                         !p.string().compare(std::string(_tmp_dir)));
  // check that it glues prefix on correctly
  std::filesystem::create_directory(std::string(_tmp_dir) + "/check_and_fix_dir");
  boost::filesystem::path suffix("check_and_fix_dir");
  ap.check_and_fix_dir(&suffix, boost::filesystem::path(std::string(_tmp_dir)), "directory requiring prefix");
}

void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir_null_pointer() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  ap.check_and_fix_dir(reinterpret_cast<boost::filesystem::path *>(NULL), boost::filesystem::path(), "bad pointer");
}

void snakemake_unit_tests::cargsTest::test_cargs_check_and_fix_dir_not_directory() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  // just in case, create a file
  std::string filename = std::string(_tmp_dir) + "/check_and_fix_dir.txt";
  std::ofstream output;
  output.open(filename.c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cargs check_and_fix_dir: cannot create file");
  }
  output.close();
  boost::filesystem::path p("check_and_fix_dir.txt");
  ap.check_and_fix_dir(&p, boost::filesystem::path(std::string(_tmp_dir)), "invalid directory");
}

void snakemake_unit_tests::cargsTest::test_cargs_vector_convert() {
  cargs ap(_arg_vec_long.size(), _argv_long);
  std::vector<std::string> vec1;
  vec1.push_back("0");
  vec1.push_back("1");
  vec1.push_back("2");
  std::vector<boost::filesystem::path> vec2 = ap.vector_convert<boost::filesystem::path>(vec1);
  CPPUNIT_ASSERT(vec1.size() == vec2.size());
  CPPUNIT_ASSERT(!vec2.at(0).string().compare("0"));
  CPPUNIT_ASSERT(!vec2.at(1).string().compare("1"));
  CPPUNIT_ASSERT(!vec2.at(2).string().compare("2"));
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::cargsTest);
