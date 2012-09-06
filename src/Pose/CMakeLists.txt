cmake_minimum_required(VERSION 2.8)
project(Pose)

find_package(qibuild)

option(POSE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)

set(_srcs
    Pose.cpp
    Pose.hpp)

# Create the Pose library
qi_create_lib(Pose SHARED ${_srcs})
qi_use_lib(Pose ALCOMMON BOOST)

# Create the test binary
qi_create_bin(Test test.cpp)
qi_use_lib(Test ALCOMMON BOOST)
target_link_libraries(Test Pose)