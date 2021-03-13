/*!
  \file cargs.cc
  \brief method implementation for command line argument parser class
  \copyright Released under the MIT License.
  Copyright 2021 Cameron Palmer
*/

#include "snakemake_unit_tests/cargs.h"

void snakemake_unit_tests::cargs::initialize_options() {
  _desc.add_options()("help,h", "emit this help message");
}
