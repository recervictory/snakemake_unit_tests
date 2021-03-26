#!/usr/bin/env python

import common
import pytest


def test_remove_headers():
    test_in = ["##header row1", "## header row2", "#CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"]
    test_out = common.remove_headers(test_in)
    expected_out = ["#CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"]
    assert test_out == expected_out
