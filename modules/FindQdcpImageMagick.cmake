# - Try to find MAGICKCORE
# Once done this will define
#
#  MAGICKCORE_FOUND - System has MAGICKCORE
#  MAGICKCORE_INCLUDE_DIR - The MAGICKCORE include directory
#  MAGICKCORE_LIBRARIES - The libraries needed to use MAGICKCORE
#  MAGICKCORE_DEFINITIONS - Compiler switches required for using MAGICKCORE
#  MAGICKCORE_XMLLINT_EXECUTABLE - The XML checking tool xmllint coming with MAGICKCORE

#------------------------------------------------------------
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




message(STATUS "Trying to find QuickDCP version of ImageMagick")


IF(IMAGEMAGICK7)
	# ------ ImageMagick 7 - includes
	FIND_PATH(MAGICKCORE7_INCLUDE_DIR MagickCore/MagickCore.h
	   PATHS 
			${INSTALL_PREFIX}
			${INSTALL_PREFIX}/include
			/usr/local
			/usr/local/include
			${STATIC_LIBS_DIR}/include
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/include
	   PATH_SUFFIXES
		   ImageMagick-7
		   qdcp-ImageMagick/ImageMagick-7
	)
	FIND_PATH(MAGICK7_BASECONFIG_INCLUDE_DIR magick/magick-baseconfig.h
	   PATHS 
			${INSTALL_PREFIX}
			${INSTALL_PREFIX}/include
			/usr/local
			/usr/local/include
			${STATIC_LIBS_DIR}/include
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/include
	   PATH_SUFFIXES
		   ImageMagick-7
		   qdcp-ImageMagick/ImageMagick-7
	)
	FIND_PATH(MAGICKWAND7_INCLUDE_DIR MagickWand/MagickWand.h
	   PATHS 
			${INSTALL_PREFIX}
			${INSTALL_PREFIX}/include
			/usr/local
			/usr/local/include
			${STATIC_LIBS_DIR}/include
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/include
	   PATH_SUFFIXES
		   ImageMagick-7
		   qdcp-ImageMagick/ImageMagick-7
	)
	
	# IM7 - libs
	FIND_LIBRARY(MAGICKCORE7_LIBRARIES 
		NAMES 
			MagickCore-7.Q16HDRI
	   PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   qdcp-ImageMagick
		   qdcp-ImageMagick-7
	)
	FIND_LIBRARY(MAGICK7_BASECONFIG_LIBRARIES 
		NAMES 
			MagickWand-7.Q16HDRI
	   PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   qdcp-ImageMagick
		   qdcp-ImageMagick-7
	)
	FIND_LIBRARY(MAGICKWAND7_LIBRARIES 
		NAMES 
			MagickWand-7.Q16HDRI
	   PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   qdcp-ImageMagick
		   qdcp-ImageMagick-7
	)
ELSE(IMAGEMAGICK7)
	# ImageMagick-6 - includes
	FIND_PATH(MAGICKCORE6_INCLUDE_DIR magick/MagickCore.h
	   PATHS 
			${INSTALL_PREFIX}
			${INSTALL_PREFIX}/include
			/usr/local
			/usr/local/include
			${STATIC_LIBS_DIR}/include
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/include
	   PATH_SUFFIXES
		   ImageMagick-6
		   qdcp-ImageMagick/ImageMagick-6
	)
	#~ FIND_PATH(MAGICK6_BASECONFIG_INCLUDE_DIR magick/magick-baseconfig.h
	   #~ PATHS 
			#~ ${STATIC_LIBS_DIR}
			#~ ${STATIC_LIBS_DIR}/include
			#~ ${INSTALL_PREFIX}
			#~ ${INSTALL_PREFIX}/include
			#~ /usr/local
			#~ /usr/local/include
	   #~ PATH_SUFFIXES
		   #~ qdcp-ImageMagick
		   #~ qdcp-ImageMagick-6
		   #~ qdcp-ImageMagick/ImageMagick-6
	#~ )
	FIND_PATH(MAGICKWAND6_INCLUDE_DIR wand/MagickWand.h
	   PATHS 
			${INSTALL_PREFIX}
			${INSTALL_PREFIX}/include
			/usr/local
			/usr/local/include
			${STATIC_LIBS_DIR}/include
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/include
	   PATH_SUFFIXES
		   ImageMagick-6
		   qdcp-ImageMagick/ImageMagick-6
	)

	# IM6 libs
	FIND_LIBRARY(MAGICKCORE6_LIBRARIES 
		NAMES 
			MagickCore-6.Q16HDRI
		PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   ImageMagick-6
		   qdcp-ImageMagick/ImageMagick-6
	)
	FIND_LIBRARY(MAGICK6_BASECONFIG_LIBRARIES 
		NAMES 
			MagickWand-6.Q16HDRI
	   PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   ImageMagick-6
		   qdcp-ImageMagick/ImageMagick-6
	)
	FIND_LIBRARY(MAGICKWAND6_LIBRARIES 
		NAMES 
			MagickWand-6.Q16HDRI
	   PATHS 
			${INSTALL_PREFIX}/lib
			/usr/local/lib
			/usr/local/lib/i386-linux-gnu
			/usr/local/lib/x86_64-linux-gnu
			${STATIC_LIBS_DIR}/lib
			${STATIC_LIBS_DIR}/qdcp-ImageMagick/lib
		PATH_SUFFIXES
		   ImageMagick-6
		   qdcp-ImageMagick/ImageMagick-6
	)
ENDIF(IMAGEMAGICK7)


IF(IMAGEMAGICK7)
	SET(MAGICKCORE_INCLUDE_DIR ${MAGICKCORE7_INCLUDE_DIR})
	SET(MAGICKCORE_LIBRARIES ${MAGICKCORE7_LIBRARIES})
	#~ SET(MAGICK_BASECONFIG_INCLUDE_DIR ${MAGICK7_BASECONFIG_INCLUDE_DIR})
	#~ SET(MAGICK_BASECONFIG_LIBRARIES ${MAGICK7_BASECONFIG_LIBRARIES})
	SET(MAGICK_BASECONFIG_INCLUDE_DIR "")
	SET(MAGICK_BASECONFIG_LIBRARIES "")
	SET(MAGICKWAND_INCLUDE_DIR ${MAGICKWAND7_INCLUDE_DIR})
	SET(MAGICKWAND_LIBRARIES ${MAGICKWAND7_LIBRARIES})
ELSE()
	SET(MAGICKCORE_INCLUDE_DIR ${MAGICKCORE6_INCLUDE_DIR})
	SET(MAGICKCORE_LIBRARIES ${MAGICKCORE6_LIBRARIES})
	SET(MAGICK_BASECONFIG_INCLUDE_DIR ${MAGICK6_BASECONFIG_INCLUDE_DIR})
	SET(MAGICK_BASECONFIG_LIBRARIES ${MAGICK6_BASECONFIG_LIBRARIES})
	SET(MAGICKWAND_INCLUDE_DIR ${MAGICKWAND6_INCLUDE_DIR})
	SET(MAGICKWAND_LIBRARIES ${MAGICKWAND6_LIBRARIES})
ENDIF()

#~ message("MAGICK_BASECONFIG_INCLUDE_DIR 	: ${MAGICK_BASECONFIG_INCLUDE_DIR}")
#~ message("MAGICK_BASECONFIG_LIBRARIES 	: ${MAGICK_BASECONFIG_LIBRARIES}")
#~ message("MAGICKWAND_INCLUDE_DIR 			: ${MAGICKWAND_INCLUDE_DIR}")
#~ message("MAGICK_BASECONFIG_LIBRARIES 	: ${MAGICK_BASECONFIG_LIBRARIES}")
#~ message("MAGICKWAND_LIBRARIES 				: ${MAGICKWAND_LIBRARIES}")


INCLUDE(FindPackageHandleStandardArgs)

# handle the QUIETLY and REQUIRED arguments and set MAGICKWAND_FOUND to TRUE if 
# all listed variables are TRUE
FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICKCORE DEFAULT_MSG 
	MAGICKCORE_LIBRARIES MAGICKCORE_INCLUDE_DIR)

IF(NOT IMAGEMAGICK7)
	FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICK_BASECONFIG DEFAULT_MSG 
		MAGICK_BASECONFIG_LIBRARIES MAGICK_BASECONFIG_INCLUDE_DIR)
	MARK_AS_ADVANCED(MAGICK_BASECONFIG_INCLUDE_DIR MAGICK_BASECONFIG_LIBRARIES)
ENDIF(NOT IMAGEMAGICK7)

FIND_PACKAGE_HANDLE_STANDARD_ARGS(MAGICKWAND DEFAULT_MSG 
	MAGICKWAND_LIBRARIES MAGICKWAND_INCLUDE_DIR)

MESSAGE(STATUS "   MAGICKWAND_LIBRARIES : ${White}${MAGICKWAND_LIBRARIES}")

MARK_AS_ADVANCED(MAGICKCORE_INCLUDE_DIR MAGICKCORE_LIBRARIES)
MARK_AS_ADVANCED(MAGICKWAND_INCLUDE_DIR MAGICKWAND_LIBRARIES)
