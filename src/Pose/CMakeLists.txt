cmake_minimum_required(VERSION 2.8)
project(Pose)

find_package(qibuild)

option(POSE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)

set(_srcs
    Pose.cpp
    Pose.hh
    main.cpp)

if(POSE_IS_REMOTE)
	add_definitions( " -DPOSE_IS_REMOTE ")
	qi_create_bin(Pose ${_srcs})
else()
	qi_create_lib(Pose SHARED ${_srcs})
endif()

qi_use_lib(Pose ALCOMMON BOOST)
