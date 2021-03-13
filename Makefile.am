bin_PROGRAMS = snakemake_unit_tests.out

AM_CXXFLAGS = $(BOOST_CPPFLAGS) -ggdb -Wall -std=c++17

snakemake_unit_tests_out_SOURCES = snakemake_unit_tests/cargs.cc snakemake_unit_tests/cargs.h snakemake_unit_tests/main.cc
snakemake_unit_tests_out_LDADD = $(BOOST_LDFLAGS) -lboost_program_options -lboost_system -lboost_filesystem -lyaml-cpp
dist_doc_DATA = README
ACLOCAL_AMFLAGS = -I m4
## TAP support
TEST_LOG_DRIVER = env AM_TAP_AWK='$(AWK)' $(SHELL) \
                  $(top_srcdir)/tap-driver.sh
TESTS = tests/placeholder.test
EXTRA_DIST = $(TESTS)
