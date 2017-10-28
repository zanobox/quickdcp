# - Try to find MAGICKWAND
# Once done this will define
#
#  MAGICKWAND_FOUND - System has MAGICKWAND
#  MAGICKWAND_INCLUDE_DIR - The MAGICKWAND include directory
#  MAGICKWAND_LIBRARIES - The libraries needed to use MAGICKWAND
#  MAGICKWAND_DEFINITIONS - Compiler switches required for using MAGICKWAND
#  MAGICKWAND_XMLLINT_EXECUTABLE - The XML checking tool xmllint coming with MAGICKWAND

# Copyright (c) 2016, <jean.sabatier@zanobox.net>
#
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.


IF (MAGICKWAND_INCLUDE_DIR AND MAGICKWAND_LIBRARIES)
   # in cache already
   SET(MAGICKWAND_FIND_QUIETLY TRUE)
ENDIF (MAGICKWAND_INCLUDE_DIR AND MAGICKWAND_LIBRARIES)

IF (NOT WIN32)
   # use pkg-config to get the directories and then use these values
   # in the FIND_PATH() and FIND_LIBRARY() calls
   FIND_PACKAGE(PkgConfig)
   PKG_CHECK_MODULES(PC_MAGICKWAND MagickWand)
   SET(MAGICKWAND_DEFINITIONS ${PC_MAGICKWAND_CFLAGS_OTHER})
ENDIF (NOT WIN32)

FIND_PATH(MAGICK_BASECONFIG_INCLUDE_DIR magick/magick-baseconfig.h
   HINTS
	   ${PC_MAGICK_BASECONFIG_INCLUDEDIR}
	   ${PC_MAGICK_BASECONFIG_INCLUDE_DIRS}
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
	   x86_64-linux-gnu/ImageMagick-6
	   i386-linux-gnu/ImageMagick-6
   )

FIND_LIBRARY(MAGICK7_BASECONFIG_LIBRARIES 
	NAMES 
		MagickWand-7.Q16HDRI
		MagickWand-7.Q16
		MagickWand-7
		MagickWand
   HINTS
	   ${PC_MAGICKWAND_LIBDIR}
	   ${PC_MAGICKWAND_LIBRARY_DIRS}
   PATH_SUFFIXES 
	   ImageMagick
	   ImageMagick-7
	   x86_64-linux-gnu
	   i386-linux-gnu
	   x86_64-linux-gnu/ImageMagick-7
	   i386-linux-gnu/ImageMagick-7
   )

FIND_LIBRARY(MAGICK_BASECONFIG_LIBRARIES 
   NAMES 
	MagickWand-6.Q16
	MagickWand-6 
	MagickWand 
   HINTS
	${PC_MAGICKWAND_LIBDIR}
	${PC_MAGICKWAND_LIBRARY_DIRS}
   PATH_SUFFIXES 
	x86_64-linux-gnu
	i386-linux-gnu
)

FIND_PATH(MAGICKWAND_INCLUDE_DIR wand/MagickWand.h
   HINTS
   ${PC_MAGICKWAND_INCLUDEDIR}
   ${PC_MAGICKWAND_INCLUDE_DIRS}
   PATHS 
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
   x86_64-linux-gnu/ImageMagick-6
   i386-linux-gnu/ImageMagick-6
)

FIND_LIBRARY(MAGICKWAND_LIBRARIES 
   NAMES 
	MagickWand-6.Q16
	MagickWand-6 
	MagickWand 
   HINTS
	${PC_MAGICKWAND_LIBDIR}
	${PC_MAGICKWAND_LIBRARY_DIRS}
   PATHS 
	/usr/include
	/usr/local/include
   PATH_SUFFIXES 
	x86_64-linux-gnu
	i386-linux-gnu
)

#~ message("MAGICK_BASECONFIG_INCLUDE_DIR 	: ${MAGICK_BASECONFIG_INCLUDE_DIR}")
#~ message("MAGICK7_BASECONFIG_LIBRARIES 	: ${MAGICK7_BASECONFIG_LIBRARIES}")
#~ message("MAGICKWAND_INCLUDE_DIR 		: ${MAGICKWAND_INCLUDE_DIR}")
#~ message("MAGICK_BASECONFIG_LIBRARIES 	: ${MAGICK_BASECONFIG_LIBRARIES}")
#~ message("MAGICKWAND_LIBRARIES 			: ${MAGICKWAND_LIBRARIES}")

#~ message("MAGICKWAND_LIBRARIES : ${MAGICKWAND_LIBRARIES}")


INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set MAGICKWAND_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICK_BASECONFIG DEFAULT_MSG 
	MAGICK_BASECONFIG_LIBRARIES MAGICK_BASECONFIG_INCLUDE_DIR)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICKWAND DEFAULT_MSG 
	MAGICKWAND_LIBRARIES MAGICKWAND_INCLUDE_DIR)

MARK_AS_ADVANCED(MAGICKWAND_INCLUDE_DIR MAGICKWAND_LIBRARIES)
