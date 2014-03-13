=OpenCL Test Suite=

Standalone cross-platform testsuite to run OpenCL test cases. Based on LLVM's test suite.

Testbench was created to provide information about potential driver issues which causes 
WebCL code to fail after memory protection is applied to original WebCL kernels. Test cases
must be plain valid OpenCL (otherwise problem would be WebCL memory protector code generation).
Test cases must be also reduced to have as little code as possible, preferably less than 10 lines.

==Requirements==

* Python 2.7 (should be easy to port to 2.5)
* CMake 2.8
* C/C++ compiler
* OpenCL Headers and libraries

==Features==

* Easy to add new test cases
* List all OpenCL devices in system
* Run tests with one device or all of them
* Collects information about environment for test report

== Usage ==

Clone repository

	git clone https://github.com/elhigu/opencl-testsuite.git

Crate build directory to run tests.

	cmake ../opencl-testsuite

Build native tester program and run testsuite with all devices.

	make check

To limit set of devices used to run tests one can write python regular expression to ´USE_DEVICE´ variable.

	make USE_DEVICE=GeForce check

