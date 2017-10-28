# - Try to find MAGICKCORE
# Once done this will define
#
#  MAGICKCORE_FOUND - System has MAGICKCORE
#  MAGICKCORE_INCLUDE_DIR - The MAGICKCORE include directory
#  MAGICKCORE_LIBRARIES - The libraries needed to use MAGICKCORE
#  MAGICKCORE_DEFINITIONS - Compiler switches required for using MAGICKCORE
#  MAGICKCORE_XMLLINT_EXECUTABLE - The XML checking tool xmllint coming with MAGICKCORE

# Copyright (c) 2016, <jean.sabatier@zanobox.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (MAGICKCORE_INCLUDE_DIR AND MAGICKCORE_LIBRARIES)
   # in cache already
   SET(MAGICKCORE_FIND_QUIETLY TRUE)
ENDIF (MAGICKCORE_INCLUDE_DIR AND MAGICKCORE_LIBRARIES)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   FIND_PACKAGE(PkgConfig)
   PKG_CHECK_MODULES(PC_MAGICKCORE MagickCore)
   SET(MAGICKCORE_DEFINITIONS ${PC_MAGICKCORE_CFLAGS_OTHER})
ENDIF (NOT WIN32)

FIND_PATH(MAGICKCORE7_INCLUDE_DIR MagickCore/MagickCore.h
   HINTS
	   ${PC_MAGICKCORE_INCLUDEDIR}
	   ${PC_MAGICKCORE_INCLUDE_DIRS}
   PATHS 
	   /usr/include
	   /usr/local/include
	   /usr/include/i386-linux-gnu
	   /usr/local/include/i386-linux-gnu
	   /usr/include/x86_64-linux-gnu
	   /usr/local/include/x86_64-linux-gnu
   PATH_SUFFIXES
	   ImageMagick
	   ImageMagick-7
   )

FIND_PATH(MAGICKCORE_INCLUDE_DIR magick/MagickCore.h
   HINTS
	   ${PC_MAGICKCORE_INCLUDEDIR}
	   ${PC_MAGICKCORE_INCLUDE_DIRS}
   PATHS 
	   /usr/include
	   /usr/local/include
	   /usr/include/i386-linux-gnu
	   /usr/local/include/i386-linux-gnu
	   /usr/include/x86_64-linux-gnu
	   /usr/local/include/x86_64-linux-gnu
   PATH_SUFFIXES
	   ImageMagick
	   ImageMagick-6
   )

FIND_LIBRARY(MAGICKCORE7_LIBRARIES 
	NAMES 
		MagickCore-7.Q16HDRI
		MagickCore-7.Q16 
		MagickCore-7 
   HINTS
	   ${PC_MAGICKCORE_LIBDIR}
	   ${PC_MAGICKCORE_LIBRARY_DIRS}
   PATHS 
	   /usr/lib
	   /usr/local/lib
   PATH_SUFFIXES 
	   i386-linux-gnu
	   x86_64-linux-gnu
   )
#~ message("MAGICKCORE7_LIBS : ${MAGICKCORE7_LIBRARIES}")

FIND_LIBRARY(MAGICKCORE_LIBRARIES 
	NAMES 
		MagickCore-6.Q16HDRI
		MagickCore-6.Q16 
		MagickCore-6 
   HINTS
	   ${PC_MAGICKCORE_LIBDIR}
	   ${PC_MAGICKCORE_LIBRARY_DIRS}
   PATHS 
	   /usr/lib
	   /usr/local/lib
   PATH_SUFFIXES 
	   i386-linux-gnu
	   x86_64-linux-gnu
   )

#~ message("MAGICKCORE6_LIBS : ${MAGICKCORE6_LIBRARIES}")

#~ IF(MAGICKCORE7_INCLUDE_DIR)
	#~ SET(MAGICKCORE_INCLUDE_DIR ${MAGICKCORE7_INCLUDE_DIR})
#~ ELSE()
#~ ENDIF()
#~ IF(MAGICKCORE6_INCLUDE_DIR)
	#~ SET(MAGICKCORE_INCLUDE_DIR ${MAGICKCORE6_INCLUDE_DIR})
#~ 
#~ ENDIF()

#~ message("MAGICKCORE7_INCLUDE_DIR : ${MAGICKCORE7_INCLUDE_DIR}")
#~ message("MAGICKCORE6_INCLUDE_DIR : ${MAGICKCORE6_INCLUDE_DIR}")
#~ message("MAGICKCORE_LIBRARIES : ${MAGICKCORE_LIBRARIES}")

INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set MAGICKCORE_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICKCORE DEFAULT_MSG MAGICKCORE_LIBRARIES MAGICKCORE_INCLUDE_DIR)

MARK_AS_ADVANCED(MAGICKCORE_INCLUDE_DIR MAGICKCORE_LIBRARIES)
