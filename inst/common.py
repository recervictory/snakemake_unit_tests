#!/usr/bin/env python

"""
Common code for unit testing of rules generated with Snakemake 6.0.0.
"""

import gzip
import os
import subprocess as sp
from pathlib import Path

import magic
import pytest

# import yaml


exclude_paths = [
    "/log/",
    "/logs/",
    "/performance_benchmarks/",
    "temp/",
    "tmp/",
    ".snakemake/",
    "__pycache__",
]
exclude_ext = [".tbi", ".html", ".log", ".bai", ".idx"]
# TODO: Read in a list of extensions to exclude from the config.  See issue #16.

# yaml.safe_load(config)


class OutputChecker:
    def __init__(self, data_path, expected_path, extra_comparison_exclusions, workdir):
        self.data_path = data_path
        self.expected_path = expected_path
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
                if any(m in str(f) for m in exclude_paths):
                    continue
                if any(m in str(f) for m in self.extra_comparison_exclusions):
                    continue
                if str(f).endswith(tuple(exclude_ext)):
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
        """Compare binary or text files.

        Use byte-wise comparison if files are binary via bash cmp.  If the
        files are plain text, then strip comment lines and compare (to
        circumvent datestamps causing assert failures).
        """
        f = magic.Magic(uncompress=True, mime=True)
        if f.from_file(str(generated_file)) != "text/plain":
            sp.check_output(["cmp", generated_file, expected_file])
        else:
            gen = process_file(generated_file)
            exp = process_file(expected_file)
            assert gen == exp


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
