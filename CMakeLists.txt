cmake_minimum_required(VERSION 2.8)

project(opencl-testsuite)

# https://gitorious.org/findopencl/findopencl
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
find_package( OpenCL REQUIRED )
include_directories( ${OPENCL_INCLUDE_DIRS} )

# TODO: allow giving explicit location of headers and libraries 
#       (with OSX this allows to try out pocl and other non official drivers)
# 

# Maybe we could git c++11 flag... opencl platforms are pretty modern
# add_definitions(-std=c99)

# find python executable 
find_program( PYTHON_EXECUTABLE python )

SET(LIT_SCRIPT ${CMAKE_CURRENT_SOURCE_DIR}/tools/lit/lit.py)
SET(TEST_DIR ${CMAKE_CURRENT_SOURCE_DIR}/tests)

add_subdirectory(tools/ocl-tester)
get_target_property(OCL_TESTER_EXEC ocl-tester LOCATION)

message(STATUS "Found python: ${PYTHON_EXECUTABLE}")
message(STATUS "Ocl tester executable: ${OCL_TESTER_EXEC}")

# one can call 'make USE_DEVICE="Apple.*" check' to run test only with selected paltforms
add_custom_target(check 
	${PYTHON_EXECUTABLE} ${LIT_SCRIPT}  -o test_out.json -sv 
	--param OCL_TESTER=${OCL_TESTER_EXEC}
	--param DEVICE=\${USE_DEVICE}
	${TEST_DIR}
	DEPENDS ocl-tester)
