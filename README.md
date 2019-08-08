# libta

libta it's a library for timing analysis. It helps to apply Extreme Value Theory in order to estimate the WCET (Worst-Case Execution Time) with a given probability.


## Usage
To use the library in your application, just add use the header `libta/libta.h` contained in the include directory. A ssmall examples regarding how to use libta is contained in the dummy folder. See the Makefile to find out how the compile.
The library leverages templates, so you can pick the datatype you want to perform the computation. We suggest to use double it may be required to perform computiation with values below *10^-10* and with single-precision float it may lead to inaccuracies. On the other side, using long double may allow to get results more precise, but the computation is way slower.

Error handling in libta is performed with the libta library (`https://github.com/LoopPerfect/neither`) which offers the either class.

## Documentation

Run `doxygen` to produce html documentation inside the `doc` folder.


## UnitTests

If you want to run the unit-test framework, go to the tests folder and run the `download_tests_dependencies.sh` script (it requires `git`), then run the tests executable and see the results.
