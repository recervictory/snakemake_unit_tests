/*!
  \file snakemake_fileTest.cc
  \brief implementation of snakemake file unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2022 Cameron Palmer.
 */

#include "snakemake_unit_tests/snakemake_fileTest.h"

void snakemake_unit_tests::snakemake_fileTest::setUp() {
  unsigned buffer_size = std::filesystem::temp_directory_path().string().size() + 20;
  _tmp_dir = new char[buffer_size];
  strncpy(_tmp_dir, (std::filesystem::temp_directory_path().string() + "/sutSFTXXXXXX").c_str(), buffer_size);
  char *res = mkdtemp(_tmp_dir);
  if (!res) {
    throw std::runtime_error("snakemake_fileTest mkdtemp failed");
  }
}

void snakemake_unit_tests::snakemake_fileTest::tearDown() {
  if (_tmp_dir) {
    std::filesystem::remove_all(std::filesystem::path(_tmp_dir));
    delete[] _tmp_dir;
  }
}

void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_default_constructor() {
  snakemake_file sf;
  CPPUNIT_ASSERT(sf._blocks.empty());
  CPPUNIT_ASSERT(sf._snakefile_relative_path.string().empty());
  CPPUNIT_ASSERT(sf._included_files.empty());
  CPPUNIT_ASSERT(sf._tag_counter.get());
  CPPUNIT_ASSERT_EQUAL(1u, *sf._tag_counter);
  CPPUNIT_ASSERT(sf._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_pointer_constructor() {
  boost::shared_ptr<unsigned> ptr(new unsigned);
  *ptr = 20u;
  snakemake_file sf(ptr);
  CPPUNIT_ASSERT(sf._blocks.empty());
  CPPUNIT_ASSERT(sf._snakefile_relative_path.string().empty());
  CPPUNIT_ASSERT(sf._included_files.empty());
  CPPUNIT_ASSERT(sf._tag_counter.get());
  CPPUNIT_ASSERT_EQUAL(20u, *sf._tag_counter);
  CPPUNIT_ASSERT(sf._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_copy_constructor() {
  snakemake_file sf1;
  boost::shared_ptr<rule_block> ptr_rb(new rule_block);
  boost::shared_ptr<snakemake_file> ptr_sf(new snakemake_file);
  sf1._blocks.push_back(ptr_rb);
  sf1._snakefile_relative_path = "/my/path";
  sf1._included_files["/other/path"] = ptr_sf;
  *sf1._tag_counter = 55u;
  sf1._updated_last_round = false;
  snakemake_file sf2(sf1);
  CPPUNIT_ASSERT(sf2._blocks.size() == 1u);
  CPPUNIT_ASSERT(*(sf2._blocks.begin()) == ptr_rb);
  CPPUNIT_ASSERT(!sf2._snakefile_relative_path.string().compare("/my/path"));
  CPPUNIT_ASSERT(sf2._included_files.size() == 1u);
  CPPUNIT_ASSERT(sf2._included_files.find("/other/path") != sf2._included_files.end());
  CPPUNIT_ASSERT(sf2._included_files["/other/path"] == ptr_sf);
  CPPUNIT_ASSERT_EQUAL(55u, *sf2._tag_counter);
  CPPUNIT_ASSERT(!sf2._updated_last_round);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_everything() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_parse_file() {
  std::vector<std::string> loaded_lines;
  loaded_lines.push_back("rule rule1:");
  loaded_lines.push_back("    input: 'filename',");
  loaded_lines.push_back("include: 'filename'");
  loaded_lines.push_back("pass");
  snakemake_file sf;
  sf.parse_file(loaded_lines, "fakefile", false);
  CPPUNIT_ASSERT(!sf._snakefile_relative_path.compare("fakefile"));
  CPPUNIT_ASSERT(sf._blocks.size() == 3);
  std::list<boost::shared_ptr<rule_block> >::iterator iter = sf._blocks.begin();
  CPPUNIT_ASSERT(!(*iter)->_rule_name.compare("rule1"));
  CPPUNIT_ASSERT((*iter)->_named_blocks.size() == 1);
  CPPUNIT_ASSERT(!(*iter)->_named_blocks.at(0).first.compare("input"));
  CPPUNIT_ASSERT(!(*iter)->_named_blocks.at(0).second.compare(" 'filename',"));
  CPPUNIT_ASSERT((*iter)->_python_tag == 1);
  CPPUNIT_ASSERT((*iter)->_resolution == UNRESOLVED);
  ++iter;
  CPPUNIT_ASSERT((*iter)->_code_chunk.size() == 1);
  CPPUNIT_ASSERT(!(*iter)->_code_chunk.at(0).compare("include: 'filename'"));
  CPPUNIT_ASSERT((*iter)->_python_tag == 2);
  CPPUNIT_ASSERT((*iter)->_resolution == UNRESOLVED);
  ++iter;
  CPPUNIT_ASSERT((*iter)->_code_chunk.size() == 1);
  CPPUNIT_ASSERT(!(*iter)->_code_chunk.at(0).compare("pass"));
  CPPUNIT_ASSERT((*iter)->_python_tag == 0);
  CPPUNIT_ASSERT((*iter)->_resolution == RESOLVED_INCLUDED);
  CPPUNIT_ASSERT(*sf._tag_counter == 3);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_load_lines() {
  // create a dummy snakefile and ensure it's loaded as anticipated
  std::ofstream output;
  boost::filesystem::path filename = boost::filesystem::path(std::string(_tmp_dir)) / "test_snakefile";
  output.open(filename.string().c_str());
  std::string content = "/usr/bin/env snakemake\n\nrule all:\n    input: TARGETS,\n\n";
  if (!output.is_open()) {
    throw std::runtime_error("cannot write test snakefile for load_lines");
  }
  if (!(output << content << std::endl)) {
    throw std::runtime_error("cannot write to disk for snakefile for load_lines");
  }
  output.close();
  std::vector<std::string> target;
  snakemake_file sf;
  sf.load_lines(filename, &target);
  CPPUNIT_ASSERT(target.size() == 6);
  CPPUNIT_ASSERT(!target.at(0).compare("/usr/bin/env snakemake"));
  CPPUNIT_ASSERT(target.at(1).empty());
  CPPUNIT_ASSERT(!target.at(2).compare("rule all:"));
  CPPUNIT_ASSERT(!target.at(3).compare("    input: TARGETS,"));
  CPPUNIT_ASSERT(target.at(4).empty());
  CPPUNIT_ASSERT(target.at(5).empty());
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_detect_known_issues() {
  std::map<std::string, bool> include_rules, exclude_rules;
  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block), rb4(new rule_block);
  rb1->_rule_name = "rule1";
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb2->_rule_name = "rule2";
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb3->_rule_name = "rule3";
  rb3->_resolution = RESOLVED_INCLUDED;
  rb3->_queried_by_python = true;
  rb4->_rule_name = "rule4";
  rb4->_resolution = RESOLVED_EXCLUDED;
  rb4->_queried_by_python = true;
  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf1->_updated_last_round = false;
  sf2->_blocks.push_back(rb3);
  sf2->_blocks.push_back(rb4);
  sf2->_updated_last_round = false;
  sf1->_included_files[boost::filesystem::path("mypath")] = sf2;
  include_rules["rule1"] = true;
  include_rules["rule2"] = true;
  exclude_rules["rule3"] = true;
  exclude_rules["rule4"] = true;

  // capture std::cout
  std::ostringstream observed;
  std::streambuf *previous_buffer(std::cout.rdbuf(observed.rdbuf()));

  try {
    sf1->detect_known_issues(include_rules, exclude_rules);
  } catch (...) {
    std::cout.rdbuf(previous_buffer);
    throw;
  }

  // reset std::cout
  std::cout.rdbuf(previous_buffer);

  std::string expected =
      "snakefile load summary\n"
      "----------------------\n"
      "total loaded candidate rules from snakefile(s): 3\n"
      "rules from snakefile(s) not excluded by configuration: 2\n"
      "\tthis number will be further reduced based on how many rules are present in the log\n";

  CPPUNIT_ASSERT(!observed.str().compare(expected));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_blocks() {
  std::list<boost::shared_ptr<rule_block> > result;
  boost::shared_ptr<rule_block> b1(new rule_block), b2(new rule_block);
  b1->_rule_name = "myrule";
  b1->_named_blocks.push_back(std::make_pair("input", " 'myinfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("output", " 'myoutfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("shell", " 'echo thing > {output}'"));
  b2->_code_chunk.push_back("localrules: myrule");
  snakemake_file sf;
  sf._blocks.push_back(b1);
  sf._blocks.push_back(b2);
  result = sf.get_blocks();
  CPPUNIT_ASSERT(result.size() == 2u);
  CPPUNIT_ASSERT(*(result.begin()) == b1);
  CPPUNIT_ASSERT(*(result.rbegin()) == b2);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_single_rule() {
  /*
    construct some dummy infrastructure to test the ability to distinguish between:
    - a rule that's included
    - a rule that's excluded
    - python supporting code
  */
  snakemake_file sf;
  boost::shared_ptr<rule_block> b1(new rule_block), b2(new rule_block), b3(new rule_block), b4(new rule_block),
      b5(new rule_block);
  b1->_code_chunk.push_back("if True:");
  b1->_resolution = RESOLVED_INCLUDED;
  b1->_queried_by_python = true;
  b2->_rule_name = "myrule";
  b2->_local_indentation = 4;
  b2->_named_blocks.push_back(std::make_pair("input", "\n            file1,"));
  b2->_resolution = RESOLVED_INCLUDED;
  b2->_queried_by_python = true;
  b3->_code_chunk.push_back("else:");
  b3->_resolution = RESOLVED_INCLUDED;
  b3->_queried_by_python = true;
  b4->_rule_name = "myrule";
  b4->_local_indentation = 4;
  b4->_resolution = RESOLVED_EXCLUDED;
  b4->_queried_by_python = true;
  b5->_rule_name = "otherrule";
  b5->_local_indentation = 0;
  b5->_named_blocks.push_back(std::make_pair("input", "\n        file2,"));
  b5->_resolution = RESOLVED_INCLUDED;
  b5->_queried_by_python = true;

  sf._blocks.push_back(b1);
  sf._blocks.push_back(b2);
  sf._blocks.push_back(b3);
  sf._blocks.push_back(b4);
  sf._blocks.push_back(b5);

  std::map<std::string, bool> ruleset;
  ruleset["myrule"] = true;
  ruleset["otherrule"] = true;
  std::ostringstream out;
  unsigned result = sf.report_single_rule(ruleset, out);
  CPPUNIT_ASSERT(result == 2);
  std::string expected =
      "if True:\n    rule myrule:\n        input:\n            file1,\n\n\n"
      "else:\n    pass\n\n\nrule otherrule:\n    input:\n        file2,\n\n\n";
  CPPUNIT_ASSERT(!out.str().compare(expected));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_fully_resolved() {
  snakemake_file sf;
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block);
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb3->_resolution = RESOLVED_INCLUDED;
  rb3->_queried_by_python = true;
  sf._blocks.push_back(rb1);
  sf._blocks.push_back(rb2);
  sf._blocks.push_back(rb3);
  CPPUNIT_ASSERT(sf.fully_resolved());
  rb3->_resolution = RESOLVED_EXCLUDED;
  CPPUNIT_ASSERT(sf.fully_resolved());
  rb3->_queried_by_python = false;
  CPPUNIT_ASSERT(!sf.fully_resolved());
  rb3->_queried_by_python = true;
  rb2->_resolution = UNRESOLVED;
  CPPUNIT_ASSERT(!sf.fully_resolved());
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_contains_blockers() {
  /*
    blockers are defined as:
    - any update the last time python tagging was attempted
    - any block that reports itself unresolved

    this is recursive through any dependent snakefiles
   */
  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block), rb4(new rule_block);
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb3->_resolution = RESOLVED_INCLUDED;
  rb3->_queried_by_python = true;
  rb4->_resolution = RESOLVED_INCLUDED;
  rb4->_queried_by_python = true;
  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf2->_blocks.push_back(rb3);
  sf2->_blocks.push_back(rb4);
  sf1->_included_files["mypath"] = sf2;
  // top-level file was updated previously
  sf1->_updated_last_round = true;
  sf2->_updated_last_round = false;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // dependent snakefile was updated previously
  sf1->_updated_last_round = false;
  sf2->_updated_last_round = true;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // rule in top-level file is not queried by python
  sf1->_updated_last_round = false;
  sf2->_updated_last_round = false;
  rb1->_queried_by_python = false;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // rule in top-level file is unresolved
  rb1->_queried_by_python = true;
  rb1->_resolution = UNRESOLVED;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // rule in dependent file is not queried by python
  rb1->_resolution = RESOLVED_EXCLUDED;
  rb3->_queried_by_python = false;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // rule in dependent file is unresolved
  rb3->_queried_by_python = true;
  rb3->_resolution = UNRESOLVED;
  CPPUNIT_ASSERT(sf1->contains_blockers());
  // everything is resolved
  rb3->_resolution = RESOLVED_EXCLUDED;
  CPPUNIT_ASSERT(!sf1->contains_blockers());
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_resolve_with_python() {
  /*
    this function emits dummy tag statements for a snakefile; runs python dryrun; captures tag
    logging data from the run; and updates rule status for everything that reported back.

    need:
    - a workspace; in theory, it just has added files/directories
    - top level of pipeline
    - run directory within pipeline
    - verbosity
    - a boolean controlling recursion behavior, which should always be true from external execution
   */
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path workspace = tmp_parent / "rwp_workspace";
  boost::filesystem::path pipeline_top = workspace;
  boost::filesystem::path pipeline_run = boost::filesystem::path(".");
  boost::filesystem::path snakefile = pipeline_top / "workflow/Snakefile";
  boost::filesystem::path unloaded_snakefile = pipeline_top / "workflow/rules/future.smk";
  bool verbose = true;
  bool disable_reporting = false;
  boost::filesystem::create_directories(workspace / "workflow/rules");

  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block), rb4(new rule_block),
      rb5(new rule_block);
  rb1->_rule_name = "rule1";
  rb1->_local_indentation = 0;
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb1->_python_tag = 1;
  rb2->_code_chunk.push_back("include: \"rules/include.smk\"");
  rb2->_local_indentation = 0;
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb2->_python_tag = 2;
  rb3->_rule_name = "rule2";
  rb3->_local_indentation = 0;
  rb3->_resolution = UNRESOLVED;
  rb3->_queried_by_python = false;
  rb3->_python_tag = 3;
  rb4->_code_chunk.push_back("include: \"future.smk\"");
  rb4->_local_indentation = 0;
  rb4->_resolution = UNRESOLVED;
  rb4->_queried_by_python = false;
  rb4->_python_tag = 4;
  rb5->_rule_name = "rule3";
  rb5->_local_indentation = 0;
  rb5->_resolution = UNRESOLVED;
  rb5->_queried_by_python = false;
  rb5->_python_tag = 5;

  sf1->_snakefile_relative_path = "workflow/Snakefile";
  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf1->_updated_last_round = true;
  *(sf1->_tag_counter) = 6;
  sf2->_blocks.push_back(rb3);
  sf2->_blocks.push_back(rb4);
  sf2->_blocks.push_back(rb5);
  sf2->_snakefile_relative_path = "workflow/rules/include.smk";
  sf2->_updated_last_round = false;
  sf1->_included_files[workspace / "workflow/rules/include.smk"] = sf2;

  std::string unloaded_snakefile_contents = "rule rule4:\n    input: \"input.txt\",\n\n";
  std::ofstream output;
  output.open(unloaded_snakefile.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write future snakefile for resolve_with_python test");
  }
  if (!(output << unloaded_snakefile_contents << std::endl)) {
    throw std::runtime_error("cannot write to future snakefile for resolve_with_python test");
  }
  output.close();

  // capture std::cout for cleanliness
  std::ostringstream endless_void;
  std::streambuf *previous_buffer(std::cout.rdbuf(endless_void.rdbuf()));

  // actually call the thing
  try {
    sf1->resolve_with_python(workspace, pipeline_top, pipeline_run, verbose, disable_reporting);
  } catch (...) {
    std::cout.rdbuf(previous_buffer);
    throw;
  }

  // reset std::cout
  std::cout.rdbuf(previous_buffer);

  // probe status of files and blocks
  CPPUNIT_ASSERT(!sf1->_updated_last_round);
  CPPUNIT_ASSERT(sf1->_included_files.size() == 1);
  CPPUNIT_ASSERT(sf1->_included_files.begin()->second == sf2);
  CPPUNIT_ASSERT(sf2->_updated_last_round);
  CPPUNIT_ASSERT(sf1->_blocks.size() == 2);
  CPPUNIT_ASSERT(sf2->_blocks.size() == 3);
  std::list<boost::shared_ptr<rule_block> >::iterator iter = sf2->_blocks.begin();
  CPPUNIT_ASSERT((*iter)->_queried_by_python);
  CPPUNIT_ASSERT((*iter)->_resolution == RESOLVED_INCLUDED);
  ++iter;
  CPPUNIT_ASSERT((*iter)->_queried_by_python);
  CPPUNIT_ASSERT((*iter)->_resolution == RESOLVED_INCLUDED);
  CPPUNIT_ASSERT(!(*iter)->_resolved_included_filename.string().compare("future.smk"));
  ++iter;
  CPPUNIT_ASSERT(!(*iter)->_queried_by_python);
  CPPUNIT_ASSERT((*iter)->_resolution == RESOLVED_EXCLUDED);
  CPPUNIT_ASSERT(sf2->_included_files.size() == 1);
  CPPUNIT_ASSERT(sf2->_included_files.begin()->second->_blocks.size() == 1);
  iter = sf2->_included_files.begin()->second->_blocks.begin();
  CPPUNIT_ASSERT(!(*iter)->_rule_name.compare("rule4"));
  CPPUNIT_ASSERT((*iter)->_resolution == UNRESOLVED);
  CPPUNIT_ASSERT(!(*iter)->_queried_by_python);
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_process_python_results() {
  /*
    this function handles the process of finding new files to include. if a snakefile has already
    been processed once, it recurses; otherwise, it conducts a full snakefile load/lexical parse/etc.
    of the newly-encountered snakefile.

    need:
    - a workspace
    - a path to the top level of the pipeline relative to the workspace
    - verbosity (probably turn on and just suppress)
    - the map of tag->value pairs pulled from the python dryrun log
    - full output path of target snakefile
   */
  boost::filesystem::path tmp_parent = boost::filesystem::path(std::string(_tmp_dir));
  boost::filesystem::path workspace = tmp_parent / "ppr_workspace";
  boost::filesystem::path pipeline_top = workspace;
  bool verbose = true;
  std::map<std::string, std::string> tag_values;
  boost::filesystem::path snakefile_path = workspace / "workflow/Snakefile";
  boost::filesystem::path snakefile_include = workspace / "workflow/rules/include.smk";
  boost::filesystem::create_directories(workspace / "workflow/rules");
  // first two snakefiles should already be loaded and tagged; third needs to be in file
  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block), rb4(new rule_block),
      rb5(new rule_block), rb6(new rule_block);
  rb1->_rule_name = "rule1";
  rb1->_named_blocks.push_back(std::make_pair("input", "\"input1.txt\","));
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb1->_python_tag = 1;
  rb2->_rule_name = "";
  rb2->_code_chunk.push_back("include: \"rules/include.smk\"");
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb2->_python_tag = 2;
  rb2->_resolved_included_filename = "rules/include.smk";
  rb3->_rule_name = "";
  rb3->_code_chunk.push_back("include: \"rules/fake.smk\"");
  rb3->_resolution = RESOLVED_INCLUDED;
  rb3->_queried_by_python = true;
  rb3->_python_tag = 3;
  rb4->_rule_name = "rule3";
  rb4->_named_blocks.push_back(std::make_pair("input", "\"input2.txt\","));
  rb4->_resolution = UNRESOLVED;
  rb4->_queried_by_python = false;
  rb4->_python_tag = 4;
  rb5->_rule_name = "";
  rb5->_code_chunk.push_back("pass");
  rb5->_resolution = UNRESOLVED;
  rb5->_queried_by_python = false;
  rb5->_python_tag = 5;
  rb6->_rule_name = "rule4";
  rb6->_named_blocks.push_back(std::make_pair("output", "\"output2.txt\","));
  rb6->_resolution = UNRESOLVED;
  rb6->_queried_by_python = false;
  rb6->_python_tag = 6;

  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf1->_blocks.push_back(rb3);
  sf2->_blocks.push_back(rb4);
  sf2->_blocks.push_back(rb5);
  sf2->_blocks.push_back(rb6);
  sf1->_included_files[workspace / "workflow/rules/fake.smk"] = sf2;

  tag_values["tag1"] = "";
  tag_values["tag2"] = "rules/include.smk";
  tag_values["tag3"] = "";
  tag_values["tag4"] = "";
  tag_values["tag5"] = "rules/future.smk";
  tag_values["tag6"] = "";

  std::string snakefile_contents = "rule rule5:\n    input: \"input5.txt\",\n\n";
  std::ofstream output;
  output.open(snakefile_include.string().c_str());
  if (!output.is_open()) {
    throw std::runtime_error("cannot write include.smk for python results processing test");
  }
  if (!(output << snakefile_contents << std::endl)) {
    throw std::runtime_error("cannot write to include.smk for python results processing test");
  }
  output.close();

  // capture std::cout for cleanliness
  std::ostringstream endless_void;
  std::streambuf *previous_buffer(std::cout.rdbuf(endless_void.rdbuf()));

  // actually call the thing
  try {
    sf1->process_python_results(workspace, pipeline_top, verbose, tag_values, snakefile_path);
  } catch (...) {
    std::cout.rdbuf(previous_buffer);
    throw;
  }

  // reset std::cout
  std::cout.rdbuf(previous_buffer);

  /*
    The following behaviors are expected:

    - it should understand that sf2 is already loaded, and iterate across it
    - it should trigger the load of sf3
   */
  CPPUNIT_ASSERT(sf2->_blocks.size() == 3);
  CPPUNIT_ASSERT(sf1->_included_files.size() == 2);
  CPPUNIT_ASSERT(sf1->_included_files.begin()->second == sf2);
  CPPUNIT_ASSERT(sf1->_included_files.rbegin()->second->_blocks.size() == 1);
  std::list<boost::shared_ptr<rule_block> >::iterator iter = sf1->_included_files.rbegin()->second->_blocks.begin();
  CPPUNIT_ASSERT(!(*iter)->_rule_name.compare("rule5"));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_capture_python_tag_values() {
  std::vector<std::string> input;
  std::map<std::string, std::string> output;
  input.push_back("some sort of infrastructure\n");
  input.push_back("tag111222\n");
  input.push_back("tag222333: valuevaluevalue\n");
  snakemake_file sf;
  sf.capture_python_tag_values(input, &output);
  CPPUNIT_ASSERT(output.size() == 2);
  CPPUNIT_ASSERT(output.find("tag111222") != output.end());
  CPPUNIT_ASSERT(output["tag1111222"].empty());
  CPPUNIT_ASSERT(output.find("tag222333") != output.end());
  CPPUNIT_ASSERT(!output["tag222333"].compare("valuevaluevalue"));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_postflight_checks() {
  // for the moment, this is just a dispatch to detect_known_issues
  std::map<std::string, bool> include_rules, exclude_rules;
  boost::shared_ptr<snakemake_file> sf1(new snakemake_file), sf2(new snakemake_file);
  boost::shared_ptr<rule_block> rb1(new rule_block), rb2(new rule_block), rb3(new rule_block), rb4(new rule_block);
  rb1->_rule_name = "rule1";
  rb1->_resolution = RESOLVED_INCLUDED;
  rb1->_queried_by_python = true;
  rb2->_rule_name = "rule2";
  rb2->_resolution = RESOLVED_INCLUDED;
  rb2->_queried_by_python = true;
  rb3->_rule_name = "rule3";
  rb3->_resolution = RESOLVED_INCLUDED;
  rb3->_queried_by_python = true;
  rb4->_rule_name = "rule4";
  rb4->_resolution = RESOLVED_EXCLUDED;
  rb4->_queried_by_python = true;
  sf1->_blocks.push_back(rb1);
  sf1->_blocks.push_back(rb2);
  sf1->_updated_last_round = false;
  sf2->_blocks.push_back(rb3);
  sf2->_blocks.push_back(rb4);
  sf2->_updated_last_round = false;
  sf1->_included_files[boost::filesystem::path("mypath")] = sf2;
  include_rules["rule1"] = true;
  include_rules["rule2"] = true;
  exclude_rules["rule3"] = true;
  exclude_rules["rule4"] = true;

  // capture std::cout
  std::ostringstream observed;
  std::streambuf *previous_buffer(std::cout.rdbuf(observed.rdbuf()));

  try {
    sf1->postflight_checks(include_rules, exclude_rules);
  } catch (...) {
    std::cout.rdbuf(previous_buffer);
    throw;
  }

  // reset std::cout
  std::cout.rdbuf(previous_buffer);

  std::string expected =
      "snakefile load summary\n"
      "----------------------\n"
      "total loaded candidate rules from snakefile(s): 3\n"
      "rules from snakefile(s) not excluded by configuration: 2\n"
      "\tthis number will be further reduced based on how many rules are present in the log\n";

  CPPUNIT_ASSERT(!observed.str().compare(expected));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_snakefile_relative_path() {
  snakemake_file sf;
  sf._snakefile_relative_path = boost::filesystem::path("mypath");
  CPPUNIT_ASSERT(!sf.get_snakefile_relative_path().string().compare("mypath"));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_loaded_files() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_set_update_status() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_report_rules() {}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name() {
  snakemake_file sf1;
  boost::shared_ptr<snakemake_file> sf2(new snakemake_file);
  boost::shared_ptr<rule_block> b0(new rule_block), b1(new rule_block), b2(new rule_block), b3(new rule_block);
  b0->_rule_name = "myrule";
  b0->_resolution = RESOLVED_EXCLUDED;
  b1->_rule_name = "myrule";
  b1->_base_rule_name = "top_level_rule";
  b1->_named_blocks.push_back(std::make_pair("input", " 'myinfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("output", " 'myoutfile.txt',"));
  b1->_named_blocks.push_back(std::make_pair("shell", " 'echo thing > {output}'"));
  b1->_resolution = RESOLVED_INCLUDED;
  b2->_code_chunk.push_back("localrules: myrule");
  b2->_resolution = RESOLVED_INCLUDED;
  b3->_rule_name = "hidden_rule";
  b3->_base_rule_name = "magical_treasure";
  b3->_named_blocks.push_back(std::make_pair("output", " 'nextoutfile.txt',"));
  b3->_named_blocks.push_back(std::make_pair("shell", " 'echo otherthing > {output}'"));
  b3->_resolution = RESOLVED_INCLUDED;
  sf1._blocks.push_back(b0);
  sf1._blocks.push_back(b1);
  sf1._blocks.push_back(b2);
  sf2->_blocks.push_back(b3);
  sf1._included_files["/path/to/file"] = sf2;
  std::string result = "";
  CPPUNIT_ASSERT(sf1.get_base_rule_name("myrule", &result));
  CPPUNIT_ASSERT(!result.compare("top_level_rule"));
  CPPUNIT_ASSERT(sf1.get_base_rule_name("hidden_rule", &result));
  CPPUNIT_ASSERT(!result.compare("magical_treasure"));
  CPPUNIT_ASSERT(!sf1.get_base_rule_name("fake_rule", &result));
}
void snakemake_unit_tests::snakemake_fileTest::test_snakemake_file_get_base_rule_name_null_pointer() {
  snakemake_file sf;
  sf.get_base_rule_name("fake_rule", NULL);
}

CPPUNIT_TEST_SUITE_REGISTRATION(snakemake_unit_tests::snakemake_fileTest);
