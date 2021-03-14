/*!
  \file utilities.cc
  \brief implementations of general utility functions
  \author Cameron Palmer
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
 */

#include "snakemake_unit_tests/utilities.h"

std::string snakemake_unit_tests::remove_comments_and_docstrings(
    const std::string &s) {
  std::string res = s;
  std::string::size_type loc = 0, cur = 0;
  // remove docstrings: any text embedded between a pair of triple quotes """
  while ((loc = res.find("\"\"\"", cur)) != std::string::npos) {
    if (loc != res.rfind("\"\"\"")) {
      res = res.substr(0, loc) + res.substr(res.find("\"\"\"", loc + 3) + 3);
      cur = loc + 3;
    } else {
      throw std::runtime_error(
          "unmatched set of triple quotes located in line: \"" + s + "\"");
    }
  }
  cur = 0;
  // remove comments: everything after the first unescaped '#', I think
  while ((loc = res.find("#")) != std::string::npos) {
    if (loc == 0 || res.at(loc - 1) != '\\') {
      res = res.substr(0, loc);
      break;
    }
    cur = loc + 1;
  }
  // remove trailing whitespace
  res = res.substr(0, res.find_last_not_of(" \t"));
  // maybe add other things later?
  // all done i hope
  return res;
}
