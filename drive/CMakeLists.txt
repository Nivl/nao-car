cmake_minimum_required(VERSION 2.8)
project(kart)

find_package(qibuild)

set(	_srcs
	Kart.cpp
	Kart.hh
	main.cpp)

option(KART_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)
if(KART_IS_REMOTE)
	add_definitions( " -DKART_IS_REMOTE ")
	qi_create_bin(kart ${_srcs})
else()
	qi_create_lib(kart SHARED ${_srcs} SUBFOLDER naoqi)
endif()

qi_use_lib(kart ALCOMMON)
