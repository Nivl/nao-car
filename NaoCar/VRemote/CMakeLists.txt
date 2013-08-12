###############################################################################
# NaoCar VRemote
###############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
CMAKE_POLICY (SET CMP0015 NEW)

PROJECT ("NaoCar VRemote")


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


SET (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/../CMakeModules ${CMAKE_MODULE_PATH})

###############################################################################
# Dependencies
###############################################################################

# Find SFML
FIND_PACKAGE (SFML 2 REQUIRED system window graphics audio network)

FIND_PACKAGE (Xinerama REQUIRED)
FIND_PACKAGE (UDev REQUIRED)

# Find and configure Qt
FIND_PACKAGE (Qt4 REQUIRED)
SET (QT_USE_QTNETWORK TRUE)
INCLUDE (${QT_USE_FILE})
ADD_DEFINITIONS (${QT_DEFINITIONS})

# Find Oculus SDK
FIND_PACKAGE (OculusSDK REQUIRED)
INCLUDE_DIRECTORIES (${OCULUS_SDK_INCLUDE_DIRS})

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR} Sources)

FIND_LIBRARY (DNS_SD_LIBRARIES dns_sd)


FILE (
    GLOB_RECURSE
    REMOTE_APP_SOURCES
    ${CMAKE_SOURCE_DIR}/Sources/*
)
FILE (
    GLOB_RECURSE
    REMOTE_APP_UI
    ${CMAKE_SOURCE_DIR}/Ui/*
)
FILE (
    GLOB_RECURSE
    REMOTE_APP_HEADERS
    ${CMAKE_SOURCE_DIR}/Sources/*.hpp
)
SET (
    REMOTE_APP_RESOURCES
    ${CMAKE_SOURCE_DIR}/Resources/Ui.qrc
)

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

ADD_EXECUTABLE (
	Remote
	${REMOTE_APP_SOURCES}
	${REMOTE_APP_UI_HEADERS}
	${REMOTE_APP_HEADERS_MOC}
	${REMOTE_APP_RESOURCES_RCC}
)

TARGET_LINK_LIBRARIES (
	Remote
	${SFML_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	${QT_QTGUI_LIBRARIES}
	${QT_QTNETWORK_LIBRARIES}
	${DNS_SD_LIBRARIES}
	${OCULUS_SDK_LIBRARIES}
	${XINERAMA_LIBRARIES}
	${UDEV_LIBRARIES}
)
