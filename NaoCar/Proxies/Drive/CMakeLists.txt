cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(DriveProxy)
set(GCC46_PATH "/usr/bin/g++-4.6")

if (EXISTS ${GCC46_PATH})
   set (CMAKE_CXX_COMPILER ${GCC46_PATH})
endif()

set (CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -Wno-ignored-qualifiers -Wno-sign-compare")

find_package(qibuild)

include_directories(.)
set(DriveProxySrcs DriveProxy.cpp)
qi_create_lib(DriveProxy ${DriveProxySrcs})
qi_use_lib(DriveProxy ALCOMMON BOOST)
