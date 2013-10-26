###############################################################################
# NaoCar Rift Sample
###############################################################################

CMAKE_MINIMUM_REQUIRED (VERSION 2.8)
CMAKE_POLICY (SET CMP0015 NEW)

PROJECT ("Nao Rift Sample")


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

# Find OpenGL
FIND_PACKAGE (OpenGL REQUIRED)

# Find and configure Qt
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

# Other dependencies
FIND_LIBRARY (PTHREAD_LIBRARIES pthread)

INCLUDE_DIRECTORIES (${CMAKE_CURRENT_BINARY_DIR} Sources)

FILE (
    GLOB_RECURSE
    SOURCES
    ${CMAKE_SOURCE_DIR}/Sources/*
)
FILE (
    GLOB_RECURSE
    UI
    ${CMAKE_SOURCE_DIR}/Ui/*
)
FILE (
    GLOB_RECURSE
    HEADERS
    ${CMAKE_SOURCE_DIR}/Sources/*.hpp
)
SET (
    RESOURCES
    ${CMAKE_SOURCE_DIR}/Resources/Ui.qrc
    ${CMAKE_SOURCE_DIR}/Resources/Qt.qrc
)

QT4_WRAP_UI (
	UI_HEADERS
	${UI}
)

QT4_WRAP_CPP (
	HEADERS_MOC
	${HEADERS}
)

QT4_ADD_RESOURCES (
	RESOURCES_RCC
	${RESOURCES}
)

ADD_EXECUTABLE (
	Sample
	${SOURCES}
	${UI_HEADERS}
	${HEADERS_MOC}
	${RESOURCES_RCC}
)

TARGET_LINK_LIBRARIES (
	Sample
    ${OPENGL_LIBRARIES}
	${QT_QTCORE_LIBRARIES}
	${QT_QTGUI_LIBRARIES}
	${QT_QTNETWORK_LIBRARIES}
	${QT_QTXML_LIBRARIES}
    ${QT_QTOPENGL_LIBRARIES}
	${OCULUS_SDK_LIBRARIES}
)

# Oculus SDK dependencies for Linux:
IF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    TARGET_LINK_LIBRARIES (
        Sample
        ${XINERAMA_LIBRARIES}
        ${UDEV_LIBRARIES}
        ${PTHREAD_LIBRARIES}
    )
ENDIF (${CMAKE_SYSTEM_NAME} MATCHES "Linux")

# Oculus SDK dependencies for Mac OSX:
IF (APPLE)
    TARGET_LINK_LIBRARIES (
        Sample
        ${CORE_FOUNDATION}
        ${CORE_GRAPHICS}
        ${IOKIT}
    )
ENDIF (APPLE)
