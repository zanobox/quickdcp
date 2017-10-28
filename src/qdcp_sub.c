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
   @file    qdcp_wav.c
   @author  jean.sabatier@zanobox.net
   @brief   Build Digital Cinema Packages in command line interface.
*/
#include "qdcp_sub.h"

qdcp_bool
init_subtitles_list(t_subtitles_list * stl) 
{	
	stl = calloc(1, sizeof(*stl));
	if (stl == NULL)
	{
		warning(T_PROJECT_LIST_FAILED);
		return QDCP_FAILURE;
	}
	
	stl->size = 0;
	stl->first = NULL;
	stl->last = NULL;
	
	return QDCP_SUCCESS;
}

qdcp_bool 
init_sub_track(t_sub_track * st, quickdcp_ctx * qdcp) 
{		
	
			char 		id_string[NAME_STR_LEN];
	struct	timeval		tv;
			char 		name[NAME_STR_LEN];
	
	gettimeofday(&tv, NULL);
	sprintf(id_string, "s%d", (int)tv.tv_sec);

	sprintf(name, "%s", DEFAULT_TRACK_NAME);

	/* -- structure values -- */

	st->s_id 			= hash(id_string);
	st->prev 			= NULL;
	st->next 			= NULL;
	st->creation 		= tv.tv_sec;

	/* -- project values -- */
	
	st->name 			= strdup(name);
    st->status 			= PJ_RAW_SUBS;
	st->original		= NULL;
	st->entry_point 	= 0;
	//~ st->lock_to_layer	= 0;
	    
    /* interface params */
    st->diamond			= 1; 	// that's where the reading iterator is sitting
	st->hide			= QDCP_NO;
	
	st->duration		= 0;
	st->time_length 	= 0;
	st->time_HHMMSS 	= NULL;
	
	return QDCP_SUCCESS;
}

qdcp_bool
import_subtitle_file(t_sub_track * st, file_obj * file, quickdcp_ctx * qdcp) 
{
		char	source_file[PATH_STR_LEN];
		
	sprintf(source_file, "%s%s", file->path, file->fname);
		
	/* convert the file and get wav info into wav project */
	//~ convert_audio_2_wav(file, nw, qdcp);
	
	/* rename the wav_project after the file basename */
	st->name = file->basename;
	
	/* save the project */
	save_sub_track(st, qdcp);
	
	return QDCP_SUCCESS;
}

qdcp_bool
add_subs_to_list(t_sub_track * track, t_subtitles_list * lst) 
{
	/* insert project at the end of projects list */
	if (lst != NULL)
	{
		if (lst->last != NULL)
		{
			lst->last->next = track;
			track->prev = lst->last;
		}
		lst->last = track;
		
		if (lst->first == NULL)
		{
			lst->first = track;
		}
	}
	
	return QDCP_SUCCESS;
}

int 	
save_sub_track(t_sub_track * track, quickdcp_ctx * qdcp) 
{

		LIBXML_TEST_VERSION
	
		int 				rc;
		xmlTextWriterPtr 	writer;
		//~ xmlChar 		*	intro-utf8;
		char			*	intro;
		char 				qdp_file[PATH_STR_LEN] = {0};
	
	/* path and name of the qdp file */
	check_dir(qdcp->proj_dir, qdcp->verbosity);
	sprintf(qdp_file, "%s%ssub_%s.qdw", qdcp->proj_dir, SEPARATOR, track->name);
	
	/* begin writing XML */
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
		printf("%sSauvegarde du sous-projet de sous-titres %s.qds\n", CLI_INDENT, track->name); 
	
	/* create a writer */
	writer = xmlNewTextWriterFilename(qdp_file, 0);
	if (writer == NULL){
		warning(CANT_WRITE_XML);
		return QDCP_FALSE;
	}
	/* we want indented output */
	rc = xmlTextWriterSetIndent(writer, 1);
	
	/* start a document */
	rc = xmlTextWriterStartDocument(writer, NULL, XML_ENCODING, NULL);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	
	/* write a comment of general information */
	intro = "This .qds (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP application.\t";
	//~ intro-utf8 = ConvertInput("This .qdx (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP application.\t",
					//~ XML_ENCODING);
	//~ if (intro != NULL) { xmlFree(intro-utf8); }
	rc = xmlTextWriterWriteFormatComment(writer, "%s", intro);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* start root element */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "SUB_TRACK");
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
		
	/* PROJECT'S DEFINITION */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ID");
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* s_id */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "s_id", "%d", track->s_id);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* creation time */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "creation", "%d", track->creation);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* status */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "status", "%d", track->status);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* endElement ID */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* -- PROJECT VALUES -- */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "VALUES");
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* original */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "original", track->original);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* name */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "name", track->name);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* extension */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "ext", track->ext);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* entry_point */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "entry_point", "%d", track->entry_point);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* duration */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "duration", "%d", track->duration);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* time_length */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "time_length", "%d", track->time_length);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* time_HHMMSS */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "time_HHMMSS", track->time_HHMMSS);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* endElement VALUES */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

    /* INTERFACE PARAMS */
    
	rc = xmlTextWriterStartElement(writer, BAD_CAST "INTERFACE");
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* diamond */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "diamond", "%d", track->diamond);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	/* hide */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "hide", "%d", track->hide);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* endElement INTERFACE */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}


	/* end root element SUB_TRACK */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}

	/* close the document */
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0){ printf("save_sub_track : error at xmlTextWriterStartElement\n"); return QDCP_FALSE;}
	
	/* clean the room before leaving */
	xmlFreeTextWriter(writer);
	xmlCleanupParser();
	xmlMemoryDump();
	return 0;
}

t_sub_track * 
load_sub_track(const char * qdw_path, t_subtitles_list * wpl, quickdcp_ctx * qdcp) 
{

#ifndef WIN32
	/* we want an absolute path */
	if (qdw_path[0] != '/' && qdw_path[0] != '~')
	{
		warning(INVALID_RELATIVE_PATH);
		return NULL;
	}
#endif

	printf("Chargement du projet : %s\n", qdw_path);
	
		t_sub_track 		* 	track = calloc(1, sizeof(*track));
		t_subtitles_list 	* 	tl = calloc(1, sizeof(*tl));
		if (track == NULL || tl == NULL){ 
			warning(BAD_CALLOC);
			return NULL;
		}
	
		xmlDoc			*	qdp;
		xmlNode			*	root, *first_level, *second_level, *third_level;
		xmlNode			*	node1, *node2, *node3;
		xmlNode			*	attr, *elem;

	qdp = xmlReadFile(qdw_path, NULL, 0);
	if (qdp == NULL)
	{
		warning(CANT_READ_XML);
		return NULL;
	}
	
	root = xmlDocGetRootElement(qdp);
	first_level = root->children;

	for (node1 = first_level; node1; node1 = node1->next)
	{
		if (strcmp(node1->name, "text") != 0)
		{
			if (qdcp->verbosity >= VERBOSE_VERBOSE)
				printf("%s\n", node1->name);
			second_level = node1->children;
			for (node2 = second_level; node2; node2 = node2->next)
			{
				if (strcmp(node2->name, "text") != 0)
				/* get track's values */
				
					/* track's attributes */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  s_id = %s\n", xmlGetProp(node2, "s_id"));
					track->s_id = strtoimax(xmlGetProp(node2, "s_id"), NULL, 10);
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  name = %s\n", xmlGetProp(node2, "name"));
					track->name = xmlGetProp(node2, "name");
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  creation  = %s\n", xmlGetProp(node2, "creation"));
					track->creation  = strtoimax(xmlGetProp(node2, "creation"), NULL, 10);

					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("    %s :  ", node2->name);
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("%s\n", xmlNodeListGetString(qdp, node2->children, 1));

					/* original */
					if (strcmp(node2->name, "original") == 0)
						track->original = xmlNodeListGetString(qdp, node2->children, 1);
					/* ext */
					if (strcmp(node2->name, "ext") == 0)
						track->ext = xmlNodeListGetString(qdp, node2->children, 1);

					/* entry_point */
					if (strcmp(node2->name, "entry_point") == 0)
						track->entry_point = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* duration */
					if (strcmp(node2->name, "duration") == 0)
						track->duration = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* time_length */
					if (strcmp(node2->name, "time_length") == 0)
						track->time_length = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* time_HHMMSS */
					if (strcmp(node2->name, "time_HHMMSS") == 0)
						track->time_HHMMSS = xmlNodeListGetString(qdp, node2->children, 1);

					/* diamond */
					if (strcmp(node2->name, "diamond") == 0)
						track->diamond = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* mute */
					if (strcmp(node2->name, "hide") == 0)
						track->hide = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
			}
		}
	}
}

t_subtitles_list * 
scan_sub_projects(quickdcp_ctx * qdcp) {

			int						i = 0, index = 0;
			DIR	   				*	qdp_dir;
	struct  dirent 				*	entry;
	struct  stat					statbuf;
			t_sub_track			*	previous = NULL;
			t_sub_track			*	current = NULL;
			t_sub_track			*	next = NULL;
			char					ref_str[] = "wav_";
			char					chk_str[TINY_STR_LEN] = {0};

			t_subtitles_list 	* 	wpl = calloc(1, sizeof(*wpl));

	if (wpl == NULL)
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
		
	wpl->size = 0;
	wpl->first = NULL;
	wpl->last = NULL;
	
	//~ qdcp->wav_dir
	
		//~ filelist->files[x]  = malloc(MAX_FILENAME_LENGTH * sizeof(char));

						
	if ((qdp_dir = opendir(qdcp->proj_dir)) == NULL)
	{
		closedir(qdp_dir);
		perror("opendir");
			
		error_msg(BAD_PATH, __FILE__, __LINE__);	
	}
	
	/* Use mime (of enum mime_types) as a filter for output files */
	
	/* begin scanning */
	
	int cwd = open(".", O_RDONLY);
	chdir(qdcp->proj_dir);

	while ((entry = readdir(qdp_dir)) != NULL)
	{		
		lstat(entry->d_name, &statbuf);
		if (! S_ISDIR(statbuf.st_mode))
		{
			/* mime type filter here */
			strncpy(chk_str, entry->d_name, strlen(ref_str));
				
			if (strcmp(chk_str, ref_str) == 0)
			{
				/* check also that we dont have a DCP project (not wav)
				 * called wav_something.qdx */
				 
				 
				/* Store file information */
					
				init_sub_track(current, qdcp);
				
				/* increment list size */
				wpl->size++;
				
				if (current->prev != NULL)
				{
					current->prev->next = current;
				}
				if (current->next != NULL)
				{
					current->next->prev = current;
				}
				
				/**
				 * If first file in the list
				 */		
				if (index == 1){
					wpl->first = current;
				}
				if (current->next == NULL)
				{
					wpl->last = current;
				}
			}
		}
	}
	fchdir(cwd);
	closedir(qdp_dir);
	close(cwd);

	/* sort files alphabetically */
	//~ src_list = sort_list(src_list);
	
	/* print out the sorted list */
	current = wpl->first;
	i = 0;
	while (current != NULL)
	{
		i++;
		current->index = i;
		printf(" %d  >\t %s\n", current->index, current->name);
		current = current->next;
	}
	
	return wpl;
}

int 
add_subs_to_project(t_sub_track * track, t_project * pj, quickdcp_ctx * qdcp) {
	
	/* adjust track parameters according to main project values */
	
	/* mix the track according to projects parameters */
	
	/* add the track struct to project's track list */
	
	
		
	return EXIT_SUCCESS;
}

