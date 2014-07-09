#include <string>

/**
 * Outputs device info JSON.
 */
std::string getDeviceInfo(void);

/**
 * Compile kernel with given device.
 *
 * @return pair(status, output)
 */
std::pair<bool, std::string> compileWithDevice(
		std::string device, std::string program, bool debug);

