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
 * @file    qdcp_mxf.h
 * @author  jean.sabatier@zanobox.net
 * @brief   Build Digital Cinema Packages in command line interface.
 */

#ifndef QDCP_MXF_H_INCLUDED
#define QDCP_MXF_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "quickdcp.h"
#include "qdcp_projects.h"
#include "qdcp_sub.h"


/*
 * MXF packing functions 
 */

/**
 * @brief
 * @return
 */
qdcp_bool
write_j2k_mxf_reel(
	t_project 	* 	pj, 
	quickdcp_ctx * 	qdcp
	);

/**
 * @brief
 * @return
 */
qdcp_bool
write_wav_mxf_reel(
	t_project 	* 	pj, 
	quickdcp_ctx * 	qdcp
	);

/**
 * @brief
 * @return
 */
qdcp_bool
write_subs_mxf_reel(
	t_project 	* 	pj, 
	quickdcp_ctx * 	qdcp
	);

/*
 * MXF extracting functions 
 */

/**
 * @todo
 * @brief Generate a project object from an existing DCP.
 * 
 * @param  path_to_dcp A pointer to the full (absolute) path string of the DCP to analyse.
 * @param	qdcp A pointer to the quickdcp context where everything happens
 * 
 * @return t_project *  A pointer to the newly deduced project
 */
t_project *
dcp_analyze(
	char * path_to_dcp, 
	quickdcp_ctx * qdcp
	);

/**
 * @brief
 * @see	extract_wav_mxf_reel
 * @see extract_subs_mxf_reel
 * 
 * @param	mxf_file A pointer to the string path of the mxf file to unpack.
 * @param  	pj   A pointer to the t_project structure receiving the result of extraction
 * @param	qdcp A pointer to the quickdcp context where everything happens
 * 
 * @return A pointer to a new graphic layer in the project
 */
t_layer * 
extract_j2k_mxf_reel(
	const char 	* 	mxf_file,
	t_project 	* 	pj, 
	quickdcp_ctx * qdcp
	);

/**
 * @brief
 * @see	extract_j2k_mxf_reel
 * @see extract_subs_mxf_reel
 * 
 * @param	mxf_file A pointer to the string path of the mxf file to unpack.
 * @param	qdcp A pointer to the quickdcp context where everything happens
 * 
 * @return
 */
t_wav_track * 
extract_wav_mxf_reel(
	const char 	* 	mxf_file,
	quickdcp_ctx * 	qdcp
	);

/**
 * @brief
 * @see	extract_j2k_mxf_reel
 * @see extract_wav_mxf_reel
 * 
 * @param	mxf_file A pointer to the string path of the mxf file to unpack.
 * @param	qdcp A pointer to the quickdcp context where everything happens
 * 
 * @return
 */
t_sub_track * 
extract_subs_mxf_reel(
	const char * mxf_file, 
	quickdcp_ctx * qdcp
	);

#endif // QDCP_MXF_H_INCLUDED
