# OpenCL Test Suite

Standalone cross-platform testsuite to run OpenCL test cases. Based on LLVM's test suite.

Testbench was created to provide information about potential driver issues which causes 
WebCL code to fail after memory protection is applied to original WebCL kernels. Test cases
must be plain valid OpenCL (otherwise problem would be WebCL memory protector code generation).
Test cases must be also reduced to have as little code as possible, preferably less than 10 lines.

Reported results are updated here: http://elhigu.github.io/opencl-testsuite/ 

## Requirements

* Python 2.7
* CMake 2.8
* C/C++ compiler
* OpenCL Headers and libraries
* Supported platforms, Windows, Linux, OSX, Android
 
## Features

* Easy to add new test cases
* List all OpenCL devices in system
* Run tests with one device or all of them
* Collects information about environment for test report
* CMake target to send test report for me to include to results page

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

### Sending test results to public

To facilitate delivering reports to results page, we added separate CMake target to send them directly from your favorite IDE.

	make send-report

This prints out filtered version of report and delivers it to our logging service, which notifies me daily about new test results to get in. If sending does not work for some reason, you may send test_out.json / filtered output directly to mikael.lepisto@vincit.fi.


### Visual Studio Express 2013

Use "Visual Studio 2013 Win64" generator.

	mkdir build-vs-project
	cd build-vs-project
	cmake -G "Visual Studio 12 Win64" ../opencl-testsuite

Open created opencl-testsuite.sln file and build `check` rule. To send results build `send-report` rule.

### Android

Running test suite on Android is implemented by creating separate tester application for Android, which is listening test cases through TCP 41322. Also local `ocl-tester` which is used to run tests locally can actually connect to remote TCP port and instead of running tests locally just send test case in JSON to remote client. So there are no extra dependencies (python, sh, etc.) for remote devices. 

Now in addition to:

	<Host test runner> -> <ocl-tester> -> <local OpenCL driver>

We can also execute tests on remote device:

	<Host test runner> -> <ocl-tester> -> TCP -> <ocl-remote-tester> -> <device OpenCL driver>

 
How to build and install Android client is explained here: https://github.com/elhigu/opencl-testsuite/tree/master/tools/android-ocl-tester/OclTester

Host machine which will send tests to be ran and collects results must be either Linux or OSX (socket code is not ported to windows yet).

After remote client is running and accepting connections from host which will execute test runner

	OCL_REMOTE_TESTER="<remote device DNS or IP>" make check

Tester currently expects TCP port to be always `41322`. 

Running commands on remote tester one can also pass `OCL_REMOTE_TESTER` environment variable directly to `ocl-tester` command. e.g.

	OCL_REMOTE_TESTER="<remote device DNS or IP>" ocl-tester list-devices	

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
