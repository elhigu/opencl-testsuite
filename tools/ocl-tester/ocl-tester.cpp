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
        return ltrim(rtrim(s));
}

// TODO: fix this, we might want to use khronos headers also in OSX
//       cmake already supports finding valid path
#ifdef __APPLE__
#define USE_APPLE_HEADERS 
#endif

#ifdef USE_APPLE_HEADERS
#include <OpenCL/cl.h>
#else
#include <CL/cl.h>
#endif

namespace
{
    typedef std::vector<cl_platform_id> platform_vector;
    typedef std::vector<cl_device_id> device_vector;

    typedef struct {
        cl_device_id dId;
        cl_platform_id pId;
        std::string platformName;
        std::string deviceName;
        size_t deviceHash;
        std::string deviceVersion;
        std::string driverVersion;
        std::string openCLCVersion;
    } DeviceInfo;

    typedef std::map<cl_device_id,DeviceInfo> device_map;
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

/**
 * Request various bits of information about the device from OpenCL driver.
 */ 
DeviceInfo getDeviceInfo(cl_platform_id platformId, cl_device_id deviceId) {
    std::string deviceName(1024, '\0');
    std::string deviceVersion(1024, '\0');
    std::string driverVersion(1024, '\0');
    std::string openCLCVersion(1024, '\0');
    std::string platformName(1024, '\0');

    clGetPlatformInfo(platformId, CL_PLATFORM_NAME, 1024, &platformName[0], NULL);
    clGetDeviceInfo(deviceId, CL_DEVICE_NAME, 1024, &deviceName[0], NULL);
    clGetDeviceInfo(deviceId, CL_DEVICE_VERSION, 1024, &deviceVersion[0], NULL);
    clGetDeviceInfo(deviceId, CL_DRIVER_VERSION, 1024, &driverVersion[0], NULL);
    clGetDeviceInfo(deviceId, CL_DEVICE_OPENCL_C_VERSION, 1024, &openCLCVersion[0], NULL);

    // cleanup strings
    platformName = std::string(platformName.c_str());
    deviceName = std::string(deviceName.c_str());
    deviceVersion = std::string(deviceVersion.c_str());
    driverVersion = std::string(driverVersion.c_str());
    openCLCVersion = std::string(openCLCVersion.c_str());


    std::string hashString = platformName + " / " + deviceName;
    std::hash<std::string> shash;

    DeviceInfo dInfo = { deviceId, platformId, trim(platformName), trim(deviceName), shash(hashString), trim(deviceVersion), trim(driverVersion), trim(openCLCVersion) };
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
    return "{\"id\":" + std::to_string(device.deviceHash) + ",\"platformName\":\"" + 
        device.platformName + "\",\"deviceName\":\"" + 
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
    // use stream iterators to copy the stream to a string
    std::istream_iterator<char> begin(std::cin);
    std::istream_iterator<char> end;
    return std::string(begin, end);
}

/**
 * Compile source code with given device.
 */
bool compileSource(std::string const& source, cl_device_id const& deviceId, bool debug)
{
    cl_int ret = CL_SUCCESS;

    // Create an OpenCL context
    cl_context context = clCreateContext(NULL, 1, &deviceId, NULL, NULL, &ret);
    if (debug) std::cerr << "Creating context.\n";
    if (ret != CL_SUCCESS)
    {
        // TODO: add constant to string function to print error code...
        std::cerr << "Error: Failed to create OpenCL context." << std::endl;
        return false;
    }

    // Create the program
    const char *buf = source.c_str();
    if (debug) std::cerr << "Creating program.\n";
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&buf, NULL, &ret);

    // Build the program
    if (debug) std::cerr << "Building program.\n";
    if (CL_SUCCESS != clBuildProgram(program, 1, &deviceId, NULL, NULL, NULL))
    {
        // TODO: add constant to string function to print error code...
        std::cerr << "Error: Failed to build program." << std::endl;
        return false;
    }

    bool cleanupOk = true;
    cleanupOk &= CL_SUCCESS == clReleaseContext(context);
    if (!cleanupOk) {
        std::cerr << "Error: Could not clean up context after build." << std::endl;
        return false;
    }

    return true;
}

/**
 * Writes JSON to stdout printing device information.
 */
bool printDeviceInfo() {
    device_map devices = getAllDeviceInfos();
    std::cout << "{\"deviceIdentifiers\":[" << std::endl;

    for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
        const DeviceInfo &dInfo = iter->second;
        std::cout << getDeviceString(dInfo);
        if ( std::next(iter) !=  devices.end() ) {
            std::cout << ",";
        }
        std::cout << std::endl;
    }
    std::cout << "]}" << std::endl;

    return true;
}

bool compileWithDevice(std::string selectedDevice, bool debug) {
    if (debug) std::cerr << "Reading stdin:" << std::endl;
    std::string source = readAllInput();
    if (debug) std::cerr << source << std::endl;

    if (debug) std::cerr << "Fetch all devices..." << std::endl;
    device_map devices = getAllDeviceInfos();

    // find correct device by id
    if (debug) std::cerr << "Finding correct device to compile..." << std::endl;
    for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
        if (std::to_string(iter->second.deviceHash) == selectedDevice) {
            if (debug) std::cerr << "Found: " << getDeviceString(iter->second) << std::endl;
            return compileSource(source, iter->second.dId, debug);
        }
    }

    std::cerr << "Error: Could not find device." << std::endl;
    return false;
}

bool runWithDevice(std::string selectedDevice) {
    std::cerr << "Running kernel not implemented yet." << std::endl;
    return false;
}


////////////////////////////////////////////////////////////////////////////
//
// Parse commandline arguments and main
//

// Options structure, this shuld be populated by cmd parser
// and used in main() where commands are interpreted
struct {
    std::string command;
    std::string device;
    bool debug;
} Options = { "none", "none", false };

std::string argv0 = "ocl-tester";
bool fail(std::string reason) {
    std::cerr << reason << std::endl;
    std::cerr << "Usage: " << argv0 << " <command> [OPTIONS] [< kernelcode.cl]" << std::endl << std::endl;
    std::cerr << argv0 << " list-devices" << std::endl;
    std::cerr << argv0 << " compile --device 16918272 < kernel.cl" << std::endl;
    std::cerr << argv0 << " run-kernel --device 16918272 < kernel.cl" << std::endl << std::endl;
    std::cerr << "Available options:" << std::endl;
    std::cerr << "--debug                  Print debug information." << std::endl;
    std::cerr << "--device <device_id>     OpenCL device id which will be used to compile test case." << std::endl 
              << "                         Ids are returned with list-devices command" << std::endl << std::endl;
    return false;
}

int parseCommandLine(int argc, char const* argv[]) {

    std::string deviceFlag = "--device";
    std::string debugFlag = "--debug";

    if (argc > 1) {
        Options.command = argv[1];
    }

    for (int i = 2; i < argc; ++i) {
        std::string current(argv[i]);

        // if device is given
        if (deviceFlag.compare(current) == 0 && i+1 < argc) {
            Options.device = std::string(argv[i+1]); i++;
        } else if (debugFlag.compare(current) == 0) {
            Options.debug = true;
        }
    }

    if (Options.command.compare("none") == 0) {
        return fail("Invalid Arguments: Command missing.");
    }

    return true;
}

int main(int argc, char const* argv[])
{
    if (!parseCommandLine(argc, argv)) {
        return EXIT_FAILURE;
    }

    bool success = true;
    if (Options.command.compare("list-devices") == 0) {
        success = printDeviceInfo();
    } else if (Options.command.compare("compile") == 0) {
        success = compileWithDevice(Options.device, Options.debug);
    } else if (Options.command.compare("run-kernel") == 0) {
        success = runWithDevice(Options.device);
    } else {
        fail("Invalid Arguments: Unknown command: " + Options.command);
        return EXIT_FAILURE;
    }
    
    if (success) {
        return EXIT_SUCCESS;
    } else {
        return EXIT_FAILURE;
    }
}
