# dynamic-graph-python

[![Building Status](https://travis-ci.org/stack-of-tasks/dynamic-graph-python.svg?branch=master)](https://travis-ci.org/stack-of-tasks/dynamic-graph-python)
[![Pipeline status](https://gitlab.laas.fr/stack-of-tasks/dynamic-graph-python/badges/master/pipeline.svg)](https://gitlab.laas.fr/stack-of-tasks/dynamic-graph-python/commits/master)
[![Coverage report](https://gitlab.laas.fr/stack-of-tasks/dynamic-graph-python/badges/master/coverage.svg?job=doc-coverage)](http://projects.laas.fr/stack-of-tasks/doc/stack-of-tasks/dynamic-graph-python/master/coverage/)

Python bindings for dynamic-graph.


**Warning:** this repository contains [Git
submodules][git-submodules]. Please clone this repository using the
`git clone --recursive` command. If you already have cloned the
repository, you can run `git submodule init && git submodule update`
to retrieve the submodules.

## Documentation

[Online](https://gepettoweb.laas.fr/doc/stack-of-tasks/dynamic-graph-python/master/doxygen-html/)


## Getting Help

Support is provided through:
 * the [issue tracker](https://github.com/stack-of-tasks/dynamic-graph-python/issues)
 * the matrix room [#stack-of-tasks:laas.fr](https://matrix.to/#/#stack-of-tasks:laas.fr)


## How can I install dynamic-graph?

### Compiling and installing the package

The manual compilation requires two steps:

 1. configuration of the build and generation of the build files
 2. compilation of the sources and installation of the package

dynamic-graph uses CMake to generate build files. It is
recommended to create a separate build directory:

```sh
mkdir build
cd build
cmake ..
```

```sh
make
make test
make install
```

### Options

Additional options can be set on the command line through the
following command: `-D<option>=<value>`.

For instance: `cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..` will set
the `CMAKE_BUILD_TYPE` option to the value `RelWithDebInfo`.


Available options are:

- `CMAKE_BUILD_TYPE` set the build profile that should be used (debug,
  release, etc.). We recommend `RelWithDebInfo` as it will provide
  performances while keeping debugging symbols enabled.
- `CMAKE_INSTALL_PREFIX` set the installation prefix (the directory
  where the software will be copied to after it has been compiled).


### Running the test suite

The test suite can be run from your build directory by running:

```sh
make test
```

Please open a ticket if some tests are failing on your computer, it
should not be the case.
