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
 * @file    qdcp_projects.c
 * @author  jean.sabatier@zanobox.net
 * @brief   Build Digital Cinema Packages in command line interface.
 */

#include "qdcp_projects.h"

/*
 *  projects functions 
 */

qdcp_bool
init_project_list(quickdcp_ctx * qdcp) 
{
		t_project_list * nls;
		
	nls = calloc(1, sizeof(*nls));
	
	if (nls == NULL)
	{
		warning(T_PROJECT_LIST_FAILED);
		return QDCP_FAILURE;
	}
	
	nls->size = 0;
	nls->first = NULL;
	nls->last = NULL;
	
	qdcp->proj_list = nls;
	
	return QDCP_SUCCESS;
}

t_project * 
new_project(int index, quickdcp_ctx * qdcp, t_project_list * pjl) 
{
	
	/* all mallocs first */
	
			t_project 	 	*	np 	= calloc(1, sizeof(*np));
			t_layer_list 	*	nll = calloc(1, sizeof(*nll));
			t_track_list	*	nwl = calloc(1, sizeof(*nwl));
			t_filenames	 	*	jfl = calloc(1, sizeof(*jfl));
			t_filenames 	*	wfl = calloc(1, sizeof(*wfl));
			t_filenames 	*	sfl = calloc(1, sizeof(*sfl));
	
	/* new_project, layers list, wav_tracks list, j2k_list, wav_files list, subtitles list */
	if (np == NULL || nll == NULL || nwl == NULL || jfl == NULL || wfl == NULL || sfl == NULL)
	{
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	}
	
	/* then other (local) variables */
	unsigned	char 			id_string[NAME_STR_LEN];
				float 			laps;
				int				framerate;
				char  		*	format;			
				int	  			dims[2];
				time_t			time_now;
				char 			name[NAME_STR_LEN];

	/* -- structure values -- */
	
	/* get an id for the new project from index, the letter p and time in seconds since 1970*/
	time(&time_now);
	sprintf(id_string, "%dp%d", index, (int)time_now);
	sprintf(name, "%s", DEFAULT_PROJECT_NAME);
	
	laps 					= qdcp->cfg->laps;
	framerate 				= qdcp->cfg->framerate;
	format 					= qdcp->cfg->format;
	get_dims_from_format(dims, format);	
	
	np->p_id 				= hash(id_string);		// Hash index string 
	np->prev 				= NULL;
	np->next 				= NULL;
	np->creation 			= time_now;
	np->last_mod			= time_now;

	/* -- project values -- */
	
	sprintf(np->name, "%s", name);
    np->status 				= PJ_INIT;
    //np->dcp_name
	
    sprintf(np->issuer, "%s", qdcp->cfg->issuer);
    sprintf(np->issuer_short, "%s", qdcp->cfg->issuer_short);
	np->category 			= qdcp->cfg->category;
    
    np->format 				= format;
    np->resolution 			= qdcp->cfg->resolution;
    np->laps 				= laps;
    np->framerate 			= framerate;
    np->nmax 				= ceil(laps * framerate);	/** < better ceil(laps * framerate) ? */
    np->mx_w 				= dims[0];
    np->mx_h 				= dims[1];
    np->zoom_factor 		= 1.0;
    np->current_frame 		= 1;
    np->lang_code 			= "FR";
    np->subtitle_code 		= "FR";
    np->country_code 		= "FR";
    np->stereoscopic		= 0;

	np->background 			= qdcp->cfg->background;

    np->fondu_d 			= qdcp->cfg->fondu_d;
    np->fondulaps_d 		= qdcp->cfg->fondulaps_d;
    np->fondu_cache_color_d = qdcp->cfg->fondu_cache_color_d;
    np->fondu_iris_cx_d 	= qdcp->cfg->fondu_iris_cx_d;
    np->fondu_iris_cy_d 	= qdcp->cfg->fondu_iris_cy_d;

    np->fondu_f 			= qdcp->cfg->fondu_f;
    np->fondulaps_f 		= qdcp->cfg->fondulaps_f;
    np->fondu_cache_color_f = qdcp->cfg->fondu_cache_color_f;
    np->fondu_iris_cx_f 	= qdcp->cfg->fondu_iris_cx_f;
    np->fondu_iris_cy_f 	= qdcp->cfg->fondu_iris_cy_f;
	
	/* layers list */
	np->layers 				= nll;
	np->active_layer 		= NULL;
	
	/* jpeg2000 files list */
	np->j2k_files 			= jfl;
	
	/* -- audio values -- */
	
	/* wav tracks list */
	np->tracks				= nwl;
	np->mix_type			= qdcp->cfg->mix_type;
	np->audio_format		= qdcp->cfg->audio_format;	
	np->samplerate			= qdcp->cfg->samplerate;		/** < samples per second (48000 or 96000/s) */
	np->encoding			= qdcp->cfg->encoding;
	np->volume				= qdcp->cfg->volume;
	//~ np->nchannels			= qdcp->cfg->nchannels;		/** < number of channels (should be inherited from project) */
	//~ np->samples				= qdcp->cfg->samples;
	//~ np->precision			= qdcp->cfg->precision;		/** < precision, bit/sample, bitdepth = 24 */


	/* subtitles files list */
	np->subs_files 	= sfl;

    np->mxf.write_hmac = 1;
	np->ns = XML_NS_SMPTE;
	np->xml_signature.sign 			= qdcp->signature;
	np->xml_signature.use_external 	= 0;
	np->xml_signature.root 			= NULL;
	np->xml_signature.ca 			= NULL;
	np->xml_signature.signer 		= NULL;
	np->xml_signature.private_key 	= NULL;
	
    sprintf(np->dcp.creator, "%.80s %.80s", QDCP_NAME, QDCP_VERSION);
    sprintf(np->dcp.annotation, "%.128s", DCP_ANNOTATION);
    sprintf(np->dcp.title, "%.80s", DCP_TITLE);
    sprintf(np->dcp.kind, "%.15s", DCP_KIND);
    //~ get_timestamp(np->dcp.issue_date);					/**< This timestamp is rewritten when finalising DCP */
    np->dcp_files					= new_file_list();
    
    /* initialize callbacks */
    np->mxf.frame_done.callback  	= qdcp_callback_null;
    np->mxf.frame_done.argument  	= NULL;
    np->mxf.file_done.callback   	= qdcp_callback_null;
    np->mxf.file_done.argument   	= NULL;
    
    np->dcp.sha1_update.callback 	= qdcp_callback_null;
    np->dcp.sha1_update.argument 	= NULL;
    np->dcp.sha1_done.callback 		= qdcp_callback_null;
    np->dcp.sha1_done.argument 		= NULL;

	/* insert project at the end of projects list */
	if (pjl != NULL)
	{
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			printf("%s%sAttaching the project to the project list !%s\n",
					CLI_INDENT, CLI_GREEN, CLI_COLOR);
		
		if (pjl->last != NULL)
		{
			pjl->last->next = np;
			np->prev = pjl->last;
		}
		pjl->last = np;
		
		if (pjl->first == NULL)
		{
			pjl->first = np;
		}
		pjl->size++;
	}
	
	printf("%s%sCreated a new project based on defaults %s\n",
			CLI_INDENT, CLI_GREEN, CLI_COLOR);

	return np;
}

qdcp_bool
rename_project(char * basename, t_project * pj, quickdcp_ctx * qdcp) 
{
	
	/* */
	get_dcp_name_from_project(pj, qdcp);

	sprintf(pj->name, "%s", basename);

	/* -- project's directories -- */

	/* preview sequence (png) directory */
	sprintf(pj->img_path, "%s%s%s", qdcp->img_dir, pj->name, SEPARATOR);
	/* preview sequence (png) directory */
	sprintf(pj->seq_path, "%s%s%s", qdcp->seq_dir, pj->name, SEPARATOR);
	//~ np->wav_path		
	sprintf(pj->wav_path, "%s%s%s", qdcp->wav_dir, pj->name, SEPARATOR);
	//~ np->sub_path		
	sprintf(pj->sub_path, "%s%s%s", qdcp->sub_dir, pj->name, SEPARATOR);
	/* rendered sequence (j2k) directory */
	sprintf(pj->j2k_path, "%s%s%s", qdcp->j2k_dir, pj->name, SEPARATOR);
	/* temporary dcp (fresh mxf and xml) directory */
	sprintf(pj->tmp_dcp_dir, "%s%s%s", qdcp->mxf_dir, pj->name, SEPARATOR);
	/* final dcp directory (inside qdcp->dcp_out) */
	sprintf(pj->final_dcp_dir, "%s%s%s", qdcp->dcp_out, pj->name, SEPARATOR);
									
	return QDCP_SUCCESS;
}

qdcp_bool 
save_project(t_project * pj, quickdcp_ctx * qdcp) 
{

	LIBXML_TEST_VERSION
	
			int 				rc;
			xmlTextWriterPtr 	writer;
			//~ xmlChar 		*	intro-utf8;
			char			*	intro;
			char 				qdp_file[PATH_STR_LEN] = {0};
	struct	timeval				tv;

	gettimeofday(&tv, NULL);
	
	/* path and name of the qdx file */
	check_dir(qdcp->proj_dir, qdcp->verbosity);
	sprintf(qdp_file, "%s%s.qdx", qdcp->proj_dir, pj->name);
	
	/* begin writing XML */
	printf("%s%sSauvegarde du projet %s%s.qdx%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_CYAN, pj->name, CLI_COLOR); 
	printf("%s%s%s %s%s%s\n", 
			CLI_INDENT, CLI_INDENT, CLI_GREEN, CLI_CYAN, qdp_file, CLI_COLOR); 
	
	/* create a writer */
	writer = xmlNewTextWriterFilename(qdp_file, 0);
	if (writer == NULL)
	{ printf("save_project : error creating the writer %s\n", qdp_file);
		return 1;}
	/* we want indented output */
	rc = xmlTextWriterSetIndent(writer, 1);
	
	/* start a document */
	rc = xmlTextWriterStartDocument(writer, NULL, XML_ENCODING, NULL);
	if (rc < 0){ printf("save_project : error at xmlTextWriterStartElement\n"); return 1;}
	
	/* write a comment of general information */
	//~ intro-utf8 = ConvertInput("This .qdx (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP application.\t",
					//~ XML_ENCODING);
	intro = "This .qdx (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP software.\t";
	rc = xmlTextWriterWriteFormatComment(writer, "%s", intro);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	//~ if (intro != NULL) { xmlFree(intro-utf8); }

	/* start root element */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "QDP_PROJECT");
	if (rc < 0){ printf("save_project : error at xmlTextWriterStartElement\n"); return 1;}
		
	/* PROJECT'S DEFINITION */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ID");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* p_id */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "p_id", "%d", pj->p_id);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* status */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "status", "%d", pj->status);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* name */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "name", (const xmlChar*)pj->name);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* creation time */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "creation", "%d", (int)pj->creation);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* last_mod */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "last_mod", "%d", (int)pj->last_mod);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* issuer */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "issuer", (const xmlChar*)pj->issuer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* issuer_short */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "issuer_short", (const xmlChar*)pj->issuer_short);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* endElement ID */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	
	/* PROJECT'S FEATURES */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "FEATURES");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* format */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "format", pj->format);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* resolution */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "resolution", pj->resolution);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* framerate */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "framerate", "%d", pj->framerate);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* category */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "category", qdcp->short_cats[pj->category]);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* laps (float) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "laps", "%.2f", pj->laps);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* background */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "background", pj->background);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* fondu_d */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "fondu_d", qdcp->fondus[pj->fondu_d]);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondulaps_d (float) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondulaps_d", "%.1f", pj->fondulaps_d);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fonducachecolor_d */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "fondu_cache_color_d", pj->fondu_cache_color_d);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondu_iris_cx_d (float) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondu_iris_cx_d", "%.1f", pj->fondu_iris_cx_d);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondu_iris_cy_d */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondu_iris_cy_d", "%.1f", pj->fondu_iris_cy_d);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* fondu_f */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "fondu_f", qdcp->fondus[pj->fondu_f]);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondulaps_f (float) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondulaps_f", "%.1f", pj->fondulaps_f);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fonducachecolor_f */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "fondu_cache_color_f", pj->fondu_cache_color_f);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondu_iris_cx_f */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondu_iris_cx_f", "%.1f", pj->fondu_iris_cx_f);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* fondu_iris_cy_f */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fondu_iris_cy_f", "%.1f", pj->fondu_iris_cy_f);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* lang_code */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "lang_code", pj->lang_code);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* subtitle_code */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "subtitle_code", pj->subtitle_code);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* country_code */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "country_code", pj->country_code);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* endElement FEATURES */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	
	/* STATUS IN GUI */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "GUI_STATUS");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* zoom_factor */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "zoom_factor", "%.2f", pj->zoom_factor);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* current_frame */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "current_frame", "%d", pj->current_frame);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* active_layer */
	if(pj->active_layer != NULL)
	{	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "active_layer", "%d", pj->active_layer->l_id);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	}
	/* endElement GUI_STATUS */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* LAYERS */

	rc = xmlTextWriterStartElement(writer, BAD_CAST "LAYERS");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* number of layers */
	rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "num_lyrs", "%d", pj->layers->size);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
	/* loop through layers */
	t_layer * cur_ly;
	cur_ly = pj->layers->first;
	while (cur_ly != NULL)
	{
		rc = xmlTextWriterStartElement(writer, BAD_CAST "layer");
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* layer type */
		rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "type", "%d", cur_ly->type);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* name of this layer */
		rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "name", cur_ly->layername);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* position in pile */
		rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "pos", "%d", cur_ly->pos);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* visibility */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "visible", "%d", cur_ly->visible);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* keep proportions */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "keep_ratio", "%d", cur_ly->keep_ratio);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* entry point */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "entry_point", "%d", cur_ly->entry_point);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* exit point */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "exit_point", "%d", cur_ly->exit_point);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* background color */
		rc = xmlTextWriterWriteElement(writer, BAD_CAST "bg_color", cur_ly->bg_color);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* fade in */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fade_in", "%d", cur_ly->fade_in);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* fade out */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "fade_out", "%d", cur_ly->fade_out);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* background color */
		rc = xmlTextWriterWriteElement(writer, BAD_CAST "bg_color", cur_ly->bg_color);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* foreground color */
		rc = xmlTextWriterWriteElement(writer, BAD_CAST "fg_color", cur_ly->fg_color);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		/* default margin */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "margin", "%d", cur_ly->margin);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

		if (cur_ly->type == LAYER_IMAGE || cur_ly->type == LAYER_VIDEO || cur_ly->type == LAYER_MXF)
		{
			/* file name */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "filename", cur_ly->filename);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* basename */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "basename", cur_ly->basename);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* mime */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "mime", cur_ly->ext);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* source file */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "source_file", cur_ly->source_file);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* file stats will have to be retrieved on project opening */
		}
		else if (cur_ly->type == LAYER_TEXT)
		{
			/* text */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text", cur_ly->text);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* font_desc */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "font_desc", cur_ly->font_desc);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* interline */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "interline", "%d", cur_ly->interline);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_align */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_align", cur_ly->text_align);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_stretch */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_stretch", cur_ly->text_stretch);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_style */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_style", cur_ly->text_style);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_variant */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_variant", cur_ly->text_variant);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_weight */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_weight", cur_ly->text_weight);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* text_lang */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "text_lang", cur_ly->text_lang);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		}
		else if (cur_ly->type == LAYER_VECTORIAL)
		{
			/* shape */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "shape", qdcp->shapes[cur_ly->shape]);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* line_width */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "line_width", "%d", cur_ly->line_width);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* line_style */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "line_style", cur_ly->line_style);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* line_join */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "line_join", cur_ly->line_join);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* line_caps */
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "line_caps", cur_ly->line_caps);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* background_on */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "background_on", "%d", cur_ly->background_on);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* border_on */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "border_on", "%d", cur_ly->border_on);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		}
		if (cur_ly->type == LAYER_VIDEO || cur_ly->type == LAYER_MXF)
		{
			/* video_stream */
			/* cur_ly->video_stream is not an int but an AVStream * pointer ! */
			//~ rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "video_stream", "%f", cur_ly->video_stream);
			//~ if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* src_w */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_w", "%d", cur_ly->src_w);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* src_h */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_h", "%d", cur_ly->src_h);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			/* src_ratio */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_ratio", "%f", cur_ly->src_ratio);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
		}
		
		/* KEYFRAMES */

		rc = xmlTextWriterStartElement(writer, BAD_CAST "KEYFRAMES");
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

		/* loop through keyframes */
		t_keyframe * cur_kf;
		cur_kf = cur_ly->keyframes->first;
		while (cur_kf != NULL)
		{
			rc = xmlTextWriterStartElement(writer, BAD_CAST "keyframe");
			/* previous and next keyframes are pointers we won't record that in a file */
			/* photogram */
			rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "photogram", "%d", cur_kf->photogram);
			//~ int 		x;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "x", "%d", cur_kf->x);
			//~ int 		y;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "y", "%d", cur_kf->y);
			//~ int 		w;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "w", "%d", cur_kf->w);
			//~ int 		h;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "h", "%d", cur_kf->h);
			//~ int 		ox;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ox", "%d", cur_kf->ox);
			//~ int 		oy;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "oy", "%d", cur_kf->oy);
			//~ int 		centerX;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "centerX", "%d", cur_kf->centerX);
			//~ int 		centerY;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "centerY", "%d", cur_kf->centerY);
			//~ float 		oxp;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "oxp", "%.2f", cur_kf->oxp);
			//~ float 		oyp;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "oyp", "%.2f", cur_kf->oyp);
			//~ float 		wp;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "wp", "%.2f", cur_kf->wp);
			//~ float 		hp;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "hp", "%.2f", cur_kf->hp);
			//~ float 		ratio; 
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "ratio", "%.2f", cur_kf->ratio);
			//~ float 		rotrad;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "radians", "%.2f", cur_kf->rotrad);
			//~ float 		rotdeg;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "degrees", "%.2f", cur_kf->rotdeg);
			//~ int 		opacity;
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "opacity", "%d", cur_kf->opacity);
			//~ string 		bg_color;
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "bg_color", cur_kf->bg_color);
			//~ string 		fg_color;
			rc = xmlTextWriterWriteElement(writer, BAD_CAST "fg_color", cur_kf->fg_color);
						
			/* endElement keyframe */
			rc = xmlTextWriterEndElement(writer);
			
			cur_kf = cur_kf->next;
		}
		
		/* endElement KEYFRAMES */
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
	
		/* endElement layer */
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }

		cur_ly = cur_ly->next;
	}
	/* endElement LAYERS */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }

	/* AUDIO SECTION */
	
	if (pj->tracks->size > 0)
	{
		rc = xmlTextWriterStartElement(writer, BAD_CAST "AUDIO");
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
		/* project wide audio values */

		/* mix_type */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mix_type", "%d", pj->mix_type);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* audio_format */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "audio_format", "%d", pj->audio_format);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* number of channels */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nchannels", "%d", pj->nchannels);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* bitdepth (bits per sample) */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "bitdepth", "%d", pj->bitdepth);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* samplerate */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "samplerate", "%f", pj->samplerate);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* samples */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "samples", "%d", pj->samples);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		/* volume */
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "volume", "%f", pj->volume);
		if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		
		/* loop through wav tracks */
		t_wav_track * cur_tk;
		cur_tk = pj->tracks->first;
		while (cur_tk != NULL)
		{
			rc = xmlTextWriterStartElement(writer, BAD_CAST "wav_track");
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}
			
			/* w_id (int) */
			rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "w_id", "%d", 	cur_tk->w_id);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

			/* creation (int) */
			rc = xmlTextWriterWriteFormatAttribute(writer, BAD_CAST "creation", "%d", cur_tk->creation);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

			/* status (int) */
			rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "status", "%d", cur_tk->status);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

			if (save_wav_track(writer, cur_tk, qdcp) == QDCP_FAILURE)
			{
				/* clean the room before leaving */
				xmlFreeTextWriter(writer);
				xmlCleanupParser();
				xmlMemoryDump();
				return QDCP_FAILURE;
			}
					
			/* endElement wav_track */
			rc = xmlTextWriterEndElement(writer);
			if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
		
			cur_tk = cur_tk->next;
		}	
		/* endElement AUDIO */
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
	}
		
	/* VIDEO SECTION */

	rc = xmlTextWriterStartElement(writer, BAD_CAST "VIDEO");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }

	/* source file */
	/* input framerate */
	/* output framerate */
	
	/* endElement VIDEO */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
	
	/* SUBTITLE SECTION */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "SUBTITLES");
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }
	/* loop through files */
	
	/* endElement SuBTITLES */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_project : Error in %s at line %d\n", __FILE__, __LINE__); }


	/* close the document */
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0)
	{
		printf("save_project : error at xmlTextWriterEndDocument\n");
		return QDCP_FAILURE;
	}
	
	/* clean the room before leaving */
	xmlFreeTextWriter(writer);
	xmlCleanupParser();
	xmlMemoryDump();
	
	/* we must set the file's content type to 'application/qdx'
	 * instead of 'application/xml' !!
	 */
	
	return QDCP_SUCCESS;
}

t_project * 
load_project(char * qdp_path, t_project_list * pjl, quickdcp_ctx * qdcp) 
{
	
#ifndef WIN32
	/* we want an absolute path */
	if (qdp_path[0] != '/' && qdp_path[0] != '~')
	{
		warning(INVALID_RELATIVE_PATH, __FILE__, __LINE__);
		return NULL;
	}
#endif

	printf("Quickdcp : - loading project %s\n", qdp_path);
	
		t_project 		* 	np = calloc(1, sizeof(*np));
		t_layer_list 	* 	ll = calloc(1, sizeof(*ll));
		if (np == NULL || ll == NULL){ 
			warning(BAD_CALLOC);
			return NULL;
		}
		
		t_kf_list		*	kfl;
		t_keyframe		*	cur_kf;
		int					layer_count = 0;
		int					kf_count = 0;
		int					track_count = 0;
		int					i = 0;
		
		xmlDoc			*	qdp;
		xmlNode			*	root, *first_level, *second_level, *third_level, *fourth_level, *fifth_level;
		xmlNode			*	node1, *node2, *node3, *node4, *node5;
		xmlNode			*	attr, *elem;
		
	np->prev = NULL;
	np->next = NULL;
	
	np->layers = ll;
	np->layers->size = 0;
	np->layers->first = NULL;
	np->layers->last = NULL;
	
	qdp = xmlReadFile(qdp_path, NULL, 0);
	if (qdp == NULL)
	{
		warning(CANT_READ_XML);
		return NULL;
	}
	
	root = xmlDocGetRootElement(qdp);
	first_level = root->children;

	/*-------------------- XML parsing begins here -------------------*/
	
	for (node1 = first_level; node1; node1 = node1->next)
	{
		if (strcmp(node1->name, "text") != 0)
		{
			if (qdcp->verbosity >= VERBOSE_VERBOSE)
				printf("%s\n", node1->name);
			second_level = node1->children;
			for (node2 = second_level; node2; node2 = node2->next)
			{
				/* LAYERS */
				if (strcmp(node2->name, "layer") == 0)
				{
					/* print "layer" */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("    %s : \n", node2->name);
					
					t_layer	*	cur_ly = init_layer(np);
											
					/* layer's attributes */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        type = %s\n", xmlGetProp(node2, "type"));
					cur_ly->type = strtoimax(xmlGetProp(node2, "type"), NULL, 10);
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        name = %s\n", xmlGetProp(node2, "layername"));
					sprintf(cur_ly->layername, "%s", xmlGetProp(node2, "layername"));
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        pos  = %s\n", xmlGetProp(node2, "pos"));
					cur_ly->pos  = strtoimax(xmlGetProp(node2, "pos"), NULL, 10);
					
					/* layer's children */
					third_level = node2->children;
					//~ cur_ly->keyframes->first = cur_kf;
					for (node3 = third_level; node3; node3 = node3->next)
					{
						if (strcmp(node3->name, "text") != 0)
						{								
							/* KEYFRAMES */
							if (strcmp(node3->name, "KEYFRAMES") == 0)
							{
								fourth_level = node3->children;
								for (node4 = fourth_level; node4; node4 = node4->next)
								{
									if (strcmp(node4->name, "keyframe") == 0)
									{
										/* print "keyframe" */
										if (qdcp->verbosity >= VERBOSE_VERBOSE)
											printf("        %s : \n", node4->name);
										
										cur_kf = init_keyframe(strtoimax(xmlGetProp(node4, "photogram"), NULL, 10), cur_ly, np);
										
										/* get keyframe prop photogram */
										if (qdcp->verbosity >= VERBOSE_VERBOSE)
											printf("            photogram = %s\n", xmlGetProp(node4, "photogram"));
										
										/* get keyframe's children */
										fifth_level = node4->children;
										for (node5 = fifth_level; node5; node5 = node5->next)
										{
											if (strcmp(node5->name, "text") != 0)
											{
												if (qdcp->verbosity >= VERBOSE_VERBOSE)
													printf("            %s :  ", node5->name);
												if (qdcp->verbosity >= VERBOSE_VERBOSE)
													printf("%s\n", xmlNodeListGetString(qdp, node5->children, 1));
												
												if (strcmp(node5->name, "ox") == 0)
													cur_kf->ox = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "oy") == 0)
													cur_kf->oy = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "x") == 0)
													cur_kf->x = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "y") == 0)
													cur_kf->y = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "w") == 0)
													cur_kf->w = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "h") == 0)
													cur_kf->h = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "centerX") == 0)
													cur_kf->centerX = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "centerY") == 0)
													cur_kf->centerY = strtoimax(xmlNodeListGetString(qdp, node5->children, 1), NULL, 10);
												if (strcmp(node5->name, "oxp") == 0)
													cur_kf->oxp = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "oyp") == 0)
													cur_kf->oyp = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "wp") == 0)
													cur_kf->wp = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "hp") == 0)
													cur_kf->hp = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "ratio") == 0)
													cur_kf->ratio = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "opacity") == 0)
													cur_kf->opacity = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "degrees") == 0)
													cur_kf->rotdeg = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
												if (strcmp(node5->name, "radians") == 0)
													cur_kf->rotrad = strtof(xmlNodeListGetString(qdp, node5->children, 1), NULL);
											}
										}
									}
								}
							}		
							else
							/* get layer's values */
							{
								if (qdcp->verbosity >= VERBOSE_VERBOSE)
									printf("        %s :  ", node3->name);
								if (qdcp->verbosity >= VERBOSE_VERBOSE)
									printf("%s\n", xmlNodeListGetString(qdp, node3->children, 1));
								
								/* l_id (int) */
								if (strcmp(node3->name, "l_id") == 0)
									cur_ly->l_id = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* visible (bool) */
								if (strcmp(node3->name, "visible") == 0)
									cur_ly->visible = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* keep_ratio (bool) */
								if (strcmp(node3->name, "keep_ratio") == 0)
									cur_ly->keep_ratio = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* entry_point (int) */
								if (strcmp(node3->name, "entry_point") == 0)
									cur_ly->entry_point = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* exit_point (int) */
								if (strcmp(node3->name, "exit_point") == 0)
									cur_ly->exit_point = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* fade_in (int) */
								if (strcmp(node3->name, "fade_in") == 0)
									cur_ly->fade_in = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* fade_out (int) */
								if (strcmp(node3->name, "fade_out") == 0)
									cur_ly->fade_out = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* bg_color */
								if (strcmp(node3->name, "bg_color") == 0)
									cur_ly->bg_color = xmlNodeListGetString(qdp, node3->children, 1);
								/* fg_color */
								if (strcmp(node3->name, "fg_color") == 0)
									cur_ly->fg_color = xmlNodeListGetString(qdp, node3->children, 1);
								/* margin (int) */
								if (strcmp(node3->name, "margin") == 0)
									cur_ly->margin = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								
								/* image values */
								/* fname */
								if (strcmp(node3->name, "filename") == 0)
									sprintf(cur_ly->filename, "%s", xmlNodeListGetString(qdp, node3->children, 1));
								/* basename */
								if (strcmp(node3->name, "basename") == 0)
									sprintf(cur_ly->basename, "%s", xmlNodeListGetString(qdp, node3->children, 1));
								/* mime */
								if (strcmp(node3->name, "mime") == 0)
									sprintf(cur_ly->ext, "%s", xmlNodeListGetString(qdp, node3->children, 1));

								/*pango values*/
								/* text */
								if (strcmp(node3->name, "text") == 0)
									cur_ly->text = xmlNodeListGetString(qdp, node3->children, 1);
								/* font_desc */
								if (strcmp(node3->name, "font_desc") == 0)
									cur_ly->font_desc = xmlNodeListGetString(qdp, node3->children, 1);
								/* interline (int) */
								if (strcmp(node3->name, "interline") == 0)
									cur_ly->interline = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* text_color */
								if (strcmp(node3->name, "text_color") == 0)
									sprintf(cur_ly->text_color, "%s", xmlNodeListGetString(qdp, node3->children, 1));
								/* text_align */
								if (strcmp(node3->name, "text_align") == 0)
									cur_ly->text_align = xmlNodeListGetString(qdp, node3->children, 1);
								/* text_stretch */
								if (strcmp(node3->name, "text_stretch") == 0)
									cur_ly->text_stretch = xmlNodeListGetString(qdp, node3->children, 1);
								/* text_style */
								if (strcmp(node3->name, "text_style") == 0)
									cur_ly->text_style = xmlNodeListGetString(qdp, node3->children, 1);
								/* text_variant */
								if (strcmp(node3->name, "text_variant") == 0)
									cur_ly->text_variant = xmlNodeListGetString(qdp, node3->children, 1);
								/* text_weight */
								if (strcmp(node3->name, "text_weight") == 0)
									cur_ly->text_weight = xmlNodeListGetString(qdp, node3->children, 1);
								/* text_lang */
								if (strcmp(node3->name, "text_lang") == 0)
									cur_ly->text_lang = xmlNodeListGetString(qdp, node3->children, 1);
			
								/*vectorial values*/
								/* shape */
									int i;
								if (strcmp(node3->name, "shape") == 0)
								{	
									shapes_types s;
									char * shape_string = xmlNodeListGetString(qdp, node3->children, 1);
									
									for (s = SHAPE_UNDEFINED; s <= SHAPE_ELLIPSE; s++)
									{
										if (strcmp(qdcp->shapes[s], shape_string) == 0)
										{
											cur_ly->shape = s;
										}
									}
								}
								/* line_width (int) */
								if (strcmp(node3->name, "line_width") == 0)
									cur_ly->line_width = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* line_style */
								if (strcmp(node3->name, "line_style") == 0)
									cur_ly->line_style = xmlNodeListGetString(qdp, node3->children, 1);
								/* line_join */
								if (strcmp(node3->name, "line_join") == 0)
									cur_ly->line_join = xmlNodeListGetString(qdp, node3->children, 1);
								/* line_caps */
								if (strcmp(node3->name, "line_caps") == 0)
									cur_ly->line_caps = xmlNodeListGetString(qdp, node3->children, 1);
								/* background_on (bool) */
								if (strcmp(node3->name, "background_on") == 0)
									cur_ly->background_on = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* border_on (bool) */
								if (strcmp(node3->name, "border_on") == 0)
									cur_ly->border_on = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
									
								/* video (or mxf) values */
								/* video_stream (int) */
								if (strcmp(node3->name, "stream_index") == 0)
									cur_ly->stream_index = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* src_w (int) */
								if (strcmp(node3->name, "src_w") == 0)
									cur_ly->src_w = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* src_h (int) */
								if (strcmp(node3->name, "src_h") == 0)
									cur_ly->src_h = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
								/* src_ratio (float) */
								if (strcmp(node3->name, "src_ratio") == 0)
									cur_ly->src_ratio = strtof(xmlNodeListGetString(qdp, node3->children, 1), NULL);
								
							}
						}
					}
					/* layer in project's layerlist */
					layer_count++;
					if (layer_count == 1)
						np->layers->first = cur_ly;
					
					if (np->layers->last != NULL)
					{
						np->layers->last->next = cur_ly;
						cur_ly->prev = np->layers->last;
					}
					cur_ly->next = NULL;
					np->layers->last = cur_ly;
					np->layers->size = layer_count;
					//~ free(cur_ly);
				}
				/* TRACKS */
				else if (strcmp(node2->name, "wav_track") == 0)
				{
					/* print "wav_track" */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("    %s : \n", node2->name);
					
					t_wav_track	*	cur_tk;
					init_wav_track(cur_tk, qdcp);
											
					/* track's attributes */
					/* w_id */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        w_id = %s\n", xmlGetProp(node2, "w_id"));
					cur_tk->w_id = strtoimax(xmlGetProp(node2, "w_id"), NULL, 10);
					/* filename */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        filename = %s\n", xmlGetProp(node2, "filename"));
					sprintf(cur_tk->filename, "%s", xmlGetProp(node2, "filename"));
					/* basename */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        basename = %s\n", xmlGetProp(node2, "basename"));
					sprintf(cur_tk->basename, "%s", xmlGetProp(node2, "basename"));					if (qdcp->verbosity >= VERBOSE_VERBOSE)
					/* creation */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("        creation  = %s\n", xmlGetProp(node2, "creation"));
					cur_tk->creation  = strtoimax(xmlGetProp(node2, "creation"), NULL, 10);
					
					/* track's children */
					third_level = node2->children;
					for (node3 = third_level; node3; node3 = node3->next)
					{
						if (strcmp(node3->name, "text") != 0)
						{								
							if (qdcp->verbosity >= VERBOSE_VERBOSE)
								printf("        %s :  ", node3->name);
							if (qdcp->verbosity >= VERBOSE_VERBOSE)
								printf("%s\n", xmlNodeListGetString(qdp, node3->children, 1));
							
							/* status (int) */
							if (strcmp(node3->name, "status") == 0)
								cur_tk->status = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* original */
							if (strcmp(node3->name, "original") == 0)
								sprintf(cur_tk->original, "%s", xmlNodeListGetString(qdp, node3->children, 1));
							/* ext */
							if (strcmp(node3->name, "ext") == 0)
								sprintf(cur_tk->ext, "%s", xmlNodeListGetString(qdp, node3->children, 1));

							/* entry_point (int) */
							if (strcmp(node3->name, "entry_point") == 0)
								cur_tk->entry_point = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* duration (int) */
							if (strcmp(node3->name, "duration") == 0)
								cur_tk->duration = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* time_length (int) */
							if (strcmp(node3->name, "time_length") == 0)
								cur_tk->time_length = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* time_HHMMSS (char[])*/
							if (strcmp(node3->name, "time_HHMMSS") == 0)
								sprintf(cur_tk->time_HHMMSS, "%s", xmlNodeListGetString(qdp, node3->children, 1));
							/* lock_to_layer (int) */
							if (strcmp(node3->name, "lock_to_layer") == 0)
								cur_tk->lock_to_layer = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);

							/* mix_type (int) */
							if (strcmp(node3->name, "mix_type") == 0)
								cur_tk->mix_type = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* src_samples (int) */
							if (strcmp(node3->name, "src_samples") == 0)
								cur_tk->src_samples = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* src_channels (int) */
							if (strcmp(node3->name, "src_channels") == 0)
								cur_tk->src_channels = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* src_bitdepth in bits per sample (int) */
							if (strcmp(node3->name, "src_bitdepth") == 0)
								cur_tk->src_bitdepth = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* volume (int) */
							if (strcmp(node3->name, "volume") == 0)
								cur_tk->volume = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* src_samplerate (float) */
							if (strcmp(node3->name, "src_samplerate") == 0)
								cur_tk->src_samplerate = strtof(xmlNodeListGetString(qdp, node3->children, 1), NULL);
							/* src_codec_id (int) */
							if (strcmp(node3->name, "src_codec_id") == 0)
								cur_tk->src_codec_id = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* encoding (int) */
							if (strcmp(node3->name, "encoding") == 0)
								cur_tk->encoding = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
			
							/* annotations */
							if (strcmp(node3->name, "annotations") == 0)
								cur_tk->annotations = xmlNodeListGetString(qdp, node3->children, 1);

							/* diamond */
							if (strcmp(node3->name, "diamond") == 0)
								cur_tk->diamond = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);
							/* mute (bool) */
							if (strcmp(node3->name, "mute") == 0)
								cur_tk->mute = strtoimax(xmlNodeListGetString(qdp, node3->children, 1), NULL, 10);

						}
					}
					/* add track in project's track list */
					track_count++;
					if (track_count == 1)
						np->tracks->first = cur_tk;
					
					if (np->tracks->last != NULL)
					{
						np->tracks->last->next = cur_tk;
						cur_tk->prev = np->tracks->last;
					}
					cur_tk->next = NULL;
					np->tracks->last = cur_tk;
					np->tracks->size = track_count;
					//~ free(cur_tk);
				}
				/* PROJECT */
				else if (strcmp(node2->name, "text") != 0)
				/* get project's values */
				{
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("    %s :  ", node2->name);
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("%s\n", xmlNodeListGetString(qdp, node2->children, 1));

					/* p_id (int) */
					if (strcmp(node2->name, "p_id") == 0)
						np->p_id = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* name */
					if (strcmp(node2->name, "name") == 0)
						sprintf(np->name, "%s", xmlNodeListGetString(qdp, node2->children, 1));
					/* status (int) */
					if (strcmp(node2->name, "status") == 0)
						np->status = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* creation time (time_t) */
					if (strcmp(node2->name, "creation") == 0)
						get_time_from_string(np->creation, xmlNodeListGetString(qdp, node2->children, 1));
					/* last_mod time (time_t) */
					if (strcmp(node2->name, "last_mod") == 0)
						get_time_from_string(np->last_mod, xmlNodeListGetString(qdp, node2->children, 1));
					/* issuer */
					if (strcmp(node2->name, "issuer") == 0)
						sprintf(np->issuer, "%s", xmlNodeListGetString(qdp, node2->children, 1));
					/* issuer_short */
					if (strcmp(node2->name, "issuer_short") == 0)
						sprintf(np->issuer_short, "%s", xmlNodeListGetString(qdp, node2->children, 1));
					/* format */
					if (strcmp(node2->name, "format") == 0)
						np->format = xmlNodeListGetString(qdp, node2->children, 1);
					/* resolution */
					if (strcmp(node2->name, "resolution") == 0)
						np->resolution = xmlNodeListGetString(qdp, node2->children, 1);
					/* framerate (int) */
					if (strcmp(node2->name, "framerate") == 0)
						np->framerate = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* category */
					if (strcmp(node2->name, "category") == 0)
					{
						for (i = 0; i < CAT_N_CATEGORIES; i++)
						{
							if (strcmp(qdcp->short_cats[i], xmlNodeListGetString(qdp, node2->children, 1)) == 0)
								np->category = i; 
						}
					}
					/* laps (float) */
					if (strcmp(node2->name, "laps") == 0)
						np->laps = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* nmax (int) */
					//~ if (strcmp(node2->name, "nmax") == 0)
						//~ np->nmax = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					//~ /* mx_w (int) */
					//~ if (strcmp(node2->name, "mx_w") == 0)
						//~ np->mx_w = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					//~ /* mx_h (int) */
					//~ if (strcmp(node2->name, "mx_h") == 0)
						//~ np->mx_h = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* zoom_factor (float) */
					if (strcmp(node2->name, "zoom_factor") == 0)
						np->zoom_factor = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* current_frame (int) */
					if (strcmp(node2->name, "current_frame") == 0)
						np->current_frame = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* active_layer (t_layer*) */
					if (strcmp(node2->name, "active_layer") == 0)
					{
							int act_layer_id;
							t_layer * loop_ly = np->layers->first;
						act_layer_id = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
						while(loop_ly != NULL)
						{
							if (loop_ly->l_id == act_layer_id)
								np->active_layer = loop_ly;
						}
					}
					/* lang_code */
					if (strcmp(node2->name, "lang_code") == 0)
						np->lang_code = xmlNodeListGetString(qdp, node2->children, 1);
					/* subtitle_code */
					if (strcmp(node2->name, "subtitle_code") == 0)
						np->subtitle_code = xmlNodeListGetString(qdp, node2->children, 1);
					/* country_code */
					if (strcmp(node2->name, "country_code") == 0)
						np->country_code = xmlNodeListGetString(qdp, node2->children, 1);
					/* background */
					if (strcmp(node2->name, "background") == 0)
						np->background = xmlNodeListGetString(qdp, node2->children, 1);
					/* fondu_d */
					if (strcmp(node2->name, "fondu_d") == 0)
					{
						for (i = 0; i <= FONDU_IRIS; i++)
						{
							if (strcmp(qdcp->fondus[i], xmlNodeListGetString(qdp, node2->children, 1)) == 0)
								np->fondu_d = i; 
						}
					}
					/* fondu_f */
					if (strcmp(node2->name, "fondu_f") == 0)
					{
						for (i = 0; i <= FONDU_IRIS; i++)
						{
							if (strcmp(qdcp->fondus[i], xmlNodeListGetString(qdp, node2->children, 1)) == 0)
								np->fondu_f = i; 
						}
					}
					/* fondulaps_d (float) */
					if (strcmp(node2->name, "fondulaps_d") == 0)
						np->fondulaps_d = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* fondulaps_f (float) */
					if (strcmp(node2->name, "fondulaps_f") == 0)
						np->fondulaps_f = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* fondu_cache_color_d */
					if (strcmp(node2->name, "fondu_cache_color_d") == 0)
						np->fondu_cache_color_d = xmlNodeListGetString(qdp, node2->children, 1);
					/* fondu_cache_color_f */
					if (strcmp(node2->name, "fondu_cache_color_f") == 0)
						np->fondu_cache_color_f = xmlNodeListGetString(qdp, node2->children, 1);
					/* fondu_iris_cx_d (float) */
					if (strcmp(node2->name, "fondu_iris_cx_d") == 0)
						np->fondu_iris_cx_d = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* fondu_iris_cy_d (float) */
					if (strcmp(node2->name, "fondu_iris_cy_d") == 0)
						np->fondu_iris_cy_d = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* fondu_iris_cx_f (float) */
					if (strcmp(node2->name, "fondu_iris_cx_f") == 0)
						np->fondu_iris_cx_f = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* fondu_iris_cy_f (float) */
					if (strcmp(node2->name, "fondu_iris_cy_f") == 0)
						np->fondu_iris_cy_f = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
				}
			}			
		}
	}
	
	xmlFreeDoc(qdp);
	
	/* compute some useful project values */
	np->nmax = (int) np->framerate * np->laps;
	int  dims[2];
	get_dims_from_format(dims, np->format);
	np->mx_w = dims[0];
	np->mx_h = dims[1];

    np->mxf.write_hmac = 1;
	np->ns = XML_NS_SMPTE;
	np->xml_signature.sign 			= qdcp->signature;
	np->xml_signature.use_external 	= 0;
	np->xml_signature.root = NULL;
	np->xml_signature.ca = NULL;
	np->xml_signature.signer = NULL;
	np->xml_signature.private_key = NULL;
	    
    sprintf(np->dcp.creator, "%.80s %.80s", QDCP_NAME, QDCP_VERSION);
    sprintf(np->dcp.annotation, "%.128s", DCP_ANNOTATION);
    sprintf(np->dcp.title, "%.80s", DCP_TITLE);
    sprintf(np->dcp.kind, "%.15s", DCP_KIND);
    //~ get_timestamp(np->dcp.issue_date);		/**< This timestamp is rewritten when finalising DCP */
    
    /* initialize callbacks */
    np->mxf.frame_done.callback  = qdcp_callback_null;
    np->mxf.frame_done.argument  = NULL;
    np->mxf.file_done.callback   = qdcp_callback_null;
    np->mxf.file_done.argument   = NULL;
    
    np->dcp.sha1_update.callback = qdcp_callback_null;
    np->dcp.sha1_update.argument = NULL;
    np->dcp.sha1_done.callback = qdcp_callback_null;
    np->dcp.sha1_done.argument = NULL;
    
	/* insert project at the end of projects list */
	if (pjl != NULL)
	{
		if (pjl->last != NULL)
		{
			pjl->last->next = np;
			np->prev = pjl->last;
		}
		pjl->last = np;
		pjl->size++;
		if (pjl->first == NULL)
		{
			pjl->first = np;
		}
	}

	return np;
}

qdcp_bool
find_project_by_name(t_project * pj, const char * bname, quickdcp_ctx * qdcp) 
{
	pj = qdcp->proj_list->first;
	
	while(pj != NULL)
	{
		//~ printf("Quickdcp : Trying project %s\n", pj->name);
		if (pj->name == bname)
		{
			printf("Quickdcp : Found project %s\n", pj->name);
			get_dcp_name_from_project(pj, qdcp);
			
			return QDCP_SUCCESS;
		}	
		pj = pj->next;
	}
	
	return QDCP_FAILURE;
}

void 
show_project_params(t_project * np, quickdcp_ctx * qdcp) 
{
	printf("\n PROJECT'S PARAMETERS :");
	printf("\n Nom de base : \t%s", 			np->name);
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n p_id : \t%d", 					np->p_id);
		printf("\n Creation :\t%d", 				(int)np->creation);
		printf("\n Modifié :\t%d", 					(int)np->last_mod);
		printf("\n Status :\t%d",					np->status);
		printf("\n Issuer :\t%s",					np->issuer);
		printf("\n Issuer short :\t%s",				np->issuer_short);
	}
	printf("\n");
	
	printf("\n Format : \t%s (%dx%d)", 			np->format , np->mx_w, np->mx_h);
	printf("\n Durée : \t%.2f\"", 				np->laps);
	printf("\n Categorie : \t%s (%s)", 			qdcp->dcp_categories[np->category], qdcp->short_cats[np->category]);
	printf("\n");
	printf("\n Resolution : \t%s", 				np->resolution);
	printf("\n Framerate : \t%d img/s", 		np->framerate);
	printf("\n Photogrammes :\t%ld", 			np->nmax);
	printf("\n");	
	printf("\n Arrière-plan :\t#%s",		 	np->background);			/*color*/
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n Langue audio :\t%s",				np->lang_code);	
		printf("\n Langue sous-titres :\t%s",		np->subtitle_code);	
		printf("\n Langue pays :\t%s",				np->country_code);	
	}
	printf("\n");
	printf("\n Fondu début :\t%d", 				np->fondu_d);
	printf("\n -  durée : \t%.1f\"", 			np->fondulaps_d);
	printf("\n -  couleur :\t#%s", 				np->fondu_cache_color_d);	/*color*/
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n -  Centre iris x : \t%.1f%%", 	np->fondu_iris_cx_d);
		printf("\n -  Centre iris y : \t%.1f%%", 	np->fondu_iris_cy_d);
	}
	printf("\n");
	printf("\n Fondu fin : \t%d", 				np->fondu_f);
	printf("\n -  durée : \t%.1f\"", 			np->fondulaps_f);
	printf("\n -  couleur :\t#%s", 				np->fondu_cache_color_f); 	/*color*/
	/* centre des fondus iris */
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n -  Centre iris x : \t%.1f%%", 	np->fondu_iris_cx_f);
		printf("\n -  Centre iris y : \t%.1f%%", 	np->fondu_iris_cy_f);
	}
	printf("\n\n");
}

int 
merge_projects(t_project * pj_dest, t_project * pj_src) 
{
}

int 
concat_projects(t_project * pj_dest, t_project * pj_src) 
{
}

int 
free_project(t_project * pj) 
{
		t_layer * ly;
	/* free project's layers */
	ly = pj->layers->first;
	while (ly != NULL)
	{
		/* free and close ffmpeg structs in layers */
		avcodec_close(ly->src_codec_ctx);
		avcodec_close(ly->out_codec_ctx);
		avformat_close_input(&ly->src_format_ctx);
		//~ avformat_close_output(ly->out_format_ctx);
		av_free(ly->src_format_ctx);
		av_free(ly->out_format_ctx);
		
		if (ly->next != NULL)
		{
			ly = ly->next;
			free(ly->prev);
		}
		else
		{
			free(ly->next);
			free(ly);
		}
	}
	
	/* free layers list */
	free(pj->layers);

	/* free project itself */
	free(pj);
	
	return 0;
}


/* layers functions 
 */

t_layer * 
init_layer(t_project * pj) 
{
		t_layer 	* nl = calloc(1, sizeof(*nl));
		t_kf_list 	* kfl = calloc(1, sizeof(*kfl));

	if (nl == NULL || kfl == NULL)
	{
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	}

		int			pos;
		struct		timeval	tv;
		char 	   	layername[NAME_STR_LEN];
		char 		id_string[NAME_STR_LEN];

	gettimeofday(&tv, NULL);
	sprintf(id_string, "layer%d", (int) tv.tv_sec);

    nl->l_id			= hash(id_string);
	
	kfl->first 			= NULL;
	kfl->last 			= NULL;
	kfl->size 			= 0;
	nl->keyframes 		= kfl;
	
	nl->l_id 			= 0;
	nl->prev 			= NULL;
	nl->next 			= NULL;
	nl->type 			= LAYER_BLANK;
	nl->parent 			= NULL;
	
    /* new layer begins at cursor position */
    nl->entry_point		= pj->current_frame;
	nl->exit_point 		= pj->nmax;
	nl->pos 			= pj->layers->size;
	/* default is visible */
	nl->visible 		= 1;
	nl->keep_ratio 		= 0;
    /* layer fades are 12 by default */
    nl->fade_in 		= 0;
    nl->fade_out 		= 0;
	nl->bg_color 		= NULL;
	nl->fg_color 		= NULL;
	nl->margin 			= 0;
	
	nl->source_file[0] 	= 0;					/** < Absolute path of source file (expectedly in workspace) */
	
	/* image layers */
	nl->filename[0] 	= 0;
	nl->basename[0] 	= 0;
	nl->ext[0] 			= 0;
	/* text layers */
	nl->text 			= NULL;
	nl->font_desc 		= NULL;
	nl->interline 		= 0;
	sprintf(nl->text_color, "#ffffff");
	nl->text_align 		= NULL;
	nl->text_stretch 	= NULL;
	nl->text_style 		= NULL;
	nl->text_variant 	= NULL;
	nl->text_weight 	= NULL;
	nl->text_lang 		= NULL;
	/* vectorial layers */
	nl->shape 			= SHAPE_UNDEFINED;
	nl->line_width 		= 0;
	nl->line_style	 	= NULL;
	nl->line_join 		= NULL;
	nl->line_caps 		= NULL;
	nl->background_on 	= 1;
	nl->border_on 		= 1;
	/* video specific variables */
	nl->src_format_ctx 	= NULL;
	nl->src_codec_ctx 	= NULL;
	nl->src_codec		= NULL;
	nl->src_codec_ctx 	= NULL;
	nl->src_codec		= NULL;
	nl->src_w			= 1;
	nl->src_h			= 1;
	nl->src_ratio		= 1.0;
	//~ nl->frame_rate		= NULL;

	/* new layer is always on top */
	if (pj->layers->size == 0)
	{
		nl->prev = NULL;
		pj->layers->first = nl;
	}
	else
	{
		pj->layers->last->next = nl;
		nl->prev = pj->layers->last;
	}
	pj->layers->last = nl;
	
	nl->next = NULL;
	
	/* increment project layers list */
	pj->layers->size++;
	pos = pj->layers->size;
	nl->pos = pos;
	
	/* new layer gets focus */
	pj->active_layer = nl;
	
	/* get a good name to the new layer */
	int max_layer = pj->layers->size;
	sprintf(nl->layername, "Calque %d", max_layer);
	if (max_layer > 1)
	{
		t_layer * cur_ly = pj->layers->first;
		while (cur_ly != NULL)
		{			
			if (strcmp(cur_ly->layername, nl->layername) == 0)
			{
				max_layer++;
				sprintf(nl->layername, "Calque %d", max_layer);
			}
			cur_ly = cur_ly->next;
		}
	}	
	return nl;
}

qdcp_bool
create_new_image_layer(t_layer * nl, char * file_path, t_project * pj, quickdcp_ctx * qdcp) 
{	
		int 		mx_w = pj->mx_w;
		int 		mx_h = pj->mx_h;
		int			src_w;
		int			src_h;
		float		fmt_ratio;
		float		src_ratio;
		int 		img_w;
		int 		img_h;
		int			img_x;
		int			img_y;

    nl->type	= LAYER_IMAGE;
    nl->keep_ratio = QDCP_YES;
	
	/* background and foreground colors + margin */
    nl->bg_color = "transparent";

	nl->fg_color = qdcp->cfg->foreground;
	nl->margin = qdcp->cfg->margin;

	/* in case we want an empty layer (e.g. blind DCP, sound only) */
	if (file_path == NULL)
	{
		sprintf(nl->filename,	"[empty]");
		sprintf(nl->basename, 	"[empty]");
		sprintf(nl->ext,		"[empty]");
		
		sprintf(nl->source_file, "[empty]");
		//~ nl->source_colorspace = NULL;
		
		src_w = pj->mx_w;
		src_h = pj->mx_h;
	}
	else
	{
		/* if the file is not in our workspace get a copy there */
		if (! import_media_file(nl, file_path, qdcp))
			warning("Import de fichier image échoué");
						
		/* get a wand */
			MagickWand		*	wand = NewMagickWand();

		if (MagickReadImage(wand, nl->source_file) == MagickFalse)
		{
			ThrowWandException(wand);
		}
		
		src_w = MagickGetImageWidth(wand);
		src_h = MagickGetImageHeight(wand);	
		
	}	/* end of test (source) file_object is NULL */
        
	
	/* compute ratios */
	fmt_ratio =  (float) mx_w /  mx_h;
	src_ratio =  (float) src_w / src_h;

	
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("%sDimensions de %s :\t%dx%d px\n", CLI_INDENT, nl->filename, src_w, src_h);
		printf("%sRATIO FORMAT : \t%.2f\n", CLI_INDENT, fmt_ratio); 
		printf("%sRATIO SOURCE : \t%.2f\n", CLI_INDENT, src_ratio); 
		printf("\n");
	}

	/* determine composition */
	if (src_ratio < fmt_ratio)
	{
		/* pillarbox */
		// On utilise toute la hauteur moins les 2 marges, haut et bas.
		img_h = mx_h - (nl->margin * 2);
		img_w = (int) img_h * src_ratio;
	}
	else
	{
		/* letterbox */
		// On utilise toute la largeur, sans marge.
		img_w = mx_w;
		img_h = (int) img_w / src_ratio;
	}

	/* scaled source image will always be centered : */
	img_x = (mx_w - img_w) / 2;
	img_y = (mx_h - img_h) / 2;

	/* default keyframes at entry and exit point */
		
	t_keyframe 	* start_kf 	= init_keyframe(nl->entry_point, nl, pj);
	t_keyframe 	* end_kf 	= init_keyframe(nl->exit_point, nl, pj);

	/* update the layer's keyframes according to last operation */
	t_keyframe * cur_kf;
	cur_kf = nl->keyframes->first;
	while (cur_kf != NULL)
	{
		cur_kf->x = img_x;
		cur_kf->y = img_y;
		cur_kf->ox = mx_w / 2;
		cur_kf->oy = mx_h / 2;
		cur_kf->w = img_w;
		cur_kf->h = img_h;
		cur_kf->centerX = img_w / 2;
		cur_kf->centerY = img_h / 2;
		cur_kf->ratio = (float) cur_kf->w / cur_kf->h;
		cur_kf->oxp = 100 * (float)(cur_kf->ox / mx_w);
		cur_kf->oyp = 100 * (float)(cur_kf->oy / mx_h);
		cur_kf->wp = 100 * (float)(cur_kf->w / mx_w);
		cur_kf->hp = 100 * (float)(cur_kf->h / mx_h);
		
		cur_kf = cur_kf->next;
	}
				
	return QDCP_SUCCESS;
}

qdcp_bool 
create_new_text_layer(t_layer * nl, file_obj * file_object, t_project * pj, quickdcp_ctx * qdcp) 
{

			int 		mx_w = pj->mx_w;
			int 		mx_h = pj->mx_h;
			int 		txt_w;
			int 		txt_h;
			int			txt_x;
			int			txt_y;
			float		txt_ratio;
			

    nl->type		= LAYER_TEXT;
    nl->keep_ratio 	= QDCP_YES;
    
	/* background and foreground colors + margin */
    nl->bg_color = "transparent";

	nl->fg_color = qdcp->cfg->foreground;
	nl->margin = qdcp->cfg->margin;
	
	/* compute ratio */
	txt_ratio =  (float) txt_w / txt_h;

	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("%sDimensions de %s :\t%dx%d px\n", CLI_INDENT, nl->layername, txt_w, txt_h);
		printf("%sRATIO TEXTE : \t%.2f\n", CLI_INDENT, txt_ratio);
		printf("\n");
	}

	/* scaled source image will always be centered : */
	txt_x = (mx_w - txt_w) / 2;
	txt_y = (mx_h - txt_h) / 2;

	/* default keyframes at entry and exit point */
		
	t_keyframe 	* start_kf 	= init_keyframe(nl->entry_point, nl, pj);
	t_keyframe 	* end_kf 	= init_keyframe(nl->exit_point, nl, pj);

	/* update the layer's keyframes according to last operation */
	t_keyframe * cur_kf;
	cur_kf = nl->keyframes->first;
	while (cur_kf != NULL)
	{
		cur_kf->x = txt_x;
		cur_kf->y = txt_y;
		cur_kf->ox = mx_w / 2;
		cur_kf->oy = mx_h / 2;
		cur_kf->w = txt_w;
		cur_kf->h = txt_h;
		cur_kf->centerX = txt_w / 2;
		cur_kf->centerY = txt_h / 2;
		cur_kf->ratio = (float) cur_kf->w / cur_kf->h;
		cur_kf->oxp = 100 * (float)(cur_kf->ox / mx_w);
		cur_kf->oyp = 100 * (float)(cur_kf->oy / mx_h);
		cur_kf->wp = 100 * (float)(cur_kf->w / mx_w);
		cur_kf->hp = 100 * (float)(cur_kf->h / mx_h);
		
		cur_kf = cur_kf->next;
	}
		
	return QDCP_SUCCESS;
}

qdcp_bool 
create_new_vectorial_layer(t_layer * nl, file_obj * file_object, t_project * pj, quickdcp_ctx * qdcp) 
{

			int 		mx_w = pj->mx_w;
			int 		mx_h = pj->mx_h;
			int			src_w;
			int			src_h;
			float		fmt_ratio;
			float		src_ratio;
			int 		img_w;
			int 		img_h;
			int			img_x;
			int			img_y;
			

    nl->type	= LAYER_VECTORIAL;        
    nl->keep_ratio = QDCP_NO;


	/* background and foreground colors + margin */
    nl->bg_color = "transparent";

	nl->fg_color = qdcp->cfg->foreground;
	nl->margin = qdcp->cfg->margin;

	

	/* compute ratios */
	fmt_ratio =  (float) mx_w /  mx_h;
	src_ratio =  (float) src_w / src_h;

	
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("%sDimensions de %s :\t%dx%d px\n", CLI_INDENT, nl->filename, src_w, src_h);
		printf("%sRATIO FORMAT : \t%.2f\n", CLI_INDENT, fmt_ratio); 
		printf("%sRATIO SOURCE : \t%.2f\n", CLI_INDENT, src_ratio); 
		printf("\n");
	}


	/* scaled source image will always be centered : */
	img_x = (mx_w - img_w) / 2;
	img_y = (mx_h - img_h) / 2;

	/* default keyframes at entry and exit point */
		
	t_keyframe 	* start_kf 	= init_keyframe(nl->entry_point, nl, pj);
	t_keyframe 	* end_kf 	= init_keyframe(nl->exit_point, nl, pj);

	/* update the layer's keyframes according to last operation */
	t_keyframe * cur_kf;
	cur_kf = nl->keyframes->first;
	while (cur_kf != NULL)
	{
		cur_kf->x = img_x;
		cur_kf->y = img_y;
		cur_kf->ox = mx_w / 2;
		cur_kf->oy = mx_h / 2;
		cur_kf->w = img_w;
		cur_kf->h = img_h;
		cur_kf->centerX = img_w / 2;
		cur_kf->centerY = img_h / 2;
		cur_kf->ratio = (float) cur_kf->w / cur_kf->h;
		cur_kf->oxp = 100 * (float)(cur_kf->ox / mx_w);
		cur_kf->oyp = 100 * (float)(cur_kf->oy / mx_h);
		cur_kf->wp = 100 * (float)(cur_kf->w / mx_w);
		cur_kf->hp = 100 * (float)(cur_kf->h / mx_h);
		
		cur_kf = cur_kf->next;
	}
		
		
	return QDCP_SUCCESS;
}

qdcp_bool
create_new_video_layer(t_layer * ly, char * file_path, t_project * pj, quickdcp_ctx * qdcp) 
{

		const	char	*	seq_path[PATH_STR_LEN];
	
				char		src_path[PATH_STR_LEN];				
				int 		mx_w = pj->mx_w;
				int 		mx_h = pj->mx_h;
				int			src_w;
				int			src_h;
				float		fmt_ratio;
				int 		img_w;
				int 		img_h;
				int			img_x;
				int			img_y;		

    ly->type	= LAYER_VIDEO;
    ly->keep_ratio = QDCP_YES;
	
	/* background and foreground colors + margin */
    ly->bg_color = qdcp->cfg->background;
	ly->fg_color = qdcp->cfg->foreground;
	ly->margin = qdcp->cfg->margin;
	
	/* store the full (absolute) file path */
	sprintf(ly->source_file, "%s", file_path);
	sprintf(ly->basename, "%s", strip_path(get_basename(file_path)));
	sprintf(ly->ext, "%s", get_filename_ext(file_path));

	/* init output codec */
	ly->out_codec_id = AV_CODEC_ID_PNG;
	//~ ly->out_codec_id = AV_CODEC_ID_JPEG2000;	
		
	/* compute ratios */
	fmt_ratio =  (float) mx_w / mx_h;

	
	if (qdcp->verbosity >= VERBOSE_NORMAL)
	{
		printf("%sDimensions de %s :\t%dx%d px\n", CLI_INDENT, ly->filename, ly->src_w, ly->src_h);
		printf("%sRATIO SOURCE : \t%.2f\n", CLI_INDENT, ly->src_ratio); 
		printf("%sRATIO FORMAT : \t%.2f\n", CLI_INDENT, fmt_ratio); 
		printf("\n");
	}
	
	/* do we extend video format (16/9) to Flat ? */
	// only if margin == 0
	
	/* determine composition */
	if (ly->src_ratio < fmt_ratio)
	{
		/* pillarbox */
		// On utilise toute la hauteur moins les 2 marges, haut et bas.
		img_h = mx_h - (ly->margin * 2);
		img_w = (int) img_h * ly->src_ratio;
	}
	else
	{
		/* letterbox */
		// On utilise toute la largeur, moins la marge de chaque côté.
		img_w = mx_w - (ly->margin *2);
		img_h = (int) img_w / ly->src_ratio;
	}

	/* scaled source image will always be centered : */
	img_x = (mx_w - img_w) / 2;
	img_y = (mx_h - img_h) / 2;

	/* default keyframes at entry and exit point */
		
	t_keyframe 	* start_kf 	= init_keyframe(ly->entry_point, ly, pj);
	t_keyframe 	* end_kf 	= init_keyframe(ly->exit_point, ly, pj);

	/* update the layer's keyframes according to last operation */
	t_keyframe * cur_kf;
	cur_kf = ly->keyframes->first;
	while (cur_kf != NULL)
	{
		cur_kf->x = img_x;
		cur_kf->y = img_y;
		cur_kf->ox = mx_w / 2;
		cur_kf->oy = mx_h / 2;
		cur_kf->w = img_w;
		cur_kf->h = img_h;
		cur_kf->centerX = img_w / 2;
		cur_kf->centerY = img_h / 2;
		cur_kf->ratio = (float) cur_kf->w / cur_kf->h;
		cur_kf->oxp = 100 * (float)(cur_kf->ox / mx_w);
		cur_kf->oyp = 100 * (float)(cur_kf->oy / mx_h);
		cur_kf->wp = 100 * (float)(cur_kf->w / mx_w);
		cur_kf->hp = 100 * (float)(cur_kf->h / mx_h);
		
		cur_kf = cur_kf->next;
	}

	/* save that good work */					
	save_project(pj, qdcp);													
				
	return QDCP_SUCCESS;
}

qdcp_bool
create_new_mxf_layer(t_layer * nl, file_obj * file_object, t_project * pj, quickdcp_ctx * qdcp) 
{

		const	char	*	seq_path[PATH_STR_LEN];
	
				char		src_path[PATH_STR_LEN];				
				int 		mx_w = pj->mx_w;
				int 		mx_h = pj->mx_h;
				int			src_w;
				int			src_h;
				float		fmt_ratio;
				float		src_ratio;
				int 		img_w;
				int 		img_h;
				int			img_x;
				int			img_y;
			

    nl->type	= LAYER_VIDEO;
	sprintf(nl->filename, "%s", get_fname(file_object->fname));
	sprintf(nl->basename, "%s", get_basename(file_object->fname));
	sprintf(nl->ext, "%s", get_filename_ext(file_object->fname));
        
    nl->keep_ratio = QDCP_YES;
	

	/* background and foreground colors + margin */
    nl->bg_color = qdcp->cfg->background;
	nl->fg_color = qdcp->cfg->foreground;
	nl->margin = qdcp->cfg->margin;
	
	/* get the full (absolute) file path */
	sprintf(src_path, "%s%s%c", qdcp->workspace, nl->filename, '\0');

	sprintf(nl->source_file, "%s", src_path);
	
	/* now use FFmpeg to get some info */

	/* register libav codecs and formats 
	 * (needed only once, so, better, may be, in main()? ) */
	av_register_all();

	/* extract video info towards layer values */
	/* needs a wav_track pointer */
	//~ get_video_info(nl, NULL, pj, qdcp);
	
		
	/* compute ratios */
	fmt_ratio =  (float) mx_w /  mx_h;
	src_ratio =  (float) src_w / src_h;

	/* input video framerate */
	//~ vid_framerate =
	
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("%sDimensions de %s :\t%dx%d px\n", CLI_INDENT, nl->filename, src_w, src_h);
		printf("%sRATIO FORMAT : \t%.2f\n", CLI_INDENT, fmt_ratio); 
		printf("%sRATIO SOURCE : \t%.2f\n", CLI_INDENT, src_ratio); 
		printf("%sMarge : \t\t%dpx\n", CLI_INDENT, nl->margin); 
		printf("\n");
	}
	
	/* do we extend video format (16/9) to Flat ? */
	// only if margin == 0
	
	/* determine composition */
	if (src_ratio < fmt_ratio)
	{
		/* pillarbox */
		// On utilise toute la hauteur moins les 2 marges, haut et bas.
		img_h = mx_h - (nl->margin * 2);
		img_w = (int) img_h * src_ratio;
	}
	else
	{
		/* letterbox */
		// On utilise toute la largeur, moins la marge de chaque côté.
		img_w = mx_w - (nl->margin *2);
		img_h = (int) img_w / src_ratio;
	}

	/* scaled source image will always be centered : */
	img_x = (mx_w - img_w) / 2;
	img_y = (mx_h - img_h) / 2;

	/* default keyframes at entry and exit point */
		
	t_keyframe 	* start_kf 	= init_keyframe(nl->entry_point, nl, pj);
	t_keyframe 	* end_kf 	= init_keyframe(nl->exit_point, nl, pj);

	/* update the layer's keyframes according to last operation */
	t_keyframe * cur_kf;
	cur_kf = nl->keyframes->first;
	while (cur_kf != NULL)
	{
		cur_kf->x = img_x;
		cur_kf->y = img_y;
		cur_kf->ox = mx_w / 2;
		cur_kf->oy = mx_h / 2;
		cur_kf->w = img_w;
		cur_kf->h = img_h;
		cur_kf->centerX = img_w / 2;
		cur_kf->centerY = img_h / 2;
		cur_kf->ratio = (float) cur_kf->w / cur_kf->h;
		cur_kf->oxp = 100 * (float)(cur_kf->ox / mx_w);
		cur_kf->oyp = 100 * (float)(cur_kf->oy / mx_h);
		cur_kf->wp = 100 * (float)(cur_kf->w / mx_w);
		cur_kf->hp = 100 * (float)(cur_kf->h / mx_h);
		
		cur_kf = cur_kf->next;
	}
				
	return QDCP_SUCCESS;
}


void 
show_layer_params(t_layer * ly, int verbosity) 
{

		t_keyframe 	* 	kf;
		t_project	*	parent = ly->parent;
	
	//~ QDCP_LOG(LOG_INFO, "Parametres du calque :");
	
	printf("\n LAYER'S PARAMETERS :");
	if (verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n     l_id :\t%d", 		ly->l_id);
		printf("\n     prev :\t%p", 		ly->prev);
		printf("\n     next :\t%p", 		ly->next);
	}
	printf("\n     Nom :\t\t%s",			ly->layername);
	printf("\n     Type :\t\t%d",			ly->type);
	printf("\n     Position :\t\t%d", 		ly->pos);
	//~ printf("\n     Parent : \t%s", 			parent->name);
	if (ly->type == LAYER_IMAGE)
	{
		printf("\n     Fichier :\t\t%s", 	ly->filename);
		printf("\n     Nom d'image :\t%s", 	ly->basename);
		printf("\n     mime :\t\t%s", 		ly->ext);
		printf("\n     source :\t\t%s", 	ly->source_file);
	}
	printf("\n     Entry point : \t%d", 	ly->entry_point);
	printf("\n     Exit point : \t%d", 		ly->exit_point);
	printf("\n     Keyframes : \t");
	kf = ly->keyframes->first;
	while (kf != NULL)
	{
		printf("%d  ", 					kf->photogram);
		kf = kf->next;
	}
	printf("\n     fade in : \t\t%d images",ly->fade_in);
	printf("\n     fade out : \t%d images", ly->fade_out);
	printf("\n     background : \t%s", 		ly->bg_color);
	printf("\n     foreground : \t%s",	 	ly->fg_color);
	printf("\n     marge : \t\t%d px", 		ly->margin);
	printf("\n     visible : \t\t%d", 		ly->visible);
	printf("\n     keep ratio : \t%d", 		ly->keep_ratio);
	if (ly->type == LAYER_TEXT)
	{
		printf("\n     text : \t\t%s", 		ly->text);
		printf("\n     font_desc : \t%s",	ly->font_desc);
		printf("\n     interline : \t%d", 	ly->interline);
		printf("\n     text_color : \t%s", 	ly->text_color);
		printf("\n     text_align : \t%s", 	ly->text_align);
		printf("\n     text_stretch : \t%s",ly->text_stretch);
		printf("\n     text_style : \t%s", 	ly->text_style);
		printf("\n     text_variant : \t%s",ly->text_variant);
		printf("\n     text_weight : \t%s", ly->text_weight);
		printf("\n     text_underline : \t%s", ly->text_underline);
		printf("\n     text_lang : \t%s", 	ly->text_lang);	
	}
	if (ly->type == LAYER_VECTORIAL)
	{
		printf("\n     shape : \t\t%d", 		ly->shape);
		printf("\n     line_width : \t%d", 		ly->line_width);
		printf("\n     line_style : \t%s", 		ly->line_style);
		printf("\n     line_join : \t%s", 		ly->line_join);
		printf("\n     line_caps : \t%s", 		ly->line_caps);
		printf("\n     background_on : \t%d", 	ly->background_on);
		printf("\n     border_on : \t%d", 		ly->border_on);
	}
	printf("\n\n");
	/* loop keyframes */
	t_keyframe * curkf = ly->keyframes->first;
	while (curkf != NULL)
	{
		show_keyframe_params(curkf, verbosity);
		curkf = curkf->next;
	}
}

int 
free_layer(t_layer * ly) 
{
		t_keyframe * kf;
	
	/* remove layer's temporary image */
	if (ly->type == LAYER_IMAGE)
	{
		DestroyMagickWand(ly->wand);
	}
	/* free layer's keyframes */
	kf = ly->keyframes->first;
	while (kf != NULL)
	{
		if (kf->next != NULL)
		{
			kf = kf->next;
			free(kf->prev);
		}
		else
		{
			free(kf->next);
			free(kf);
		}
	}
	/* free keyframes list */
	free(ly->keyframes);
	
	/* free layer itself */
	free(ly);
		
	return QDCP_SUCCESS;
}


/* keyframes functions 
 */
 
t_keyframe * 
init_keyframe(int photogram, t_layer * ly, t_project * pj) 
{
	t_keyframe * nkf = calloc(1, sizeof(*nkf));
	if (nkf == NULL)
	{
		warning(BAD_CALLOC);
		return NULL;
	}
	t_keyframe * prevkf = NULL;
	t_keyframe * nextkf = NULL;
	
	nkf->photogram = photogram;
	nkf->prev = NULL;
	nkf->next = NULL;
		
	/* inserting keyframe in layers's keyframes list */ 
	if (ly->keyframes->size == 0)
	{
		ly->keyframes->first = nkf;
		ly->keyframes->last = nkf;
	}
	else
	{
		/* insert a keyframe before or after layer's current time span */
		/* before */
		if (nkf->photogram < ly->keyframes->first->photogram)
		{
			//~ printf("Inserting keyframe kf%d before kf%d\n", nkf->photogram, ly->keyframes->first->photogram);
			nkf->next = ly->keyframes->first;
			ly->keyframes->first->prev = nkf;
			ly->keyframes->first = nkf;
		}
		/* after */
		else if (nkf->photogram > ly->keyframes->last->photogram)
		{
			//~ printf("Inserting keyframe kf%d after kf%d\n", nkf->photogram, ly->keyframes->last->photogram);
			nkf->prev = ly->keyframes->last;
			ly->keyframes->last->next = nkf;
			ly->keyframes->last = nkf;
		}
		else
		/* insert a keyframe within layer's current time span */
		{
			prevkf = ly->keyframes->first;
			nextkf = ly->keyframes->last;
			
			/* look for previous keyframe if any */
			if (ly->keyframes->first->next == NULL)
			{
				prevkf->next = nkf;
			}
			else
			{
				while (prevkf->next != NULL)
				{
					if (prevkf->next->photogram < photogram)
					{
						prevkf = prevkf->next;
						nkf->prev = prevkf;
					}
				}
			}
			/* look for next keyframe if any */
			if (ly->keyframes->last->prev == NULL)
			{
				nextkf->prev = nkf;
			}
			else
			{
				while (nextkf->prev != NULL)
				{
					if (nextkf->prev->photogram > photogram)
					{
						nkf->next = nextkf;
						nextkf = nextkf->prev;
					}
				}
			}
		}
	}
	ly->keyframes->size++;
	
	/* keyframe default geometrical values */
	nkf->x 	= 0;
	nkf->y 	= 0;
	nkf->ox = pj->mx_w / 2;
	nkf->oy = pj->mx_h / 2;
	nkf->w 	= pj->mx_w;
	nkf->h 	= pj->mx_h;
	nkf->centerX = nkf->w / 2;
	nkf->centerY = nkf->h / 2;
	nkf->ratio 	= (float) nkf->w / nkf->h;
	nkf->oxp 	= 50.0;
	nkf->oyp 	= 50.0;
	nkf->wp 	= 100.0;
	nkf->hp 	= 100.0;
	nkf->rotrad = 0.0;
	nkf->rotdeg = 0.0;
	nkf->opacity = 100;
	nkf->bg_color = ly->bg_color;
	nkf->fg_color = ly->fg_color;

	return nkf;
}

t_keyframe * 
dupli_keyframe(int photogram, t_keyframe * kf1, t_layer * ly, t_project * pj) 
{
	t_keyframe * nkf = calloc(1, sizeof(*nkf));
	if (nkf == NULL)
	{
		warning(BAD_CALLOC);
		return NULL;
	}
	t_keyframe * prevkf = NULL;
	t_keyframe * nextkf = NULL;
	
	nkf->photogram = photogram;
	nkf->prev = NULL;
	nkf->next = NULL;
		
	/* inserting keyframe in layers's keyframes list */ 
	if (ly->keyframes->size == 0)
	{
		ly->keyframes->first = nkf;
		ly->keyframes->last = nkf;
	}
	else
	{
		/* insert a keyframe before or after layer's current time span */
		/* before */
		if (nkf->photogram < ly->keyframes->first->photogram)
		{
			//~ printf("Inserting keyframe kf%d before kf%d\n", nkf->photogram, ly->keyframes->first->photogram);
			nkf->next = ly->keyframes->first;
			ly->keyframes->first->prev = nkf;
			ly->keyframes->first = nkf;
		}
		/* after */
		else if (nkf->photogram > ly->keyframes->last->photogram)
		{
			//~ printf("Inserting keyframe kf%d after kf%d\n", nkf->photogram, ly->keyframes->last->photogram);
			nkf->prev = ly->keyframes->last;
			ly->keyframes->last->next = nkf;
			ly->keyframes->last = nkf;
		}
		else
		/* insert a keyframe within layer's current time span */
		{
			prevkf = ly->keyframes->first;
			nextkf = ly->keyframes->last;
			
			/* look for previous keyframe if any */
			if (ly->keyframes->first->next == NULL)
			{
				prevkf->next = nkf;
			}
			else
			{
				while (prevkf->next != NULL)
				{
					if (prevkf->next->photogram < photogram)
					{
						prevkf = prevkf->next;
						nkf->prev = prevkf;
					}
				}
			}
			/* look for next keyframe if any */
			if (ly->keyframes->last->prev == NULL)
			{
				nextkf->prev = nkf;
			}
			else
			{
				while (nextkf->prev != NULL)
				{
					if (nextkf->prev->photogram > photogram)
					{
						nkf->next = nextkf;
						nextkf = nextkf->prev;
					}
				}
			}
		}
	}
	ly->keyframes->size++;

	/* keyframe default geometrical values */
	nkf->photogram 	= photogram;
	
	nkf->x 			= kf1->x;
	nkf->y 			= kf1->y;
	nkf->ox 		= kf1->ox;
	nkf->oy 		= kf1->oy;
	nkf->w 			= kf1->w;
	nkf->h 			= kf1->h;
	nkf->centerX 	= kf1->centerX;
	nkf->centerY 	= kf1->centerY;
	nkf->ratio 		= kf1->ratio;
	nkf->oxp 		= kf1->oxp;
	nkf->oyp 		= kf1->oyp;
	nkf->wp 		= kf1->wp;
	nkf->hp 		= kf1->hp;
	nkf->rotrad 	= kf1->rotrad;
	nkf->rotdeg 	= kf1->rotdeg;
	nkf->opacity	= kf1->opacity;
	nkf->bg_color 	= strdup(kf1->bg_color);
	nkf->fg_color 	= strdup(kf1->fg_color);
	
	return nkf;
}
 
t_keyframe * 
copy_frame(int photogram, t_keyframe * kf1) 
{
	t_keyframe * kf2 = calloc(1, sizeof(*kf2));
	if (kf2 == NULL)
	{
		warning(BAD_CALLOC);
		return NULL;
	}
	/* keyframe default geometrical values */
	kf2->photogram 	= photogram;
	
	kf2->x 			= kf1->x;
	kf2->y 			= kf1->y;
	kf2->ox 		= kf1->ox;
	kf2->oy 		= kf1->oy;
	kf2->w 			= kf1->w;
	kf2->h 			= kf1->h;
	kf2->centerX 	= kf1->centerX;
	kf2->centerY 	= kf1->centerY;
	kf2->ratio 		= kf1->ratio;
	kf2->oxp 		= kf1->oxp;
	kf2->oyp 		= kf1->oyp;
	kf2->wp 		= kf1->wp;
	kf2->hp 		= kf1->hp;
	kf2->rotrad 	= kf1->rotrad;
	kf2->rotdeg 	= kf1->rotdeg;
	kf2->opacity	= kf1->opacity;
	kf2->bg_color 	= strdup(kf1->bg_color);
	kf2->fg_color 	= strdup(kf1->fg_color);
	
	return kf2;
}
 
int 
different_keyframes(t_keyframe * kf1, t_keyframe * kf2) 
{
	if (sizeof(*kf1) != sizeof(*kf2))
		return 1;
		
	/* test every value of a keyframe against other */
	if (kf1->ox != kf2->ox) return 1;
	if (kf1->oy != kf2->oy) return 1;
	if (kf1->x != kf2->x) return 1;
	if (kf1->y != kf2->y) return 1;
	if (kf1->w != kf2->w) return 1;
	if (kf1->h != kf2->h) return 1;
	if (kf1->centerX != kf2->centerX) return 1;
	if (kf1->centerY != kf2->centerY) return 1;
	if (kf1->rotdeg != kf2->rotdeg) return 1;
	if (kf1->rotrad != kf2->rotrad) return 1;
	if (kf1->opacity != kf2->opacity) return 1;
	if (kf1->bg_color != kf2->bg_color) return 1;
	if (kf1->fg_color != kf2->fg_color) return 1;
	
	return 0;
}

int 
free_keyframe(t_keyframe * kf) 
{
	free(kf);
	
	return QDCP_SUCCESS;
}

void 
show_keyframe_params(t_keyframe * kf, int verbosity) 
{
	
	//~ QDCP_LOG(LOG_INFO, "Parametres de l'image-clef :");
	
	printf("\n\t photogram : \t%d", 	kf->photogram);
	if (verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n\t prev : \t%p", 		kf->prev);
		printf("\n\t next : \t%p", 		kf->next);
		printf("\n\t ox : \t\t%d",		kf->ox);
		printf("\n\t oy : \t\t%d",		kf->oy);
	}
	printf("\n\t x : \t\t%d", 			kf->x);
	printf("\n\t y : \t\t%d", 			kf->y);
	printf("\n\t w : \t\t%d", 			kf->w);
	printf("\n\t h : \t\t%d", 			kf->h);
	printf("\n\t ratio : \t%.2f", 		kf->ratio);
	printf("\n\t opacity :\t%d",		kf->opacity);
	if (verbosity >= VERBOSE_VERBOSE)
	{
		printf("\n\t centerX : \t%d", 	kf->centerX);
		printf("\n\t centerY : \t%d", 	kf->centerY);
		printf("\n\t oxp : \t\t%.2f", 	kf->oxp);
		printf("\n\t oyp : \t\t%.2f", 	kf->oyp);
		printf("\n\t wp : \t\t%.2f", 	kf->wp);
		printf("\n\t hp : \t\t%.2f", 	kf->hp);
		printf("\n\t rotrad : \t%.2f", 	kf->rotrad);
	}
	printf("\n\t rotation : \t%.2f", 	kf->rotdeg);
	printf("\n\t bg_color : \t%s", 		kf->bg_color);
	printf("\n\t fg_color : \t%s", 		kf->fg_color);
	printf("\n");
}

/* Computing frame values for a layer in a given photogram
 */
t_keyframe	*
get_frame_values(int n, t_layer * ly) 
{
		
		t_keyframe * frame_values;
		t_keyframe * prev_kf = NULL;
		t_keyframe * next_kf = NULL;
		t_keyframe * cur_kf;
		
		int			is_keyframe = 0;

	/** loop through keyframes */
	
	cur_kf = ly->keyframes->first;
	while (cur_kf != NULL)
	{
		/** if this photogram is a keyframe of the current layer */
		if (cur_kf->photogram == n)
		{
			frame_values = cur_kf;
			prev_kf = cur_kf;
			next_kf = cur_kf;	/* this avoids to compute differences for nothing */
			
			is_keyframe = 1;
			
			/** we shall try to resize for sure */
			frame_values->resize = 1;
			/** and get ready to any modification of source image */
			frame_values->modify = 1;			
			
			frame_values->rotate = 0;
			frame_values->transparent = 0;
			frame_values->color_grades = 0;

			/** we rotate when needed */
			if (cur_kf->rotrad != 0.0 || cur_kf->rotdeg != 0.0)
				frame_values->rotate = 1;
			/** we ghost when needed */
			if (cur_kf->opacity < 100)
				frame_values->transparent = 1;
				
			break;
		}

		/** and get previous and next current layer's keyframes */

		if (cur_kf->photogram < n)
			prev_kf = cur_kf;
			
		if (cur_kf->photogram > n)
		{
			next_kf = cur_kf;
			break;
		}

		cur_kf = cur_kf->next;
	}
	
	/** get frame values */
	/* ----------------- */
	if (! is_keyframe)
	{
		frame_values = copy_frame(n, prev_kf);
		frame_values->photogram = n;
		
		frame_values->modify = 0;
		frame_values->resize = 0;
		frame_values->rotate = 0;
		frame_values->transparent = 0;
		frame_values->color_grades = 0;

		if (different_keyframes(prev_kf, next_kf))
		{
			compute_frame_values(frame_values, prev_kf, next_kf);
			frame_values->modify = 1;
			
			/* should we resize ? */
			if (prev_kf->x != next_kf->x || prev_kf->y != next_kf->y || prev_kf->w != next_kf->w || prev_kf->h != next_kf->h)
				frame_values->resize = 1;
				
			/* shall we rotate ? */
			if (prev_kf->rotrad != next_kf->rotrad || prev_kf->rotdeg != next_kf->rotdeg)
				frame_values->rotate = 1;
				
				frame_values->rotdeg = compute_frame_float_value(prev_kf->rotdeg, next_kf->rotdeg, frame_values, prev_kf, next_kf);
				frame_values->rotrad = deg2rad(frame_values->rotdeg);

			/* are we fading ? */
			if (prev_kf->opacity != next_kf->opacity)
				frame_values->opacity = compute_frame_int_value(prev_kf->opacity, next_kf->opacity, frame_values, prev_kf, next_kf);
				
			/* do we compute colors ? */
			if (prev_kf->bg_color != next_kf->bg_color)
				frame_values->bg_color = compute_frame_color_value(prev_kf->bg_color, next_kf->bg_color, frame_values, prev_kf, next_kf);
			if (prev_kf->fg_color != next_kf->fg_color)
				frame_values->fg_color = compute_frame_color_value(prev_kf->fg_color, next_kf->fg_color, frame_values, prev_kf, next_kf);
		}
	}
	
	return frame_values;
}

int 
compute_frame_values(t_keyframe * frame_values, t_keyframe * prev_kf, t_keyframe * next_kf) 
{
	/* compare each value and compute a mean when necessary */	
	
	/* ox */
	frame_values->ox = (prev_kf->ox != next_kf->ox) ? compute_frame_int_value(prev_kf->ox, next_kf->ox, frame_values, prev_kf, next_kf) : prev_kf->ox;	
	/* oy */
	frame_values->oy = (prev_kf->oy != next_kf->oy) ? compute_frame_int_value(prev_kf->oy, next_kf->oy, frame_values, prev_kf, next_kf) : prev_kf->oy;	
	/* oxp */
	frame_values->oxp = (prev_kf->oxp != next_kf->oxp) ? compute_frame_float_value(prev_kf->oxp, next_kf->oxp, frame_values, prev_kf, next_kf) : prev_kf->oxp;	
	/* oyp */
	frame_values->oyp = (prev_kf->oyp != next_kf->oyp) ? compute_frame_float_value(prev_kf->oyp, next_kf->oyp, frame_values, prev_kf, next_kf) : prev_kf->oyp;	
	/* x */
	frame_values->x = (prev_kf->x != next_kf->x) ? compute_frame_int_value(prev_kf->x, next_kf->x, frame_values, prev_kf, next_kf) : prev_kf->x;	
	/* y */
	frame_values->y = (prev_kf->y != next_kf->y) ? compute_frame_int_value(prev_kf->y, next_kf->y, frame_values, prev_kf, next_kf) : prev_kf->y;	
	/* w */
	frame_values->w = (prev_kf->w != next_kf->w) ? compute_frame_int_value(prev_kf->w, next_kf->w, frame_values, prev_kf, next_kf) : prev_kf->w;	
	/* h */
	frame_values->h = (prev_kf->h != next_kf->h) ? compute_frame_int_value(prev_kf->h, next_kf->h, frame_values, prev_kf, next_kf) : prev_kf->h;	
	/* wp (float) */
	frame_values->wp = (prev_kf->wp != next_kf->wp) ? compute_frame_float_value(prev_kf->wp, next_kf->wp, frame_values, prev_kf, next_kf) : prev_kf->wp;	
	/* hp */
	frame_values->hp = (prev_kf->hp != next_kf->hp) ? compute_frame_float_value(prev_kf->hp, next_kf->hp, frame_values, prev_kf, next_kf) : prev_kf->hp;	
	/* centerX */
	frame_values->centerX = (prev_kf->centerX != next_kf->centerX) ? compute_frame_int_value(prev_kf->centerX, next_kf->centerX, frame_values, prev_kf, next_kf) : prev_kf->centerX;	
	/* centerY */
	frame_values->centerY = (prev_kf->centerY != next_kf->centerY) ? compute_frame_int_value(prev_kf->centerY, next_kf->centerY, frame_values, prev_kf, next_kf) : prev_kf->centerY;	
	/* ratio */
	frame_values->ratio = (prev_kf->ratio != next_kf->ratio) ? compute_frame_float_value(prev_kf->ratio, next_kf->ratio, frame_values, prev_kf, next_kf) : prev_kf->ratio;	
	/* rotdeg */
	frame_values->rotdeg = (prev_kf->rotdeg != next_kf->rotdeg) ? compute_frame_float_value(prev_kf->rotdeg, next_kf->rotdeg, frame_values, prev_kf, next_kf) : prev_kf->rotdeg;	
	/* rotrad */
	frame_values->rotrad = (prev_kf->rotrad != next_kf->rotrad) ? compute_frame_float_value(prev_kf->rotrad, next_kf->rotrad, frame_values, prev_kf, next_kf) : prev_kf->rotrad;	
	/* opacity */
	frame_values->opacity = (prev_kf->opacity != next_kf->opacity) ? compute_frame_int_value(prev_kf->opacity, next_kf->opacity, frame_values, prev_kf, next_kf) : prev_kf->opacity;	
	/* bg_color */
	if (strcmp(prev_kf->bg_color, next_kf->bg_color))
	{
		frame_values->bg_color = compute_frame_color_value(prev_kf->bg_color, next_kf->bg_color, frame_values, prev_kf, next_kf);	
	}
	/* fg_color */
	frame_values->fg_color = strcmp(prev_kf->fg_color, next_kf->fg_color) ? compute_frame_color_value(prev_kf->fg_color, next_kf->fg_color, frame_values, prev_kf, next_kf) : prev_kf->fg_color;	
	
	return QDCP_SUCCESS;
}

int 
compute_frame_int_value(int prev_val, int next_val, t_keyframe * frame, t_keyframe * prev_kf, t_keyframe * next_kf) 
{
		int 	trip = next_kf->photogram - prev_kf->photogram;
		int 	pos = frame->photogram - prev_kf->photogram;
		int		diff;
		float	factor;
		int		value;
			
	diff = next_val - prev_val;
	factor = (float) diff / trip;
	value = (int) prev_val + (factor * pos);
	
	return value;
}

float 
compute_frame_float_value(float prev_val, float next_val, t_keyframe * frame, t_keyframe * prev_kf, t_keyframe * next_kf) 
{
		int 	trip = next_kf->photogram - prev_kf->photogram;
		int 	pos = frame->photogram - prev_kf->photogram;
		float	diff;
		float	factor;
		float	value;
			
	diff = next_val - prev_val;
	factor = diff / trip;
	value = prev_val + (factor * pos);
	
	return value;
}

char * 
compute_frame_color_value(char * prev_val, char * next_val, t_keyframe * frame, t_keyframe * prev_kf, t_keyframe * next_kf) 
{

		int 			trip = next_kf->photogram - prev_kf->photogram;
		int 			pos = frame->photogram - prev_kf->photogram;
		char		*	return_color;
		RGB_color	 	start_color = color_hex2rgb(prev_val);
		RGB_color	 	end_color = color_hex2rgb(next_val);
		//~ RGB_color		mean_color = calloc(1, sizeof(mean_color));
		RGB_color		mean_color;
		
		float 			red_diff 	= end_color.r - start_color.r;
		float 			green_diff	= end_color.g - start_color.g;
		float			blue_diff	= end_color.b - start_color.b;
		
	mean_color.r = start_color.r + ((red_diff / trip) * pos) ;
	mean_color.g = start_color.g + ((green_diff / trip) * pos);
	mean_color.b = start_color.b + ((blue_diff / trip) * pos);
			
	printf("\nred   : %f\n", mean_color.r); 
	printf("\ngreen : %f\n", mean_color.g); 
	printf("\nblue  : %f\n", mean_color.b);
	
	return_color = color_rgb2hex(mean_color);
	printf("\nHex : %s\n", return_color);
	
	return return_color;
}

int * 
get_dims_from_format(int * dims, char * format) 
{
	/*
	 * The three formats taken in consideration here
	 * are Flat, Scope and Full
	 */
	
	if (strcmp(format, "Flat") == 0)
	{
		dims[0] = 1998;
	}
	else
	{
		dims[0] = 2048;
	}
	if (strcmp(format, "Scope") == 0)
	{
		dims[1] = 858;
	}
	else
	{
		dims[1] = 1080;
	}
	
	return dims;
}

float 
deg2rad(float degrees) 
{
		float radians;
		
	radians = degrees * (M_PI / 180);
	
	return radians;
}

float 
rad2deg(float radians) 
{
		float degrees;
		
	degrees = radians * (180 / M_PI);
	
	return degrees;
}

xmlChar * 
ConvertInput(const char *in, const char *encoding) 
{
		xmlChar 				* 	out;
		int 						ret;
		int 						size;
		int 						out_size;
		int 						temp;
		xmlCharEncodingHandlerPtr 	handler;
	
	if (in == 0)
		return 0;
	
	handler = xmlFindCharEncodingHandler(encoding);
	
	if (!handler) {
		printf("ConvertInput : no encoding handler found for '%s'\n",
			encoding ? encoding : "");
		return 0;
	}
	
	size = (int) strlen(in) + 1;
	out_size = size * 2 -1;
	out = (unsigned char *) xmlMalloc((size_t) out_size);
	
	if (out != 0) {
		temp = size - 1;
		ret = handler->input(out, &out_size, (const xmlChar *) in, &temp);
		if (ret < 0) {
			printf("ConvertInput: conversion wasn't successful.\n");
			xmlFree(out);
			out = 0;					
		}
		
	}else {
		printf("ConvertInput: no mem\n");
	}
	
	return out;
}

/* qdp archives import and export 
 */
char * 
qdp_export(t_project * pj, quickdcp_ctx * qdcp) 
{
	
		char	ar_path[PATH_STR_LEN];
		
	/* open an archive file */
	/* push every source file or list of source files in it */
		/* images */
		/* wav */
		/* videos */
	/* push .qdx project file */
	
	/* compress the archive */
	
	/* return the archive path */
	return strdup(ar_path);
}

t_project *
qdp_import(const char * archive_path) 
{
	t_project	*	pj = NULL;
	
	/* extract the archive contents */
	/* dispatch archive contents in proper directories */
		/* images */
		/* wav files */
		/* video files */
		/* .qdx project file */
	/* load the project from .qdx file */
	
	/* in each layer, modify the source_file full path of layer's source 
	 * according to new local current workspace */
	
		
	return pj;
}

