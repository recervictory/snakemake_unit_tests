"""
Common code for unit testing of rules generated with Snakemake 6.0.0.
"""

from pathlib import Path
import subprocess as sp
import os
import pytest
import gzip

class OutputChecker:
    def __init__(self, data_path, expected_path, workdir):
        self.data_path = data_path
        self.expected_path = expected_path
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
                if "/log/" in str(f):
                    continue
                if "/logs/" in str(f):
                    continue
                if str(f).endswith(".tbi"):
                    continue
                if f in expected_files:
                    self.compare_files(self.workdir / f, self.expected_path / f)
                elif f in input_files:
                    # ignore input files
                    pass
                else:
                    unexpected_files.add(f)
        if unexpected_files:
            raise ValueError(
                "Unexpected files:\n{}".format(
                    "\n".join(sorted(map(str, unexpected_files)))
                )
            )

    def compare_files(self, generated_file, expected_file):
        if str(generated_file).lower().endswith((".vcf.gz", ".vcf")):
            gen = remove_headers(generated_file)
            exp = remove_headers(expected_file)
            assert gen == exp
        else:
            sp.check_output(["cmp", generated_file, expected_file])

def remove_headers(infile):
    n = []
    if str(infile).lower().endswith(".gz"):
        with gzip.open(infile, mode="rt") as f:
            for l in (l for l in f if not l.startswith("##")):
                n.append(l)
    else:
        with open(infile, "r") as f:
            for l in (l for l in f if not l.startswith("##")):
                n.append(l)
    return n

# open an issue for config list of file endings to ignore