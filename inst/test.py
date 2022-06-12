import os
import shutil
import subprocess as sp
import sys
from pathlib import Path, PurePosixPath
from tempfile import TemporaryDirectory

import yaml

sys.path.insert(0, os.path.dirname(__file__))

import common

exclude_patterns = [
    "\\.snakemake/",
    "__pycache__",
]
with open("{}/unit/config.yaml".format(testdir), "r") as f:
    config = yaml.safe_load(f)
if config["exclude-patterns"] is not None:
    exclude_patterns.extend(config["exclude-patterns"])
comparators = config["comparators"] if "comparators" in config else {}


def test_function():

    with TemporaryDirectory() as tmpdir:
        rundir = PurePosixPath("{}/unit/{}/output".format(testdir, rulename))
        workspace_path = PurePosixPath("{}/unit/{}/workspace".format(testdir, rulename))
        expected_path = PurePosixPath("{}/unit/{}/expected".format(testdir, rulename))

        # Copy data to the temporary workdir.
        shutil.copytree(workspace_path, rundir)

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

        # Check the output using assorted comparators.
        # To modify this behavior, you can inherit from common.OutputChecker in here
        # and overwrite the method `compare_files(generated_file, expected_file),
        # also see common.py.

        common.OutputChecker(
            workspace_path,
            expected_path,
            exclude_patterns,
            comparators,
            extra_comparison_exclusions,
            rundir,
        ).check()
