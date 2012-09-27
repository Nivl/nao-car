cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0015 NEW)

project(Drive)
set(CMAKE_MODULE_PATH "~/rendu/nao/nao-car/src/Drive/cmake/Modules/")
set(GCC46_PATH "/usr/bin/g++-4.6")

if (EXISTS ${GCC46_PATH})
   set (CMAKE_CXX_COMPILER ${GCC46_PATH})
endif()

set (CMAKE_CXX_FLAGS "-std=c++0x -W -Wall -Wno-ignored-qualifiers")

find_package(Qt4 REQUIRED)
find_package(qibuild)
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake/Modules ${CMAKE_MODULE_PATH})
find_package (SFML 2.0 REQUIRED window graphics system)

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

set(QtAppSrcs qtMain.cpp Window.cpp)
set(QtAppHeaders Window.hpp)
qt4_wrap_cpp(QtAppHeadersMoc ${QtAppHeaders})
qi_create_bin(QtApp ${QtAppSrcs} ${QtAppHeadersMoc})
qi_use_lib(QtApp ALCOMMON BOOST)
add_definitions(${QT_DEFINITIONS})
include(${QT_USE_FILE})
include_directories(libPose)
target_link_libraries(QtApp ${QT_LIBRARIES} ${POSE_DIR})

set(SfmlAppSrcs sfmlMain.cpp sfWindow.cpp)
qi_create_bin(SfmlApp ${SfmlAppSrcs})
qi_use_lib(SfmlApp ALCOMMON BOOST)
target_link_libraries(SfmlApp ${SFML_LIBRARIES} ${POSE_DIR})
