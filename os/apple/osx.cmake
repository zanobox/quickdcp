#--set OS Specific Compiler Flags-----------------------------------------------
SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OSXMIN} -O3 -mtune=native")
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-search_paths_first")
SET(AUTO_CFLAGS "CFLAGS=${OSXMIN} ${OSXSDK} -Wl,-search_paths_first -O3 -mtune=native")
SET(AUTO_CXXFLAGS "CXXFLAGS=${OSXMIN} ${OSXSDK} -Wl,-search_paths_first")
SET(AUTO_LDFLAGS "LDFLAGS=-Wl,-search_paths_first")
#-------------------------------------------------------------------------------

#--gett osx version-------------------------------------------------------------
EXEC_PROGRAM(uname ARGS -v  OUTPUT_VARIABLE DARWIN_VERSION)
STRING(REGEX MATCH "[0-9]+" DARWIN_VERSION ${DARWIN_VERSION})
#-------------------------------------------------------------------------------

#--set os specifc linking mode--------------------------------------------------
IF(${CMAKE_COMPILER_IS_GNUCC})
    SET(ENABLE_CLANG OFF)
ENDIF()

IF(ENABLE_OPENMP AND DARWIN_VERSION GREATER 10 AND ENABLE_CLANG)
    SET(ENABLE_OPENMP OFF)
ENDIF()

SET(DOWNLOAD ON)
INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib)

SET(LIB_DIR ${PREFIX}/lib)

SET(LIBS ${LIBS} -lz)
IF (DARWIN_VERSION GREATER 10)
  FIND_LIBRARY(LZMA_LIB liblzma.a)
ENDIF ()

SET(LIBS ${LIBS} -L${LIB_DIR} -lssl -lcrypto ${LZMA_LIB})

INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib/libxml2)
SET(COMPILE_LIBXML2 1)
SET(LIBS ${LIBS} -L${LIB_DIR} -lxml2)

#~ INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib/libxslt)
#~ INCLUDE_DIRECTORIES(${PROJECT_BINARY_DIR}/lib/xmlsec1)
#~ SET(COMPILE_LIBXSLT 1)
#~ SET(COMPILE_XMLSEC 1)
#~ SET(LIBS ${LIBS} -L${LIB_DIR} -lxmlsec1-openssl -lxmlsec1 -lxslt)
#~ 
#~ SET(COMPILE_EXPAT 1)
#~ SET(LIBS ${LIBS} -L${LIB_DIR} -lexpat)
#~ 
#~ SET(COMPILE_TIFF 1)
#~ SET(LIBS ${LIBS} -L${LIB_DIR} -ltiff)
#~ 
#~ SET(COMPILE_OPENJPEG 1)
#~ SET(LIBS ${LIBS} -L${LIB_DIR} -lopenjp2)

#SET(COMPILE_SNDFILE 0)
#SET(LIBS ${LIBS} -L${LIB_DIR} -lsndfile)

IF(ENABLE_GUI)
	# Use the package PkgConfig to detect GTK+ headers/library files
	FIND_PACKAGE(PkgConfig REQUIRED)
	PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

	# Setup CMake to use GTK+, tell the compiler where to look for headers
	# and to the linker where to look for libraries
	INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
	LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

	# Add other flags to the compiler
	ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})
ENDIF(ENABLE_GUI)
#-------------------------------------------------------------------------------
