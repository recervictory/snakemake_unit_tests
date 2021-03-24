/*!
  @file yaml_reader.h
  @brief interface for yaml config files
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer.
 */

#ifndef SNAKEMAKE_UNIT_TESTS_YAML_READER_H_
#define SNAKEMAKE_UNIT_TESTS_YAML_READER_H_

#include <string>
#include <utility>
#include <vector>

#include "yaml-cpp/yaml.h"

namespace snakemake_unit_tests {
/*!
  @class yaml_reader
  @brief provide somewhat higher level interface
  with yaml-cpp nodes
 */
class yaml_reader {
 public:
  /*!
    @brief default constructor
   */
  yaml_reader() {}
  /*!
    @brief constructor: load yaml file
    @param filename name of yaml file to (attempt to) load
   */
  explicit yaml_reader(const std::string &filename) { load_file(filename); }
  /*!
    @brief destructor
   */
  ~yaml_reader() throw() {}
  /*!
    @brief load yaml from file
    @param filename name of yaml file to (attempt to) load
   */
  void load_file(const std::string &filename);
  /*!
    @brief get a single value corresponding to a single query
    @param query key to query in yaml node
    @return value of key in file, cast as std::string
   */
  std::string get_entry(const std::string &query) const {
    std::vector<std::string> queries;
    queries.push_back(query);
    return get_entry(queries);
  }
  /*!
    @brief get a single value corresponding to an arbitrarily
    deep query
    @param queries ordered set of keys to query in node
    @return value of full query in file, cast as std::string
   */
  std::string get_entry(const std::vector<std::string> &queries) const {
    std::vector<std::string> all_results;
    all_results = get_sequence(queries);
    if (all_results.size() != 1) {
      std::ostringstream o;
      o << *queries.begin();
      for (unsigned i = 1; i <= queries.size(); ++i) {
        o << "::" << queries.at(i);
      }
      throw std::runtime_error("invalid number of results for entry query " +
                               o.str() + "\": found " +
                               std::to_string(all_results.size()));
    }
    return *all_results.begin();
  }
  /*!
    @brief get a sequence of values corresponding to a single query
    @param query key to query in yaml node
    @return vector of values corresponding to query

    this is: config["key"]

    where the corresponding config entry is:

    key:
      - val1
      - val2
      - val3
   */
  std::vector<std::string> get_sequence(const std::string &query) const {
    std::vector<std::string> queries;
    queries.push_back(query);
    return get_sequence(queries);
  }
  /*!
    @brief get a sequence of values corresponding to an arbitrarily
    deep query
    @param queries ordered set of keys to query in node
    @return vector of values corresponding to full query

    this is: config["key1"]{["key2"]...}

    where the corresponding config entry is:

    key1:
      key2:
        key3:
          - val1
          - val2
          - val3
   */
  std::vector<std::string> get_sequence(
      const std::vector<std::string> &queries) const;
  /*!
    @brief get map entries corresponding to a query
    @param query key to query in node
    @return vector of key:value pairs
   */
  std::vector<std::pair<std::string, std::string> > get_map(
      const std::string &query) const {
    std::vector<std::string> queries;
    queries.push_back(query);
    return get_map(queries);
  }
  /*!
    @brief get map entries corresponding to a series of queries
    @param queries ordered set of keys to query in node
    @return vector of key:value pairs corresponding to full query
   */
  std::vector<std::pair<std::string, std::string> > get_map(
      const std::vector<std::string> &queries) const;
  /*!
    @brief get arbitrary YAML Node corresponding to a query
    @param query key to query in node
    @return Node result of query
   */
  YAML::Node get_node(const std::string &query) const {
    std::vector<std::string> queries;
    queries.push_back(query);
    return get_node(queries);
  }
  /*!
    @brief get arbitrary YAML Node corresponding to a series of queries
    @param queries ordered set of keys to query in node
    @return Node result of full query
   */
  YAML::Node get_node(const std::vector<std::string> &queries) const;
  /*!
    @brief determine whether key exists in Node
    @param query key to query in node
    @return whether the key exists
   */
  bool query_valid(const std::string &query) const {
    std::vector<std::string> queries;
    queries.push_back(query);
    return query_valid(queries);
  }
  /*!
    @brief determine whether series of keys exists in Node
    @param queries ordered set of keys to query in node
    @return whether the series of keys exists
   */
  bool query_valid(const std::vector<std::string> &queries) const;

 private:
  /*!
    @brief apply an arbitrary query to a node
    @param queries keys to search in node
    @param current top-level node to query
    @param next result of query in current node
   */
  void apply_queries(const std::vector<std::string> &queries,
                     YAML::Node *current, YAML::Node *next) const;
  /*!
    @brief top level node representing config file, usually
   */
  YAML::Node _data;
};

}  // namespace snakemake_unit_tests

#endif  // SNAKEMAKE_UNIT_TESTS_YAML_READER_H_
