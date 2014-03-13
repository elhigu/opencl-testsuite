
add_executable(ocl-tester ocl-tester.cpp)
include_directories(${OPENCL_INCLUDE_DIR})
target_link_libraries(ocl-tester ${OPENCL_LIBRARIES})
