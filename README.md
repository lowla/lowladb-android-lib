# lowladb-android-lib

This project provides a native implementation of LowlaDB with a Java API for use on Android devices. Its primary use is
to power the Cordova Plugin for LowlaDB but it may also be used as a standalone library.

# License
lowladb-android-lib is available under the MIT license. See the LICENSE file for more info.

# Requirements
lowladb-android-lib has been developed using Android Studio 1.1.0 on OS X. Some parts of the build may not yet work correctly on Windows.
In addition to Android Studio, you also need to download and install the android NDK version r9c or later. The generated Android archive
(aar) file should run on any ARM device running Android 4.0.3 or above.

# Installation
The library depends on the core C++ liblowladb library and the build assumes that these projects are siblings:

```bash
mkdir lowla-dev
cd lowla-dev
git clone https://github.com/lowla/liblowladb.git
git clone https://github.com/lowla/lowladb-android-lib.git
```

Once you have the code locally, open the project in Android Studio. This will create a `local.properties` file ending with the line

```ini
sdk.dir=<path to android sdk>
```

You need to add an additional line specifying the location of the ndk directory.

```ini
ndk.dir=<path to android ndk>
```

The project should now build. To run the tests, select *Run* from the *Run* menu and choose *Tests in io.lowla.lowladb* from the
popup menu. This should build and run the test suite. Note that the build is only generating an ARM image and so will not
run on the emulator. If running the tests attempts to launch an emulator, terminate the launch, ensure a device is connected and
edit the run configuration to use the device.

Once the build is complete, the generated Android Archive (aar) file may be found in the lowladb-android/build/outputs/aar directory.

# Author
- Mark Dixon, mark_dixon@lowla.io


