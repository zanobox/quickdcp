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

find_path(FFMPEG_INCLUDE_DIR1 avformat.h
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/ffmpeg
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/libavformat
  #~ ${QDCP_BINARY_DIR}/include/libavformat
  ${QDCP_BINARY_DIR}/include/ffmpeg
  $ENV{HOME}/bin/qdcp-ffmpeg
  /usr/local/include/qdcp-ffmpeg
  #~ /usr/local/include/ffmpeg
  #~ /usr/include/ffmpeg
  #/usr/include/libavformat
  /usr/include/ffmpeg/libavformat
  /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/libavformat
  /usr/local/include/libavformat
)

find_path(FFMPEG_INCLUDE_DIR2 avutil.h
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/ffmpeg
  ${QDCP_BINARY_DIR}/libavutil
  # ${QDCP_BINARY_DIR}/include/libavutil	-> leads to a system conflicting time.h
  ${QDCP_BINARY_DIR}/include/ffmpeg
  $ENV{HOME}/bin/qdcp-ffmpeg
  /usr/local/include/qdcp-ffmpeg
  #~ /usr/local/include/ffmpeg
  #~ /usr/include/ffmpeg
  # /usr/include/libavutil	-> leads to a system conflicting time.h
  /usr/include/ffmpeg/libavutil
  /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/libavutil
  /usr/local/include/libavutil
)

find_path(FFMPEG_INCLUDE_DIR3 avcodec.h
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/ffmpeg
  ${QDCP_BINARY_DIR}/libavcodec
  #~ ${QDCP_BINARY_DIR}/include/libavcodec
  ${QDCP_BINARY_DIR}/include/ffmpeg
  $ENV{HOME}/bin/qdcp-ffmpeg
  /usr/local/include/qdcp-ffmpeg
  #~ /usr/local/include/ffmpeg
  #~ /usr/include/ffmpeg
  #~ /usr/include/libavcodec
  /usr/include/ffmpeg/libavcodec
  /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/libavcodec
  /usr/local/include/libavcodec
)

find_path(FFMPEG_INCLUDE_DIR4 swscale.h
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/ffmpeg
  ${QDCP_BINARY_DIR}/libswscale
  #~ ${QDCP_BINARY_DIR}/include/libswscale
  ${QDCP_BINARY_DIR}/include/ffmpeg
  $ENV{HOME}/bin/qdcp-ffmpeg
  /usr/local/include/qdcp-ffmpeg
  #~ /usr/local/include/ffmpeg
  #~ /usr/include/ffmpeg
  #~ /usr/include/libswscale
  /usr/include/ffmpeg/libswscale
  /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/libswscale
  /usr/local/include/libswscale
)

find_path(FFMPEG_INCLUDE_DIR5 avdevice.h
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/ffmpeg
  #~ ${QDCP_BINARY_DIR}/libavdevice
  ${QDCP_BINARY_DIR}/include/libavdevice
  ${QDCP_BINARY_DIR}/include/ffmpeg
  $ENV{HOME}/bin/qdcp-ffmpeg
  /usr/local/include/qdcp-ffmpeg
  #~ /usr/local/include/ffmpeg
  #~ /usr/include/ffmpeg
  #~ /usr/include/libavdevice
  /usr/include/ffmpeg/libavdevice
  /usr/include/${CMAKE_LIBRARY_ARCHITECTURE}/libavdevice
  /usr/local/include/libavdevice
)

if(FFMPEG_INCLUDE_DIR1)
  if(FFMPEG_INCLUDE_DIR2)
    if(FFMPEG_INCLUDE_DIR3)
      set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR1}
                             ${FFMPEG_INCLUDE_DIR2}
                             ${FFMPEG_INCLUDE_DIR3})
    endif()
  endif()
endif()

if(FFMPEG_INCLUDE_DIR4)
  set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         ${FFMPEG_INCLUDE_DIR4})
endif()

if(FFMPEG_INCLUDE_DIR5)
  set(FFMPEG_INCLUDE_DIR ${FFMPEG_INCLUDE_DIR}
                         ${FFMPEG_INCLUDE_DIR5}
                         ${FFMPEG_INCLUDE_DIR5}/..)
endif()

find_library(FFMPEG_avformat_LIBRARY avformat
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/lib
  ${QDCP_BINARY_DIR}/libavformat
  $ENV{HOME}/lib/qdcp-ffmpeg
  /usr/local/lib/qdcp-ffmpeg
  /usr/local/lib
  /usr/lib
)

find_library(FFMPEG_avcodec_LIBRARY avcodec
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/lib
  ${QDCP_BINARY_DIR}/libavcodec
  $ENV{HOME}/lib/qdcp-ffmpeg
  /usr/local/lib/qdcp-ffmpeg
  /usr/local/lib
  /usr/lib
)

find_library(FFMPEG_avutil_LIBRARY avutil
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/lib
  ${QDCP_BINARY_DIR}/libavutil
  $ENV{HOME}/lib/qdcp-ffmpeg
  /usr/local/lib/qdcp-ffmpeg
  /usr/local/lib
  /usr/lib
)

if(NOT DISABLE_SWSCALE)
  find_library(FFMPEG_swscale_LIBRARY swscale
    ${QDCP_BINARY_DIR}
	${QDCP_BINARY_DIR}/qdcp-ffmpeg
    ${QDCP_BINARY_DIR}/lib
    ${QDCP_BINARY_DIR}/libswscale
	$ENV{HOME}/lib/qdcp-ffmpeg
	/usr/local/lib/qdcp-ffmpeg
    /usr/local/lib
    /usr/lib
  )
endif(NOT DISABLE_SWSCALE)

find_library(FFMPEG_avdevice_LIBRARY avdevice
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/lib
  ${QDCP_BINARY_DIR}/libavdevice
  $ENV{HOME}/lib/qdcp-ffmpeg
  /usr/local/lib/qdcp-ffmpeg
  /usr/local/lib
  /usr/lib
)

find_library(_FFMPEG_z_LIBRARY_ z
  ${QDCP_BINARY_DIR}
  ${QDCP_BINARY_DIR}/qdcp-ffmpeg
  ${QDCP_BINARY_DIR}/lib
  $ENV{HOME}/lib/qdcp-ffmpeg
  /usr/local/lib/qdcp-ffmpeg
  /usr/local/lib
  /usr/lib
)



if(FFMPEG_INCLUDE_DIR)
  if(FFMPEG_avformat_LIBRARY)
    if(FFMPEG_avcodec_LIBRARY)
      if(FFMPEG_avutil_LIBRARY)
        set(FFMPEG_FOUND "YES")
        set(FFMPEG_LIBRARIES ${FFMPEG_avformat_LIBRARY}
                             ${FFMPEG_avcodec_LIBRARY}
                             ${FFMPEG_avutil_LIBRARY}
          )
        if(FFMPEG_swscale_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_swscale_LIBRARY}
          )
        endif()
        if(FFMPEG_avdevice_LIBRARY)
          set(FFMPEG_LIBRARIES ${FFMPEG_LIBRARIES}
                               ${FFMPEG_avdevice_LIBRARY}
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
