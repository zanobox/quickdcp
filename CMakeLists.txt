CMAKE_MINIMUM_REQUIRED(VERSION 3.0)
#--let's have some happiness in the console output-----------------------------
INCLUDE("modules/CMakeColors.cmake")

#--project definition----------------------------------------------------------
PROJECT(QuickDCP)
SET(CMAKE_BUILD_TYPE Release)
#~ SET(CMAKE_BUILD_TYPE Debug)
STRING(TOLOWER ${PROJECT_NAME} PACKAGE_NAME)
SET(VERSION_MAJOR 0)
SET(VERSION_MINOR 1)
SET(VERSION_BUILD 1)
SET(VERSION_DATE "2016-09-24")
SET(QDCP_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_BUILD})
SET(QDCP_NAME ${PROJECT_NAME})
SET(QDCP_FULLNAME "QuickDCP DCP Generator")
SET(QDCP_COMMENT "QuickDCP generates home cooked Digital Cinema Packages")
SET(QDCP_CONTACT "jean.sabatier@zanobox.net")
SET(QDCP_COPYRIGHT "(c) 2014-2016 jean.sabatier@zanobox.net.")
SET(QDCP_LICENSE "The program is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.")
SET(QDCP_URL "https://github.com/zanobox/quickdcp")
SET(QDCP_SVG_ICON "src/gui/icons/scalable/quickdcp.svg")
SET(QDCP_DOXY_ICON "src/gui/icons/96/quickdcp.png")
SET(QUICKDCP_TARGETS "")
#-------------------------------------------------------------------------------

#--static libraries versions----------------------------------------------------
SET(QDCP_IMAGICK_VERSION 		"ImageMagick-7.0.3-4")	
SET(QDCP_FFMPEG_VERSION 		"ffmpeg-3.1.5")
SET(QDCP_ASDCP_VERSION 			"asdcplib-2.7.19")	# released in 2016-12-10
SET(QDCP_OPJP2_VERSION 			"openjpeg-2.1.2")	
SET(QDCP_OPENSSL_VERSION 		"openssl-1.0.2l")		# long time support until 2019
#~ SET(QDCP_OPENSSL_VERSION 		"openssl-1.1.0f")		# incompatible
SET(QDCP_XMLSEC_VERSION 		"xmlsec1-1.2.23")
SET(QDCP_XMLSEC_WIN32_VERSION 	"xmlsec1-1.2.18")
#-------------------------------------------------------------------------------

#--options-(default values)-----------------------------------------------------
OPTION(NO_CLI			"Disable compiling of command-line program" ON)
OPTION(NO_GUI			"Disable compiling of Gtk+ program" OFF)
OPTION(NO_DOC			"Disable generating documentation" ON)

OPTION(USERLAND			"Install as unprivileged user" OFF)
OPTION(DOWNLOAD			"Download source of compiled libraries" OFF)
OPTION(ENABLE_OPENMP   	"Enable OPENMP multithreading" ON)
OPTION(GENERATE_LANGUAGE_FILES "Generate Translation files" OFF)
OPTION(ENABLE_DEBUG    	"Enable debug symbols" OFF)
OPTION(ENABLE_CLANG    	"Enable CLANG compiling (OSX)" OFF)

OPTION(DEB             	"Create a debian package" ON)
OPTION(RPM             	"Create an RPM package" OFF)
OPTION(TAR             	"Create a source package" OFF)

OPTION(XML2_STATIC		"Build libxml2 as a shared library" OFF)
OPTION(XSLT_STATIC   	"Build libxslt as a shared library" OFF)
OPTION(COMPILE_XMLSEC   "Build libxmlsec1 as a static library" OFF)
OPTION(COMPILE_ASDCP	"Build asdcplib as static libraries" OFF)

OPTION(COMPILE_OJPEG2	"Build openjpeg as a static library" OFF)
OPTION(COMPILE_JASPER	"Build libjasper as a static library" OFF)
OPTION(COMPILE_IMAGICK 	"Build ImageMagick as static libraries" OFF)
OPTION(COMPILE_GMAGICK 	"Build GraphicsMagick as static libraries" OFF)
OPTION(COMPILE_FFMPEG	"Build FFmpeg as static libraries" OFF)
OPTION(IMAGEMAGICK7		"Compile ImageMagick-7 rather than 6" ON)

SET(ENABLE_CLI 1)
SET(ENABLE_GUI 1)
IF(NO_CLI)
	SET(ENABLE_CLI 0)
ENDIF()
IF(NO_GUI)
	SET(ENABLE_GUI 0)
ENDIF()
#-------------------------------------------------------------------------------

#--directories------------------------------------------------------------------
SET(QDCP_SOURCE_DIR 		 "${CMAKE_CURRENT_SOURCE_DIR}")
SET(QDCP_BINARY_DIR 		 "${QDCP_SOURCE_DIR}/build")
SET(CMAKE_BINARY_DIR 		 "${QDCP_BINARY_DIR}")
SET(CMAKE_CURRENT_BINARY_DIR "${QDCP_BINARY_DIR}")
SET(CMAKE_MODULE_PATH 		 "${QDCP_SOURCE_DIR}/modules")
SET(STATIC_LIBS_DIR 		 "${QDCP_SOURCE_DIR}/libs")
SET(EXECUTABLE_OUTPUT_PATH 	 "${QDCP_SOURCE_DIR}/dist")
SET(QDCP_ICON_DIR			 "${QDCP_SOURCE_DIR}/src/gui/icons")

IF(WIN32)
	SET(INSTALL_PREFIX "C:Programs Files\${PACKAGE_NAME}")
ELSE(WIN32)
	SET(INSTALL_PREFIX "/usr/local")
	IF(USERLAND)
		SET(INSTALL_PREFIX "$ENV{HOME}")
	ENDIF(USERLAND)
ENDIF(WIN32)
SET(PREFIX ${INSTALL_PREFIX})
SET(PREFIX_ARG --prefix=${PREFIX})
#-------------------------------------------------------------------------------

#--check 32 or 64-bit-----------------------------------------------------------
IF(CMAKE_SIZEOF_VOID_P MATCHES "8")
    SET(HOST_ARCH x86_64)
ELSE()
    #~ SET(HOST_ARCH i686)
    SET(HOST_ARCH i386)
ENDIF()
SET(TARGET_ARCH ${HOST_ARCH})

ADD_DEFINITIONS(-D_FILE_OFFSET_BITS=64)
#-------------------------------------------------------------------------------

#--cmake options----------------------------------------------------------------
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
#-------------------------------------------------------------------------------

#~ MESSAGE(STATUS)
MESSAGE(CYAN "================================================================================")
MESSAGE(CYAN "    ${PROJECT_NAME} Version ${QDCP_VERSION} on ${CMAKE_SYSTEM} (${TARGET_ARCH})")
MESSAGE(CYAN "================================================================================")
#~ MESSAGE(STATUS)
#-------------------------------------------------------------------------------

#--set base compiler flags------------------------------------------------------
SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -pg -g -D_REENTRANT")
IF(ENABLE_DEBUG)
    SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} -Wall -Wextra -O3 ${OPENMP}")
ENDIF()
SET(CMAKE_CXX_FLAGS ${CMAKE_C_FLAGS})

IF(ENABLE_OPENMP)
    ADD_DEFINITIONS(-DOPENMP)
    FIND_PACKAGE(OpenMP)
    if(OPENMP_FOUND)
        SET(CMAKE_C_FLAGS   "${OpenMP_C_FLAGS} ${CMAKE_C_FLAGS}")
        SET(CMAKE_CXX_FLAGS "${OpenMP_CXX_FLAGS} ${CMAKE_CXX_FLAGS}")
    endif()
ENDIF()

#-------------------------------------------------------------------------------

#--inlcude os specific----------------------------------------------------------
INCLUDE(ExternalProject)
IF (APPLE)
	IF (NOT ENABLE_CLANG)
		set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -fvisibility=default")
	ENDIF()
	SET(QDCP_OS_DIR "${QDCP_SOURCE_DIR}/os/apple")
    INCLUDE("${QDCP_OS_DIR}/osx.cmake")
ELSEIF(WIN32)
	SET(QDCP_OS_DIR "${QDCP_SOURCE_DIR}/os/win32")
    INCLUDE("${QDCP_OS_DIR}/win32.cmake")
ELSE()
	SET(QDCP_OS_DIR "${QDCP_SOURCE_DIR}/os/debian")
    INCLUDE("${QDCP_OS_DIR}/linux.cmake")
ENDIF()
#-------------------------------------------------------------------------------

#--build libraries--------------------------------------------------------------
INCLUDE(${QDCP_SOURCE_DIR}/srclib/CompileLibs.cmake)
#-------------------------------------------------------------------------------

#--headers----------------------------------------------------------------------
INCLUDE_DIRECTORIES(${QDCP_SOURCE_DIR}/src)
INCLUDE_DIRECTORIES(${INSTALL_PREFIX}/include)

INCLUDE_DIRECTORIES(${LIBXML2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENSSL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBXSLT_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${XMLSEC1_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${XMLSEC1-OPENSSL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${EXPAT_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${ASDCP_INCLUDE_DIRS})
INCLUDE_DIRECTORIES(${KUMU_INCLUDE_DIRS})

INCLUDE_DIRECTORIES(${BZIP2_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${ZLIB_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${LIBLZMA_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${JASPER_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${FFMPEG_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${OPENJPEG2_INCLUDE_DIR}) 
INCLUDE_DIRECTORIES(${MAGICKCORE_INCLUDE_DIR}) 
INCLUDE_DIRECTORIES(${MAGICK_BASECONFIG_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${MAGICKWAND_INCLUDE_DIR})

INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
#-------------------------------------------------------------------------------

#--libraries--------------------------------------------------------------------
## About linking errors :
## http://stackoverflow.com/questions/12573816/what-is-an-undefined-reference-unresolved-external-symbol-error-and-how-do-i-fix
SET(LIBS ${LIBS} ${MAGICKWAND_LIBRARIES})
SET(LIBS ${LIBS} ${MAGICKCORE_LIBRARIES}) 
SET(LIBS ${LIBS} ${MAGICK_BASECONFIG_LIBRARIES})
SET(LIBS ${LIBS} ${OPENJPEG2_LIBRARIES}) 
SET(LIBS ${LIBS} ${FONTCONFIG_LIBRARY})
SET(LIBS ${LIBS} ${RSVG_LIBRARY})
SET(LIBS ${LIBS} ${PNG_LIBRARIES}) 
SET(LIBS ${LIBS} ${JPEG_LIBRARIES}) 
SET(LIBS ${LIBS} ${TIFF_LIBRARIES}) 
SET(LIBS ${LIBS} ${JBIG_LIBRARY}) 
SET(LIBS ${LIBS} ${DJVU_LIBRARY}) 
#~ SET(LIBS ${LIBS} ${LCMS2_LIBRARY}) 
#~ SET(LIBS ${LIBS} ${WMF_LIBRARIES}) 
#~ SET(LIBS ${LIBS} ${LQR_LIBRARY}) 
#~ SET(LIBS ${LIBS} ${IMF_LIBRARIES}) 

SET(LIBS ${LIBS} ${FFMPEG_LIBRARIES})
SET(LIBS ${LIBS} ${JASPER_LIBRARIES})
SET(LIBS ${LIBS} ${LIBLZMA_LIBRARIES})
SET(LIBS ${LIBS} ${BZIP2_LIBRARIES})
SET(LIBS ${LIBS} ${ZLIB_LIBRARIES})
SET(LIBS ${LIBS} ${LIBDL})
SET(LIBS ${LIBS} ${M_LIB})

IF(ENABLE_GUI)
	LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})
	SET(LIBS ${LIBS} ${GTK3_LIBRARIES})
	SET(LIBS ${LIBS} ${FREETYPE_LIBRARY}) 
	SET(LIBS ${LIBS} ${LIBX11_LIBRARY}) 
	SET(LIBS ${LIBS} ${XEXT_LIBRARY}) 
ELSE()
	# we still need pango and gobject for imagemagick
	SET(LIBS ${LIBS} ${PANGOCAIRO_LIBRARY}) 
	SET(LIBS ${LIBS} ${PANGO_LIBRARY}) 
	SET(LIBS ${LIBS} ${GOBJECT_LIBRARY}) 
	SET(LIBS ${LIBS} ${FREETYPE_LIBRARY}) 
	SET(LIBS ${LIBS} ${CAIRO_LIBRARY}) 
	SET(LIBS ${LIBS} ${LIBX11_LIBRARY}) 
	SET(LIBS ${LIBS} ${XEXT_LIBRARY}) 
	SET(LIBS ${LIBS} ${GLIB_LIBRARY})
ENDIF()

SET(LIBS ${LIBS} ${ASDCP_LIBRARIES})
SET(LIBS ${LIBS} ${EXPAT_LIBRARIES})
SET(LIBS ${LIBS} ${XMLSEC1_LIBRARIES})
SET(LIBS ${LIBS} ${XMLSEC1-OPENSSL_LIBRARIES})
SET(LIBS ${LIBS} ${OPENSSL_LIBRARIES})
SET(LIBS ${LIBS} ${LIBXSLT_LIBRARIES})
SET(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
#-------------------------------------------------------------------------------

#--definitions------------------------------------------------------------------
#~ ADD_DEFINITIONS(-DXMLSEC)
#~ ADD_DEFINITIONS(-DLIBXML_STATIC)
#~ ADD_DEFINITIONS(-DLIBXSLT_STATIC)
#~ ADD_DEFINITIONS(-DXMLSEC_STATIC)
#~ ADD_DEFINITIONS(XMLSEC1_DEFINITIONS)
#~ ADD_DEFINITIONS(XMLSEC1-OPENSSL_DEFINITIONS)
# for imagemagick
ADD_DEFINITIONS(-DMAGICKCORE_HDRI_ENABLE=1)
ADD_DEFINITIONS(-DMAGICKCORE_QUANTUM_DEPTH=16)

ADD_DEFINITIONS(${OPENJPEG2_DEFINITIONS})
ADD_DEFINITIONS(${GTK3_CFLAGS_OTHER})
#-------------------------------------------------------------------------------

#--configure files--------------------------------------------------------------
MESSAGE(CYAN "=== Configuring ${PACKAGE_NAME} ================================")
CONFIGURE_FILE(${QDCP_SOURCE_DIR}/src/quickdcp.h.cmake ${QDCP_SOURCE_DIR}/src/quickdcp.h)
#-------------------------------------------------------------------------------

#--quickdcp sources lists-------------------------------------------------------
SET(QUICKDCP_SRC
	src/quickdcp.c
	src/qdcp_config.c 
	src/qdcp_projects.c 
	src/qdcp_photograms.c 
	src/qdcp_wav.c 
	src/qdcp_sub.c 
	src/qdcp_video.c 
	src/qdcp_mxf.c 
	src/qdcp_checksum.c 
	src/qdcp_xml.c 
	src/qdcp_xml_sign.c 
	src/qdcp_xml_verify.c 
	src/iniparser.c 
	src/dictionary.c
	src/asdcp_intf.cpp
)

SET(QUICKDCP_CLI_SRC "")
IF (ENABLE_CLI)
	SET(QUICKDCP_CLI_SRC
		src/qdcp_cli.c
		${QUICKDCP_SRC}
	)
ENDIF(ENABLE_CLI)

SET(QUICKDCP_GUI_SRC "")
IF (ENABLE_GUI)
	#~ IF(GTK3_FOUND)
		#~ SET(QDCP_GTK_SRC "src/qdcp_gtk3.c")
		#~ MESSAGE("-- Loading gtk3 api : ${QDCP_GTK_SRC}")
	#~ ELSE()
		#~ SET(QDCP_GTK_SRC "src/qdcp_gtk2.c")
		#~ MESSAGE(WARNING "-- Loading gtk2 api : ${QDCP_GTK_SRC}")
	#~ ENDIF()
	
	MESSAGE(STATUS "Compiling Gtk user interface XML resources")
	EXECUTE_PROCESS(
		COMMAND glib-compile-resources gui/gtk/quickdcp.gresource.xml --target=qdcp_resources.c --generate-source
		WORKING_DIRECTORY ${QDCP_SOURCE_DIR}/src
	)

	SET(QUICKDCP_GUI_SRC
		src/qdcp_gui.c
		src/qdcp_resources.c
		${QUICKDCP_SRC}
	)
ENDIF(ENABLE_GUI)
#-------------------------------------------------------------------------------

#--internatiolization-----------------------------------------------------------
FIND_PACKAGE(Gettext)
IF(GETTEXT_FOUND)
	MESSAGE(STATUS "Generating I18n with Gettext")
#~ GETTEXT_MSGMERGE_EXECUTABLE
#~ GETTEXT_MSGFMT_EXECUTABLE
#~ GETTEXT_CREATE_TRANSLATIONS( ${QDCP_SOURCE_DIR}/src/i18n/${LOCALE}.pot [ALL] ${QUICKDCP_CLI_SRC} ${QUICKDCP_GUI_SRC} )
#~ GETTEXT_PROCESS_POT( <potfile> [ALL] [INSTALL_DESTINATION <destdir>] LANGUAGES <lang1> <lang2> ... )
#~ GETTEXT_PROCESS_PO_FILES( <lang> [ALL] [INSTALL_DESTINATION <dir>] PO_FILES <po1> <po2> ... )
ENDIF(GETTEXT_FOUND)
#-------------------------------------------------------------------------------

#-------------------------------------------------------------------------------
IF(CMAKE_BUILD_TYPE STREQUAL "Debug")
	MESSAGE(STATUS "Useful information :")
	MESSAGE(GREEN "CMAKE_C_FLAGS : ${White}${CMAKE_C_FLAGS}")
	MESSAGE(" ")
	MESSAGE(GREEN "Libs : ${White}${LIBS}")
	MESSAGE(" ")
	MESSAGE(GREEN "Includes : ${White}${LIBS}")
	MESSAGE(" ")
ENDIF()
#-------------------------------------------------------------------------------

#--set output targets and paths-------------------------------------------------
IF(ENABLE_CLI)
	ADD_EXECUTABLE(${PACKAGE_NAME}-cli ${QUICKDCP_CLI_SRC})
	TARGET_LINK_LIBRARIES(${PACKAGE_NAME}-cli ${LIBS})
	
	SET(QUICKDCP_TARGETS ${QUICKDCP_TARGETS} ${PACKAGE_NAME}-cli)
ENDIF(ENABLE_CLI)

IF(ENABLE_GUI)
    ADD_EXECUTABLE(${PACKAGE_NAME} ${QUICKDCP_GUI_SRC})
	TARGET_LINK_LIBRARIES(${PACKAGE_NAME} ${LIBS})
	
	SET(QUICKDCP_TARGETS ${QUICKDCP_TARGETS} ${PACKAGE_NAME})
ENDIF(ENABLE_GUI)
#-------------------------------------------------------------------------------


IF(CMAKE_BUILD_TYPE STREQUAL "Release")

	IF(NOT NO_DOC)
		#--generating documentaiton-----------------------------------------------------
		MESSAGE(CYAN "=== Generating documentation =============================")
		CONFIGURE_FILE(${QDCP_SOURCE_DIR}/DoxyFile.cmake ${QDCP_SOURCE_DIR}/src/doc/DoxyFile)
		EXECUTE_PROCESS(
			COMMAND doxygen ../DoxyFile
			WORKING_DIRECTORY ${QDCP_SOURCE_DIR}/src/doc/Doxygen
		)
	ENDIF(NOT NO_DOC)
	#-------------------------------------------------------------------------------

	#--platform related build with cpack--------------------------------------------
	MESSAGE(CYAN "=== Packaging with Cpack =================================")
	# build a CPack driven installer package
	INCLUDE(InstallRequiredSystemLibraries)
	SET(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
	SET(CPACK_PACKAGE_VERSION_MAJOR "${VERSION_MAJOR}")
	SET(CPACK_PACKAGE_VERSION_MINOR "${VERSION_MINOR}")
	SET(CPACK_PACKAGE_VERSION_BUILD "${VERSION_BUILD}")
	SET(CPACK_PACKAGE_SUMMARY "${QDCP_FULLNAME}")
	SET(CPACK_PACKAGE_DESCRIPTION "${QDCP_COMMENT}")
	SET(CPACK_PACKAGE_VENDOR "${QDCP_CONTACT}")
	SET(CPACK_PACKAGE_CONTACT "${QDCP_CONTACT}")
	SET(CPACK_RESOURCE_FILE_LICENSE "${QDCP_SOURCE_DIR}/GNU_GPL_LICENSE")

	SET(CPACK_BINARY_DEB ON)
	SET(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "${TARGET_ARCH}")
	SET(CPACK_DEBIAN_PACKAGE_DEPENDS "")
	SET(CPACK_DEBIAN_PACKAGE_MAINTAINER "${QDCP_CONTACT}")
	SET(CPACK_DEBIAN_PACKAGE_DESCRIPTION "${QDCP_COMMENT}")
	#~ SET(CPACK_DEBIAN_PACKAGE_SECTION "devel")
	#~ SET(CPACK_DEBIAN_PACKAGE_PRIORITY "optional")
	#~ SET(CPACK_DEBIAN_PACKAGE_RECOMMENDS "")
	#~ SET(CPACK_DEBIAN_PACKAGE_SUGGESTS "")
	#~ SET(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "")

	INCLUDE(CPack)

	#-------------------------------------------------------------------------------
	## build the platform related binary distribution with command : 
	## 		cpack --config CPackConfig.cmake
	## build the platform related source distribution with command : 
	## 		cpack --config CPackSourceConfig.cmake

	#--install resources------------------------------------------------------------
	# this may require root privileges and should be done at install time

	IF(UNIX)
		MESSAGE(STATUS "Resource files to be installed in ${INSTALL_PREFIX}/share")
		CONFIGURE_FILE(${QDCP_OS_DIR}/${PACKAGE_NAME}.desktop.cmake ${QDCP_OS_DIR}/${PACKAGE_NAME}.desktop)
		CONFIGURE_FILE(${QDCP_OS_DIR}/PostInstall.cmake.cmake ${QDCP_SOURCE_DIR}/PostInstall.cmake)

		# generate icons (if not already done before)
		IF(NOT EXISTS ${QDCP_ICON_DIR}/)
			EXECUTE_PROCESS(
				COMMAND ./generate_icons.sh
				WORKING_DIRECTORY ${QDCP_SOURCE_DIR}/src/gui
			)
		ENDIF()
		# install icons (appicon, tools, bullets, buttons, ...)
		# app_icon
		INSTALL(FILES ${QDCP_ICON_DIR}/${PACKAGE_NAME}/scalable/apps/${PACKAGE_NAME}.svg DESTINATION ${INSTALL_PREFIX}/share/icons/${PACKAGE_NAME})
		# scalable and scaled icons in quickdcp gtk-theme and/or hicolor (default) gtk-theme
		INSTALL(DIRECTORY ${QDCP_SOURCE_DIR}/src/gui/icons DESTINATION ${INSTALL_PREFIX}/share)

		# patterns directory in /usr/local/share/quickdcp
		INSTALL(DIRECTORY ${QDCP_SOURCE_DIR}/src/gui/patterns DESTINATION ${INSTALL_PREFIX}/share/${PACKAGE_NAME})
		# bullets directory in /usr/local/share/quickdcp
		#~ INSTALL(DIRECTORY ${QDCP_SOURCE_DIR}/src/gui/bullets DESTINATION ${INSTALL_PREFIX}/share/${PACKAGE_NAME})
		# darkzano gtk-theme in /usr/local/share/quickdcp
		INSTALL(DIRECTORY src/gui/themes DESTINATION ${INSTALL_PREFIX}/share/${PACKAGE_NAME})

		# install desktop file in /usr/local/share/applications
		INSTALL(FILES ${QDCP_OS_DIR}/${PACKAGE_NAME}.desktop DESTINATION ${INSTALL_PREFIX}/share/applications)
		# install mime info xml file in /usr/local/share/mime/packages
		INSTALL(FILES ${QDCP_OS_DIR}/${PACKAGE_NAME}.xml DESTINATION ${INSTALL_PREFIX}/share/mime/packages)
		
		# man pages go to /usr/[local]/share/man/[fr]/man1/[app_name].gz
		INSTALL(DIRECTORY ${QDCP_SOURCE_DIR}/src/doc/man DESTINATION ${INSTALL_PREFIX}/share)
		# fonts directory into /usr/local/share
		#~ INSTALL(DIRECTORY src/gui/fonts DESTINATION ${INSTALL_PREFIX}/share/${PACKAGE_NAME})
		MESSAGE(STATUS "Binary targets to be installed in ${INSTALL_PREFIX}/bin")
	ENDIF(UNIX)
	#-------------------------------------------------------------------------------
	
	#--basic tests------------------------------------------------------------------
	IF(ENABLE_CLI)
		INSTALL(FILES "dist/${PACKAGE_NAME}-cli" 
				DESTINATION "${INSTALL_PREFIX}/bin"
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
							GROUP_READ GROUP_EXECUTE 
							WORLD_READ WORLD_EXECUTE
		)
		ADD_TEST(QDCP-CLIRuns quickdcp-cli)
	ENDIF(ENABLE_CLI)
	IF(ENABLE_GUI)
		INSTALL(FILES "dist/${PACKAGE_NAME}" 
				DESTINATION "${INSTALL_PREFIX}/bin"
				PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE 
							GROUP_READ GROUP_EXECUTE 
							WORLD_READ WORLD_EXECUTE
		)
		ADD_TEST(QDCPRuns quickdcp)
	ENDIF(ENABLE_GUI)
	
	#--post-install script----------------------------------------------------------
	INSTALL(SCRIPT "${QDCP_SOURCE_DIR}/PostInstall.cmake")
	#-------------------------------------------------------------------------------

ENDIF(CMAKE_BUILD_TYPE STREQUAL "Release")

#--EOF--------------------------------------------------------------------------
