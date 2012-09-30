cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(Drive)
set(GCC46_PATH "/usr/bin/g++-4.6")

if (EXISTS ${GCC46_PATH})
   set (CMAKE_CXX_COMPILER ${GCC46_PATH})
endif()

set (CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -Wno-ignored-qualifiers")

find_package(qibuild)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules)
find_package (SFML 2.0 REQUIRED window graphics system network)
option(DRIVE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  OFF)
set(_srcs
    Drive.cpp)

option(NAO_LOCAL_COMPILATION
  "module is compiled as a local module for the nao (ON or OFF)"
  ON)
set(_srcs
    Drive.cpp)

if(NAO_LOCAL_COMPILATION)
  add_definitions( " -DNAO_LOCAL_COMPILATION ")
endif()
if(DRIVE_IS_REMOTE)
  add_definitions( " -DDRIVE_IS_REMOTE ")
  qi_create_bin(Drive ${_srcs})
else()
  qi_create_lib(Drive SHARED ${_srcs})
endif()
qi_use_lib(Drive ALCOMMON BOOST)
find_library(POSE_DIR Pose lib)
target_link_libraries(Drive ${POSE_DIR})
include_directories(include)

include_directories(.)
set(DriveProxySrcs DriveProxy.cpp)
qi_create_lib(DriveProxy ${DriveProxySrcs})
qi_use_lib(DriveProxy ALCOMMON BOOST)
target_link_libraries(DriveProxy)

set(SfmlAppSrcs sfmlMain.cpp sfWindow.cpp)
qi_create_bin(SfmlApp ${SfmlAppSrcs})
qi_use_lib(SfmlApp ALCOMMON BOOST)
include_directories(/usr/include/gstreamer-0.10
		    /usr/include/glib-2.0
		    /usr/lib/i386-linux-gnu/glib-2.0/include
		    /usr/include/libxml2)
target_link_libraries(SfmlApp ${SFML_LIBRARIES} ${POSE_DIR}
		      pthread gstreamer-0.10 gobject-2.0
		      gmodule-2.0 xml2 gthread-2.0 rt
		      glib-2.0 gstapp-0.10 DriveProxy)
