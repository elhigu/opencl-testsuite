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
        std::cerr << "Error: Failed to build program... reading build log..." << std::endl;

        size_t len = 0;
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, 0, NULL, &len);
        std::string buildLog(len, '\0');
        clGetProgramBuildInfo(program, deviceId, CL_PROGRAM_BUILD_LOG, len, &buildLog[0], NULL);
        
        buildLog = std::string(buildLog.c_str());

        std::cerr << "-------- Build log: --------" << std::endl << trim(buildLog) << std::endl;

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
 * Returns true if we should use remote tester to run test cases
 */
bool useRemoteTester(void) {
    return getenv("OCL_REMOTE_TESTER") != NULL;
}

#ifdef WIN32
std::string sendRemoteCall(std::string command) {
    return "Remote test runner not implemented for Windows (winsock).";
}

#else
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

/**
 * Send command to remote tester client 
 * platform independent way
 */
std::string sendRemoteCall(std::string command) {

    int sockfd = 0, n = 0;
    char recvBuff[1024];
    struct sockaddr_in serv_addr; 
    std::stringstream retVal;

    memset(recvBuff, '0',sizeof(recvBuff));
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        return "Error : Could not create socket\n";
    }
    memset(&serv_addr, '0', sizeof(serv_addr)); 
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(41523); 

    char *remote_address = getenv("OCL_REMOTE_TESTER");
    if(inet_pton(AF_INET, remote_address, &serv_addr.sin_addr)<=0)
    {
        return "Error : inet_pton error occured\n";
    } 

    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
       return "Error : Connect Failed\n";
    } 

    write(sockfd, command.c_str(), command.length());

    while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0)
    {
        recvBuff[n] = 0;
        retVal << recvBuff;
    } 

    if(n < 0)
    {
        return "Error: Socket reading failed.\n";
    } 

    return retVal.str();
}
#endif

/**
 * Writes JSON to stdout printing device information.
 */
bool printDeviceInfo() {
    if (useRemoteTester()) {
        std::cout << sendRemoteCall("{ \"command\" : \"info\" }");
    } else {
        device_map devices = getAllDeviceInfos();
        std::cout << "{\"deviceIdentifiers\":[" << std::endl;
        for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
            const DeviceInfo &dInfo = iter->second;
            std::cout << getDeviceString(dInfo);
            if ( peek_next(iter) !=  devices.end() ) {
                std::cout << ",";
            }
            std::cout << std::endl;
        }
        std::cout << "]}" << std::endl;
    }
    return true;
}

bool compileWithDevice(std::string selectedDevice, bool debug) {
    if (debug) std::cerr << "Reading stdin:" << std::endl;
    std::string source = readAllInput();
    if (debug) std::cerr << source << std::endl;
    if (useRemoteTester()) {
        std::string command = 
            std::string("{ \"command\" : \"compile\", \"device\" : \"") + 
            selectedDevice + 
            std::string("\", \"code\" : \"") +
            source +
            std::string("\" : }");
        std::string result = sendRemoteCall(command);
        // TODO parse result and output...
        return false;
    } else {
        if (debug) std::cerr << "Fetch all devices..." << std::endl;
        device_map devices = getAllDeviceInfos();

        // find correct device by id
        if (debug) std::cerr << "Finding correct device to compile..." << std::endl;
        for (device_map::const_iterator iter = devices.begin(); iter != devices.end(); iter++) {
            if (to_string(iter->second.deviceHash) == selectedDevice) {
                if (debug) std::cerr << "Found: " << getDeviceString(iter->second) << std::endl;
                return compileSource(source, iter->second.dId, debug);
            }
        }

        std::cerr << "Error: Could not find device." << std::endl;
        return false;
    }
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
