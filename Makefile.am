bin_PROGRAMS = snakemake_unit_tests.out

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -ggdb -Wall -std=c++17 -DBOOST_FILESYSTEM_NO_DEPRECATED

snakemake_unit_tests_out_SOURCES = snakemake_unit_tests/cargs.cc snakemake_unit_tests/cargs.h snakemake_unit_tests/main.cc snakemake_unit_tests/rule_block.cc snakemake_unit_tests/rule_block.h snakemake_unit_tests/snakemake_file.cc snakemake_unit_tests/snakemake_file.h snakemake_unit_tests/solved_rules.cc snakemake_unit_tests/solved_rules.h snakemake_unit_tests/utilities.cc snakemake_unit_tests/utilities.h snakemake_unit_tests/yaml_reader.cc snakemake_unit_tests/yaml_reader.h
snakemake_unit_tests_out_LDADD = $(BOOST_LDFLAGS) -lboost_program_options -lboost_system -lboost_filesystem -lboost_regex -lyaml-cpp
dist_doc_DATA = README
ACLOCAL_AMFLAGS = -I m4
## TAP support
TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh
TESTS = tests/test0.test \
	tests/test1.test \
	tests/test2.test \
	tests/test3.test \
	tests/test4.test \
	tests/test5.test \
	tests/test6.test \
	tests/test7.test \
	tests/test8.test \
	tests/test9.test \
	tests/test10.test \
	tests/test11.test \
	tests/test12.test \
	tests/test13.test \
	tests/test14.test \
	tests/test15.test \
	tests/test_config_defaults.test \
	tests/test_config_override_output_dir.test \
	tests/test_config_override_snakemake_log.test \
	tests/test_nonstandard_snakefile_location.test \
	tests/test_update_snakefile.test
EXTRA_DIST = $(TESTS)
