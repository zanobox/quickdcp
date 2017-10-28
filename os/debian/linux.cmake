#--set OS Specific Compiler Flags-----------------------------------------------
IF(TARGET_ARCH STREQUAL "x86_64")
    SET(AUTO_CFLAGS "CFLAGS=-fPIC -O3")
ELSE()
    SET(AUTO_CFLAGS "CFLAGS=-m32")
    SET(TARGET --target=i686)
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -m32")
    SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -m32")
ENDIF()
#-------------------------------------------------------------------------------

#--find dynamic libraries-------------------------------------------------
MESSAGE(CYAN "·······································")
MESSAGE(CYAN "···· Looking for dynamic libraries ····")
MESSAGE(CYAN "·······································")

## GTK3 dependencies :
# we should check for iconv() function needed by gtk+ for character encoding conversion
# "The libintl library from the GNU gettext package is needed if your system 
# doesn't have the gettext() functionality for handling message translation databases. "
# The libraries from the X window system are needed to build Pango and GTK+. You should already have these installed on your system, but it's possible that you'll need to install the development environment for these libraries that your operating system vendor provides.
# The fontconfig library provides Pango with a standard way of locating fonts and matching them against font names.
# Cairo is a graphics library that supports vector graphics and image compositing. Both Pango and GTK+ use cairo for all of their drawing.
# libepoxy is a library that abstracts the differences between different OpenGL libraries. GTK+ uses it for cross-platform GL support.
# The Wayland libraries are needed to build GTK+ with the Wayland backend.
# The shared-mime-info package is not a hard dependency of GTK+, but it contains definitions for mime types that are used by GIO and, indirectly, by GTK+. gdk-pixbuf will use GIO for mime type detection if possible. For this to work, shared-mime-info needs to be installed and XDG_DATA_DIRS set accordingly at configure time. Otherwise, gdk-pixbuf falls back to its built-in mime type detection. 


FIND_LIBRARY(M_LIB m)
# libdl
FIND_LIBRARY(LIBDL dl)
# zlib
FIND_LIBRARY(ZLIB REQUIRED)
FIND_PACKAGE(ZLIB)
# bz2
FIND_LIBRARY(BZip2 REQUIRED)
FIND_PACKAGE(BZip2)
# liblzma
FIND_LIBRARY(LibLZMA REQUIRED)
FIND_PACKAGE(LibLZMA)

MESSAGE(CYAN ">>>> ASDCP dependencies ")

# LibXml2
FIND_PACKAGE(LibXml2)

# LibXslt
FIND_PACKAGE(LibXslt)
IF(LIBXSLT_FOUND AND NOT XSLT_STATIC)
	ADD_DEFINITIONS(${LIBXSLT_DEFINITIONS})
ELSE()
	SET(COMPILE_XSLT 1)
ENDIF()

# OpenSSL
FIND_PACKAGE(OpenSSL 1.0 EXACT)
IF(NOT OPENSSL_FOUND)
	SET(COMPILE_OPENSSL 1)
ENDIF()

# Expat
FIND_PACKAGE(EXPAT)
IF(NOT EXPAT_FOUND)
	#~ FIND_PACKAGE(Xerces) ## Todo : a module to find Xerces
	SET(COMPILE_EXPAT 1)
ENDIF()

# xmlsec1
FIND_PACKAGE(XMLSec)
IF(NOT XMLSEC1_FOUND)
	SET(COMPILE_XMLSEC 1)
ENDIF()

MESSAGE(CYAN ">>>> IMAGEMAGICK dependencies ")

# jpeg, png, tiff
FIND_PACKAGE(JPEG REQUIRED)
IF(NOT JPEG_FOUND)
	MESSAGE(WARNING "-- Warning : ${JPEG_LIBRARIES}")
ENDIF()
FIND_PACKAGE(PNG REQUIRED)
IF(NOT PNG_FOUND)
	MESSAGE(WARNING "-- Warning : ${PNG_LIBRARIES}")
ENDIF()
FIND_PACKAGE(TIFF REQUIRED)
IF(NOT TIFF_FOUND)
	MESSAGE(WARNING "-- Warning : ${TIFF_LIBRARIES}")
ENDIF()

# jbig library (for imagemagick)
FIND_LIBRARY(JBIG jbig)
IF(JBIG)
	MESSAGE(GREEN "jbig : ${White}${JBIG}")
	SET(JBIG_LIBRARY ${JBIG})
ELSE()
	MESSAGE(RED "jbig : ${JBIG}")
ENDIF()

# librsvg (for ImageMagick)
FIND_LIBRARY(RSVG rsvg-2)
IF(RSVG)
	MESSAGE(GREEN "librsvg-2 : ${White}${RSVG}")
	SET(RSVG_LIBRARY ${RSVG})
ELSE()
	MESSAGE(RED "librsvg-2 : ${RSVG}")
ENDIF()

# FontConfig (for imagemagick)
FIND_LIBRARY(FONTCONFIG fontconfig)
IF(FONTCONFIG)
	MESSAGE(GREEN "fontconfig : ${White}${FONTCONFIG}")
	SET(FONTCONFIG_LIBRARY ${FONTCONFIG})
ELSE()
	MESSAGE(RED "fontconfig : ${FONTCONFIG}")
ENDIF()

#~ # Djvu(libre) (for imagemagick)
FIND_LIBRARY(DJVU djvulibre)
IF(DJVU)
	MESSAGE(GREEN "dejavu : ${White}${DJVU}")
	SET(DJVU_LIBRARY ${DJVU})
ELSE()
	MESSAGE(RED "dejavu : ${DJVU}")
ENDIF()

# Freetype (for cairo, ImageMagick, GTK+-3.0)
FIND_LIBRARY(FREETYPE freetype)
IF(FREETYPE)
	MESSAGE(GREEN "freetype : ${White}${FREETYPE}")
	SET(FREETYPE_LIBRARY ${FREETYPE})
ELSE()
	MESSAGE(RED "freetype : ${FREETYPE}")
ENDIF()

#~ # 'little colors manager system' library (for imagemagick)
#~ FIND_LIBRARY(LCMS2 lcms2)
#~ IF(LCMS2)
	#~ MESSAGE(GREEN "lcms2 : ${White}${LCMS2}")
	#~ SET(LCMS2_LIBRARY ${LCMS2})
#~ ELSE()
	#~ MESSAGE(RED "lcms2 : ${LCMS2}")
#~ ENDIF()

#~ # Wmf(lite) (vector format for imagemagick)
#~ FIND_LIBRARY(WMF wmf)
#~ FIND_LIBRARY(WMFLITE wmflite)
#~ IF(WMF)
	#~ MESSAGE(GREEN "wmf : ${White}${WMF}")
	#~ SET(WMF_LIBRARIES ${WMF})
	#~ IF(WMFLITE)
		#~ MESSAGE(GREEN "wmflite : ${White}${WMFLITE}")
		#~ SET(WMF_LIBRARIES ${WMF_LIBRARIES} ${WMFLITE})
	#~ ELSE()
		#~ MESSAGE(RED "wmflite : ${WMFLITE}")
	#~ ENDIF()
#~ ELSE()
	#~ MESSAGE(RED "wmf : ${WMF}")
#~ ENDIF()

#~ # Liquid rescale library (for ImageMagick)
#~ FIND_LIBRARY(LQR lqr-1)
#~ IF(LQR)
	#~ MESSAGE(GREEN "liquid rescale : ${White}${LQR}")
	#~ SET(LQR_LIBRARY ${LQR})
#~ ELSE()
	#~ MESSAGE(RED "liquid rescale : ${LQR}")
#~ ENDIF()

#~ # Freeimage (for ImageMagick)
#~ FIND_LIBRARY(IMF freeimage-3.15.4)
#~ IF(IMF)
	#~ MESSAGE(GREEN "freeimage : ${White}${IMF}")
	#~ SET(IMF_LIBRARIES ${IMF})
	#~ FIND_LIBRARY(ILMIMF IlmImf)
	#~ IF(ILMIMF)
		#~ MESSAGE(GREEN "IlmImf : ${White}${ILMIMF}")
		#~ SET(IMF_LIBRARIES ${IMF_LIBRARIES} ${ILMIMF})
	#~ ELSE()
		#~ MESSAGE(RED "IlmImf : ${ILMIMF}")
	#~ ENDIF()
#~ ELSE()
	#~ MESSAGE(RED "freeimage : ${IMF}")
#~ ENDIF()


IF(NO_GUI)
	# Glib-2.0 (for imagemagick...)
	FIND_LIBRARY(GLIB glib-2.0)
	IF(GLIB)
		MESSAGE(GREEN "glib-2.0 : ${White}${GLIB}")
		SET(GLIB_LIBRARY ${GLIB})
	ELSE()
		MESSAGE(RED "glib-2.0 : ${GLIB}")
	ENDIF()

	# GObject (for pango...)
	FIND_LIBRARY(GOBJECT gobject-2.0)
	IF(GOBJECT)
		MESSAGE(GREEN "gobject : ${White}${GOBJECT}")
		SET(GOBJECT_LIBRARY ${GOBJECT})
	ELSE()
		MESSAGE(RED "gobject : ${GOBJECT}")
	ENDIF()
	
	# Pango (for imagemagick if cli mode only)
	FIND_LIBRARY(PANGO pango-1.0)
	IF(PANGO)
		MESSAGE(GREEN "pango : ${White}${PANGO}")
		SET(PANGO_LIBRARY ${PANGO})
	ELSE()
		MESSAGE(RED "pango : ${PANGO}")
	ENDIF()
	
	# Cairo (for pango...)
	FIND_LIBRARY(CAIRO cairo)
	IF(CAIRO)
		MESSAGE(GREEN "Cairo : ${White}${CAIRO}")
		SET(CAIRO_LIBRARY ${CAIRO})
	ELSE()
		MESSAGE(RED "Cairo : ${CAIRO}")
	ENDIF()
	
	# Pangocairo (for pango...)
	FIND_LIBRARY(PANGOCAIRO pangocairo-1.0)
	IF(PANGOCAIRO)
		MESSAGE(GREEN "Pangocairo : ${White}${PANGOCAIRO}")
		SET(PANGOCAIRO_LIBRARY ${PANGOCAIRO})
	ELSE()
		MESSAGE(RED "Pangocairo : ${PANGOCAIRO}")
	ENDIF()

	# LibXext.so.6 (for cairo on debian jessie...)
	FIND_LIBRARY(XEXT Xext)
	IF(XEXT)
		MESSAGE(GREEN "libXext : ${White}${XEXT}")
		SET(XEXT_LIBRARY ${XEXT})
	ELSE()
		MESSAGE(RED "libXext : ${XEXT}")
	ENDIF()

	# LibX11.so.6 (for libgdk-3.0 on debian jessie...)
	FIND_LIBRARY(LIBX11 X11)
	IF(LIBX11)
		MESSAGE(GREEN "libX11 : ${White}${LIBX11}")
		SET(LIBX11_LIBRARY ${LIBX11})
	ELSE()
		MESSAGE(RED "libX11 : ${LIBX11}")
	ENDIF()
ENDIF(NO_GUI)

MESSAGE(CYAN ">>>> FFMPEG dependencies ")

# Yasm (asembler needed by FFmpeg)
FIND_LIBRARY(YASM_LIBRARY
	NAMES libyasm.a
)
FIND_PATH(YASM_INCLUDE_DIR
	NAMES libyasm.h
	HINTS /usr/include /usr/local/include
)
IF(YASM_LIBRARY-NOTFOUND)
	MESSAGE(RED "Couldn't find Yasm library.")
	SET(YASM_STATIC 1)
ELSE()
	IF(YASM_INCLUDE_DIR-NOTFOUND)
		MESSAGE(RED "Couldn't find Yasm header file.")
		SET(YASM_STATIC 1)
	ELSE()
		MESSAGE(GREEN "Yasm : ${White}${YASM_LIBRARY}")
	ENDIF()
ENDIF()

# Opus (for FFmpeg)
FIND_LIBRARY(OPUS opus REQUIRED)
IF(OPUS)
	MESSAGE(GREEN "libopus : ${White}${OPUS}")
ELSE()
	MESSAGE(RED "libopus : ${OPUS}")
ENDIF()

# Jasper (for FFmpeg)
#~ FIND_PACKAGE(Jasper)
#~ IF(NOT JASPER_FOUND)
	#~ SET(COMPILE_JASPER 1)
#~ ENDIF()

# libx264 (encoder for FFmpeg) Debian : libx264-dev

# libfdk-aac (audio encoder for FFmpeg) Debian : libfdk-aac-dev

MESSAGE(CYAN ">>>> QUICKDCP dependencies ")

# Doxygen executable (for documentation generation)
FIND_PACKAGE(Doxygen)
IF(NOT DOXYGEN_FOUND)
	MESSAGE(YELLOW "Optionnal Doxygen package not found")
	MESSAGE(YELLOW "We just won't generate API documentation")
	SET(ENABLE_DOC 0)
ENDIF()

MESSAGE(CYAN "·······································")
MESSAGE(CYAN "···· Looking for static libraries ·····")
MESSAGE(CYAN "·······································")

MESSAGE(CYAN ">>>> ASDCP libraries ")

# Asdcp & kumu

FIND_PACKAGE(QdcpASDCP REQUIRED)
IF(NOT ASDCP_FOUND OR NOT KUMU_FOUND)
	MESSAGE(RED "   ASDCP not found. We gonna have to compile.")
	SET(COMPILE_ASDCP 1)
ELSE()
	MESSAGE(STATUS "   Found QdcpAsdcp !")	
	SET(COMPILE_ASDCP 0)
ENDIF()

MESSAGE(CYAN ">>>> OPENJP2 library ")

# OpenJPEG2
FIND_PACKAGE(QdcpOpenJPEG2)
IF(NOT OPENJPEG2_FOUND)
	FIND_PACKAGE(OpenJPEG2)
	IF(NOT OPENJPEG2_FOUND)
		SET(COMPILE_OJPEG2 1)
	ENDIF()
ENDIF()


# ImageMagick

MESSAGE(CYAN ">>>> IMAGEMAGICK libraries ")

FIND_PACKAGE(QdcpImageMagick)
IF(MAGICKCORE_FOUND AND NOT COMPILE_IMAGICK)
	#~ FIND_PACKAGE(MagickWand)
	IF(MAGICKWAND_FOUND)
		MESSAGE(STATUS "   Found QdcpImageMagick !")
		ADD_DEFINITIONS(${MAGICKCORE_DEFINITIONS} ${MAGICKWAND_DEFINITIONS})
	ENDIF()
	SET(COMPILE_IMAGICK 0)
ELSE()
	MESSAGE(RED "   ImageMagick not found. Get a beer, we gonna have to compile.")
	SET(COMPILE_IMAGICK 1)
ENDIF()


# FFmpeg

MESSAGE(CYAN ">>>> FFMPEG libraries ")

FIND_PACKAGE(QdcpFFMPEG)
IF(FFMPEG_FOUND)
	MESSAGE(STATUS "   Found QdcpFFmpeg !")
	IF(COMPILE_FFMPEG)
		MESSAGE(YELLOW "   But force QdcpFFmpeg to recompile")
	ENDIF()
ELSE()
	message(RED "   Could not find QdcpFFMPEG libraries")
	message(YELLOW "   Searching an existing native version of FFMPEG")
	FIND_PACKAGE(FFMPEG QUIET)
	IF(FFMPEG_FOUND AND NOT COMPILE_FFMPEG)
		MESSAGE(GREEN "   Found regular FFmpeg : ${FFMPEG_LIBRARIES}")
		MESSAGE(YELLOW "   Should check its version and built-in options, but for now recompile a Qdcp FFmpeg version anyway...")
		SET(COMPILE_FFMPEG 1)
	ELSE()
		MESSAGE(RED "   FFmpeg not found. Order some food, we gonna have to compile.")
		SET(COMPILE_FFMPEG 1)
	ENDIF()
ENDIF()

IF(ENABLE_GUI)
	MESSAGE(CYAN ">>>> GTK3 libraries ")

	# Use the package PkgConfig to detect GTK+ headers/library files
	FIND_PACKAGE(PkgConfig REQUIRED)
	PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

	IF(GTK3_FOUND)
		MESSAGE(GREEN "GTK3 includes  : ${White}${GTK3_INCLUDE_DIRS}")
		MESSAGE(GREEN "GTK3 libraries : ${White}${GTK3_LIBRARIES}")
		MESSAGE(GREEN "GTK3 lib dirs  : ${White}${GTK3_LIBRARY_DIRS}")
		MESSAGE(GREEN "GTK3 definitions : ${White}${GTK3_CFLAGS_OTHER}")
	ELSE()
		MESSAGE(RED "GTK3 not found. Get some pizza, we gonna have to compile.")
		SET(COMPILE_GTK3 1)
	ENDIF()
ENDIF(ENABLE_GUI)
#-------------------------------------------------------------------------------
