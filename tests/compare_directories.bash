#!/usr/bin/env bash

if [[ "$#" -ne 3 ]] ; then
    echo "compare_directories.bash usage: expected_output_dir actual_output_dir test_description"
    exit 1
fi

EXPECTEDDIR="$1"
OUTPUTDIR="$2"
TESTDESCRIPTION="$3"

## compare expected to observed output, ignoring pytest infrastructure
for file in $(find "$EXPECTEDDIR" -type f \( -name "*" ! -name "*.py" \) -print);
do
    actual=$(echo "$file" | sed 's/\/expected\//\/output\//')
    if [[ ! -f "$actual" ]] ; then
	echo "not ok - $TESTDESCRIPTION expected file $file not present in results"
	exit 2
    else
	actualdiff=$(diff $file $actual | wc -l)
	if [[ "$actualdiff" -gt 0 ]] ; then
	    echo "not ok - $TESTDESCRIPTION expected file $file differs from observed $actual"
	    exit 3
	fi
    fi
done

## compare obsered to expected output, ignoring pytest infrastructure
##   flag files present in one absent in other
for file in $(find "$OUTPUTDIR" -type f \( -name "*" ! -name "*.py" \) -print);
do
    expected=$(echo "$file" | sed 's/\/output\//\/expected\//')
    if [[ ! -f "$expected" ]] ; then
	echo "not ok - $TESTDESCRIPTION observed file $file not present in expected"
	exit 4
    fi
done
