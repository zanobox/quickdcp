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
   @file    qdcp_projects.h
   @author  jean.sabatier@zanobox.net
   @brief   Project objects related functions.
*/

#ifndef QDCP_PROJECTS_H_INCLUDED
#define QDCP_PROJECTS_H_INCLUDED

#include "quickdcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <math.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#ifdef IMAGEMAGICK7
	#include <MagickWand/MagickWand.h>
#else
	#include <wand/MagickWand.h>
#endif
#include "qdcp_config.h"
#include "qdcp_wav.h"
#include "qdcp_video.h"


/*
 * Functions on projects
 */
 
/** 
 * @brief	Allocate a new empty project and give it some known default values.
 * 
 * @param	index	An int
 * @param	qdcp	A quickdcp context containing default configuration among other things.
 * @param	pjl		A project list, since a project should never be an orphan.
 * @return	A pointer to the new t_project object.
 */
t_project * 
new_project(
	int index, 
	quickdcp_ctx * qdcp, 
	t_project_list * pjl
	);

/** 
 * @brief Save a project on hard disk as an xml file, with a .qdx extension.
 * 
 * @param	pj		A pointer to the project object to be saved.
 * @param	qdcp 	A pointer to the quickdcp context in which all this occurs.
 * @return 	A qdcp_bool, namely QDCP_SUCCESS or QDCP_FAILURE.
 */
qdcp_bool				  
save_project(
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/** 
 * @brief Load a project from a .qdx (quickdcp xml) file (needs an absolute path) 
 * and add it to the current project list 
 * 
 * @param	qdp_path A pointer to the string of the full (absolute) path of the .qdx file to process.
 * @param	pjl		 A pointer to the project list to which append the loaded project.
 * @param	qdcp	 The quickdcp context in which all this occurs.
 * @return	A pointer to the loaded t_project object
 */
t_project * 
load_project(
	char * qdp_path, 
	t_project_list * pjl, 
	quickdcp_ctx * qdcp
	);

/**
 * @brief
 * @return
 */
qdcp_bool
rename_project(
	char * name, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

qdcp_bool 
find_project_by_name(
	t_project * pj, 
	const char * bname, 
	quickdcp_ctx * qdcp
	); 


/**
 * @brief
 * @return
 */
void 			  
show_project_params(
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/**
 * @brief
 * @return
 */
int 
merge_projects(
	t_project * pj_dest, 
	t_project * pj_src
	);

/**
 * @brief
 * @return
 */
int 
concat_projects(
	t_project * pj_dest, 
	t_project * pj_src
	);

/**
 * @brief
 * @return
 */
int
free_project(
	t_project * pj
	);

/*
 * Functions on layers
 */

/** 
 * @brief Allocate a new empty layer to an existing project. 
 */
t_layer	* 
init_layer(
	t_project * pj
	);


/** 
 * @brief Fill an empty layer with data from an image file.
 */
int 
create_new_image_layer(
	t_layer * nl, 
	char * file_path, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);


/** 
 * @brief Fill an empty layer with data from a video file. 
 */
int
create_new_video_layer(
	t_layer * nl, 
	char * file_path, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/** 
 * @brief Fill an empty layer with data from an mxf (jpeg2000 of course) file.
 */
int
create_new_mxf_layer(
	t_layer * nl, 
	file_obj * file_object, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/** 
 * @brief Attach an orphan layer to an existing project.
 */
int 
add_layer_to_project(
	t_layer * layer, 
	t_project * pj
	);


/** 
 * @brief In CLI or debug mode, display a layer's parameters on standard out.
 */
void 
show_layer_params(
	t_layer * ly, 
	int verbosity
	);


/** 
 * @brief One day, all layers will be free 
 */
int 
free_layer(
	t_layer * ly
	);


/*
 * Functions on keyframes
 * 	dupli_keyframe() copies the values of a keyframe and insert a new keyframe at photogram point.
 *  copy_frame() only copies all the values of a frame or keyframe in a non-key-frame for internal computings.
 */
/* keyframes */
/**
 * @brief
 * @return
 */
t_keyframe *
init_keyframe(
	int photogram, 
	t_layer * ly, 
	t_project * pj
	);

/**
 * @brief
 * @return
 */
t_keyframe *
dupli_keyframe(
	int photogram, 
	t_keyframe * kf1, 
	t_layer * ly, 
	t_project * pj
	);

/**
 * @brief
 * @return
 */
t_keyframe *
copy_frame(
	int photogram, 
	t_keyframe * kf1
	);

/**
 * @brief
 * @return
 */
int
insert_new_keyframe(
	int photogram, 
	t_layer *nl, 
	t_project * pj
	);

/**
 * @brief
 * @return
 */
void
show_keyframe_params(
	t_keyframe * kf, 
	int verbosity
	);

/**
 * @brief
 * @return
 */
int
different_keyframes(
	t_keyframe * kf1, 
	t_keyframe * kf2
	);

/**
 * @brief
 * @return
 */
int
free_keyframe(
	t_keyframe * kf
	);

/**
 * @brief
 * @return
 */
int
compute_frame_values(
	t_keyframe * frame_values, 
	t_keyframe * prev_kf, 
	t_keyframe * next_kf
	);

/**
 * @brief
 * @return
 */
int
compute_frame_int_value(
	int prev_val, 
	int next_val, 
	t_keyframe * frame, 
	t_keyframe * prev_kf, 
	t_keyframe * next_kf
	);

/**
 * @brief
 * @return
 */
float
compute_frame_float_value(
	float prev_val, 
	float next_val, 
	t_keyframe * frame, 
	t_keyframe * prev_kf, 
	t_keyframe * next_kf
	);

/**
 * @brief
 * @return
 */
char *
compute_frame_color_value(
	char * prev_val, 
	char * next_val, 
	t_keyframe * frame, 
	t_keyframe * prev_kf, 
	t_keyframe * next_kf
	);

/**
 * @brief
 * @return
 */
t_keyframe *
get_frame_values(
	int n, 
	t_layer * ly
	);

/*
 * Data manipulation
 */
/** 
 * @brief Get a resolution width and height from a named format.
 * 
 * @param 	dims	An int array to populate with acquired dimensions.
 * @param	format	A string describing a standard digital cinema format (Flat, Scope, etc.).
 * @return  dims	The same int array given as argument.
 */
int *
get_dims_from_format(
	int * dims, 
	char * format
	); 
	
/**
 * This is supposed to convert xml text into UTF-8.
 * But it caused more problems than has solved.
 */
xmlChar *
ConvertInput(
	const char *in, 
	const char *encoding
	);
	
/**
 * @brief Convert a rotation value from degrees to radians.
 * @param	degrees	A float number of degrees.
 * @return A float number of radians.
 */ 
float
deg2rad(
	float degrees
	);


/* 
 * qdp stuff 
 */
/** 
 * @brief Archive a project as a .qdp file with all its related files.
 * 
 * A .qdp file is a zipped archive with a qdp extension, containing
 * a .qdx file and all resources (images, videos, audio files, fonts, etc.) 
 * needed to generate the corresponding DCP.
 * It can also contain half-way files from previous processings of the project.
 * 
 * @param	pj		A pointer to the project object to export.
 * @param	qdcp	A pointer to the quickdcp context in which it all happens.
 * @return	A string pointer of the newly created .qdp archive.
 */
char * 
qdp_export(
	t_project * pj, 
	quickdcp_ctx * qdcp
	);
	
/** 
 * @brief Extract a .qdp archive into proper local directories. 
 * 
 * A .qdp file is a zipped archive with a qdp extension, containing
 * a .qdx file and all resources (images, videos, audio files, fonts, etc.) 
 * needed to generate the corresponding DCP.
 * 
 * @param	file_path	A pointer to the full (absolute) path string of a .qdp file.
 * @return	A pointer to the newly imported project object.
 */
t_project *
qdp_import(
	const char * file_path
	);

#endif // QDCP_PROJECTS_H_INCLUDED
