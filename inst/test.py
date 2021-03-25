import os
import shutil
import subprocess as sp
import sys
from pathlib import Path, PurePosixPath
from tempfile import TemporaryDirectory

sys.path.insert(0, os.path.dirname(__file__))

import common


def test_function():

    with TemporaryDirectory() as tmpdir:
        rundir = Path(tmpdir) / "rundir"  # FOR TESTING: PurePosixPath(testdir + "/out_" + rulename)
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
                "--keep-target-files",
                "--use-conda",
                "--conda-frontend",
                "mamba",
                "--snakefile",
                "{}/workflow/Snakefile".format(rundir),
                "--directory",
                rundir,
            ]
        )

        # Check the output byte by byte using cmp.
        # To modify this behavior, you can inherit from common.OutputChecker in here
        # and overwrite the method `compare_files(generated_file, expected_file),
        # also see common.py.

        common.OutputChecker(workspace_path, expected_path, rundir).check()
