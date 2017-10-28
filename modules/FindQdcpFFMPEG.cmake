#.rst:
# FindFFMPEG
# ----------
#
# Find the native FFMPEG includes and library
#
# This module defines::
#
#  FFMPEG_INCLUDE_DIR, where to find avcodec.h, avformat.h ...
#  FFMPEG_LIBRARIES, the libraries to link against to use FFMPEG.
#  FFMPEG_FOUND, If false, do not try to use FFMPEG.
#
# also defined, but not for general use are::
#
#   FFMPEG_avformat_LIBRARY, where to find the FFMPEG avformat library.
#   FFMPEG_avcodec_LIBRARY, where to find the FFMPEG avcodec library.
#
# This is useful to do it this way so that we can always add more libraries
# if needed to ``FFMPEG_LIBRARIES`` if ffmpeg ever changes...

#=============================================================================
# Copyright: 1993-2008 Ken Martin, Will Schroeder, Bill Lorensen
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of YCM, substitute the full
#  License text for the above reference.)

# Originally from VTK project

## zanobox.net : outcommented some lines for the sake of the
## time.h 'bug' (see http://trac.ffmpeg.org/ticket/1783)
## other lines were commented out to find only quickdcp-compatible
## version of FFmpeg

message(STATUS "Trying to find QuickDCP version of FFmpeg")

find_path(FFMPEG_INCLUDE_DIR1 libavformat/avformat.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

find_path(FFMPEG_INCLUDE_DIR2 libavutil/avutil.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

find_path(FFMPEG_INCLUDE_DIR3 libavcodec/avcodec.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

find_path(FFMPEG_INCLUDE_DIR4 libswscale/swscale.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

find_path(FFMPEG_INCLUDE_DIR5 libavdevice/avdevice.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

find_path(FFMPEG_INCLUDE_DIR6 libswresample/swresample.h
	PATHS
  ${STATIC_LIBS_DIR}/include/qdcp-ffmpeg
  ${INSTALL_PREFIX}/include/qdcp-ffmpeg
  NO_DEFAULT_PATH
)

if(FFMPEG_INCLUDE_DIR1)
  if(FFMPEG_INCLUDE_DIR2)
    if(FFMPEG_INCLUDE_DIR3)
      #~ set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR1}
                             #~ ${FFMPEG_INCLUDE_DIR2}
                             #~ ${FFMPEG_INCLUDE_DIR3})
    endif()
  endif()
endif()

if(FFMPEG_INCLUDE_DIR4)
  #~ set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         #~ ${FFMPEG_INCLUDE_DIR4})
endif()

if(FFMPEG_INCLUDE_DIR5)
  #~ set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         #~ ${FFMPEG_INCLUDE_DIR5})
endif()

if(FFMPEG_INCLUDE_DIR6)
  #~ set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         #~ ${FFMPEG_INCLUDE_DIR5}
                         #~ ${FFMPEG_INCLUDE_DIR5}/..)
	set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR5})
	message(STATUS "   Found QdcpFFMPEG includes : ${FFMPEG_INCLUDE_DIR}")
endif()

find_library(FFMPEG_avformat_LIBRARY avformat
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)

find_library(FFMPEG_avcodec_LIBRARY avcodec
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)

find_library(FFMPEG_avutil_LIBRARY avutil
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)

if(NOT DISABLE_SWSCALE)
  find_library(FFMPEG_swscale_LIBRARY swscale
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)
endif(NOT DISABLE_SWSCALE)

find_library(FFMPEG_avdevice_LIBRARY avdevice
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)

find_library(FFMPEG_swresample_LIBRARY swresample
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)

find_library(_FFMPEG_z_LIBRARY_ z
	PATHS
  ${STATIC_LIBS_DIR}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib/qdcp-ffmpeg
  ${INSTALL_PREFIX}/lib
  NO_DEFAULT_PATH
)



if(FFMPEG_INCLUDE_DIR)
  if(FFMPEG_avformat_LIBRARY)
    if(FFMPEG_avcodec_LIBRARY)
      if(FFMPEG_avutil_LIBRARY)
        if(FFMPEG_avdevice_LIBRARY)
			set(FFMPEG_FOUND "YES")
			set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES} 
								 ${FFMPEG_avdevice_LIBRARY}
								 ${FFMPEG_avformat_LIBRARY}
								 ${FFMPEG_avcodec_LIBRARY}
								 ${FFMPEG_avutil_LIBRARY}
			)
        endif()
        if(FFMPEG_swresample_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swresample_LIBRARY}
          )
        endif()
        if(FFMPEG_swscale_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swscale_LIBRARY}
          )
        endif()
        if(_FFMPEG_z_LIBRARY_)
          set( FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                                ${_FFMPEG_z_LIBRARY_}
          )
        endif()
      endif()
    endif()
  endif()
endif()

IF(FFMPEG_FOUND)
	message(STATUS "   Found QdcpFFMPEG libraries : ${White}${FFMPEG_LIBRARIES}")
	message(STATUS "   Found QdcpFFMPEG includes  : ${White}${FFMPEG_INCLUDE_DIR}")
ENDIF()

mark_as_advanced(
  FFMPEG_INCLUDE_DIR
  FFMPEG_INCLUDE_DIR1
  FFMPEG_INCLUDE_DIR2
  FFMPEG_INCLUDE_DIR3
  FFMPEG_INCLUDE_DIR4
  FFMPEG_INCLUDE_DIR5
  FFMPEG_avformat_LIBRARY
  FFMPEG_avcodec_LIBRARY
  FFMPEG_avutil_LIBRARY
  FFMPEG_swscale_LIBRARY
  FFMPEG_avdevice_LIBRARY
  _FFMPEG_z_LIBRARY_
)

# Set package properties if FeatureSummary was included
if(COMMAND set_package_properties)
  set_package_properties(FFMPEG PROPERTIES DESCRIPTION "A complete, cross-platform solution to record, convert and stream audio and video")
  set_package_properties(FFMPEG PROPERTIES URL "http://ffmpeg.org/")
endif()
