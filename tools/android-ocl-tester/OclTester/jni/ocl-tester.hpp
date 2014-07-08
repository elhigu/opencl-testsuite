#include <string>

/**
 * Outputs device info JSON.
 */
std::string printDeviceInfo(void);

/**
 * Compile kernel with given device.
 *
 * @return pair(status, output)
 */
std::pair<int, std::string> compileWithDevice(std::string device, std::string program, bool debug);

