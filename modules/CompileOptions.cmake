## Activate FFmpeg options
## for libraries found on the system

## x264
## x264 is a free software library and application for encoding 
## video streams into the H.264/MPEG-4 AVC compression format, 
## and is released under the terms of the GNU GPL.
IF(FIND_LIBRARY "libx264")
	SET(FFMPEG_OPT_X264, "--enable-libx264")
ELSE()
	SET(FFMPEG_OPT_X264, "--disable-libx264")
ENDIF()

## Wavpack
## WavPack is a completely open audio compression format providing 
## lossless, high-quality lossy, and a unique hybrid compression mode.
IF(FIND_LIBRARY "libwavpack")
	SET(FFMPEG_OPT_WAVPACK, "--enable-libwavpack")
ELSE()
	SET(FFMPEG_OPT_WAVPACK, "--disable-libwavpack")
ENDIF()

## Theora (theora.org)
## Open video codec that can compete with MPEG-4 format. 
IF(FIND_LIBRARY "libtheora")
	SET(FFMPEG_OPT_THEORA, "--enable-libtheora")
ELSE()
	SET(FFMPEG_OPT_THEORA, "--disable-libtheora")
ENDIF()

IF(FIND_LIBRARY "libvorbis")
	SET(FFMPEG_OPT_VORBIS, "--enable-libvorbis")
ELSE()
	SET(FFMPEG_OPT_VORBIS, "--disable-libvorbis")
ENDIF()

IF(FIND_LIBRARY "libopus")
	SET(FFMPEG_OPT_OPUS, "--enable-libopus")
ELSE()
	SET(FFMPEG_OPT_OPUS, "--disable-libopus")
ENDIF()

IF(FIND_LIBRARY "libmp3lame")
	SET(FFMPEG_OPT_MP3LAME, "--enable-libmp3lame")
ELSE()
	SET(FFMPEG_OPT_MP3LAME, "--disable-libmp3lame")
ENDIF()

IF(FIND_LIBRARY "libbluray")
	SET(FFMPEG_OPT_BLURAY, "--enable-libbluray")
ELSE()
	SET(FFMPEG_OPT_BLURAY, "--disable-libbluray")
ENDIF()

IF(FIND_LIBRARY "libfdk-aac")
	SET(FFMPEG_OPT_FDKAAC, "--enable-libfdk-aac")
ELSE()
	SET(FFMPEG_OPT_FDKAAC, "--disable-libfdk-aac")
ENDIF()

IF(FIND_LIBRARY "libcdio")
	SET(FFMPEG_OPT_CDIO, "--enable-libcdio")
ELSE()
	SET(FFMPEG_OPT_CDIO, "--disable-libcdio")
ENDIF()

IF(FIND_LIBRARY "libass")
	SET(FFMPEG_OPT_ASS, "--enable-libass")
ELSE()
	SET(FFMPEG_OPT_ASS, "--disable-libass")
ENDIF()

IF(FIND_LIBRARY "libxvid")
	SET(FFMPEG_OPT_XVID, "--enable-libxvid")
ELSE()
	SET(FFMPEG_OPT_XVID, "--disable-libxvid")
ENDIF()

IF(FIND_LIBRARY "libzvbi")
	SET(FFMPEG_OPT_ZVBI, "--enable-libzvbi")
ELSE()
	SET(FFMPEG_OPT_ZVBI, "--disable-libzvbi")
ENDIF()

IF(FIND_LIBRARY "libshine")
	SET(FFMPEG_OPT_SHINE, "--enable-libshine")
ELSE()
	SET(FFMPEG_OPT_SHINE, "--disable-libshine")
ENDIF()

