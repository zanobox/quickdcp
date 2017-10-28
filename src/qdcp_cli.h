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
   @file    qdcp_cli.h
   @author  jean.sabatier@zanobox.net
   @brief   quickdcp-cli builds Digital Cinema Packages in command line interface.
*/

#ifndef QDCP_CLI_H_INCLUDED
#define QDCP_CLI_H_INCLUDED

#define CLI_MODE 	QDCP_ON
#define GUI_ENABLED QDCP_NO


#ifdef WIN32
	#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
	#include <time.h> // for nanosleep
#else
	#include <unistd.h> // for usleep
#endif

#include "quickdcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <inttypes.h>
#ifdef IMAGEMAGICK7
	#include <MagickWand/MagickWand.h>
#else
	#include <wand/MagickWand.h>
#endif
#include "qdcp_config.h"
#include "qdcp_projects.h"
#include "qdcp_photograms.h"
#include "qdcp_wav.h"
#include "qdcp_video.h"
#include "qdcp_mxf.h"

/* int main(); */

/** 
 * @brief What we say when we say hello
 */
static 	void	
qdcp_intro_block();

/** 
 * @brief Process a user given image file into a still image DCP project
 * and add it to the main project list.
 */
qdcp_bool
process_image_file(
	char * current_file,
	t_project * pj,
	quickdcp_ctx * qdcp
	);

/** 
 * @brief Process a user given audio file into a wav project, and 
 * optionnaly into an audio only DCP project and add it to
 * the main project list.
 */
qdcp_bool
process_audio_file(
	char * file_path, 
	t_wav_track * track,
	t_project * pj,
	quickdcp_ctx * qdcp
	);
	
/** 
 * @brief Process a user given video file into a DCP project and add it to
 * the main project list.
 */
qdcp_bool
process_video_file(
	char * file_path,
	t_project * pj,
	t_wav_track * track, 
	quickdcp_ctx * qdcp
	);

/** 
 * @brief What we say when the job is done
 */
static	void 	
qdcp_compte_rendu(
	t_project * pj, 
	int preview
	);

/** 
 * @brief Display some helpful explanations
 */
static 	void 	
qdcp_show_help();

/** 
 * @brief Display a short usage synopsis
 */
static 	void 	
qdcp_show_usage();

/** 
 * @brief Get a user's answer from standard in.
 * 
 * @param string A pointer to the string containing the user answer.
 * @param length Maximum length of the expected answer.
 * @return An integer (qdcp_bool) : QDCP_SUCCESS or QDCP_FAILURE.
 */
static	qdcp_bool
qdcp_get_answer(
	char * string, 
	int length
	);

/** 
 * @brief Clean standard in buffer
 */
void 	
qdcp_clean_buffer();

#endif // CLI_H_INCLUDED
