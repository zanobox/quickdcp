/*
 * QuickDCP: The swiss army knife of the modern movie theater operator!
 * Copyright (c) 2015-2016 jean.sabatier@zanobo.net, All Rights Reserved
 * 
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
   @file    qdcp_video.h
   @author  jean.sabatier@zanobox.net
   @brief   QuickDCP's video functions set. Header file.
*/

#ifndef QDCP_VIDEO_H_INCLUDED
#define QDCP_VIDEO_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <libavutil/avutil.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include "quickdcp.h"
#include "qdcp_wav.h"


qdcp_bool
get_video_info(
	t_layer * ly, 
	t_wav_track * track, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

qdcp_bool
extract_video_images(t_layer * ly, t_project * pj, quickdcp_ctx * qdcp);

qdcp_bool
extract_video_sound(t_wav_track * track, t_layer * ly, t_project * pj, quickdcp_ctx * qdcp);

qdcp_bool 
extract_video_subtitles(AVStream * stream, t_sub_track * subs, t_project * pj, quickdcp_ctx * qdcp);


int 
import_video_sequence(char * video_file, t_layer * ly, t_project * pj, quickdcp_ctx * qdcp);

void
save_video_frame (AVFrame * pFrame, int width, int height, int iFrame, char * trgt_dir);

/** Convert all video streams to mxf with ffmpeg, no quickdcp project */
qdcp_bool
convert_video_to_mxf(const char * video_file, quickdcp_ctx * qdcp);






#endif // QDCP_VIDEO_H_INCLUDED
