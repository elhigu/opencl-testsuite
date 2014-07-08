/*
** Copyright (c) 2014 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/

#include <stdlib.h>
#include <iostream>
#include <set>
#include <vector>
#include <map>
#include <string>

#include <algorithm>
#include <functional>
#include <cctype>
#include <locale>

struct NonAsciiChar {
    bool operator()(char c) const {
        return ((unsigned char)c) > 127;
    }
};

// trim from start
static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

// trim from end
static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

// trim from both ends
static inline std::string &trim(std::string &s) {
    // driver might return some trash...
    s.erase(std::remove_if(s.begin(),s.end(),NonAsciiChar()), s.end());
    return ltrim(rtrim(s));
}

/**
 * Simple hash function modified from
 * http://stackoverflow.com/questions/8317508/hash-function-for-a-string
 */
#define A 54059 /* a prime */
#define B 76963 /* another prime */
#define C 86969 /* yet another prime */
static inline unsigned long simple_hash(std::string str)
{
   const char* s = str.c_str();
   unsigned long h = 31 /* also prime */;
   while (*s) {
     h = (h * A) ^ (s[0] * B);
     s++;
   }
   return h; // or return h % C;
}

/**
 * Cases where we would like to use std::string.
 */
#include <sstream>
static inline std::string to_string(size_t val) {
  std::stringstream ss;
  ss << val;
  return ss.str();
}

/**
 * Implementation for c++11 std::next
 *
 * http://stackoverflow.com/questions/3673684/peek-the-next-element-in-stl-container
 */
template <typename ForwardIt>
ForwardIt peek_next(ForwardIt it, typename std::iterator_traits<ForwardIt>::difference_type n = 1)
{
    std::advance(it, n);
    return it;
}

#include <CL/cl.h>

#ifndef CL_DEVICE_OPENCL_C_VERSION // NVIDIA didn't declare this
#define CL_DEVICE_OPENCL_C_VERSION  0x103D
#endif

namespace
{
    typedef std::vector<cl_platform_id> platform_vector;
    typedef std::vector<cl_device_id> device_vector;

    typedef struct {
        cl_device_id dId;
        cl_platform_id pId;
        std::string platformName;
        std::string platformVersion;
        std::string deviceName;
        size_t deviceHash;
        std::string deviceVersion;
        std::string driverVersion;
        std::string openCLCVersion;
    } DeviceInfo;

    typedef std::map<cl_device_id,DeviceInfo> device_map;
}

size_t device_hash(DeviceInfo &dInfo) {
    return simple_hash(
        dInfo.platformName + "/" +
        dInfo.platformVersion + "/" +
        dInfo.deviceName + "/" +
        dInfo.deviceVersion + "/" +
        dInfo.openCLCVersion
    );
}

/**
 * Returns platform ids in a vector.
 */
platform_vector getPlatformIDs()
{
    const platform_vector::size_type maxPlatforms = 10;
    platform_vector platformIds(maxPlatforms);

    cl_uint numOfPlatforms = 0;
    cl_int ret = clGetPlatformIDs(maxPlatforms, platformIds.data(), &numOfPlatforms);
    if (ret != CL_SUCCESS)
    {
        std::cerr << "Failed to get platform ids." << std::endl;
        numOfPlatforms = 0;
    }
    platformIds.resize(numOfPlatforms);
    return platformIds;
}

/**
 * Returns device ids in a vector.
 */
device_vector getDeviceIDs(cl_platform_id const& platformId)
{
    cl_uint num_devices = 0;
    if (CL_SUCCESS != clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices))
    {
        std::cerr << "Failed to get number of devices." << std::endl;
        return device_vector();
    }

    device_vector devices(num_devices);
    if (CL_SUCCESS != clGetDeviceIDs(platformId, CL_DEVICE_TYPE_ALL, num_devices, devices.data(), NULL))
    {
        std::cerr << "clGetDeviceIDs failed." << std::endl;
        num_devices = 0;
    }
    devices.resize(num_devices);
    return devices;
}

std::string clean_string(std::string in) {
    std::string out = std::string(in.c_str());
    return trim(out);
}

/**
 * Request various bits of information about the device from OpenCL driver.
 */
DeviceInfo getDeviceInfo(cl_platform_id platformId, cl_device_id deviceId) {
    std::string deviceName(1024, '\0');
    std::string deviceVersion(1024, '\0');
    std::string driverVersion(1024, '\0');
    std::string openCLCVersion(1024, '\0');
    std::string platformName(1024, '\0');
    std::string platformVersion(1024, '\0');

    clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 1024, &platformName[0], NULL);
    clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 1024, &platformVersion[0], NULL);
    clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 1024, &deviceName[0], NULL);
    clGetDeviceInfo(deviceId, CL_DEVICE_VERSION, 1024, &deviceVersion[0], NULL);
    clGetDeviceInfo(deviceId, CL_DRIVER_VERSION, 1024, &driverVersion[0], NULL);
	clGetDeviceInfo(deviceId, CL_DEVICE_OPENCL_C_VERSION, 1024, &openCLCVersion[0], NULL);

    // cleanup strings.
    platformName    = clean_string(platformName);
    platformVersion = clean_string(platformVersion);
    deviceName      = clean_string(deviceName);
    deviceVersion   = clean_string(deviceVersion);
    driverVersion   = clean_string(driverVersion);
    openCLCVersion  = clean_string(openCLCVersion);

    DeviceInfo dInfo = { deviceId, platformId, platformName, platformVersion, deviceName, 0, deviceVersion, driverVersion, openCLCVersion };
    dInfo.deviceHash = device_hash(dInfo);
    return dInfo;
}

/**
 * Collect information about all the devices in system.
 */
device_map getAllDeviceInfos() {
    platform_vector platform_ids = getPlatformIDs();
    device_map devices;
    for (unsigned i = 0; i < platform_ids.size(); i++) {
        device_vector device_ids = getDeviceIDs(platform_ids[i]);
        for (unsigned j = 0; j < device_ids.size(); j++) {
            devices[device_ids[j]] = getDeviceInfo(platform_ids[i], device_ids[j]);
        }
    }
    return devices;
}

/**
 * Prints DeviceInfo as a JSON
 */
std::string getDeviceString(const DeviceInfo &device) {
    return "{\"id\":" + to_string(device.deviceHash) + ",\"platformName\":\"" +
        device.platformName + "\",\"platformVersion\":\"" +
        device.platformVersion + "\",\"deviceName\":\"" +
        device.deviceName + "\",\"deviceVersion\":\"" +
        device.deviceVersion + "\",\"driverVersion\":\"" +
        device.driverVersion + "\",\"openCLCVersion\":\"" +
        device.openCLCVersion + "\"}";
}

/**
 * Read kernel code from stdin.
 */
std::string readAllInput()
{
    // don't skip the whitespace while reading
    std::cin >> std::noskipws;
    std::istreambuf_iterator<char> eos;
    return std::string(std::istreambuf_iterator<char>(std::cin), eos);
}

/**
 * Compile source code with given device.
 */
bool compileSource(std::string const& source, cl_device_id const& deviceId, bool debug, std::stringstream &output)
{
    cl_int ret = CL_SUCCESS;

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &ret);
    if (debug) output << "Creating context.\n";
    if (ret != CL_SUCCESS)
    {
        // TODO: add constant to string function to print error code...
        std::cerr << "Error: Failed to create OpenCL context." << std::endl;
        return false;
    }

    // Create the program
    const char *buf = source.c_str();
    if (debug) output << "Creating program.\n";
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&buf, NULL, &ret);

    // Build the program
    if (debug) output << "Building program.\n";
    if (CL_SUCCESS != clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL))
    {
    	output << "Error: Failed to build program... reading build log..." << std::endl;

        size_t len = 0;
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        std::string buildLog(len, '\0');
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, len, &buildLog[0], NULL);

        buildLog = std::string(buildLog.c_str());

        output << "-------- Build log: --------" << std::endl << trim(buildLog) << std::endl;

        return false;
    }

    bool cleanupOk = true;
    cleanupOk &= CL_SUCCESS == clReleaseContext(context);
    if (!cleanupOk) {
    	output << "Error: Could not clean up context after build." << std::endl;
        return false;
    }

    return true;
}

/**
 * Writes JSON to stdout printing device information.
 */
std::string printDeviceInfo(void) {
	std::stringstream devInfo;

	device_map devices = getAllDeviceInfos();
	devInfo << "{\"deviceIdentifiers\":[" << std::endl;

    for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
        const DeviceInfo &dInfo = iter->second;
        devInfo << getDeviceString(dInfo);
        if ( peek_next(iter) !=  devices.end() ) {
        	devInfo << ",";
        }
        devInfo << std::endl;
    }
    devInfo << "]}" << std::endl;

    return devInfo.str();
}

std::pair<bool, std::string> compileWithDevice(std::string selectedDevice, bool debug) {
	std::stringstream output;

    if (debug) output << "Reading stdin:" << std::endl;
    std::string source = readAllInput();
    if (debug) output << source << std::endl;

    if (debug) output << "Fetch all devices..." << std::endl;
    device_map devices = getAllDeviceInfos();

    // find correct device by id
    if (debug) output << "Finding correct device to compile..." << std::endl;
    for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
        if (to_string(iter->second.deviceHash) == selectedDevice) {
            if (debug) output << "Found: " << getDeviceString(iter->second) << std::endl;
            return std::pair<bool, std::string>(compileSource(source, iter->second.dId, debug, output), output.str());
        }
    }

    output << "Error: Could not find device." << std::endl;
    return false;
}

bool runWithDevice(std::string selectedDevice) {
    std::cerr << "Running kernel not implemented yet." << std::endl;
    return false;
}


