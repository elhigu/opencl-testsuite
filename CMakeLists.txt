cmake_minimum_required(VERSION 2.8)

project(opencl-testsuite)

# https://gitorious.org/findopencl/findopencl
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package( OpenCL REQUIRED )
message(STATUS "OpenCL includes: ${OPENCL_INCLUDE_DIRS}")

include_directories( ${OPENCL_INCLUDE_DIRS} )

# Maybe we could git c++11 flag... opencl platforms are pretty modern
# add_definitions(-std=c99)

# find python executable 
find_program( PYTHON_EXECUTABLE python )

SET(LIT_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/tools/lit/lit.py)
SET(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)
SET(REPORTER_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/tools/report-results.py)
SET(RESULTS_JSON ${CMAKE_BINARY_DIR}/test_out.json)
add_subdirectory(tools/ocl-tester)
get_target_property(OCL_TESTER_EXEC ocl-tester LOCATION)

message(STATUS "Found python: ${PYTHON_EXECUTABLE}")
message(STATUS "Ocl tester executable: ${OCL_TESTER_EXEC}")

# one can call 'make USE_DEVICE="Apple.*" check' to run test only with selected platforms
add_custom_target(check 
	${PYTHON_EXECUTABLE} ${LIT_SCRIPT}  -o ${RESULTS_JSON} -sv 
	--param OCL_TESTER=${OCL_TESTER_EXEC}
	--param DEVICE=\${USE_DEVICE}
	${TEST_DIR}
	DEPENDS ocl-tester)

# one can call 'make list-checks' to print out information about devices
add_custom_target(list-checks 
	${PYTHON_EXECUTABLE} ${LIT_SCRIPT}  -o ${RESULTS_JSON} -sv 
	--param OCL_TESTER=${OCL_TESTER_EXEC}
	--param DEVICE=\${USE_DEVICE}
	--show-suites
	--show-suites
	${TEST_DIR}
	DEPENDS ocl-tester)

add_custom_target(send-report ${PYTHON_EXECUTABLE} ${REPORTER_SCRIPT} ${RESULTS_JSON})
