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

### Install Android dev tools, emulator and acceleration

1. Install Eclipse Android SDK : https://developer.android.com/sdk/installing/index.html
2. Install 4.2.2 Android x86 image and emulator: https://developer.android.com/sdk/installing/adding-packages.html 
3. Install Android NDK: https://developer.android.com/tools/sdk/ndk/index.html
4. Install Intel's emulator accelerator: https://software.intel.com/en-us/android/articles/intel-hardware-accelerated-execution-manager

Steps above are pretty much the same that described in https://software.intel.com/en-us/node/505964

### Create emulator configuration 

1. Open Android SDK manager and create new AVD like this: https://software.intel.com/en-us/node/505965
2. Select also Use GPU checkbox
3. Run emulator from commandline `sdk/tools/emulator -partition-size 1024 -avd test_emulator -gpu on`

Now you should have emulator running... check it out with `adb devices`

### Install OpenCL driver to running emulator and get libraries and headers for compiling JNI code

1. Download Intel's OpenCL SDK for CPU (on OSX get Linux distribution packages) https://software.intel.com/en-us/vcsource/tools/opencl-sdk
2. With Linux / Windows follow these instructions: https://software.intel.com/en-us/node/505966
3. On OSX you need to get Android OpenCL libraries from Linux dirver installer. It's name is something like this `intel_sdk_for_ocl_applications_2014_ubuntu_4.4.0.117_x64.tgz`
3.1 Libraries to link with JNI code are in `opencl-1.2-intel-devel-android-4.4.0.117-1.x86_64.deb/data.gz/`. At least `The Unarchiver.app` extracts `.deb` packages.
3.2 Copy `opencl-1.2-intel-devel-android-4.4.0.117-1.x86_64/opt/intel/opencl-1.2-4.4.0.117/android-preinstall/libOpenCL.*` to `opencl-1.2-intel-devel-android-4.4.0.117-1.x86_64/opt/intel/opencl-1.2-4.4.0.117/lib64/android32/`
3.3 Get OpenCL header files from `opencl-1.2-devel-4.4.0.117-1.x86_64/opt/intel/opencl-1.2-4.4.0.117/include` to `opencl-1.2-intel-devel-android-4.4.0.117-1.x86_64/opt/intel/opencl-1.2-4.4.0.117/include`
3.4 Now you should have all directories required by Android NDK to compile OpenCL code.
4. Install OpenCL driver to running emulator. Go to `opencl-1.2-intel-devel-android-4.4.0.117-1.x86_64/opt/intel/opencl-1.2-4.4.0.117/android-preinstall` and run `./OpenCL_Android_Install -d emulator-5554`. Check your emulator name with `adb devices`.

Now you have OpenCL installed on running android emulator and libraries and headers for compiling.

### Setup workspace and create project

1. Open Eclipse
2. Import OclTester project to workspace `File -> Import -> Android -> Existing Android Code Into Workspace`, browse `Root directory` to `tools/android-ocl-tester` directory and import new project with name `OclTester`. Click Finish.
3. Setup NDK path `Eclipse -> Preferences -> Android -> NDK` to point where is your NDK.
4. Check `jni/Android.mk` and set `-L` and `-I` flags correctly so that Android OpenCL headers and libraries are found.
5. Run `ndk-build` in project directory to compile JNI code. Or setup automatic building to Eclipse http://mobilepearls.com/labs/ndk-builder-in-eclipse/

### Run Android client and try if it works

1. Create `.apk` and run it in emulator right click `src (folder) -> Run as -> Android Application`.
1.1. If app says that activity crashed but it stays running, probably something went wrong with installing OpenCL driver (check LogCat)...
2. Add port redirection from emulator to localhost `echo "redir add tcp:41233:41233" | nc localhost 5554`. Check emulator port with `adb devices`.
3. Test that your emulator is responding `echo '{ "command" : "info" }' | nc localhost 41233`

Now your emulator should have returned information about it's OpenCL environment. e.g. `{"output":"{\"deviceIdentifiers\":[\n{\"id\":1050148873,\"platformName\":\"Intel(R) OpenCL\",\"platformVersion\":\"OpenCL 1.2 LINUX\",\"deviceName\":\"Intel(R) Pentium(R) 4 CPU 1500MHz\",\"deviceVersion\":\"OpenCL 1.2 (Build 83082)\",\"driverVersion\":\"1.2.0.83082\",\"openCLCVersion\":\"OpenCL C 1.2\"}\n]}\n","status":true}`



