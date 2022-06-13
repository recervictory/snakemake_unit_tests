#!/usr/bin/env python

"""
Common code for unit testing of rules generated with Snakemake 6.0.0.
"""

import gzip
import os
import re
import subprocess as sp
from pathlib import Path

import magic
import pandas as pd
import pytest


class OutputChecker:
    def __init__(
        self,
        data_path,
        expected_path,
        exclude_patterns,
        comparators,
        extra_comparison_exclusions,
        workdir,
    ):
        self.data_path = data_path
        self.expected_path = expected_path
        self.exclude_patterns = exclude_patterns
        self.comparators = comparators
        self.extra_comparison_exclusions = extra_comparison_exclusions
        self.workdir = workdir

    def check(self):
        input_files = set(
            (Path(path) / f).relative_to(self.data_path)
            for path, subdirs, files in os.walk(self.data_path)
            for f in files
        )
        expected_files = set(
            (Path(path) / f).relative_to(self.expected_path)
            for path, subdirs, files in os.walk(self.expected_path)
            for f in files
        )
        unexpected_files = set()
        for path, subdirs, files in os.walk(self.workdir):
            for f in files:
                f = (Path(path) / f).relative_to(self.workdir)
                if str(f).startswith(".snakemake"):
                    continue
                if any(regex.search(str(f)) for regex in map(re.compile, self.exclude_patterns)):
                    continue
                if any(m in str(f) for m in self.extra_comparison_exclusions):
                    continue
                if f in expected_files:
                    self.compare_files(self.workdir / f, self.expected_path / f)
                elif f in input_files:
                    # ignore input files
                    continue
                else:
                    unexpected_files.add(f)
        if unexpected_files:
            raise ValueError(
                "Unexpected files: {}".format(";".join(sorted(map(str, unexpected_files))))
            )

    def compare_files(self, generated_file, expected_file):
        """Compare input files.

        Logic chain is as follows:
        - if user config contains defined comparators and patterns, match against those
        - if the file does not match any pattern in user config:
           - if the file is not plaintext, use cmp
           - else directly compare in text mode

        If the files are plain text, then strip comment lines and compare (to
        circumvent datestamps causing assert failures).
        """
        f = magic.Magic(uncompress=True, mime=True)
        found_handler = False
        if self.comparators is not None:
            for comparator in self.comparators:
                if any(
                    regex.search(str(generated_file))
                    for regex in map(re.compile, comparator["patterns"])
                ):
                    found_handler = True
                    if comparator["type"] == "byte":
                        sp.check_output(["cmp", generated_file, expected_file])
                    elif comparator["type"] == "frame":
                        pandas_assert_frame_equal(
                            generated_file,
                            expected_file,
                            comparator["args"] if "args" in comparator else {},
                        )
                    elif comparator["type"] == "plaintext":
                        gen = process_file(generated_file)
                        exp = process_file(expected_file)
                        assert gen == exp
                    else:
                        raise LookupError(
                            "comparator type {} is not defined in snakemake_unit_tests".format(
                                comparator["type"]
                            )
                        )
        if not found_handler:
            if f.from_file(str(generated_file)) != "text/plain":
                sp.check_output(["cmp", generated_file, expected_file])
            else:
                gen = process_file(generated_file)
                exp = process_file(expected_file)
                assert gen == exp


def pandas_assert_frame_equal(infile1, infile2, args):
    df1 = pd.read_table(
        infile1, sep=args["sep"], header=args["header"], index_col=args["index_col"]
    )
    df2 = pd.read_table(
        infile2, sep=args["sep"], header=args["header"], index_col=args["index_col"]
    )
    pd.testing.assert_frame_equal(
        df1,
        df2,
        check_exact=False,
        check_like=args["check_like"],
        rtol=args["rtol"],
        atol=args["atol"],
    )


def process_file(infile):
    rmv = "##" if str(infile).lower().endswith((".vcf", ".vcf.gz")) else "#"
    if str(infile).lower().endswith(".gz"):
        with gzip.open(infile, mode="rt") as f:
            n = remove_headers(f, rmv)
    else:
        with open(infile, "r") as f:
            n = remove_headers(f, rmv)
    return n


def remove_headers(f, rmv):
    n = []
    for line in (line for line in f if not line.startswith(rmv)):
        n.append(line)
    return n
