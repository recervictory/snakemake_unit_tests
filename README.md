# snakemake unit tests

[![unit tests](https://github.com/cpalmer718/snakemake_unit_tests/actions/workflows/cicd.yml/badge.svg)](https://github.com/cpalmer718/snakemake_unit_tests/actions/workflows/cicd.yml)

## Brief Summary

Candidate supplement to `snakemake --generate-unit-tests` with more
compatible use cases and flexibility.

## Overview

The goal of this software is to enable [unit testing](https://en.wikipedia.org/wiki/Unit_testing)
with [snakemake](https://snakemake.readthedocs.io/en/stable/). Our hope is that we can remove
many of the barriers to unit testing in bioinformatics pipelines, and encourage developers to
inspect individual stages of their pipelines, integrate their work with [CI/CD](https://en.wikipedia.org/wiki/CI/CD),
and generally more regularly adopt testing practices in the process of their work.

### Our testing model

Our goal is to make it possible to set up individual, automated tests for each rule of a
pipeline, and then rerun those tests at each code change or commit, to flag expected
or unexpected changes. The projected unit test workflow is as follows:

 - write a pipeline
 - format the snakefiles with [snakefmt](https://github.com/snakemake/snakefmt)
   - (optional) consider automatic code linting with [pre-commit](https://pre-commit.com/)
 - construct one or more small test cases to run on all or part of a pipeline
 - run the pipeline and capture log output
 - feed the pipeline and run log into `snakemake_unit_tests`
 - run tests with [pytest](https://docs.pytest.org/en/stable/)
 - whenever you make code changes, update your unit code with `snakemake_unit_tests`
 - rerun tests with [pytest](https://docs.pytest.org/en/stable/), and immediately
 flag changes in test results

Running `snakemake_unit_tests` requires a one-time configuration step, to generate a [yaml](https://yaml.org/) format
configuration file. The configuration just requires paths to the pipeline you're running, its top-level
snakefile, your output log, etc. After that, you can rerun `snakemake_unit_tests` with the same configuration
file and a flag instructing it to update code in-place. We're trying to make the process as streamlined
as possible; if there's a step you see that you'd like streamlined in some way, please post an issue
or MR/PR.

### Why unit testing?

Many bioinformaticians are familiar with the concept of "test cases": inputs and outputs
that they want to feed through their system to verify that (1) their software runs to completion,
and (2) the output seems suitable to their use case. This is an important type of testing,
but just one form of testing in software development models.

When creating pipelines, we often want to be able to do three things:

 - inspect the input and output of a particular pipeline rule for errors
 - after making changes to our pipelines, check to see that
   - the steps we wanted to change have in fact changed; and
   - the steps we didn't want to change are still working as expected

In all but the most basic pipelines, these issues can be very challenging
to rigorously evaluate with end-to-end tests. Most commonly, when testing, 
if our end-to-end test output changes, we have to manually track back those changes 
step by step until we find the step that has introduced some difference; or just as often, 
we note the output still looks" pretty good" and we assume that means the system is 
working as intended.

Unfortunately, those assumptions don't hold in many situations, and we often suspect
many small (or large) issues are being hidden by the complexity of our pipelines.
We would ideally like to be able to split up our software into individual units; inspect
the input and output of those units for issues on our initial test cases; and then
track whether later software modifications introduce changes to those expected outputs
(and have the step at which the change happened immediately flagged for inspection).
In the case of pipeline development, a very reasonable "unit" is the individual rule,
and this project tries to facilitate the process of splitting out those rules into
individual units, so the end user can skip the tedious and error-prone process of manually
extracting those units into test code, so they can focus on the debugging they need to do.

### Alternatives

[snakemake](https://snakemake.readthedocs.io/en/stable/) has internal functionality that 
addresses a similar problem, in its
[--generate-unit-tests](https://snakemake.readthedocs.io/en/stable/snakefiles/testing.html)
option. However, in our testing, that option does not work out of the box, except for
extremely simple pipeline structures that don't apply to our use cases. We have taken
what we understand of `--generate-unit-tests` aims and integrated them into our alternative,
with certain modification to increase the pipeline structures with which our option is
compatible. Specifically:

 - our method creates synthetic snakefiles with just a single rule present along with
 python infrastructure; this substantially reduces the time required to construct the 
 DAG in the test cases
 - each rule unit is tested within its own restricted test space, with all its inputs
 and outputs contained within
 - the user can flag external files and directories (config, manifest, scripts, 
 environments, etc.) that can be ported into the per-unit restricted test spaces.
 As such, the number of pipeline conventions supported by the program out of the box
 is dramatically increased
 - downstream [pytest](https://docs.pytest.org/en/stable/) testing recognizes files
 by extension and, when needed, applies custom comparison scripts that avoid meaningless
 differences and compare the parts of the file that matter (no more VCF header comments
 causing test failures). Additional file extension handlers can be added and specified
 in configuration, avoiding manual interaction with dozens of unit tests.



## Installation Instructions

### Conda

TODO(cpalmer718): create conda package and provide conda installation instructions

### From GitLab

#### Requirements

  - g++ >= 8.2.0
  - automake/autoconf
  - make >= 4.2
  - git >= 2.28.0
  - nodejs (for commitizen)
  - pre-commit
  - associated linting tools for C++: cppcheck, clang-format
  - [boost headers](https://www.boost.org)
  - [boost program_options](https://www.boost.org/doc/libs/1_75_0/doc/html/program_options.html)
  - [boost filesystem/system](https://www.boost.org/doc/libs/1_75_0/libs/filesystem/doc/index.htm)
  - [yaml-cpp](https://github.com/jbeder/yaml-cpp)
  - [cppunit](https://freedesktop.org/wiki/Software/cppunit/)

#### Build

By default, a build process involving a [conda](https://docs.conda.io/en/latest/) environment is supported.

  - if you wish to use `conda` and it's not currently available, you can install it with the instructions [here](https://docs.conda.io/en/latest/miniconda.html)
  - navigate into your project directory (snakemake_unit_tests)
  - create the `conda` environment for installation as follows:
  
     `conda env create -f environment.yaml`
  - activate the conda environment:
  
     `conda activate snakemake_unit_tests-env`
  - (one time only per environment) install `commitizen`:
  
     `npm install -g commitizen cz-conventional-changelog`
  - (one time only per environment) install `pre-commit` linters:
  
     `pre-commit install`

  - update (create) the necessary `configure` scripts with `autoreconf`:
  
     `autoreconf --force --install`
	 
     - note that this can also be run with `./generate.bash` inside the repo
  - run `configure`:
  
	 `./configure --with-boost=/path/to/miniconda3/envs/snakemake_unit_tests-env --with-boost-libdir=/path/to/miniconda3/envs/snakemake_unit_tests-env/lib`

	 - if you are planning on installing software to a local directory, run instead `./configure --prefix=/install/dir [...]`
	 - periodically there are some incompatibility issues between `configure` and `conda`. if so, you may need to override
	   some default locations detected by `configure`. for example, you might override the detected compiler with:
	   `CC=gcc CXX=g++ ./configure [...]`
  - run `make CPPFLAGS=""`
	 - this is a non-standard `make` invocation. the reason this is included is because the project
	   is configured to specifically use a `boost` installation in the accompanying `conda` environment.
	   if you'd rather remove `boost` from the conda environment, or ignore it in favor of a system-wide
	   `boost` installation, you can adjust the appropriate `configure` parameters accordingly
	   and instead invoke `make` without any further variable overrides
  - run `make check` to run `TAP/automake` tests
     - if you run this command without compiling first, you will again need to override `CPPFLAGS`
	   as follows: `make CPPFLAGS="" check`

  - if desired, run `make install`. if permissions issues are reported, see above for reconfiguring with `./configure --prefix`.
     - as above, if you run installation without compiling first, you will again need to override `CPPFLAGS`
	   as follows: `make CPPFLAGS="" check`
  
## Usage

To see available command line options, the program can be run as follows

`snakemake_unit_tests.out -h`

For most permanent project settings, we highly recommend creating a `config.yaml` file that can be provided
during each run, to streamline and simplify the process.

### Available Options

Note that most options can be specified either on the command line or in a configuration file,
and the names of the options in those different contexts are listed when applicable.

- **yaml Configuration File**
  - command line: `-c` or `--config`
  - argument type: string
  - description: set configuration settings from a yaml-format configuration file
  - notes: this is highly recommended for all projects. See [the example configuration file](config.example.yaml)
	for example formatting, or below for example accepted keys. Ideally, `snakemake_unit_tests`
	should be run each time with `-c {configfile}`, and then any additional flags to override
	your project default settings.
- **Verbose Mode**
  - command line: `-v` or `--verbose`
  - argument type: none
  - description: request verbose logging output during parsing operations
  - notes: currently only useful for `snakemake_unit_tests` C++ debugging
- **Output Test Directory**
  - command line: `-o` or `--output-test-dir`
  - yaml configuration key: `output-test-dir`
  - argument type: string
  - behavior if multiply specified: command line takes priority
  - description: directory under which unit tests will be written
  - notes: following `--generate-unit-tests` convention, the tests will in fact
	be written to `{output-test-dir}/unit/`, anticipating the presence of other
	tests in other subdirectories. These paths will be created if they do not already
	exist. Note however that if existing tests are present in `{output-test-dir}/unit/`,
	the default behavior of this program is to **overwrite in place**, so if you want
	to preserve existing tests, choose a new path.
- **Pipeline Entry Point Snakefile**
  - command line: `-s` or `--snakefile`
  - yaml configuration key: `snakefile`
  - argument type: string
  - behavior if multiply specified: command line takes priority
  - description: path to and name of top-level pipeline snakefile
  - notes: this can be relative to your current directory, or absolute. This program
	currently assumes the snakefile for your pipeline will be in something like
	`pipeline-dir/workflow/Snakefile`; support for other snakefile locations is planned
	but not currently tested. Dependent snakefiles included with `include:` directives
	should not be specified here, but rather are automatically detected.
- **Pipeline Installation Directory**
  - command line: `-p` or `--pipeline-dir`
  - yaml configuration key: `pipeline-dir`
  - argument type: string
  - behavior if multiply specified: command line takes priority
  - description: installation path to the pipeline being tested
  - notes: this can be relative to your current directory, or absolute. This
    parameter is optional, but if absent will be assumed to be `{snakefile_dir}/..`,
	again assuming the snakefile is installed in `pipeline-dir/workflow`. If this
	automatic behavior is inappropriate, this parameter should be separately specified.
- **`snakemake_unit_tests` `inst/` Directory Location**
  - command line: `-i` or `--inst-dir`
  - yaml configuration key: `inst-dir`
  - argument type: string
  - behavior if multiply specified: command line takes priority
  - description: path to and name of `inst/` directory from the `snakemake_unit_tests` repo
  - notes: this directory contains infrastructure files for [pytest](https://docs.pytest.org/en/stable/)
    compatibility. Customizations to `snakemake_unit_tests` [pytest](https://docs.pytest.org/en/stable/)
	behavior can be injected by creating a custom copy of this directory. If the [conda](https://docs.conda.io/en/latest/miniconda.html)
	installation option for `snakemake_unit_tests` is used, this can be set simply to
	`$CONDA_PREFIX/share/snakemake_unit_tests/inst`; otherwise, the path to the GitLab repo
	clone `inst/` directory should be used. Note that this path can be relative or absolute
	if desired.
- **Pipeline Run Log**
  - command line: `-l` or `--snakemake-log`
  - yaml configuration key: `snakemake-log`
  - argument type: string
  - behavior if multiply specified: command line takes priority
  - description: log file from successful run of the pipeline being tested.
  - notes: this log file controls which rules, inputs, and outputs are sent into unit test
	workspaces. As such, in the basic use case, it should probably be the results of a command
	like `snakemake -F --notemp > run.log 2>&1`. However, more complicated use cases can
	involve manually manipulating this log file. Have two partial runs' logs and want to glue them
	together? Go right ahead! That actually works.
  - TODO(cpalmer718): add TAP test confirming this actually works lol
- **Supplemental Files for Unit Test Workspaces**
  - command line: `-f` or `--added-files`
  - yaml configuration key: `added-files`
  - argument type: string (multiple values accepted)
  - behavior if multiply specified: all values used
  - description: auxiliary files, outside of dependencies, to install with unit tests
  - warning: these should be **relative paths from `pipeline-dir`**
  - notes: any file that is required by the pipeline to operate, but isn't explicitly tracked by
    rules' input or output blocks, should be specified here. The most common files falling into this
	category are a pipeline `config.yaml` file, or an experiment's manifest file.
- **Supplemental Directories for Unit Test Workspaces**
  - command line: `-d` or `--added-directories`
  - yaml configuration key: `added-directories`
  - argument type: string (multiple values accepted)
  - behavior if multiply specified: all values used
  - description: auxiliary directories, outside of dependencies, to recursively install with unit tests
  - warning: these should be **relative paths from `pipeline-dir`**
  - notes: any whole directory that is required by the pipeline to operate, but isn't explicitly tracked by
	rules' input or output blocks, should be specified here. The most common directory falling into this
	category is a pipeline's `config/` directory. Note however that this should only be used for directories
	that are exclusively fixed infrastructure files. An alternative would be, for example, if a config directory
	is used to contain *output* from a rule for some reason. In that case, please individually specify the
	files to include from that directory using `added-files`.
- **Excluded Rules, to Ignore from Log**
  - command line: `-e` or `--exclude-rules`
  - yaml configuration key: `exclude-rules`
  - argument type: string (multiple values accepted)
  - behavior if multiply specified: all values used
  - description: rules, by name, that should be ignored for generating tests
  - notes: some rules are simply not suitable for unit tests. An example might be a rule that downloads
    50GB of reference sequence data from UCSC. In that case, if the rule is present in the `snakemake-log`
	and you don't want to remove the rule from that log, you can add the rule name here and
	`snakemake_unit_tests` will skip over it when emitting tests. Another common use case for this 
	exclusion is when intermediate files from the pipeline run are not present, most commonly due 
	to removal as temp files. This behavior of [snakemake](https://snakemake.readthedocs.io/en/stable/)
	can be disabled with `--notemp`, but if that is not desired or not feasible, impacted rules
	can be added to the exclusion list. Note that any rule with the missing file as an *input*
	or an *output* must be excluded for `snakemake_unit_tests` to run successfully.
	`snakemake_unit_tests` will report any such missing files to the command line as an error,
	so you will have an opportunity to either rerun the upstream pipeline or iteratively add
	impacted rules to `exclude-rules` as desired.
	
### Example Vignettes

#### A Standard Run

- Create a pipeline, or choose one for unit testing
- Run the pipeline end-to-end and capture log output:
  
  `snakemake -j1 -F --notemp > run.log 2>&1`

- Set configuration options for your pipeline run

  - see [the example configuration file](config.example.yaml) for example settings
  - call this set file `config.yaml`

- Run `snakemake_unit_tests` for your project

  `snakemake_unit_tests.out -c config.yaml`

- Inspect the input and output for each generated test

  - these will be installed to `{output-test-dir}/unit/*/`
    - inputs are installed to `workspace/`
    - outputs are installed to `expected/`

- Run [pytest](https://docs.pytest.org/en/stable/)

  `pytest {output-test-dir}/unit/test_*py`

TODO(cpalmer718): add more examples

## Contributing

### Adding TAP Tests

Many tests are required to cover both `snakemake_unit_tests` internal functionality,
and support for `snakemake` features, as well as eventual downstream `pytest` integration.

Here are brief instructions for writing additional tests for TAP/Automake integration:

- run the current tests to see what's being tested already:
  - `git checkout simple_conditionals`
  - `make check`
- pick a single concept to test
  - can be existing feature that needs confirmed functionality
  - can also be desired output for currently unsupported feature (e.g. discriminating between multiple rules with the same name but different definitions)
- write example inputs and outputs that can demonstrate what you want to have happen
  - see folders under `tests/examples` for minimalist inspiration
- write a Snakefile representing an example pipeline that contains the feature you're testing
- **run the Snakefile through `snakefmt`**
  - program functionality strongly assumes `snakefmt` compliance
- run the test pipeline, fixing as needed; example:
  - `snakemake -j1 all > run.log 2>&1`
  - `rm -Rf .snakemake`
- write a test script
  - should be named `{something}.test`
    - extensions can be changed if desired, requires some configuration
  - should be placed in `tests/`
  - existing tests are in bash and defined by shebang
  - relative paths in the tests are relative to `snakemake_unit_tests`, *not `snakemake_unit_tests/tests`*
  - TAP syntax as follows:
    - should emit either at the beginning or end `1..{n}` where `{n}` is the total tests run in the script
    - language doesn't matter, only cares about formatted screen output
    - will flag non-zero exit codes as an error in addition to test pass/failure
    - pass/fail messages optionally have a number after `{ok or not ok}` indicating which of `{n}` tests it is
    - PASS: `ok - description of test/condition`
    - FAIL: `not ok - description of failure`
    - XFAIL: `not ok - description of failure # TODO todo message`
    - SKIP: `{ok or not ok} - description # SKIP skip reason`
- make the test script executable
- (optional) test the test before integration:
  - `./tests/{something}.test`
- add the test script to the `TESTS` variable in `Makefile.am`
- run `make check`
- (optional) iterate as needed
- add the files to the repo
  - in `tests/`, only add the `.test` files, not `.trs` or `.log`
  - in `tests/examples/`, do not add `.snakemake` directories
  - remember to add `Makefile.am`
- commit, referencing issue #18 and optionally the tested feature

## Version History

28 03 2021: this readme expanded to reflect project design

26 03 2021: MR !8, draft conditional evaluation support without interpretation

13 03 2021: project generated from cookiecutter template
