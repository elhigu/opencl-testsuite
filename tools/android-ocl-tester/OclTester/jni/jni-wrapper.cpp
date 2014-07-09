/**
 * JNI wrapper for ocl-tester library.
 */

#include <jni.h>
#include <android/log.h>
#include <sys/time.h>

#include "ocl-tester.hpp"

#define  LOG_TAG    "AndroidOclTester"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#define  LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

/**
 *
 */
extern "C" jstring Java_com_lepisto_mikael_ocltester_OclTester_getDeviceInfo
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
extern "C" jstring Java_com_lepisto_mikael_ocltester_OclTester_compileWithDevice
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

