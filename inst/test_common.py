#!/usr/bin/env python

from unittest import mock

import common
import pytest


@pytest.mark.parametrize(
    "test_in, test_param, exp_out",
    [
        (
            ["##header row", "#CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"],
            "##",
            ["#CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"],
        ),
        (
            ["##header row", "#CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"],
            "#",
            ["chr1\t123\t.\tA\tT"],
        ),
        (
            ["CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"],
            "#",
            ["CHROM\tPOS\tID\tREF\tALT", "chr1\t123\t.\tA\tT"],
        ),
    ],
)
def test_remove_headers(test_in, test_param, exp_out):
    assert common.remove_headers(test_in, test_param) == exp_out


# @pytest.mark.parametrize("test_in, exp_out", [(), ()])
# def test_process_file():
#     m = mock.mock_open(read_data="##head1\n##head2\n#CHROM\nother stuff")
#     with mock.patch("common.process_file", m):
#         test_out = common.process_file("file.vcf", True)
#     exp_out = ["#CHROM", "other stuff"]
#     assert test_out == exp_out
