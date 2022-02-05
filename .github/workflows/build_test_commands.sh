#!/bin/bash
mamba env create -f environment-linux.yaml
conda activate snakemake_unit_tests-env
./generate.bash
./configure --with-boost=$CONDA_PREFIX --with-boost-libdir=$CONDA_PREFIX/lib
make -j2
make -j2 check
./test_suite.out
