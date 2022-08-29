/*!
  @file yaml_reader.cc
  @brief implementation of yaml_reader class
  @copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer.
*/

#include "snakemake_unit_tests/yaml_reader.h"

bool snakemake_unit_tests::yaml_reader::operator==(const yaml_reader &obj) const {
  YAML::Emitter out1, out2;
  out1 << _data;
  out2 << obj._data;
  return !std::string(out1.c_str()).compare(std::string(out2.c_str()));
}

void snakemake_unit_tests::yaml_reader::load_file(const std::string &filename) {
  // just use the yaml-cpp method for this operation
  _data = YAML::LoadFile(filename.c_str());
}

std::vector<std::string> snakemake_unit_tests::yaml_reader::get_sequence(
    const std::vector<std::string> &queries) const {
  // copy the top node of the file, lest it get scoped out of existence
  YAML::Node current = YAML::Clone(_data), next;
  std::vector<std::string> res;
  // query the copy using the provided keys
  apply_queries(queries, &current, &next);
  // handle detected type
  if (current.Type() == YAML::NodeType::Scalar) {
    // cast a Scalar as a string and return it
    res.resize(1);
    res.at(0) = current.as<std::string>();
  } else if (current.Type() == YAML::NodeType::Sequence) {
    // cast the entire Sequence as strings and return it
    res.reserve(current.size());
    for (YAML::const_iterator iter = current.begin(); iter != current.end(); ++iter) {
      res.push_back(iter->as<std::string>());
    }
  } else {
    // the user asked for a Sequence tho
    throw std::runtime_error("get_value: query chain does not end in compatible type");
  }
  return res;
}

YAML::Node snakemake_unit_tests::yaml_reader::get_node(const std::vector<std::string> &queries) const {
  // the user just wants a node, so give it to them
  // note that they get the copy, so they can do what they want with it
  YAML::Node current = YAML::Clone(_data), next;
  apply_queries(queries, &current, &next);
  return current;
}

std::vector<std::pair<std::string, std::string> > snakemake_unit_tests::yaml_reader::get_map(
    const std::vector<std::string> &queries) const {
  // copy the top node of the file, lest it get scoped out of existence
  YAML::Node current = YAML::Clone(_data), next;
  std::vector<std::pair<std::string, std::string> > res;
  // search for keys
  apply_queries(queries, &current, &next);
  if (current.Type() == YAML::NodeType::Map) {
    // if you find a Map, cast keys and values as strings and return them
    res.reserve(current.size());
    for (YAML::const_iterator iter = current.begin(); iter != current.end(); ++iter) {
      std::pair<std::string, std::string> val(iter->first.as<std::string>(), iter->second.as<std::string>());
      res.push_back(val);
    }
  } else {
    // the user asked for a Map tho
    throw std::runtime_error("get_value: query chain does not end in compatible type");
  }
  return res;
}

bool snakemake_unit_tests::yaml_reader::query_valid(const std::vector<std::string> &queries) const {
  // copy the top level node as always
  YAML::Node current = YAML::Clone(_data), next;
  try {
    // make sure the process works
    apply_queries(queries, &current, &next);
  } catch (const std::runtime_error &e) {
    // exit gracefully if not
    return false;
  }
  return true;
}

void snakemake_unit_tests::yaml_reader::apply_queries(const std::vector<std::string> &queries, YAML::Node *current,
                                                      YAML::Node *next) const {
  // sanity check input pointers
  if (!current || !next) throw std::runtime_error("apply_queries: null pointer");
  // require that the input keys exist at all
  if (queries.empty()) throw std::runtime_error("apply_queries: no query provided");
  // can't probe Scalars or Null
  if (current->Type() != YAML::NodeType::Sequence && current->Type() != YAML::NodeType::Map) {
    throw std::runtime_error("apply_queries: starting node not Sequence or Map, cannot apply query \"" + queries.at(0) +
                             "\"");
  }
  // for each query
  for (unsigned i = 0; i < queries.size(); ++i) {
    // apply the query, scoping inward each time
    *next = (*current)[queries.at(i)];
    // handle different outcomes
    switch (next->Type()) {
        // Null at this step is bad
      case YAML::NodeType::Null:
        throw std::runtime_error("apply_queries: query \"" + queries.at(i) + "\" not present at query level");
        // Scalar is bad unless it's the endpoint
      case YAML::NodeType::Scalar:
        if (i != queries.size() - 1)
          throw std::runtime_error("apply_queries: query \"" + queries.at(i) + "\" found scalar terminator");
        break;
        // Sequence is fine
      case YAML::NodeType::Sequence:
        break;
        // Map is fine
      case YAML::NodeType::Map:
        break;
        // Undefined is probably catastrophic
      case YAML::NodeType::Undefined:
        throw std::runtime_error("apply_queries: Undefined node type encountered for query \"" + queries.at(i) + "\"");
        // logic error?
      default:
        throw std::logic_error("apply_queries: unrecognized node type?");
    }
    // update trackers as needed
    *current = *next;
  }
  // no need to return anything, given the pointer parameters
}

std::string snakemake_unit_tests::yaml_reader::get_entry(const std::string &query) const {
  std::vector<std::string> queries;
  queries.push_back(query);
  return get_entry(queries);
}

std::string snakemake_unit_tests::yaml_reader::get_entry(const std::vector<std::string> &queries) const {
  std::vector<std::string> all_results;
  all_results = get_sequence(queries);
  if (all_results.size() != 1) {
    std::ostringstream o;
    o << *queries.begin();
    for (unsigned i = 1; i <= queries.size(); ++i) {
      o << "::" << queries.at(i);
    }
    throw std::runtime_error("invalid number of results for entry query " + o.str() + "\": found " +
                             std::to_string(all_results.size()));
  }
  return *all_results.begin();
}

std::vector<std::string> snakemake_unit_tests::yaml_reader::get_sequence(const std::string &query) const {
  std::vector<std::string> queries;
  queries.push_back(query);
  return get_sequence(queries);
}

std::vector<std::pair<std::string, std::string> > snakemake_unit_tests::yaml_reader::get_map(
    const std::string &query) const {
  std::vector<std::string> queries;
  queries.push_back(query);
  return get_map(queries);
}

YAML::Node snakemake_unit_tests::yaml_reader::get_node(const std::string &query) const {
  std::vector<std::string> queries;
  queries.push_back(query);
  return get_node(queries);
}

bool snakemake_unit_tests::yaml_reader::query_valid(const std::string &query) const {
  std::vector<std::string> queries;
  queries.push_back(query);
  return query_valid(queries);
}
