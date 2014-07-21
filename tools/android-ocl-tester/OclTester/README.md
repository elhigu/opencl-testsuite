# Android client for running OpenCL test cases

Android client listens TCP port 41233 for running test cases. 

Client supports getting device info of platform's OpenCL driver and compiling given test case
and returns result to remote caller. This way we are able to control and collect test results
on desktop and still run tests on Android device. 

JNI calls to OpenCL driver are wrapped to separate process (remote Service) so even if OpenCL
driver crashes, tester application will keep running and restarts crashed process.

You can test if client is working with `netcat`:

```
echo '{ "command" : "info" }' | nc <device ip> 41233
echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) {}" }' | nc <device ip> 41233
echo '{ "command" : "compile", "device" : "1050148873", "code" : "kernel void test(void) { Syntax Error. }" }' | nc <device ip> 41233
```

If you are using Android emulator you should first add port routings for emulator to expose them to your host. 
To get emultor's telnet port run `adb devices` command.

```
telnet localhost 5554
redir add tcp:41233:41233
quit
```

Currently tested with Intel's OpenCL SDK with x86 android emulator, if you like to use 
some other SDK for compiling fix paths in `Android.mk` file. Eclipse project is configured
to compile JNI code automatically when project is saved.

## Installing Intel's OpenCL driver for Android on OSX

Intel's guide for installing their OpenCL driver to Android emulator didn't include how to
do it on OSX. Here are simple steps how to extract driver from Linux distribution and 
install it to Android x86 emulator.

<TODO>
