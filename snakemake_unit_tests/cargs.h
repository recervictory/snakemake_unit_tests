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

#include <fstream>
#include <map>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"
#include "snakemake_unit_tests/yaml_reader.h"
#include "yaml-cpp/yaml.h"

namespace snakemake_unit_tests {
/*!
  @brief take entries of a vector and fill a map with them as keys
  @tparam value_type storage type of key
  @param vec input vector of values
  @return map with vector values as keys

  all map values are set to true
 */
template <class value_type>
std::map<value_type, bool> vector_to_map(const std::vector<value_type> &vec) {
  std::map<value_type, bool> res;
  for (typename std::vector<value_type>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter) {
    res[*iter] = true;
  }
  return res;
}
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
        update_all(false),
        update_snakefiles(false),
        update_added_content(false),
        update_config(false),
        update_inputs(false),
        update_outputs(false),
        update_pytest(false),
        include_entire_dag(false),
        config_filename(""),
        output_test_dir(""),
        snakefile(""),
        pipeline_top_dir(""),
        pipeline_run_dir(""),
        inst_dir(""),
        snakemake_log("") {}
  /*!
    @brief copy constructor
    @param obj existing params object
  */
  params(const params &obj)
      : verbose(obj.verbose),
        update_all(obj.update_all),
        update_snakefiles(obj.update_snakefiles),
        update_added_content(obj.update_added_content),
        update_config(obj.update_config),
        update_inputs(obj.update_inputs),
        update_outputs(obj.update_outputs),
        update_pytest(obj.update_pytest),
        include_entire_dag(obj.include_entire_dag),
        config_filename(obj.config_filename),
        config(obj.config),
        output_test_dir(obj.output_test_dir),
        snakefile(obj.snakefile),
        pipeline_top_dir(obj.pipeline_top_dir),
        pipeline_run_dir(obj.pipeline_run_dir),
        inst_dir(obj.inst_dir),
        snakemake_log(obj.snakemake_log),
        added_files(obj.added_files),
        added_directories(obj.added_directories),
        include_rules(obj.include_rules),
        exclude_rules(obj.exclude_rules),
        exclude_patterns(obj.exclude_patterns),
        comparators(obj.comparators) {}
  /*!
    @brief destructor
   */
  ~params() throw() {}
  /*!
    @brief report settings to yaml file
    @param filename name of file to write

    the reported settings will be the result of
    resolving the input config and any command line flags,
    but will not report any binary flags

    TODO(cpalmer718): check if BB wants binary flags
   */
  void report_settings(const boost::filesystem::path &filename) const;
  /*!
    @brief emit map contents as key->sequence in a YAML::Map
    @param out YAML Emitter to which to report YAML content
    @param data keys for output sequence
    @param key YAML key corresponding to this sequence

    note that null sequences are emitted as "key: ~" in YAML style

    @warning this is expected to be run on a YAML::Emitter that has
    already received a YAML::BeginMap token. if not, the output
    format will be undefined and probably not what you want.
   */
  void emit_yaml_map(YAML::Emitter *out, const std::map<std::string, bool> &data, const std::string &key) const;
  /*!
    @brief emit vector contents as key->sequence in a YAML::Map
    @param out YAML Emitter to which to report YAML content
    @param data keys for output sequence
    @param key YAML key corresponding to this sequence

    note that null sequences are emitted as "key: ~" in YAML style

    @warning this is expected to be run on a YAML::Emitter that has
    already received a YAML::BeginMap token. if not, the output
    format will be undefined and probably not what you want.
   */
  void emit_yaml_vector(YAML::Emitter *out, const std::vector<boost::filesystem::path> &data,
                        const std::string &key) const;
  /*!
    @brief provide verbose logging output
   */
  bool verbose;
  /*!
    @brief update all parts of output unit tests
   */
  bool update_all;
  /*!
    @brief update snakefiles in unit tests
   */
  bool update_snakefiles;
  /*!
    @brief update added files and directories in unit tests
   */
  bool update_added_content;
  /*!
    @brief update configuration report emittd to output unit
    test directory
   */
  bool update_config;
  /*!
    @brief update rule inputs in unit tests
   */
  bool update_inputs;
  /*!
    @brief update rule outputs in unit tests
   */
  bool update_outputs;
  /*!
    @brief update pytest infrastructure in output directories
   */
  bool update_pytest;
  /*!
    @brief spike entire dag into each output snakefile, instead of
   just the rule(s) being tested

   designed as a last ditch solution for unsupported calls to `rules.`.
   will cause significant performance degradation while running the
   actual tests.
   */
  bool include_entire_dag;
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
  boost::filesystem::path pipeline_top_dir;
  /*!
    @brief execution directory of pipeline, relative to
    top level directory of pipeline
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
    @brief user-defined rulenames to include in test generation
   */
  std::map<std::string, bool> include_rules;
  /*!
    @brief user-defined rulenames to skip in test generation
   */
  std::map<std::string, bool> exclude_rules;
  /*!
    @brief user-defined file/path regular expressions to exclude from pytest comparison
   */
  std::map<std::string, bool> exclude_patterns;
  /*!
    @brief user-defined file extensions to flag as needing binary comparison
   */
  YAML::Node comparators;
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
  cargs(int argc, const char **const argv) : _desc("Recognized options") {
    initialize_options();
    boost::program_options::store(boost::program_options::parse_command_line(argc, argv, _desc), _vm);
    boost::program_options::notify(_vm);
    _permitted_flags["help"] = true;
    _permitted_flags["verbose"] = true;
    _permitted_flags["include-entire-dag"] = true;
    _permitted_flags["update-all"] = true;
    _permitted_flags["update-pytest"] = true;
    _permitted_flags["update-added-content"] = true;
    _permitted_flags["update-snakefiles"] = true;
    _permitted_flags["update-config"] = true;
    _permitted_flags["update-inputs"] = true;
    _permitted_flags["update-outputs"] = true;
  }
  /*!
    @brief copy constructor
    @param obj existing cargs object
   */
  cargs(const cargs &obj) : _desc(obj._desc), _vm(obj._vm), _permitted_flags(obj._permitted_flags) {}
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
  std::string get_config_yaml() const { return compute_parameter<std::string>("config", true); }

  /*!
    @brief get the top-level snakefile used for the full workflow
    @return name of and path to snakefile as a string

    this just points to the top-level snakefile. any modularized
    rules in secondary snakefiles are loaded in automatically
   */
  std::string get_snakefile() const { return compute_parameter<std::string>("snakefile", true); }

  /*!
    @brief get the snakemake log for the successful pipeline run that
    needs unit tests
    @return name of and path to file as a string

    I'll eventually come back here and annotate this with the full
    list of snakemake run settings that need to be used to make this
    log sufficiently informative
   */
  std::string get_snakemake_log() const { return compute_parameter<std::string>("snakemake-log", true); }

  /*!
    @brief get top-level directory under which tests should be installed
    @return top-level test directory

    as of this writing (March 2021), the default value for this in snakemake
    is '.tests' and that default is carried over here
   */
  std::string get_output_test_dir() const { return compute_parameter<std::string>("output-test-dir", true); }

  /*!
    @brief get top-level directory of test pipeline
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
  std::string get_pipeline_top_dir() const { return compute_parameter<std::string>("pipeline-top-dir", true); }
  /*!
    @brief get directory of actual pipeline run, relative to top-level
    pipeline directory.
    @return directory of actual pipeline run relative to top-level
    pipeline directory.

    this parameter is optional. if not specified, it will be assumed
    to be '.'; that is, that the pipeline was run in the top level of
    the pipeline installation itself. a valid option here might be
    'workflows/rnaseq' or equivalent.
   */
  std::string get_pipeline_run_dir() const { return compute_parameter<std::string>("pipeline-run-dir", true); }

  /*!
    @brief get path to inst directory for this copy of snakemake_unit_tests
    @return path to (including 'inst') inst directory from snakemake_unit_tests
    repo

    once this is bundled for conda, this will default to
    $CONDA_PREFIX/share/snakemake_unit_tests/inst and the user's life will
    improve concomitantly.
   */
  std::string get_inst_dir() const { return compute_parameter<std::string>("inst-dir", true); }

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
    return compute_parameter<std::vector<std::string> >("added-directories", true);
  }

  /*!
    @brief get optional rule names to include in testing
    @return vector of all provided rules to include from test output
   */
  std::vector<std::string> get_include_rules() const {
    return compute_parameter<std::vector<std::string> >("include-rules", true);
  }

  /*!
    @brief get optional rule names to exclude from testing
    @return vector of all provided rules to exclude from test output
   */
  std::vector<std::string> get_exclude_rules() const {
    return compute_parameter<std::vector<std::string> >("exclude-rules", true);
  }

  /*!
    @brief get user flag for overriding default behavior and adding entire DAG
    to synthetic snakefiles
    @return whether the user wants the full DAG

    the user should only activate this when default `rules.` detection has
    failed due to wrapping `rules.` calls under defined function blocks or
    other infrastructure. this will potentially massively slow down execution,
    and should only be used for individual problematic rules.
   */
  bool include_entire_dag() const { return compute_flag("include-entire-dag"); }

  /*!
    @brief get user flag for updating all parts of unit tests
    @return whether the user wants a full replacement of all unit test content
   */
  bool update_all() const { return compute_flag("update-all"); }

  /*!
    @brief get user flag for updating existing tests in place (snakefiles only)
    @return whether the user wants to update existing tests' snakefiles
   */
  bool update_snakefiles() const { return compute_flag("update-snakefiles"); }

  /*!
    @brief get user flag for updating added files and directories in place
    @return whether the user wants to update existing tests' added files and
    directories
   */
  bool update_added_content() const { return compute_flag("update-added-content"); }
  /*!
    @brief get user flag for updating config report to unit directory top level
    @return whether the user wants to update run configuration report based on
    the current settings

    the distinction here is that there are definitely some cases in which
    the user won't want to update config based on some test or rule subset
    or different log they're working with for testing purposes
   */
  bool update_config() const { return compute_flag("update-config"); }
  /*!
    @brief get user flag for updating unit tests' inputs
    @return whether the user wants to update existing tests' inputs

    @warning you probably don't want to do this, unless you're updating
    everything
   */
  bool update_inputs() const { return compute_flag("update-inputs"); }
  /*!
    @brief get user flag for updating unit tests' outputs
    @return whether the user wants to update existing tests' outputs

    @warning you probably don't want to do this, unless you're updating
    everything
   */
  bool update_outputs() const { return compute_flag("update-outputs"); }

  /*!
    @brief get user flag for updating pytest infrastructure
    @return whether the user wants to update existing tests' pytest content
   */
  bool update_pytest() const { return compute_flag("update-pytest"); }

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
  bool compute_flag(const std::string &tag) const {
    /*
      because of the way flags work, the optional handler in compute_parameter
      doesn't have a good direct analogue here. is it reasonable to have
      a hard list of permitted flags, and screen the parameter to this
      function against them?
     */
    if (_permitted_flags.find(tag) == _permitted_flags.end()) {
      throw std::logic_error("invalid flag requested from argument parser: \"" + tag + "\"");
    }
    return _vm.count(tag);
  }

  /*!
    @brief find value of arbitrary parameter
    @tparam value_type class to which the value should be cast
    @param tag name of parameter
    @param optional whether the parameter should be allowed to be unset
    @return value of parameter if specified

    @warning throws exception if parameter was not set and had no default
   */
  template <class value_type>
  value_type compute_parameter(const std::string &tag, bool optional = false) const {
    if (_vm.count(tag)) {
      return _vm[tag].as<value_type>();
    } else if (optional) {
      return value_type();
    } else {
      throw std::domain_error("cargs: requested parameter \"" + tag + "\" unset");
    }
  }

  /*!
    @brief report help documentation to arbitrary output stream
    @param out stream to which to write help documentation

    Parameter should probably be std::cout or std::cerr at your preference.
   */
  void print_help(std::ostream &out) {
    if (!(out << _desc)) throw std::domain_error("cargs::print_help: unable to write to stream");
  }

 private:
  friend class cargsTest;
  /*!
    @brief default constructor
    @warning disabled
   */
  cargs() { throw std::domain_error("cargs: do not use default constructor"); }
  boost::program_options::options_description _desc;  //!< help documentation string
  boost::program_options::variables_map _vm;          //!< storage of parsed command line settings
  std::map<std::string, bool> _permitted_flags;       //!< lookup of permitted indicator flags
  /*!
    @brief if the first parameter is nonempty, return it; otherwise return
    the second
    @param cli_entry the value for the parameter given on the command line
    @param params_entry the value for the parameter loaded from the yaml config
    @return the resolved value from the two provided
  */
  boost::filesystem::path override_if_specified(const std::string &cli_entry,
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
  void add_contents(const std::vector<std::string> &cli_entries, std::vector<value_type> *params_entries) const {
    if (!params_entries) throw std::runtime_error("null pointer to add_contents");
    for (std::vector<std::string>::const_iterator iter = cli_entries.begin(); iter != cli_entries.end(); ++iter) {
      params_entries->push_back(value_type(*iter));
    }
  }
  /*!
    @brief append any CLI entries for a multitoken parameter to
    those already found in the config yaml
    @tparam value_type class to cast command line strings into (can just be
    strings)
    @param cli_entries values for the parameter found on the command line
    @param params_entries values for the parameter found in the config yaml,
    as keys in a map
   */
  template <class value_type>
  void add_contents(const std::vector<std::string> &cli_entries, std::map<value_type, bool> *params_entries) const {
    if (!params_entries) throw std::runtime_error("null pointer to add_contents");
    for (std::vector<std::string>::const_iterator iter = cli_entries.begin(); iter != cli_entries.end(); ++iter) {
      params_entries->insert(std::make_pair(value_type(*iter), true));
    }
  }
  /*!
    @brief make sure a path isn't just an empty string
    @param p path to check
    @param msg name of parameter flag for error messages
   */
  void check_nonempty(const boost::filesystem::path &p, const std::string &msg) const;
  /*!
    @brief given a path, enforce it being a regular file
    @param p input candidate file
    @param prefix optional prefix for actual file; can be empty
    @param msg name of parameter flag for error messages
   */
  void check_regular_file(const boost::filesystem::path &p, const boost::filesystem::path &prefix,
                          const std::string &msg) const;
  /*!
    @brief given a path, enforce it being a directory, and
    gently clean up its format
    @param p input candidate directory
    @param prefix optional prefix for actual directory; can be empty
    @param msg name of parameter flag for error messages
   */
  void check_and_fix_dir(boost::filesystem::path *p, const boost::filesystem::path &prefix,
                         const std::string &msg) const;

  /*!
    @brief cast entries of a vector into something else
    @tparam target type for conversion
    @param vec vector of strings to convert
    @return converted vector

    there are definitely less silly ways of doing this
   */
  template <class value_type>
  std::vector<value_type> vector_convert(const std::vector<std::string> &vec) const {
    std::vector<value_type> res;
    for (std::vector<std::string>::const_iterator iter = vec.begin(); iter != vec.end(); ++iter) {
      res.push_back(value_type(*iter));
    }
    return res;
  }
};
}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_CARGS_H_
