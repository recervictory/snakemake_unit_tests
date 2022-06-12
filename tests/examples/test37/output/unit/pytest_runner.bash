#!/usr/bin/env bash
SNAKEMAKE_UNIT_TESTS_DIR=tests/examples/test37/output
VALID_TARGETS=""
CANDIDATE_TARGETS=""
if [[ "$#" -gt 0 ]] ; then
    CANDIDATE_TARGETS="$@"
else
    CANDIDATE_TARGETS=$(cd ${SNAKEMAKE_UNIT_TESTS_DIR}/unit && find . -maxdepth 1 -name "test*py" -print | sed 's|^\./test_|| ; s|.py$||')
fi
## execute pytest for specified rules if provided
for i in ${CANDIDATE_TARGETS}; do
    if [[ ! -d "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}" ]] ; then
        echo "rule ${i} does not seem to have a unit test installed under ${SNAKEMAKE_UNIT_TESTS_DIR}/unit"
	continue
    elif [[ -d "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}/output" ]] ; then
	echo "removing output from failed prior run for rule ${i}"
        ## remove any existing output directory from a previous failed run
        rm -Rf "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}/output"
    fi
    ## add to global target list
    VALID_TARGETS="${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_${i}.py ${VALID_TARGETS}"
done

## run pytest on aggregated targets
exec 5>&1
## h/t to https://stackoverflow.com/questions/17998978/removing-colors-from-output
PYTEST_RESULTS=$(script -q /dev/null --command "pytest ${VALID_TARGETS}" | tee /dev/fd/5)
PYTEST_RESULTS=$(echo "${PYTEST_RESULTS}" | sed -E "s/\x1B\[([0-9]{1,3}(;[0-9]{1,2})?)?[mGK]//g ; s/\r/\n/g")
PYTEST_RESULTS=$(echo "${PYTEST_RESULTS}" | awk "/^${SNAKEMAKE_UNIT_TESTS_DIR}\/unit\/test_/ && /.py \./ {print \$1}")
PYTEST_RESULTS=$(echo "${PYTEST_RESULTS}" | sed -E "s:^${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_:: ; s:\.py$::")
## only if a test succeeds, remove the output directory
for pytest_file in $(echo ${PYTEST_RESULTS}) ; do
    if ! [[ -z "${pytest_file}" ]] ; then
	echo "removing output directory for successful test: ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${pytest_file}/output"
        rm -Rf "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${pytest_file}/output"
    fi
done
