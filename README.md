# OpenCL Test Suite

Standalone cross-platform testsuite to run OpenCL test cases. Based on LLVM's test suite.

Testbench was created to provide information about potential driver issues which causes 
WebCL code to fail after memory protection is applied to original WebCL kernels. Test cases
must be plain valid OpenCL (otherwise problem would be WebCL memory protector code generation).
Test cases must be also reduced to have as little code as possible, preferably less than 10 lines.

## Requirements

* Python 2.7 (should be easy to port to 2.5)
* CMake 2.8
* C/C++ compiler
* OpenCL Headers and libraries

## Features

* Easy to add new test cases
* List all OpenCL devices in system
* Run tests with one device or all of them
* Collects information about environment for test report

## Usage

Clone repository, create build and run cmake

	cd working_directory
	git clone https://github.com/elhigu/opencl-testsuite.git
	mkdir build
	cd build
	cmake ../opencl-testsuite

To choose where to find OpenCL one can set environment variables `OpenCL_LIBPATH` and  `OpenCL_INCPATH` e.g. 

	OpenCL_LIBPATH=/usr/local/opt/pocl/lib OpenCL_INCPATH=/usr/local/include cmake ../opencl-testsuite
	
Build native tester program and run testsuite with all devices.

	make check

To limit set of devices used to run tests one can write python regular expression to ´USE_DEVICE´ variable.

	make USE_DEVICE=GeForce check

To list available test cases and devices

	make list-checks

### Test results and running singe test cases

After running tests commandline scripts for each test case are generated to `run_scripts/device_129481944/` directory. One can repeat single test cases by running those. Test results are written also to `test_out.json`.

NOTE: tests might hang for few seconds time to time, because when driver fails compilation, it may hang for a moment.

## Adding new tests

All test cases are separate code files, which contains the code to test and in comments test scripts are inside test file in comments.

test_example.cl
```C
// RUN: %{ocl_tester} compile %{device_id} < %s
kernel void foo() {	}
```
 
In the previous example `%{ocl_tester}` is replaced with path to `ocl-tester` program, `%s` is path to `test_example.cl` file and `%{device_id}` is id of the OpenCL device. One can check it's usage by running `tools/ocl-tester/ocl-tester --help`. `%{device_id}`s can be listed with `ocl-tester`

```
Usage: ocl-tester <command> [OPTIONS] [< kernelcode.cl]

ocl-tester list-devices
ocl-tester compile --device 16918272 < kernel.cl
ocl-tester run-kernel --device 16918272 < kernel.cl

Available options:
--debug                  Print debug information.
--device <device_id>     OpenCL device id which will be used to compile test case.
                         Ids are returned with list-devices command
```

When writing run scripts it is important to not to use any tools, which are not available in Windows. We have python as dependency so that still can be used.

### Test compiling kernel code

Create new file with `.cl` extension to tests/kernel and write your test code there. All `.cl` files are interpreted as test cases. `ocl-tester compile --device <device_id>` can be used to test compiling.

### Test running kernel code

This is not yet supported. Need to write first support to `ocl-tester` to be able create buffers and enqueue kernels with arguments and to test results. 

### Test host code

Not yet supported, basically what is needed is to add some variables to test bench to compile and run `.cpp` host code files.
