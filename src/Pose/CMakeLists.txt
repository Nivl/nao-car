cmake_minimum_required(VERSION 2.8)
project(Pose)

set (CMAKE_CXX_FLAGS "-std=c++0x -W -Wall")

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
qi_create_bin(Test test.cpp)
qi_use_lib(Test ALCOMMON BOOST)
target_link_libraries(Test Pose)

# Create the save_pose binary
qi_create_bin(save_pose save_pose.cpp)
qi_use_lib(save_pose ALCOMMON BOOST)
target_link_libraries(save_pose Pose)

# Create the take_pose binary
qi_create_bin(take_pose take_pose.cpp)
qi_use_lib(take_pose ALCOMMON BOOST)
target_link_libraries(take_pose Pose)

qi_create_bin(CreateAnimation createAnimation.cpp)
qi_use_lib(CreateAnimation ALCOMMON BOOST)
target_link_libraries(CreateAnimation Pose)


qi_create_bin(set_stiffnesses set_stiffnesses.cpp)
qi_use_lib(set_stiffnesses ALCOMMON BOOST)
