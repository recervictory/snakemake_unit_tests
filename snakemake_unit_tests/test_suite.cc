/*!
  \file test_suite.cc
  \brief run class unit tests for snakemake_unit_tests
  \author Cameron Palmer
  \copyright Released under the MIT License. Copyright 2021 Cameron Palmer.
 */

#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>

int main(int argc, char **argv) {
  CppUnit::TextUi::TestRunner runner;
  CppUnit::TestFactoryRegistry &registry =
      CppUnit::TestFactoryRegistry::getRegistry();
  runner.addTest(registry.makeTest());
  bool wasSuccessful = runner.run("", false);
  return !wasSuccessful;
}
