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

# Add /usr/local/include to search paths (XCode 4.3 doesn't include it by default)
IF (APPLE)
    INCLUDE_DIRECTORIES (/usr/local/include)
ENDIF (APPLE)

###############################################################################
# Dependencies
###############################################################################

# Find SFML
FIND_PACKAGE (SFML 2 REQUIRED system window graphics audio network)

# Find and configure Qt
FIND_PACKAGE (OpenGL REQUIRED)
FIND_PACKAGE (Qt4 COMPONENTS QtCore QtGui QtNetwork QtOpenGL QtXml REQUIRED)
INCLUDE (${QT_USE_FILE})
ADD_DEFINITIONS (${QT_DEFINITIONS})

# Find Oculus SDK
FIND_PACKAGE (OculusSDK REQUIRED)
INCLUDE_DIRECTORIES (${OCULUS_SDK_INCLUDE_DIRS})

# Oculus SDK dependencies for Linux:
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # FIND_PACKAGE (Xinerama REQUIRED)
    # FIND_PACKAGE (UDev REQUIRED)
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Oculus SDK dependencies for Mac OSX:
IF (APPLE)
    FIND_LIBRARY (CORE_FOUNDATION CoreFoundation)
    FIND_LIBRARY (CORE_GRAPHICS CoreGraphics)
    FIND_LIBRARY (IOKIT IOKit)
ENDIF (APPLE)

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR} Sources)

FIND_LIBRARY (DNS_SD_LIBRARIES dns_sd)
FIND_LIBRARY (LEAP_LIBRARIES Leap)
FIND_LIBRARY (PTHREAD_LIBRARIES pthread)
FIND_LIBRARY (UDEV_LIBRARIES udev)


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
    ${CMAKE_SOURCE_DIR}/Resources/Qt.qrc
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
        ${OPENGL_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	${QT_QTGUI_LIBRARIES}
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTXML_LIBRARIES}
        ${QT_QTOPENGL_LIBRARIES}
	${LEAP_LIBRARIES}
	${DNS_SD_LIBRARIES}
	${OCULUS_SDK_LIBRARIES}
)

# Oculus SDK dependencies for Linux:
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    TARGET_LINK_LIBRARIES (
        Remote
        ${XINERAMA_LIBRARIES}
        ${UDEV_LIBRARIES}
        ${PTHREAD_LIBRARIES}
    )
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Oculus SDK dependencies for Mac OSX:
IF (APPLE)
    TARGET_LINK_LIBRARIES (
        Remote
        ${CORE_FOUNDATION}
        ${CORE_GRAPHICS}
        ${IOKIT}
    )
ENDIF (APPLE)
