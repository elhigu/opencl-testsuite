/**
 * JNI wrapper for ocl-tester library.
 */

#include <jni.h>
#include <android/log.h>
#include <sys/time.h>

#include "ocl-tester.hpp"

/**
 * Wrapper should implement more or less following functionality to be able to call tester functions of
 * ocl-tester library:
 *
 *  success = printDeviceInfo();
 *  success = compileWithDevice(Options.device, Options.debug);
 *  success = runWithDevice(Options.device);
 *
 * Also I should make driver to decide where to write output
 * instead of hardcoded stdout / stderr
 *
 * Additionally jni should declare signal handlers to prevent silent crashes...
 *

if (Options.command.compare("list-devices") == 0) {
	success = printDeviceInfo();
} else if (Options.command.compare("compile") == 0) {
	success = compileWithDevice(Options.device, Options.debug);
} else if (Options.command.compare("run-kernel") == 0) {
	success = runWithDevice(Options.device);

// Something like this...
extern "C" void Java_com_intel_sample_androidbasicocl_MainActivity_initOpenCL
(
    JNIEnv* env,
    jobject thisObject,
    jstring openCLProgramText
)
{
    initOpenCL
    (
        env,
        thisObject,
        openCLProgramText,
        CL_DEVICE_TYPE_GPU,    // primary device type
        CL_DEVICE_TYPE_CPU,    // secondary device type if primary one is not available
        openCLObjects
    );
}


void shutdownOpenCL (OpenCLObjects& openCLObjects)
{

    cl_int err = CL_SUCCESS;

    if(openCLObjects.isInputBufferInitialized)
    {
        err = clReleaseMemObject(openCLObjects.inputBuffer);
        SAMPLE_CHECK_ERRORS(err);
    }

    err = clReleaseKernel(openCLObjects.kernel);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseProgram(openCLObjects.program);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseCommandQueue(openCLObjects.queue);
    SAMPLE_CHECK_ERRORS(err);

    err = clReleaseContext(openCLObjects.context);
    SAMPLE_CHECK_ERRORS(err);
}


extern "C" void Java_com_intel_sample_androidbasicocl_MainActivity_shutdownOpenCL
(
    JNIEnv* env,
    jobject thisObject
)
{
    shutdownOpenCL(openCLObjects);
    LOGD("shutdownOpenCL(openCLObjects) was called");
}


*/

#define  LOG_TAG    "AndroidOclTester"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

extern "C" jstring Java_com_lepisto_mikael_ocltester_MainActivity_getDeviceInfo
(
    JNIEnv* env,
    jobject thisObject
)
{
	return env->NewStringUTF(getDeviceInfo().c_str());
}


/**
 * To keep JNI maddness minimal, return status and output as one encoded string
 * e.g. true:output
 */
extern "C" jstring Java_com_lepisto_mikael_ocltester_MainActivity_compileWithDevice
(
    JNIEnv* env,
    jobject thisObject,
    jstring deviceStr,
    jstring sourceCode
)
{
	std::string device(env->GetStringUTFChars(deviceStr, 0));
	std::string source(env->GetStringUTFChars(sourceCode, 0));
	std::pair<bool, std::string> retVal = compileWithDevice(device, source, false);
	std::string retStatus = retVal.first ? "true" : "false";
	return env->NewStringUTF((retStatus + ":" + retVal.second).c_str());
}

