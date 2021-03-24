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

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "snakemake_unit_tests/yaml_reader.h"

namespace snakemake_unit_tests {
/*!
  @class params
  @brief resolved set of parameters to control program operation
 */
class params {
 public:
  /*!
    @brief constructor
   */
  params()
      : verbose(false),
        config_filename(""),
        output_test_dir(""),
        snakefile(""),
        pipeline_run_dir(""),
        inst_dir(""),
        snakemake_log("") {}
  /*!
    @brief copy constructor
    @param obj existing params object
  */
  params(const params &obj)
      : verbose(obj.verbose),
        config_filename(obj.config_filename),
        config(obj.config),
        output_test_dir(obj.output_test_dir),
        snakefile(obj.snakefile),
        pipeline_run_dir(obj.pipeline_run_dir),
        inst_dir(obj.inst_dir),
        snakemake_log(obj.snakemake_log),
        added_files(obj.added_files),
        added_directories(obj.added_directories),
        exclude_rules(obj.exclude_rules) {}
  /*!
    @brief destructor
   */
  ~params() throw() {}
  /*!
    @brief provide verbose logging output
   */
  bool verbose;
  /*!
    @brief name of yaml configuration file
   */
  boost::filesystem::path config_filename;
  /*!
    @brief parsed contents of yaml configuration file
   */
  snakemake_unit_tests::yaml_reader config;
  /*!
    @brief directory to which to write unit tests
   */
  boost::filesystem::path output_test_dir;
  /*!
    @brief top-level snakefile for successful pipeline run
   */
  boost::filesystem::path snakefile;
  /*!
    @brief top-level directory of successful pipeline run
   */
  boost::filesystem::path pipeline_run_dir;
  /*!
    @brief path to inst directory of snakemake_unit_tests
   */
  boost::filesystem::path inst_dir;
  /*!
    @brief name of log file of successful pipeline run
   */
  boost::filesystem::path snakemake_log;
  /*!
    @brief user-defined added files to place in test workspaces
   */
  std::vector<boost::filesystem::path> added_files;
  /*!
    @brief user-defined directories to recursively place in test workspaces
   */
  std::vector<boost::filesystem::path> added_directories;
  /*!
    @brief user-defined rulenames to skip in test generation
   */
  std::vector<std::string> exclude_rules;
};

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
    @brief deal with parameter settings, across CLI and config yaml
    @return params object containing consistent parameter settings

    note that this should be called after initialize_options(), and will
    have fairly lackluster effects otherwise lol
   */
  params set_parameters() const;

  /*!
    @brief determine whether the user has requested help documentation
    @return whether the user has requested help documentation

    This test is separate from whether the user has run the software with no
    flags
   */
  bool help() const { return compute_flag("help"); }

  /*!
    @brief get user-specified config yaml file,
    containing all the various parameters they've specified
    so they don't have to write out the horrible command
    line call each time
    @return string filename of config yaml file
   */
  std::string get_config_yaml() const {
    return compute_parameter<std::string>("config", true);
  }

  /*!
    @brief get the top-level snakefile used for the full workflow
    @return name of and path to snakefile as a string

    this just points to the top-level snakefile. any modularized
    rules in secondary snakefiles are loaded in automatically
   */
  std::string get_snakefile() const {
    return compute_parameter<std::string>("snakefile", true);
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
    return compute_parameter<std::string>("snakemake-log", true);
  }

  /*!
    @brief get top-level directory under which tests should be installed
    @return top-level test directory

    as of this writing (March 2021), the default value for this in snakemake
    is '.tests' and that default is carried over here
   */
  std::string get_output_test_dir() const {
    return compute_parameter<std::string>("output-test-dir", true);
  }

  /*!
    @brief get top-level directory under which actual pipeline was run
    @return top-level run directory

    this parameter is optional. if not specified, it will be computed
    as the directory above the one the Snakefile (specified from --snakefile).
    this assumes therefore that the pipeline is configured such that the
    top-level snakefile is installed in ~/workflow.

    if specified, be cautious to ensure that this directory and the snakefile
    are provided from corresponding pipeline installations, or undefined
    behaviors may occur. but you can use this to provide base pipeline
    directories for non-compliant (but still valid) snakemake configurations,
    such as when the file is in ~/Snakefile instead of ~/workflow/Snakefile.
   */
  std::string get_pipeline_dir() const {
    return compute_parameter<std::string>("pipeline-dir", true);
  }

  /*!
    @brief get path to inst directory for this copy of snakemake_unit_tests
    @return path to (including 'inst') inst directory from snakemake_unit_tests
    repo

    once this is bundled for conda, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst and the user's life will
    improve concomitantly.
   */
  std::string get_inst_dir() const {
    return compute_parameter<std::string>("inst-dir", true);
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
    @brief get optional rule names to exclude from testing
    @return vector of all provided rules to exclude from test output
   */
  std::vector<std::string> get_exclude_rules() const {
    return compute_parameter<std::vector<std::string> >("exclude-rules", true);
  }

  /*!
    @brief determine whether the user has requested verbose logging output
    @return whether the user has requested verbose logging output
   */
  bool verbose() const { return compute_flag("verbose"); }

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
  /*!
    @brief if the first parameter is nonempty, return it; otherwise return
    the second
    @param cli_entry the value for the parameter given on the command line
    @param params_entry the value for the parameter loaded from the yaml config
    @return the resolved value from the two provided
  */
  boost::filesystem::path override_if_specified(
      const std::string &cli_entry,
      const boost::filesystem::path &params_entry) const;

  /*!
    @brief append any CLI entries for a multitoken parameter to
    those already found in the config yaml
    @tparam value_type class to cast command line strings into (can just be
    strings)
    @param cli_entries values for the parameter found on the command line
    @param params_entries values for the parameter found in the config yaml
   */
  template <class value_type>
  void add_contents(const std::vector<std::string> &cli_entries,
                    std::vector<value_type> *params_entries) const {
    if (!params_entries)
      throw std::runtime_error("null pointer to add_contents");
    for (std::vector<std::string>::const_iterator iter = cli_entries.begin();
         iter != cli_entries.end(); ++iter) {
      params_entries->push_back(value_type(*iter));
    }
  }
  /*!
    @brief make sure a path isn't just an empty string
    @param p path to check
    @param msg name of parameter flag for error messages
   */
  void check_nonempty(const boost::filesystem::path &p,
                      const std::string &msg) const;
  /*!
    @brief given a path, enforce it being a regular file
    @param p input candidate file
    @param prefix optional prefix for actual file; can be empty
    @param msg name of parameter flag for error messages
   */
  void check_regular_file(const boost::filesystem::path &p,
                          const boost::filesystem::path &prefix,
                          const std::string &msg) const;
  /*!
    @brief given a path, enforce it being a directory, and
    gently clean up its format
    @param p input candidate directory
    @param prefix optional prefix for actual directory; can be empty
    @param msg name of parameter flag for error messages
   */
  void check_and_fix_dir(boost::filesystem::path *p,
                         const boost::filesystem::path &prefix,
                         const std::string &msg) const;

  /*!
    @brief cast entries of a vector into something else
    @tparam target type for conversion
    @param vec vector of strings to convert
    @return converted vector

    there are definitely less silly ways of doing this
   */
  template <class value_type>
  std::vector<value_type> vector_convert(
      const std::vector<std::string> &vec) const {
    std::vector<value_type> res;
    for (std::vector<std::string>::const_iterator iter = vec.begin();
         iter != vec.end(); ++iter) {
      res.push_back(value_type(*iter));
    }
    return res;
  }
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_CARGS_H_
