/*
 * QuickDCP: The swiss army knife of movie theater operators.
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
   @file    qdcp_sub.h
   @author  jean.sabatier@zanobox.net
   @brief   Build Digital Cinema Packages in command line interface.
*/

#ifndef QDCP_SUB_H_INCLUDED
#define QDCP_SUB_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include "quickdcp.h"


/** 
 * @brief create a new sub_project with default or empty field values 
 * 
 * @param st A just calloc'ed t_sub_track structure.
 * @param qdcp The quickdcp context in which everything takes place.
 * @return A qdcp_bool (always QDCP_SUCCESS), so currently, could return a void instead.
 */
qdcp_bool
init_sub_track(
	t_sub_track * st, 
	quickdcp_ctx * qdcp
	);

/**
 * get a wav_project from an audio file 
 * audio_file must be an absolute path including the file name
 */
qdcp_bool
import_subtitle_file(t_sub_track * nw, file_obj * file, quickdcp_ctx * qdcp);

qdcp_bool
add_subs_to_list(t_sub_track * st, t_subtitles_list * wpjl);

int
save_sub_track(t_sub_track * st, quickdcp_ctx * qdcp);

t_sub_track * 
load_sub_track(const char * qds_file, t_subtitles_list * wpl, quickdcp_ctx * qdcp);

t_subtitles_list * 	
scan_sub_projects(quickdcp_ctx * qdcp);

int 					
add_subs_to_project(t_sub_track * st, t_project * pj, quickdcp_ctx * qdcp);

#endif // QDCP_SUB_H_INCLUDED
