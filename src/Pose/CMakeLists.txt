cmake_minimum_required(VERSION 2.8)
project(Pose)

set(GCC46_PATH "/usr/bin/g++-4.6")

if (EXISTS ${GCC46_PATH})
   set (CMAKE_CXX_COMPILER ${GCC46_PATH})
endif()

set (CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -Wno-ignored-qualifiers")

find_package(qibuild)

option(POSE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)

set(_srcs
    Pose.cpp
    Pose.hpp
    PoseManager.cpp
    PoseManager.hpp
    Animation.hpp
    Animation.cpp)

# Create the Pose library
qi_create_lib(Pose SHARED ${_srcs})
qi_use_lib(Pose ALCOMMON BOOST)

# Create the test binary
qi_create_bin(test test.cpp)
qi_use_lib(test ALCOMMON BOOST)
target_link_libraries(test Pose)

# Create the save_pose binary
qi_create_bin(save_pose save_pose.cpp)
qi_use_lib(save_pose ALCOMMON BOOST)
target_link_libraries(save_pose Pose)

# Create the take_pose binary
qi_create_bin(take_pose take_pose.cpp)
qi_use_lib(take_pose ALCOMMON BOOST)
target_link_libraries(take_pose Pose)

qi_create_bin(createAnimation createAnimation.cpp)
target_link_libraries(createAnimation Pose)

qi_create_bin(set_stiffnesses set_stiffnesses.cpp)
qi_use_lib(set_stiffnesses ALCOMMON BOOST)

# Create the launchAnimation binary
qi_create_bin(launchAnimation launchAnimation.cpp)
qi_use_lib(launchAnimation ALCOMMON BOOST)
target_link_libraries(launchAnimation Pose)
