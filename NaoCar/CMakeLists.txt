###############################################################################
# NaoCar
###############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
CMAKE_POLICY (SET CMP0015 NEW)

PROJECT ("NaoCar")



###############################################################################
# Compiler
###############################################################################

SET (GCC46_PATH
  "/usr/bin/g++-4.6"
)
IF (EXISTS ${GCC46_PATH})
   SET (CMAKE_CXX_COMPILER ${GCC46_PATH})
ENDIF ()

SET (CMAKE_CXX_FLAGS "-std=c++0x -Wall -Wextra -Wno-ignored-qualifiers")



###############################################################################
# Variables
###############################################################################

# Global
SET (NAOCAR_MODULES_PATH ${CMAKE_SOURCE_DIR}/Modules)
SET (NAOCAR_PROXIES_PATH ${CMAKE_SOURCE_DIR}/Proxies)
SET (NAOCAR_APPS_PATH ${CMAKE_SOURCE_DIR}/Apps)

# Modules
SET (NAOCAR_POSE_MODULE_PATH ${NAOCAR_MODULES_PATH}/Pose)
SET (NAOCAR_DRIVE_MODULE_PATH ${NAOCAR_MODULES_PATH}/Drive)
SET (NAOCAR_AUTODRIVE_MODULE_PATH ${NAOCAR_MODULES_PATH}/AutoDrive)
SET (NAOCAR_REMOTE_SERVER_MODULE_PATH ${NAOCAR_MODULES_PATH}/RemoteServer)

# Proxies
SET (NAOCAR_DRIVE_PROXY_PATH ${NAOCAR_PROXIES_PATH}/Drive)
SET (NAOCAR_AUTODRIVE_PROXY_PATH ${NAOCAR_PROXIES_PATH}/AutoDrive)

# Apps
SET (NAOCAR_REMOTE_APP_PATH ${NAOCAR_APPS_PATH}/Remote)
SET (NAOCAR_REMOTE_CONTROL_APP_PATH ${NAOCAR_APPS_PATH}/RemoteControl)
SET (NAOCAR_AUTODRIVE_APP_PATH ${NAOCAR_APPS_PATH}/AutoDrive)
SET (NAOCAR_AUTODRIVELOCAL_APP_PATH ${NAOCAR_APPS_PATH}/AutoDriveLocal)
SET (NAOCAR_SAVE_POSE_APP_PATH ${NAOCAR_APPS_PATH}/SavePose)
SET (NAOCAR_TAKE_POSE_APP_PATH ${NAOCAR_APPS_PATH}/TakePose)
SET (NAOCAR_CREATE_ANIMATION_APP_PATH ${NAOCAR_APPS_PATH}/CreateAnimation)
SET (NAOCAR_SET_STIFFNESSES_APP_PATH ${NAOCAR_APPS_PATH}/SetStiffnesses)
SET (NAOCAR_LAUNCH_ANIMATION_APP_PATH ${NAOCAR_APPS_PATH}/LaunchAnimation)



###############################################################################
# Local CMake modules
###############################################################################

SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/CMakeModules ${CMAKE_MODULE_PATH})



###############################################################################
# Dependencies
###############################################################################

# Find qibuild
FIND_PACKAGE (qibuild)

# Find SFML
FIND_PACKAGE (SFML 2 REQUIRED system window graphics audio network)

# Find and configure Qt
FIND_PACKAGE (Qt4 REQUIRED)
SET (QT_USE_QTNETWORK TRUE)
INCLUDE (${QT_USE_FILE})

ADD_DEFINITIONS (${QT_DEFINITIONS})
INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR})

###############################################################################
# Include Directories
###############################################################################

INCLUDE_DIRECTORIES (
    ${NAOCAR_POSE_MODULE_PATH}
    ${NAOCAR_DRIVE_PROXY_PATH}
    ${NAOCAR_AUTODRIVE_PROXY_PATH}
    /usr/lib64/glib-2.0/include
    /usr/include/gstreamer-0.10
    /usr/include/glib-2.0
    /usr/lib/i386-linux-gnu/glib-2.0/include
    /usr/include/libxml2
)



###############################################################################
# Sources
###############################################################################


#
# Modules
#

# Pose Module
FILE (
    GLOB_RECURSE
    POSE_MODULE_SOURCES
    ${NAOCAR_POSE_MODULE_PATH}/*
)

# Drive Module
FILE (
    GLOB_RECURSE
    DRIVE_MODULE_SOURCES
    ${NAOCAR_DRIVE_MODULE_PATH}/*
)

# AutoDrive Module
FILE (
    GLOB_RECURSE
    AUTODRIVE_MODULE_SOURCES
    ${NAOCAR_AUTODRIVE_MODULE_PATH}/*
)

# Remote Server Module
FILE (
    GLOB_RECURSE
    REMOTE_SERVER_MODULE_SOURCES
    ${NAOCAR_REMOTE_SERVER_MODULE_PATH}/*
)


#
# Proxies
#

# Drive Proxy
FILE (
    GLOB_RECURSE
    DRIVE_PROXY_SOURCES
    ${NAOCAR_DRIVE_PROXY_PATH}/*
)

# AutoDrive Proxy
FILE (
    GLOB_RECURSE
    AUTODRIVE_PROXY_SOURCES
    ${NAOCAR_AUTODRIVE_PROXY_PATH}/*
)


#
# Apps
#

# Remote App
FILE (
    GLOB_RECURSE
    REMOTE_APP_SOURCES
    ${NAOCAR_REMOTE_APP_PATH}/Sources/*
)
FILE (
    GLOB_RECURSE
    REMOTE_APP_UI
    ${NAOCAR_REMOTE_APP_PATH}/Ui/*
)
FILE (
    GLOB_RECURSE
    REMOTE_APP_HEADERS
    ${NAOCAR_REMOTE_APP_PATH}/Sources/*.hpp
)
SET (REMOTE_APP_RESOURCES)

# Remote Control App
FILE (
    GLOB_RECURSE
    REMOTE_CONTROL_APP_SOURCES
    ${NAOCAR_REMOTE_CONTROL_APP_PATH}/*
)

# AutoDrive App
FILE (
    GLOB_RECURSE
    AUTODRIVE_APP_SOURCES
    ${NAOCAR_AUTODRIVE_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    AUTODRIVELOCAL_APP_SOURCES
    ${NAOCAR_AUTODRIVELOCAL_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    SAVE_POSE_APP_SOURCES
    ${NAOCAR_SAVE_POSE_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    TAKE_POSE_APP_SOURCES
    ${NAOCAR_TAKE_POSE_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    CREATE_ANIMATION_APP_SOURCES
    ${NAOCAR_CREATE_ANIMATION_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    SET_STIFFNESSES_APP_SOURCES
    ${NAOCAR_SET_STIFFNESSES_APP_PATH}/*
)

# AutoDriveLocal App
FILE (
    GLOB_RECURSE
    LAUNCH_ANIMATION_APP_SOURCES
    ${NAOCAR_LAUNCH_ANIMATION_APP_PATH}/*
)



###############################################################################
# Modules Compilation And Linking
###############################################################################

#
# Pose Module
#

QI_CREATE_LIB (
	Pose
	SHARED
	${POSE_MODULE_SOURCES}
)
QI_USE_LIB (
	Pose
	ALCOMMON
	BOOST
)


#
# Drive Module
#

OPTION (DRIVE_IS_REMOTE
  "module is compiled as a remote module (ON or OFF)"
  ON)
OPTION (NAO_LOCAL_COMPILATION
  "module is compiled as a local module for the nao (ON or OFF)"
  OFF)
IF (NAO_LOCAL_COMPILATION)
  ADD_DEFINITIONS ( " -DNAO_LOCAL_COMPILATION ")
ENDIF ()
IF (DRIVE_IS_REMOTE)
  ADD_DEFINITIONS ( " -DDRIVE_IS_REMOTE ")
  QI_CREATE_BIN(
  	DriveModule
	${DRIVE_MODULE_SOURCES}
  )
ELSE ()
  QI_CREATE_LIB(
  	DriveModule
	SHARED
	${DRIVE_MODULE_SOURCES}
  )
ENDIF ()
QI_USE_LIB (
	DriveModule
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	DriveModule
	Pose
)


#
# AutoDrive Module
#

OPTION (
	AUTODRIVE_IS_REMOTE
  	"module is compiled as a remote module (ON or OFF)"
  	OFF
)
IF (AUTODRIVE_IS_REMOTE)
  ADD_DEFINITIONS (" -DAUTODRIVE_IS_REMOTE ")
  QI_CREATE_BIN (
	AutoDriveModule
	${AUTODRIVE_MODULE_SOURCES}
  )
ELSE ()
  QI_CREATE_LIB (
	AutoDriveModule
	SHARED
	${AUTODRIVE_MODULE_SOURCES}
  )
ENDIF ()
QI_USE_LIB (
	AutoDriveModule
	ALCOMMON
	BOOST
	OPENCV2_CORE
	OPENCV2_IMGPROC
)
TARGET_LINK_LIBRARIES (
	AutoDriveModule
	pthread gstreamer-0.10 gobject-2.0
	gmodule-2.0 xml2 gthread-2.0 rt
	glib-2.0 gstapp-0.10
	DriveProxy
)

#
# RemoteServer Module
#

OPTION (
	REMOTE_SERVER_IS_REMOTE
  	"module is compiled as a remote module (ON or OFF)"
  	ON
)
IF (REMOTE_SERVER_IS_REMOTE)
  ADD_DEFINITIONS (" -DREMOTE_SERVER_IS_REMOTE ")
  QI_CREATE_BIN (
	RemoteServerModule
	${REMOTE_SERVER_MODULE_SOURCES}
  )
ELSE ()
  QI_CREATE_LIB (
	RemoteServerModule
	SHARED
	${REMOTE_SERVER_MODULE_SOURCES}
  )
ENDIF ()
QI_USE_LIB (
	RemoteServerModule
	ALCOMMON
	BOOST
	OPENCV2_CORE
	OPENCV2_IMGPROC
)
TARGET_LINK_LIBRARIES (
	RemoteServerModule
	DriveProxy
	dns_sd
)


###############################################################################
# Proxies Compilation And Linking
###############################################################################

#
# Drive Proxy
#

QI_CREATE_LIB (
	DriveProxy
	${DRIVE_PROXY_SOURCES}
)
QI_USE_LIB (
	DriveProxy
	ALCOMMON
	BOOST
)


#
# AutoDrive Proxy
#

QI_CREATE_LIB (
	AutoDriveProxy
	${AUTODRIVE_PROXY_SOURCES}
)
QI_USE_LIB (
	AutoDriveProxy
	ALCOMMON
	BOOST
)



###############################################################################
# Apps Compilation And Linking
###############################################################################

#
# Remote App
#

QT4_WRAP_UI (
	REMOTE_APP_UI_HEADERS
	${REMOTE_APP_UI}
)
QT4_WRAP_CPP (
	REMOTE_APP_HEADERS_MOC
	${REMOTE_APP_HEADERS}
)
QT4_ADD_RESOURCES (
	REMOTE_APP_RESOURCES_RCC
	${REMOTE_APP_RESOURCES}
)

QI_CREATE_BIN (
	Remote
	${REMOTE_APP_SOURCES}
	${REMOTE_APP_UI_HEADERS}
	${REMOTE_APP_HEADERS_MOC}
	${REMOTE_APP_RESOURCES_RCC}
)
QI_USE_LIB (
	Remote
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	Remote
	dns_sd
	${QT_QTCORE_LIBRARIES}
	${QT_QTGUI_LIBRARIES}
	${QT_QTNETWORK_LIBRARIES}
	${SFML_LIBRARIES}
	Pose
	DriveProxy
)

#
# Remote Control App
#

QI_CREATE_BIN (
	RemoteControl
	${REMOTE_CONTROL_APP_SOURCES}
)
QI_USE_LIB (
	RemoteControl
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	RemoteControl
	${SFML_LIBRARIES}
	Pose
	DriveProxy
	pthread
	gstreamer-0.10
	gobject-2.0
	gmodule-2.0
	gthread-2.0
	glib-2.0
	gstapp-0.10
	xml2
	rt
)


#
# AutoDrive App
#

QI_CREATE_BIN (
	AutoDriveApp
	${AUTODRIVE_APP_SOURCES}
)
QI_USE_LIB (
	AutoDriveApp
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	AutoDriveApp
	AutoDriveProxy
)


#
# AutoDriveLocal App
#

QI_CREATE_BIN (
	AutoDriveLocal
	${AUTODRIVELOCAL_APP_SOURCES}
)
QI_USE_LIB (
	AutoDriveLocal
	ALCOMMON
	BOOST
	OPENCV2_CORE
	OPENCV2_IMGPROC
	OPENCV2_HIGHGUI
)
TARGET_LINK_LIBRARIES(
	AutoDriveLocal
	pthread
	gstreamer-0.10
	gobject-2.0
	gmodule-2.0
	gthread-2.0
	glib-2.0
	gstapp-0.10
	xml2
	rt
	DriveProxy
)


#
# SavePose App
#

QI_CREATE_BIN (
	SavePose
	${SAVE_POSE_APP_SOURCES}
)
QI_USE_LIB (
	SavePose
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	SavePose
	Pose
)


#
# TakePose App
#

QI_CREATE_BIN (
	TakePose
	${TAKE_POSE_APP_SOURCES})
QI_USE_LIB (
	TakePose
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	TakePose
	Pose
)


#
# CreateAnimation App
#

QI_CREATE_BIN (
	CreateAnimation
	${CREATE_ANIMATION_APP_SOURCES}
)
TARGET_LINK_LIBRARIES (
	CreateAnimation
	Pose
)


#
# SetStiffnesses App
#

QI_CREATE_BIN (
	SetStiffnesses
	${SET_STIFFNESSES_APP_SOURCES}
)
QI_USE_LIB (
	SetStiffnesses
	ALCOMMON
	BOOST
)


#
# LaunchAnimation App
#

QI_CREATE_BIN (
	LaunchAnimation
	${LAUNCH_ANIMATION_APP_SOURCES}
)
QI_USE_LIB (
	LaunchAnimation
	ALCOMMON
	BOOST
)
TARGET_LINK_LIBRARIES (
	LaunchAnimation
	Pose
)
