#!/usr/bin/env python3
testdir = "tests/examples/test37/output"
rulename = "simple_rule"
snakefile_relative_path = "workflow/Snakefile"
snakemake_exec_path = "."
extra_comparison_exclusions = []
import os  # noqa: E402
import shutil  # noqa: E402
import subprocess as sp  # noqa: E402
import sys  # noqa: E402
from pathlib import Path, PurePosixPath  # noqa: E402
from tempfile import TemporaryDirectory  # noqa: E402

import yaml  # noqa: E402

sys.path.insert(0, os.path.dirname(__file__))

import common  # noqa: E402

exclude_patterns = ["\\.snakemake/", "__pycache__", "\\.mpeg$"]
with open("{}/unit/config.yaml".format(testdir), "r") as f:
    config = yaml.safe_load(f)
if config["exclude-patterns"] is not None:
    exclude_patterns.extend(config["exclude-patterns"])
byte_comparisons = list(config["byte-comparisons"] or "")


def test_function():

    rundir = PurePosixPath("{}/unit/{}/output".format(testdir, rulename))
    workspace_path = PurePosixPath("{}/unit/{}/workspace".format(testdir, rulename))
    expected_path = PurePosixPath("{}/unit/{}/expected".format(testdir, rulename))

    # Copy data to the temporary workdir.
    shutil.copytree(workspace_path, rundir)

    # MANUAL INJECTION: for unit testing reasons, inject a weird file into
    # input and output locations, and attempt to control its processing via
    # exclusion patterns
    with open("{}/badfile.mpeg".format(rundir), "w") as f:
        f.write("content1\n")
    with open("{}/badfile.mpeg".format(expected_path), "w") as f:
        f.write("content2\n")

    # Run the test job.
    sp.check_output(
        [
            "python",
            "-m",
            "snakemake",
            "all",
            "-f",
            "-j1",
            "--notemp",
            "--keep-target-files",
            "--use-conda",
            "--conda-frontend",
            "mamba",
            "--snakefile",
            "{}/{}".format(rundir, snakefile_relative_path),
            "--allowed-rules",
            rulename,
            "--directory",
            "{}/{}".format(rundir, snakemake_exec_path),
        ]
    )

    # Check the output byte by byte using cmp.
    # To modify this behavior, you can inherit from common.OutputChecker in here
    # and overwrite the method `compare_files(generated_file, expected_file),
    # also see common.py.

    common.OutputChecker(
        workspace_path,
        expected_path,
        exclude_patterns,
        byte_comparisons,
        extra_comparison_exclusions,
        rundir,
    ).check()
