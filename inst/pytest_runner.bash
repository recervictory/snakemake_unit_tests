#!/usr/bin/env bash
SNAKEMAKE_UNIT_TESTS_DIR=tests

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
    else
        ## remove any existing output directory from a previous failed run
        rm -Rf "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}/output"
        ## add to global target list
        VALID_TARGETS="${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_${i}.py ${VALID_TARGETS}"
    fi
done

## run pytest on aggregated targets
exec 5>&1
PYTEST_RESULTS=$(script -q /dev/null --command "pytest ${VALID_TARGETS}" | tee /dev/fd/5)
## only if a test succeeds, remove the output directory
for pytest_line in "$(echo \"${PYTEST_RESULTS}\")" ; do
    PYTEST_SUCCESS=$(echo ${pytest_line} | sed -E "s:^${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_(.*)\.py \..*$:\1:")
    if ! [[ -z "${PYTEST_SUCCESS}" ]] ; then
        rm -Rf "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${PYTEST_SUCCESS}/output"
    fi
done
