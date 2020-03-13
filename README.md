# libta

libta is a library for timing analysis. It applies Extreme Value Theory to a set of time
measurements in order to estimate the WCET (Worst-Case Execution Time) with a given probability.

## Requirements
- CMake (>=3.8)
- G++ or CLang (any version with C++14 support)
- GTest (for library testing only)

## Compilation
The compilation followss the usual CMake pattern:
```console
foo@bar:~$ cd libta/
foo@bar:~/libta$ mkdir build
foo@bar:~/libta/build$ cd build/
foo@bar:~/libta/build$ cmake .. -DIMPLEMENTATION=impl
foo@bar:~/libta/build$ make
```
Replace `impl` with one of the available implementation. The available implementations are currently:
- `bscta`

The compilation will produce both a static library and a dynamic library in the directory of the selected documentation.

## Usage
To use the library in your application, just add use the header `libta/libta.h` contained in the
include directory.
The library leverages templates, so you can pick the datatype you want to perform the computation.
Depending on the implementation, the library uses different data type for the input time series.

### bscta
We suggest to use double it may be required to perform computiation with values below *10^-10* and
with single-precision float it may lead to inaccuracies. On the other side, using long double may
allow to get results more precise, but the computation is way slower.

## Documentation
To build the documentation, please run the following commands:
```console
foo@bar:~/libta$ mkdir doc && cd doc
foo@bar:~/libta/doc$ doxygen ..
```

## Testing
If you want to run the unit-test framework, please ensure to have the google testing framework
installed and run `make check` in the `build` directory.

