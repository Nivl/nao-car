cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(Drive)

find_package(qibuild)

option(DRIVE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)

set(_srcs
    Drive.cpp
    Drive.hh
    main.cpp)

if(DRIVE_IS_REMOTE)
	add_definitions( " -DDRIVE_IS_REMOTE ")
	qi_create_bin(Drive ${_srcs})
else()
	qi_create_lib(Drive SHARED ${_srcs})
endif()

qi_use_lib(Drive ALCOMMON BOOST)
link_directories(libPose)
find_library(POSE_DIR Pose libPose)
include_directories(libPose)
target_link_libraries(Drive ${POSE_DIR})
