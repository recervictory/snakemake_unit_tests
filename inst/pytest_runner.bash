## execute pytest for specified rules if provided
if [[ "$#" -gt 0 ]] ; then
    for i in "$@"; do
	if [[ ! -d "${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}" ]] ; then
	    echo "rule ${i} does not seem to have a unit test installed under ${SNAKEMAKE_UNIT_TESTS_DIR}/unit"
	else
	    ## remove any existing output directory from a previous failed run
	    rm -Rf ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}/output
	    ## run pytest on a single target, and if the test succeeds, remove the output directory
	    pytest ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_${i}.py && rm -Rf ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${i}/output
	fi
    done
else
    ## execute pytest for all pytest files in output directory
    for rule in $(cd ${SNAKEMAKE_UNIT_TESTS_DIR}/unit && find . -maxdepth 1 -name "test*py" -print | sed 's|^\./test_|| ; s|.py$||');
    do
	## remove any existing output directory from a previous failed run
	rm -Rf ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${rule}/output
	## run pytest on a single target, and if the test succeeds, remove the output directory
	pytest ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/test_${rule}.py && rm -Rf ${SNAKEMAKE_UNIT_TESTS_DIR}/unit/${rule}/output
    done
fi
