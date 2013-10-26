# Nao Rift Sample

This sample is intented to show a basic demonstration of Nao used with an Oculus Rift.
At this time, it is just the VRemote application from NaoCar project simplified with less dependencies (removed Bonjour, Gamepad and LEAP stuff)

Build environment
=================

The sample is provided with a CMake file. It is designed to build on Linux and Mac OS but the Rift demo has been test on Mac OS only.

To build the project, make sure you have CMake installed on your system.
Then install the needed dependencies:

* OpenGL
* Qt
* Oculus SDK

# Oculus SDK

Just download the SDK and build the samples (at least libovr). If you have a directory "OculusSDK" in your system containing something like libovr.a (in LibOVR/Lib...) you are all set!

Build instructions
==================

To generate the project build files, go to your project directory and type the following commands:

    > mkdir build-xcode
    > cmake -g Xcode ..

If you have OpenGL and Qt libraries installed on your system and you are a little bit lucky, you should have no errors with these libraries. An error may occur because of Oculus SDK. Simply open build-xcode/CMakeCache.txt and specify "OCULUS\_SDK\_ROOT\_DIR" (to your OculusSDK directory).