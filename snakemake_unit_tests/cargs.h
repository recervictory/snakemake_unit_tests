/*!
 @file cargs.h
 @brief command line argument handling
 @author Cameron Palmer
 @note requires boost::program_options library + headers
 @copyright Released under the MIT License.
 Copyright 2021 Cameron Palmer

 Thanks to
 https://www.boost.org/doc/libs/1_70_0/doc/html/program_options/tutorial.html#id-1.3.32.4.3
 */

#ifndef SNAKEMAKE_UNIT_TESTS_CARGS_H_
#define SNAKEMAKE_UNIT_TESTS_CARGS_H_

#include <stdexcept>
#include <string>
#include <vector>

#include "boost/program_options.hpp"

namespace snakemake_unit_tests {
/*!
  @class cargs
  @brief command line argument parser using boost::program_options
 */
class cargs {
 public:
  /*!
    @brief constructor with program arguments
    @param argc number of arguments including program name
    @param argv string array containing actual arguments
   */
  cargs(int argc, char **argv) : _desc("Recognized options") {
    initialize_options();
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, _desc), _vm);
    boost::program_options::notify(_vm);
  }
  /*!
    @brief copy constructor
    @param obj existing cargs object
   */
  cargs(const cargs &obj) : _desc(obj._desc), _vm(obj._vm) {}
  /*!
    \brief destructor
   */
  ~cargs() throw() {}

  /*!
    @brief set user help documentation and default values for parameters as
    needed

    Note the weird syntax with overloaded () operators. The lists are not
    separated by commas.
   */
  void initialize_options();

  /*!
    @brief determine whether the user has requested help documentation
    @return whether the user has requested help documentation

    This test is separate from whether the user has run the software with no
    flags
   */
  bool help() const { return compute_flag("help"); }

  /*!
    @brief get the top-level snakefile used for the full workflow
    @return name of and path to snakefile as a string

    this just points to the top-level snakefile. any modularized
    rules in secondary snakefiles are loaded in automatically
   */
  std::string get_snakefile() const {
    return compute_parameter<std::string>("snakefile");
  }

  /*!
    @brief get the snakemake log for the successful pipeline run that
    needs unit tests
    @return name of and path to file as a string

    I'll eventually come back here and annotate this with the full
    list of snakemake run settings that need to be used to make this
    log sufficiently informative
   */
  std::string get_snakemake_log() const {
    return compute_parameter<std::string>("snakemake-log");
  }

  /*!
    @brief get top-level directory under which tests should be installed
    @return top-level test directory

    as of this writing (March 2021), the default value for this in snakemake
    is '.tests' and that default is carried over here
   */
  std::string get_output_test_dir() const {
    return compute_parameter<std::string>("output-test-dir");
  }

  /*!
    @brief get optional multiple files (with relative paths) that will be
    installed alongside each unit test
    @return all provided relative paths (if any) in a string vector

    This flag can be specified multiple times, or none. Note that the intention
    is for this option to be unnecessary, but it's exposed as an option just
    in case people come up with corner cases that the main logic cannot handle
   */
  std::vector<std::string> get_added_files() const {
    return compute_parameter<std::vector<std::string> >("added-files", true);
  }

  /*!
    @brief get optional multiple directories (with relative paths) that will be
    installed alongside each unit test
    @return all provided relative directories (if any) in a string vector

    This flag can be specified multiple times, or none. Note that the intention
    is for this option to be unnecessary, but it's exposed as an option just
    in case people come up with corner  cases that the main logic cannot handle
   */
  std::vector<std::string> get_added_directories() const {
    return compute_parameter<std::vector<std::string> >("added-directories",
                                                        true);
  }

  /*!
    @brief find status of arbitrary flag
    @param tag name of flag
    @return whether the flag is set

    This is the underlying accessor function used by the custom flag accessors,
    and can be used for arbitrary flag additions if you don't want to type out
    the custom access functions or want to allow dynamic specification from a
    config file.
   */
  bool compute_flag(const std::string &tag) const { return _vm.count(tag); }
  /*!
    @brief find value of arbitrary parameter
    @tparam value_type class to which the value should be cast
    @param tag name of parameter
    @param optional whether the parameter should be allowed to be unset
    @return value of parameter if specified

    @warning throws exception if parameter was not set and had no default
   */
  template <class value_type>
  value_type compute_parameter(const std::string &tag,
                               bool optional = false) const {
    if (_vm.count(tag)) {
      return _vm[tag].as<value_type>();
    } else if (optional) {
      return value_type();
    } else {
      throw std::domain_error("cargs: requested parameter \"" + tag +
                              "\" unset");
    }
  }

  /*!
    @brief report help documentation to arbitrary output stream
    @param out stream to which to write help documentation

    Parameter should probably be std::cout or std::cerr at your preference.
   */
  void print_help(std::ostream &out) {
    if (!(out << _desc))
      throw std::domain_error("cargs::print_help: unable to write to stream");
  }

 private:
  /*!
    @brief default constructor
    @warning disabled
   */
  cargs() { throw std::domain_error("cargs: do not use default constructor"); }
  boost::program_options::options_description
      _desc;  //!< help documentation string
  boost::program_options::variables_map
      _vm;  //!< storage of parsed command line settings
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_CARGS_H_
