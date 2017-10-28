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
#include "qdcp_wav.h"

/*--------------------------- QuickDCP API ---------------------------*/

qdcp_bool
init_track_list(t_track_list * wl) 
{
	wl = calloc(1, sizeof(*wl));
	
	if (wl == NULL)
	{
		warning(BAD_CALLOC);
		return QDCP_FAILURE;
	}
	
	wl->size = 0;
	wl->first = NULL;
	wl->last = NULL;
	
	return QDCP_SUCCESS;
}

qdcp_bool 
init_wav_track(t_wav_track  *	wp, quickdcp_ctx * qdcp) {

	wp = calloc(1, sizeof(*wp));
	if (wp == NULL) {
		warning(BAD_CALLOC);
		return QDCP_FAILURE;
	}
	
			char 			id_string[NAME_STR_LEN];
	struct	timeval			tv;
	
	gettimeofday(&tv, NULL);
	sprintf(id_string, "w%d", (int)tv.tv_sec);

	/* -- structure values -- */
	wp->w_id 				= hash(id_string);
	wp->status				= TRACK_UNCHECKED;
	wp->prev 				= NULL;
	wp->next 				= NULL;
	wp->creation 			= tv.tv_sec;

	/* -- project values -- */
	sprintf(wp->basename, "%s", DEFAULT_TRACK_NAME);
	sprintf(wp->filename, "%s", "[none]");
	sprintf(wp->ext, 	  "%s", "[none]");
	sprintf(wp->original, "%s", "[none]");
    wp->status 				= PJ_UNMIXED_WAV;
	wp->entry_point 		= 0;
	wp->lock_to_layer		= 0;
	    
    /* interface params */
    wp->diamond				= 1; 	// that's the (video) frame where the audio reading iterator is sitting
	wp->mute				= QDCP_NO;

	wp->volume				= qdcp->cfg->volume;		/**< @todo to be clarified... */
	wp->time_length 		= 0;
	sprintf(wp->time_HHMMSS, "");
	wp->annotations 		= "";
	
	/* audio source info */
	wp->src_channels 		= 0;
	wp->src_samples			= 0;
	wp->src_samplerate 		= 0;
	wp->src_sample_fmt		= AV_SAMPLE_FMT_NONE;
	wp->src_bitdepth		= 0;
	wp->src_format_ctx		= NULL;	
	wp->src_stream_index	= 0;						/**< stream index from FFmpeg */
	wp->src_stream_index	= 0;						/**< stream index from FFmpeg */
	wp->src_audio_stream	= NULL;						/**< A pointer to an AVStream struct from FFmpeg */
	wp->src_codec_ctx		= NULL;						/**< A pointer to an AVCodecContext struct from FFmpeg */
	wp->src_codec			= NULL;						/**< A pointer to an AVCodec struct from FFmpeg*/
	wp->src_codec_params	= NULL;						/**< A pointer to an ACodecParameters struct from FFmpeg */
	
	/* target values */
	wp->encoding			= AUD_ENC_UNSET;			/**< current audio encoding */
	wp->mix_type			= qdcp->cfg->mix_type;		/**< One of Repartition, Mixage, Inversion */

	wp->out_channels 		= qdcp->cfg->audio_format;	/**< number of channels of the output audio. Can be one of CH_MONO, CH_STEREO, CH_5_1, CH_7_1 (see enum channel_set) */
	wp->out_channel_layout	= qdcp->cfg->channel_layout;
	wp->out_samples			= 0;						/**< total number of samples */
	wp->out_samplerate		= qdcp->cfg->samplerate;	/**< Targeted rate in samples per second (48000 or 96000/s). See enum accepted_samplerates. By default, will be KHZ_48. */
	wp->out_sample_fmt		= qdcp->cfg->sample_fmt;
	wp->out_bitdepth		= qdcp->cfg->bitdepth;		/**< @todo To clarify. Bit rate used in audio conversion. */
	wp->out_codec_id		= qdcp->cfg->codec_id;		/**< Encoder FFmpeg Codec ID from AVCodecID enumeration, as defined in avcodec.h. Should be AV_CODEC_ID_PCM_S24DAUD */

	wp->out_format_ctx		= NULL;
	wp->out_stream_index	= 0;						/**< stream index from FFmpeg */
	wp->out_stream_index	= 0;						/**< stream index from FFmpeg */
	wp->out_audio_stream	= NULL;						/**< A pointer to an AVStream struct from FFmpeg */
	wp->out_codec_ctx		= NULL;						/**< A pointer to an AVCodecContext struct from FFmpeg */
	wp->out_codec			= NULL;						/**< A pointer to an AVCodec struct from FFmpeg*/
	wp->out_codec_params	= NULL;						/**< A pointer to an ACodecParameters struct from FFmpeg */

	return QDCP_SUCCESS;
}

qdcp_bool
add_wav_to_list(t_wav_track * track, t_track_list * wpjl) 
{

	/* insert a wav sub-project at the end of project's track list */
	if (wpjl != NULL)
	{	
		/* if list was empty */
		if (wpjl->first == NULL)
		{
			wpjl->first = track;
		}
		/* if list was not empty */
		if (wpjl->last != NULL)
		{
			wpjl->last->next = track;
			track->prev = wpjl->last;
		}
		/* in any case */
		wpjl->last = track;
		wpjl->size++;
	}
	
	return QDCP_SUCCESS;
}

qdcp_bool
save_wav_subproject(t_wav_track * track, quickdcp_ctx * qdcp) 
{
		LIBXML_TEST_VERSION
	
		int 				rc;
		xmlTextWriterPtr 	writer;
		//~ xmlChar 		*	intro-utf8;
		char			*	intro;
		char 				qdp_file[PATH_STR_LEN] = {0};
	
	/* path and name of the qdp file */
	check_dir(qdcp->proj_dir, qdcp->verbosity);
	sprintf(qdp_file, "%s%swav_%s.qdw", qdcp->proj_dir, SEPARATOR, track->basename);
	
	/* begin writing XML */
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
		printf("%sSauvegarde du sous-projet audio %s\n", CLI_INDENT, qdp_file); 
	
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
	if (rc < 0){ printf("save_wav_track : error at xmlTextWriterStartDocment\n"); return QDCP_FALSE;}
	
	/* write a comment of general information */
	intro = "This .qdx (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP application.\t";
	//~ intro-utf8 = ConvertInput("This .qdx (XML) file is part of a .qdp file (archive),\t\t-->\n<!--to be read as a project file by QuickDCP application.\t",
					//~ XML_ENCODING);
	//~ if (intro != NULL) { xmlFree(intro-utf8); }
	rc = xmlTextWriterWriteFormatComment(writer, "%s", intro);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* start root element */
	rc = xmlTextWriterStartElement(writer, BAD_CAST "WAV_TRACK");
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
		
	/* PROJECT'S DEFINITION */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ID");
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* w_id */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "w_id", "%d", track->w_id);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* creation time */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "creation", "%d", track->creation);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* status */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "status", "%d", track->status);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* endElement ID */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}


	if (save_wav_track(writer, track, qdcp) == QDCP_FAILURE)
	{
		/* clean the room before leaving */
		xmlFreeTextWriter(writer);
		xmlCleanupParser();
		xmlMemoryDump();
		return QDCP_FAILURE;
	}


	/* end root element WAV_TRACK */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* close the document */
	rc = xmlTextWriterEndDocument(writer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	
	/* clean the room before leaving */
	xmlFreeTextWriter(writer);
	xmlCleanupParser();
	xmlMemoryDump();
	
	return QDCP_SUCCESS;
}

qdcp_bool
save_wav_track(xmlTextWriterPtr writer, t_wav_track * track, quickdcp_ctx * qdcp) 
{
		int 				rc;

	/* -- PROJECT VALUES -- */
	
	rc = xmlTextWriterStartElement(writer, BAD_CAST "VALUES");
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* original */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "original", track->original);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* filename */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "filename", track->filename);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* basename */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "basename", track->basename);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* extension */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "ext", track->ext);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* entry_point (int) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "entry_point", "%d", track->entry_point);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* duration (int) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "duration", "%d", track->duration);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* time_length */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "time_length", "%d", track->time_length);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* time_HHMMSS */
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "time_HHMMSS", track->time_HHMMSS);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* AUDIO SOURCE INFO */
	
	/* volume */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "volume", "%.6d", track->volume);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* mix_type */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mix_type", "%d", track->mix_type);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* encoding */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "encoding", "%d", track->encoding);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* annotations */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "annotations", "%s", track->annotations);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* src_codec_id */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_codec_id", "%d", track->src_codec_id);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* number of channels */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_channels", "%d", track->src_channels);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* src_samples */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_samples", "%d", track->src_samples);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* src_samplerate */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_samplerate", "%f", track->src_samplerate);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* src_bitrate */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_bitrate", "%d", track->src_bitrate);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* src_bitdepth (bits per sample) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "src_bitdepth", "%d", track->src_bitdepth);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* out_codec_id */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_codec_id", "%d", track->out_codec_id);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* out_channels */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_channels", "%d", track->out_channels);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* out_samples */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_samples", "%d", track->out_samples);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* out_samplerate */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_samplerate", "%f", track->out_samplerate);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* out_bitrate */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_bitrate", "%d", track->out_bitrate);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* out_bitdepth (bits per sample) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "out_bitdepth", "%d", track->out_bitdepth);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* endElement VALUES */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

    /* INTERFACE PARAMS */
    
	rc = xmlTextWriterStartElement(writer, BAD_CAST "INTERFACE");
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	/* diamond */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "diamond", "%d", track->diamond);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* mute */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mute", "%d", track->mute);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}
	/* lock_to_layer (int) */
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "lock_to_layer", "%d", track->lock_to_layer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return 1;}

	/* endElement INTERFACE */
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0) { printf("save_wav_track : Error in %s at line %d\n", __FILE__, __LINE__); return QDCP_FAILURE;}

	return QDCP_SUCCESS;
}

t_wav_track * 
load_wav_subproject(const char * qdw_path, t_track_list * wpl, quickdcp_ctx * qdcp) 
{
}

t_wav_track * 
load_wav_track(const char * qdw_path, t_track_list * wpl, quickdcp_ctx * qdcp) 
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
	
		t_wav_track 	* 	track = calloc(1, sizeof(*track));
		t_track_list 	* 	tl = calloc(1, sizeof(*tl));
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
						printf("  w_id = %s\n", xmlGetProp(node2, "w_id"));
					track->w_id = strtoimax(xmlGetProp(node2, "w_id"), NULL, 10);
					/* filename */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  filename = %s\n", xmlGetProp(node2, "filename"));
					sprintf(track->filename, "%s", xmlGetProp(node2, "filename"));
					/* basename */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  basename = %s\n", xmlGetProp(node2, "basename"));
					sprintf(track->basename, "%s", xmlGetProp(node2, "basename"));
					/* creation */
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("  creation  = %s\n", xmlGetProp(node2, "creation"));
					track->creation  = strtoimax(xmlGetProp(node2, "creation"), NULL, 10);

					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("    %s :  ", node2->name);
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
						printf("%s\n", xmlNodeListGetString(qdp, node2->children, 1));

					/* original */
					if (strcmp(node2->name, "original") == 0)
						sprintf(track->original, "%s", xmlNodeListGetString(qdp, node2->children, 1));
					/* ext */
					if (strcmp(node2->name, "ext") == 0)
						sprintf(track->ext, "%s", xmlNodeListGetString(qdp, node2->children, 1));

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
						sprintf(track->time_HHMMSS, "%.10s", xmlNodeListGetString(qdp, node2->children, 1));

					/* mix_type */
					if (strcmp(node2->name, "mix_type") == 0)
						track->mix_type = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* src_samples */
					if (strcmp(node2->name, "src_samples") == 0)
						track->src_samples = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* src_channels */
					if (strcmp(node2->name, "src_channels") == 0)
						track->src_channels = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* src_bitdepth (bits per sample) */
					if (strcmp(node2->name, "src_bitdepth") == 0)
						track->src_bitdepth = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* volume */
					if (strcmp(node2->name, "volume") == 0)
						track->volume = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* src_samplerate */
					if (strcmp(node2->name, "src_samplerate") == 0)
						track->src_samplerate = strtof(xmlNodeListGetString(qdp, node2->children, 1), NULL);
					/* src_codec_id */
					if (strcmp(node2->name, "src_codec_id") == 0)
						track->src_codec_id = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* encoding */
					if (strcmp(node2->name, "encoding") == 0)
						track->encoding = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* annotations */
					if (strcmp(node2->name, "annotations") == 0)
						track->annotations = xmlNodeListGetString(qdp, node2->children, 1);

					/* diamond */
					if (strcmp(node2->name, "diamond") == 0)
						track->diamond = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
					/* mute */
					if (strcmp(node2->name, "mute") == 0)
						track->mute = strtoimax(xmlNodeListGetString(qdp, node2->children, 1), NULL, 10);
			}
		}
	}
}

t_track_list * 
scan_wav_projects(quickdcp_ctx * qdcp) 
{

			int						i = 0, index = 0;
			DIR	   				*	qdp_dir;
	struct  dirent 				*	entry;
	struct  stat					statbuf;
			t_wav_track			*	previous = NULL;
			t_wav_track			*	current = NULL;
			t_wav_track			*	next = NULL;
			char					ref_str[] = "wav_";
			char					chk_str[TINY_STR_LEN] = {0};

			t_track_list 	* 	wpl = calloc(1, sizeof(*wpl));

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
				init_wav_track(current, qdcp);
				
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
		printf(" %d  >\t %s\n", current->index, current->filename);
		current = current->next;
	}
	
	return wpl;
}

qdcp_bool
add_track_to_project(t_wav_track * track, t_project * pj, quickdcp_ctx * qdcp) 
{
		int 	w_duration;
		char	filepath[PATH_STR_LEN];
	
	sprintf(filepath, "%s%s", track->wav_path, track->filename);
	w_duration = get_wav_duration(filepath, pj->framerate);
	
	if (qdcp->verbosity >= VERBOSE_NORMAL)
		printf("%s%sRecalibrage de la durée du projet : %d frames%s\n", 
			CLI_INDENT, CLI_GREEN, w_duration, CLI_COLOR);
			
	/* adjust project's duration to wav duration */
	if (pj->nmax < w_duration)
		pj->nmax = w_duration;
		pj->laps = (float) w_duration / pj->framerate;
		
	/* adjust track parameters according to main project values */
	
	/* mix the track according to projects parameters */
	
	/* add the track struct to project's track list */
	add_wav_to_list(track, pj->tracks);
	
	if(qdcp->verbosity >= VERBOSE_NORMAL)
		printf("%s%s%sNombre de pistes son du projet : %d%s\n",
			CLI_INDENT, CLI_GREEN, CLI_INDENT, pj->tracks->size, CLI_COLOR);
		
	return QDCP_SUCCESS;
}

qdcp_bool
import_audio_file(t_wav_track * nw, char * input_file, quickdcp_ctx * qdcp) {

		char		err_buffer[80];
	
	/* if we don't have a path, we have to be in the workspace */
	if (strcmp(get_path_from_file(input_file), "") == 0)
	{
		sprintf(nw->filename,	"%s", 	input_file);
		sprintf(nw->basename,	"%s", 	get_basename(input_file));
		sprintf(nw->ext,		"%s", 	get_filename_ext(input_file));
		sprintf(nw->wav_path, 	"%s", 	qdcp->workspace);
		sprintf(nw->original,	"%s%s",	qdcp->workspace, input_file);
	}
	else
	{
		/* rename the wav_project after the file basename */
		sprintf(nw->filename, 	"%s", 	strip_path(input_file));
		sprintf(nw->basename, 	"%s", 	get_basename(nw->filename));
		sprintf(nw->ext,		"%s", 	get_filename_ext(nw->filename));
		sprintf(nw->wav_path, 	"%s", 	get_path_from_file(input_file));
		sprintf(nw->original, 	"%s", 	input_file);
	}
		
	/* Does the input file even exist ? */
	if (check_file(nw->original) == QDCP_NO)
	{
		sprintf(err_buffer, "File does not exist : %s", input_file);
		warning(err_buffer);
		return QDCP_FAILURE;
	}
	
	/* if the file to import is not in our workspace, we get it in */
	if (strcmp(nw->wav_path, qdcp->workspace) != 0)
	{
		if (qdcp->verbosity >= VERBOSE_NORMAL)
			printf("%s%sImportation du fichier audio %s%s\n", 
						CLI_INDENT, CLI_GREEN, nw->filename, CLI_COLOR);

			char	home_file[PATH_STR_LEN];

		sprintf(home_file, 	 "%s%s", qdcp->workspace, nw->filename);
		
		if (cp(nw->original, home_file) != 0)
		{	
			sprintf(nw->original, "%s", home_file);
			sprintf(nw->wav_path, "%s", qdcp->workspace);
		}
		else
		{
			warning("Couldn't copy an audio file");
		}
	}
	
	return QDCP_SUCCESS;
}


qdcp_bool
render_audio(t_project * pj, quickdcp_ctx * qdcp) 
{
	
		char	final_track[PATH_STR_LEN];

	printf("%sRendering audio : Not implemented yet\n");

	if (pj->tracks->size > 1)
	{
			
		/* loop project's tracks */
		/* merge tracks if they don't overlap */
		/* mix (i.e. blend) overlapping tracks */
		/* we mix channel by channel, all files are supposed to have
		 * been pre-mixed with the same number of channels */
		/* the result's duration must match the project's pj->nmax */
		
		/* now we should have only one full track for the whole project */
		/* mix it according to project's audio parameters */
		
	}
	else
	{
		/* just adjust the main audio track and rename it after the project */
			char src_file[PATH_STR_LEN];
			char trgt_file[PATH_STR_LEN];
		sprintf(src_file, "%s%s.wav", pj->wav_path, pj->tracks->first->basename);
		sprintf(trgt_file, "%s%s.wav", pj->wav_path, pj->name);
		rename(src_file, trgt_file);
	}
	/* then we can go for mxf packing */
	
	return QDCP_SUCCESS;
}


/* Global timestamp for the audio frames */
int64_t pts = 0;

qdcp_bool
read_audio_input(t_wav_track * track, int verbosity)
{
		int					error;
		int					i;
		
		AVFormatContext *	format_ctx = avformat_alloc_context();

		AVStream		*	stream;
		AVCodec			*	codec;
		
		AVRational 			r_frame_rate;
	
	/* open the file so we can read from it */
    error = avformat_open_input(&format_ctx, track->original, NULL, NULL); 
    if (error < 0){
        warning(get_ffmpeg_error(error));
		fprintf(stderr, "track->original : %s", track->original);
        return QDCP_FAILURE;
    }
    
    track->src_format_ctx = format_ctx;
    
	/* get information about streams */
	error = avformat_find_stream_info(track->src_format_ctx, NULL);
	if (error < 0) {
		fprintf(stderr, "%s", "Couldn't get stream info");
		fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sAcquisistion de données sur la source%s\n", 
					CLI_INDENT, CLI_GREEN, CLI_COLOR);

	if (verbosity >= VERBOSE_VERBOSE)
	{
		printf("%sSortie de av_dump_format() pour le flux #%d: %s\n", 
				CLI_INDENT, 0, CLI_BOLD_YELLOW);    
		av_dump_format(format_ctx, 0, track->original, 0);
		printf("%s\n", CLI_COLOR);
	}

    
    /* looping through input streams */
    for (i = 0; i < track->src_format_ctx->nb_streams; i++) {

			AVStream 	* stream;
			AVFrame		* frame;

        stream = track->src_format_ctx->streams[i];
        
		/* AVStream members */
		
		/* if we have the audio stream we're looking for */
		
        if (stream->codecpar->codec_type == AVMEDIA_TYPE_AUDIO) 
        {
			/* source codec_id */
			track->src_codec_id = stream->codecpar->codec_id;

			/* source codec */
			
			/* decoder */
			if (!(track->src_codec = avcodec_find_decoder(stream->codecpar->codec_id))) {
				fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
				return QDCP_FAILURE;
			}
			/* codec context */
			track->src_codec_ctx = avcodec_alloc_context3(track->src_codec);
			if (! track->src_codec_ctx) {
				fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
				return QDCP_FAILURE;
			}
			
			/* source codec parameters and format context */
			
			/* duration in seconds */
			track->time_length = track->src_format_ctx->duration;

			/* source number of channels */
			track->src_channels = stream->codecpar->channels;
			
			/* source channel_layout (from select_channel_layout() ) */
			track->src_channel_layout = select_channel_layout(track->src_codec);

			/* source sample format */
			track->src_sample_fmt = stream->codecpar->format;

			/* bits per coded sample */
			track->src_bitdepth = stream->codecpar->bits_per_coded_sample;
											
			/* source sample rate */
			track->src_samplerate = stream->codecpar->sample_rate;

			/* source frame size */
			track->src_framesize = stream->codecpar->frame_size;
			
			if (verbosity >= VERBOSE_VERBOSE)
				printf("%s%sFrames already demuxed : \t%d\n", CLI_INDENT, CLI_INDENT, 
					stream->codec_info_nb_frames); 
			
			printf("Stream->Nb_frames : %d\n",  stream->nb_frames);
        }
        

		/* other more problematic values */


		//~ r_frame_rate = av_stream_get_r_frame_rate(stream);
        //~ printf("Stream->Framerate : %d\n", r_frame_rate.num);
        //~ printf("Stream->Framerate : %d\n", r_frame_rate.den);
        
        //~ AVRational avg_frame_rate = av_guess_frame_rate(format_ctx, stream, frame);
        //~ printf("Stream->Framerate : %d\n", avg_frame_rate.num);
        //~ printf("Stream->Framerate : %d\n", avg_frame_rate.den);
        //~ 
        //~ printf("Stream->sample_aspect_ratio.num : %d\n", stream->sample_aspect_ratio.num);
        //~ printf("Stream->sample_aspect_ratio.den : %d\n", stream->sample_aspect_ratio.den);

		/* says that codecpar does'nt have a time_base member... */
        //~ printf("Codecpar->time_base.num : %d\n", stream->codecpar->time_base.num);
        //~ printf("Codecpar->time_base.den : %d\n", stream->codecpar->time_base.den);

        
	} /* looping streams */
	   		
	/* source codec_id */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource codec id : \t%d\n", 
		CLI_INDENT, CLI_INDENT, track->src_codec_id);
	
	/* duration in seconds */
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sSource - duration : \t%.2fs\n", 
		CLI_INDENT, CLI_INDENT, (float) (track->time_length) / 1000000 );

	/* source number of channels */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource - canaux : \t%d\n", 
		CLI_INDENT, CLI_INDENT, track->src_channels);
	
	/* source channel_layout (from select_channel_layout() ) */
	if (verbosity >= VERBOSE_NORMAL)
	{
		char buff[20];
		av_get_channel_layout_string(buff, 20, track->src_channels, track->src_channel_layout);
		printf("%s%sSource - canaux : \t%s\n", 
		CLI_INDENT, CLI_INDENT, buff);
	}
	
	/* source sample format */
		char samplefmtstr[10];
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sSource - sample format : \t%s\n", 
		CLI_INDENT, CLI_INDENT, 
		av_get_sample_fmt_string(samplefmtstr, 10, track->src_sample_fmt));

	/* bits per coded sample */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource bits per coded sample : \t%d\n", 
		CLI_INDENT, CLI_INDENT, 
		track->src_bitdepth);
					
	/* number of streams in the media file */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource nombre de streams : \t%d\n",	
		CLI_INDENT, CLI_INDENT, track->src_format_ctx->nb_streams);
			
	/* bit rate (in kb/s) */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource - bit rate: \t%.3f kb/s\n",
		CLI_INDENT, CLI_INDENT, (float) (track->src_format_ctx->bit_rate) / 1000);

	/* source sample rate */
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sSource - sample rate : \t%d Hz\n", 
		CLI_INDENT, CLI_INDENT, track->src_samplerate);

	/* source frame size */
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sSource - frame_size : \t%d bits\n", 
		CLI_INDENT, CLI_INDENT, track->src_framesize);

	/* this gives 0 on tested wav files */
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sSource packet_size : \t%d\n",
		CLI_INDENT, CLI_INDENT, track->src_format_ctx->packet_size);


	return QDCP_SUCCESS;
}

/* open the output file so we can write into it */
qdcp_bool
open_output_file(t_wav_track * track, int verbosity)
{
			int 				error, frame_size, samples;
			AVFormatContext *	out_format_ctx = NULL;
			AVIOContext 	*	output_io_context = NULL;	
			AVCodec			*	out_codec = NULL;
			AVStream		*	out_audio_stream = NULL;
	
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sParamètrage de la cible%s\n", 
				CLI_INDENT, CLI_GREEN, CLI_COLOR);

	
	/* allocate the format_context for the output container */
	
	avformat_alloc_output_context2(&out_format_ctx, NULL, NULL, track->mixed_file);
    if (!out_format_ctx) {
        fprintf(stderr, "Could not create output context\n");
        error = AVERROR_UNKNOWN;
        return QDCP_FAILURE;
    }
    track->out_format_ctx = out_format_ctx;
    
	/* open the output file handle */
	
    error = avio_open(&output_io_context, track->mixed_file, AVIO_FLAG_WRITE);
    if (error < 0) {
        warning("Cannot open the file's output_io_context");
		fprintf(stderr, "Target output file : %s\n", track->mixed_file);
        return QDCP_FAILURE;
    }

    
	/* find the audio encoding codec (among registered encoders : see init_ffmpeg_api() ) */
	/* most probably, it is PCM D-Cinema audio signed 24-bit (referenced in FFmpeg enum AVCodecID as AV_CODEC_ID_PCM_S24DAUD */
	 
    track->out_codec = avcodec_find_encoder(track->out_codec_id);
    if (! track->out_codec){
        warning("Could not get encoder for the output codec");
		fprintf(stderr, "%s", "Out codec_id : %d", track->out_codec_id);
		return QDCP_FAILURE;
	}

	
	/* create a new stream */
	
	/* thanks to out_codec, the stream is initialized with corresponding 
	 * default codec_context */	
    out_audio_stream = avformat_new_stream(track->out_format_ctx, track->out_codec);
    if (out_audio_stream == NULL){
        warning("Could not create a new audio stream");
        return QDCP_FAILURE;
	}

	/* allocate a codec context */
	
    track->out_codec_ctx = avcodec_alloc_context3(track->out_codec);
    if (track->out_codec_ctx == NULL) {
        warning("Could not alloc context for the given output codec");
		fprintf(stderr, "%s", "Out codec_id : %d", track->out_codec_id);
        return QDCP_FAILURE;
	}
		
	/* so we can get our codec_paramaters */
    if (avcodec_parameters_from_context(
				out_audio_stream->codecpar, track->out_codec_ctx) < 0) 
    {
        fprintf(stderr, "Could not initialize stream parameters\n");
		return QDCP_FAILURE;
    }


	/* number of channels */
	track->out_codec_ctx->channels = track->out_channels;
	if (verbosity >= VERBOSE_VERBOSE)
		printf("%s%sTarget - number of channels : \t%d\n", 
				CLI_INDENT, CLI_INDENT, track->out_codec_ctx->channels);
	
	/* channel layout */
	track->out_codec_ctx->channel_layout = track->out_channel_layout;
	if (verbosity >= VERBOSE_NORMAL)
	{
		char buff[20];
		av_get_channel_layout_string(buff, 40, track->out_channels, track->out_channel_layout);
		printf("%s%sTarget - canaux : \t%s\n", 
		CLI_INDENT, CLI_INDENT, buff);
	}
	
	/* sample format */
	track->out_codec_ctx->sample_fmt = track->out_sample_fmt; //AV_SAMPLE_FMT_S32;
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sTarget - sample format : \t%s\n", 
				CLI_INDENT, CLI_INDENT, av_get_sample_fmt_name(track->out_sample_fmt));
	
	/* bit per sample */
	track->out_codec_ctx->bits_per_raw_sample = 24;
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sTarget - bits per raw sample : %d\n", 
				CLI_INDENT, CLI_INDENT, track->out_codec_ctx->bits_per_raw_sample);

	/* sample rate */
	track->out_codec_ctx->sample_rate = track->out_samplerate;
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sTarget - sample rate : \t%d Hz\n", 
				CLI_INDENT, CLI_INDENT, track->out_codec_ctx->sample_rate);

	/* bit rate */
	track->out_codec_ctx->bit_rate = track->out_bitrate;
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sTarget - bit rate : \t%d\n", 
				CLI_INDENT, CLI_INDENT, track->out_codec_ctx->bit_rate);

	/* frame size */
	/* bits per sample * number of channels */
	track->out_codec_ctx->frame_size = track->out_codec_ctx->bits_per_raw_sample * track->out_codec_ctx->channels;
	if (verbosity >= VERBOSE_NORMAL)
		printf("%s%sTarget - frame size : \t%d\n", 
				CLI_INDENT, CLI_INDENT, track->out_codec_ctx->frame_size);

	
    /* Set the sample rate for the container. */
    //~ track->out_audio_stream->time_base.den = track->in_codec_ctx->sample_rate;
    //~ track->out_audio_stream->time_base.num = 1;
    
    track->out_audio_stream = out_audio_stream;
    track->out_samplerate	= track->out_codec_ctx->sample_rate;
    
	/* Open the encoder for the audio stream to use it later. */
    if ((error = avcodec_open2(track->out_codec_ctx, track->out_codec, NULL)) < 0) {
        fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
        return QDCP_FAILURE;
    }

    /* associate the output file (pointer) with the container format context */
    track->out_format_ctx->pb = output_io_context;

	/* this provokes a segfault : */
    //~ av_dump_format(track->out_format_ctx, 0, track->mixed_file, 1);
        
	return QDCP_SUCCESS;
}

qdcp_bool
convert_audio_2_wav(t_wav_track * track, quickdcp_ctx * qdcp) 
{
			int					error, i;
			
	/* format the output file path and name */
	sprintf(track->mixed_file, "%s%s.wav", qdcp->wav_dir, track->basename);

	if (qdcp->verbosity >= VERBOSE_NORMAL)
		printf("%s%sConversion et mixage audio pour %s%s\n", 
				CLI_INDENT, CLI_GREEN, track->basename, CLI_COLOR);
		
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("%s%s   Conversion de : %s%s \n   %svers %s%s%s \n", 
			CLI_INDENT, CLI_GREEN, CLI_CYAN, track->original, 
			CLI_GREEN, CLI_CYAN, track->mixed_file, CLI_COLOR);
	}
	
	check_dir(qdcp->wav_dir, qdcp->verbosity);

	
	/*--------------------- Preparing the decoder --------------------*/
	
	/* When decoding, we take an audio file and feed it to a bitstream */
	
	/* so we first get some basic info about the given file 
	 * and store it into our t_wav_track struct */
	read_audio_input(track, qdcp->verbosity);
	
	/*--------------------- Preparing the encoder --------------------*/
	
	/* When encoding, we receive a bitstream and stuff it, through a buffer, into a file. */
	
	/* default data needed is supposedly already passed to the track on init_wav_track() */
	open_output_file(track, qdcp->verbosity);
	
	/*-------------------- Preparing the resampler -------------------*/

			SwrContext 		*	resample_context = NULL;
			AVAudioFifo 	*	fifo = NULL;
    
	 /* Initialize the resampler to be able to convert audio sample formats. */
	init_resampler(track, &resample_context, qdcp->verbosity);
	
	/* Initialize the FIFO buffer to store audio samples to be encoded. */
	init_fifo(&fifo, track->out_codec_ctx);

	/* Write the header of the output file container. */
	write_output_file_header(track->out_format_ctx);
	
	printf("%sWrote output file header\n", CLI_INDENT);
	
			float				t, tincr;
			short 			* 	samples;
			uint8_t 		*	outbuf;
				
	/*
	* Loop as long as we have input samples to read or output samples
	* to write; abort as soon as we have neither.
	*/
	while (1) {

			int finished = 0;
		/*
		* Make sure that there is one frame worth of samples in the FIFO
		* buffer so that the encoder can do its work.
		* Since the decoder's and the encoder's frame size may differ, we
		* need to FIFO buffer to store as many frames worth of input samples
		* that they make up at least one frame worth of output samples.
		*/
		/* Use the encoder's desired frame size for processing. */
		while (av_audio_fifo_size(fifo) < track->out_codec_ctx->frame_size) {
			/*
			* Decode one frame worth of audio samples, convert it to the
			* output sample format and put it into the FIFO buffer.
			*/
			read_decode_convert_and_store(
						fifo, 
						track->src_format_ctx,
						track->src_codec_ctx,
						track->out_codec_ctx,
						resample_context, 
						&finished);
			/*
			* If we are at the end of the input file, we continue
			* encoding the remaining audio samples to the output file.
			*/
			if (finished)
				break;
		}
		/*
		* If we have enough samples for the encoder, we encode them.
		* At the end of the file, we pass the remaining samples to
		* the encoder.
		*/
		while (av_audio_fifo_size(fifo) >= track->out_codec_ctx->frame_size ||
		(finished && av_audio_fifo_size(fifo) > 0))
		{
			/*
			* Take one frame worth of audio samples from the FIFO buffer,
			* encode it and write it to the output file.
			*/
			if (load_encode_and_write(fifo, track->out_format_ctx,
										track->out_codec_ctx) != 0)
				break;
		}

		/*
		* If we are at the end of the input file and have encoded
		* all remaining samples, we can exit this loop and finish.
		*/
		if (finished) {
				int data_written;
			/* Flush the encoder as it may have delayed frames. */
			do {
				encode_audio_frame(NULL, track->out_format_ctx,
						track->out_codec_ctx, &data_written);
			} while (data_written);
			
			break;
		}
	}
		
	/* Write the trailer of the output file container. */
	write_output_file_trailer(track->out_format_ctx);
	
	printf("%sWrote output file trailer\n", CLI_INDENT);

	/*-------------------- Conclude audio conversion -----------------*/
	track->status = TRACK_MIXED;
	sprintf(track->wav_path, "%s", qdcp->wav_dir);
	
	/* save the sub_project */
	save_wav_subproject(track, qdcp);
	
	/* clean up everything */

	printf("End of conversion");
	
	return QDCP_SUCCESS;
}

/* select layout with the highest channel count */
int 
select_channel_layout(AVCodec *codec)
{
    const uint64_t *p;
    uint64_t best_ch_layout = 0;
    int best_nb_channels   = 0;
    if (!codec->channel_layouts)
        return AV_CH_LAYOUT_STEREO;
    p = codec->channel_layouts;
    while (*p) {
        int nb_channels = av_get_channel_layout_nb_channels(*p);
        if (nb_channels > best_nb_channels) {
            best_ch_layout    = *p;
            best_nb_channels = nb_channels;
        }
        p++;
    }
    return best_ch_layout;
}

/* check that a given sample format is supported by the encoder */
int 
check_sample_fmt(AVCodec *codec, enum AVSampleFormat sample_fmt)
{
	//~ printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);

    const enum AVSampleFormat *p = codec->sample_fmts;

    while (*p != AV_SAMPLE_FMT_NONE) {
        if (*p == sample_fmt)
            return 1;
        p++;
    }
    return 0;
}

/*
 * Initialize the audio resampler based on the input and output codec settings.
 * If the input and output sample formats differ, a conversion is required
 * libswresample takes care of this, but requires initialization.
 */
int 
init_resampler(t_wav_track * track, SwrContext ** resample_context, int verbosity)
{
	//~ printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);
	
		int error;
	/*
	 * Create a resampler context for the conversion.
	 * Set the conversion parameters.
	 * Default channel layouts based on the number of channels
	 * are assumed for simplicity (they are sometimes not detected
	 * properly by the demuxer and/or decoder).
	 */

	if (verbosity >= VERBOSE_DEBUG)
	{
		printf("src channel layout  : %d\n", track->src_channel_layout);
		printf("out channel layout  : %d\n", track->out_channel_layout);
		printf("src sample format   : %d\n", track->src_sample_fmt);
		printf("out sample format   : %d\n", track->out_sample_fmt);
		printf("src sample rate     : %d\n", track->src_samplerate);
		printf("out sample rate     : %d\n", track->out_samplerate);
	}	

	if (check_sample_fmt(track->src_codec, track->src_sample_fmt))
		if (verbosity >= VERBOSE_VERBOSE)
			printf("%s%sSource sample format checked : %s\n", 
				CLI_INDENT, CLI_INDENT, 
				av_get_sample_fmt_name(track->src_sample_fmt));
	
	if (check_sample_fmt(track->out_codec, track->out_sample_fmt))
		if (verbosity >= VERBOSE_VERBOSE)
			printf("%s%sTarget sample format checked : %s\n", 
				CLI_INDENT, CLI_INDENT, 
				av_get_sample_fmt_name(track->out_sample_fmt));
		
	/* cf. https://www.ffmpeg.org/doxygen/3.1/group__lswr.html#details */
	* resample_context = swr_alloc_set_opts(NULL,
				  track->out_channel_layout,	// out_ch_layout
				  track->out_sample_fmt,     	// out_sample_fmt
				  track->out_samplerate, 		// out_sample_rate
				  track->src_channel_layout,  	// in_ch_layout
				  track->src_sample_fmt,   		// in_sample_fmt
				  track->src_samplerate,   		// in_sample_rate
				  0, NULL);   					// log_offset, log_ctx

	if (!*resample_context) {
		fprintf(stderr, "Could not allocate resample context\n");
		return AVERROR(ENOMEM);
	}
	
	/*
	* Perform a sanity check so that the number of converted samples is
	* not greater than the number of samples to be converted.
	* If the sample rates differ, this case has to be handled differently
	*/
	av_assert0(track->out_samplerate == track->src_samplerate);

	/* Open the resampler with the specified parameters. */
	if ((error = swr_init(*resample_context)) < 0) {
		fprintf(stderr, "Could not open resample context\n");
		swr_free(resample_context);
		return error;
	}
	
    return 0;
}

/* Initialize a FIFO buffer for the audio samples to be encoded. */
int
init_fifo(AVAudioFifo **fifo, AVCodecContext *output_codec_context)
{
	//~ printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);

    /* Create the FIFO buffer based on the specified output sample format. */
    if (!(*fifo = av_audio_fifo_alloc(output_codec_context->sample_fmt,
                                      output_codec_context->channels, 1))) {
        fprintf(stderr, "Could not allocate FIFO\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

/* Initialize one data packet for reading or writing. */
void 
init_packet(AVPacket *packet)
{
    av_init_packet(packet);
    /* Set the packet data and size so that it is recognized as being empty. */
    packet->data = NULL;
    packet->size = 0;
}

/* Initialize one audio frame for reading from the input file */
int 
init_input_frame(AVFrame **frame)
{
    if (!(*frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate input frame\n");
        return AVERROR(ENOMEM);
    }
    return 0;
}

/* Decode one audio frame from the input file. */
int 
decode_audio_frame(AVFrame *frame, AVFormatContext *input_format_context,
      AVCodecContext *input_codec_context, int *data_present, int *finished)
{
    /* Packet used for temporary storage. */
    AVPacket input_packet;
    int error;
    init_packet(&input_packet);
    /* Read one audio frame from the input file into a temporary packet. */
    if ((error = av_read_frame(input_format_context, &input_packet)) < 0) 
    {
        /* If we are at the end of the file, flush the decoder below. */
        if (error == AVERROR_EOF)
            *finished = 1;
        else {
            fprintf(stderr, "Could not read frame (error '%s')\n",
                    get_ffmpeg_error(error));
            return error;
        }
    }
    /*
     * Decode the audio frame stored in the temporary packet.
     * The input audio stream decoder is used to do this.
     * If we are at the end of the file, pass an empty packet to the decoder
     * to flush it.
     */
     /* avcodec_decode_audio4 is deprecated we must use
      * avcodec_send_packet() and avcodec_receive_frame()
      */
    if ((error = avcodec_receive_frame(input_codec_context, frame)) < 0)
    {
        fprintf(stderr, "Could not decode frame (error '%s')\n",
                get_ffmpeg_error(error));
        av_packet_unref(&input_packet);
        return error;
	}
     
    //~ if ((error = avcodec_decode_audio4(input_codec_context, frame,
                                       //~ data_present, &input_packet)) < 0) {
        //~ fprintf(stderr, "Could not decode frame (error '%s')\n",
                //~ get_ffmpeg_error(error));
        //~ av_packet_unref(&input_packet);
        //~ return error;
    //~ }

    /*
     * If the decoder has not been flushed completely, we are not finished,
     * so that this function has to be called again.
     */
    if (*finished && *data_present)
        *finished = 0;
        
    av_packet_unref(&input_packet);
    return 0;
}

/*
 * Convert the input audio samples into the output sample format.
 * The conversion happens on a per-frame basis, the size of which is specified
 * by frame_size.
 */
int 
convert_samples(const uint8_t ** input_data, uint8_t ** converted_data, 
                const int frame_size, SwrContext * resample_context)
{
		int error;
    
    /* Convert the samples using the resampler. */
    if ((error = swr_convert(resample_context,
                             converted_data, frame_size,
                             input_data    , frame_size)) < 0) 
    {
        fprintf(stderr, "Could not convert input samples (error '%s')\n",
                get_ffmpeg_error(error));
        return error;
    }
    return 0;
}

/*
 * Initialize a temporary storage for the specified number of audio samples.
 * The conversion requires temporary storage due to the different format.
 * The number of audio samples to be allocated is specified in frame_size.
 */
int 
init_converted_samples(uint8_t ***converted_input_samples,
                 AVCodecContext *output_codec_context, int frame_size)
{
    int error;
    /**
     * Allocate as many pointers as there are audio channels.
     * Each pointer will later point to the audio samples of the corresponding
     * channels (although it may be NULL for interleaved formats).
     */
    if (!(*converted_input_samples = calloc(output_codec_context->channels,
                                            sizeof(**converted_input_samples)))) {
        fprintf(stderr, "Could not allocate converted input sample pointers\n");
        return AVERROR(ENOMEM);
    }
    /**
     * Allocate memory for the samples of all channels in one consecutive
     * block for convenience.
     */
    if ((error = av_samples_alloc(*converted_input_samples, NULL,
                                  output_codec_context->channels,
                                  frame_size,
                                  output_codec_context->sample_fmt, 0)) < 0) {
        fprintf(stderr,
                "Could not allocate converted input samples (error '%s')\n",
                get_ffmpeg_error(error));
        av_freep(&(*converted_input_samples)[0]);
        free(*converted_input_samples);
        return error;
    }
    return 0;
}

/* Add converted input audio samples to the FIFO buffer for later processing. */
int
add_samples_to_fifo(AVAudioFifo *fifo, uint8_t **converted_input_samples,
                    const int frame_size)
{
    int error;
    /**
     * Make the FIFO as large as it needs to be to hold both,
     * the old and the new samples.
     */
    if ((error = av_audio_fifo_realloc(fifo, av_audio_fifo_size(fifo) + frame_size)) < 0) {
        fprintf(stderr, "Could not reallocate FIFO\n");
        return error;
    }
    /** Store the new samples in the FIFO buffer. */
    if (av_audio_fifo_write(fifo, (void **)converted_input_samples,
                            frame_size) < frame_size) {
        fprintf(stderr, "Could not write data to FIFO\n");
        return AVERROR_EXIT;
    }
    return 0;
}

/*
 * Read one audio frame from the input file, decodes, converts and stores
 * it in the FIFO buffer.
 */
int
read_decode_convert_and_store(AVAudioFifo *fifo, AVFormatContext *input_format_context,
                AVCodecContext *input_codec_context, AVCodecContext *output_codec_context,
                SwrContext *resampler_context, int *finished)
{
    /* Temporary storage of the input samples of the frame read from the file. */
		AVFrame *input_frame = NULL;
    /* Temporary storage for the converted input samples. */
		uint8_t **converted_input_samples = NULL;
    
		int data_present;
		int ret = AVERROR_EXIT;
    
    /* Initialize temporary storage for one input frame. */
    init_input_frame(&input_frame);

    /* Decode one frame worth of audio samples. */
    decode_audio_frame(input_frame, input_format_context,
                           input_codec_context, &data_present, finished);

    /*
     * If we are at the end of the file and there are no more samples
     * in the decoder which are delayed, we are actually finished.
     * This must not be treated as an error.
     */
    if (*finished && !data_present) {
        ret = 0;
    }
    /* If there is decoded data, convert and store it */
    if (data_present) {
        /* Initialize the temporary storage for the converted input samples. */
        init_converted_samples(&converted_input_samples, 
								output_codec_context,
                                input_frame->nb_samples);

        /*
         * Convert the input samples to the desired output sample format.
         * This requires a temporary storage provided by converted_input_samples.
         */
		convert_samples((const uint8_t**)input_frame->extended_data, converted_input_samples,
                            input_frame->nb_samples, resampler_context);

        /* Add the converted input samples to the FIFO buffer for later processing. */
        add_samples_to_fifo(fifo, converted_input_samples,
                                input_frame->nb_samples);
        ret = 0;
    }
    ret = 0;

    return ret;
}

/*
 * Initialize one input frame for writing to the output file.
 * The frame will be exactly frame_size samples large.
 */
int
init_output_frame(
	AVFrame **frame, AVCodecContext *output_codec_context, int frame_size)
{
		int error;
		
    /* Create a new frame to store the audio samples. */
    if (!(*frame = av_frame_alloc())) {
        fprintf(stderr, "Could not allocate output frame\n");
        return AVERROR_EXIT;
    }
    /*
     * Set the frame's parameters, especially its size and format.
     * av_frame_get_buffer needs this to allocate memory for the
     * audio samples of the frame.
     * Default channel layouts based on the number of channels
     * are assumed for simplicity.
     */
    (*frame)->nb_samples     = frame_size;
    (*frame)->channel_layout = output_codec_context->channel_layout;
    (*frame)->format         = output_codec_context->sample_fmt;
    (*frame)->sample_rate    = output_codec_context->sample_rate;
    /*
     * Allocate the samples of the created frame. This call will make
     * sure that the audio frame can hold as many samples as specified.
     */
    if ((error = av_frame_get_buffer(*frame, 0)) < 0) {
        fprintf(stderr, "Could not allocate output frame samples (error '%s')\n",
                get_ffmpeg_error(error));
        av_frame_free(frame);
        /* for debugging before the loop */
        exit(QDCP_SUCCESS);
        return error;
        
    }
    return 0;
}

/* Encode one frame worth of audio to the output file. */
int
encode_audio_frame(AVFrame *frame, AVFormatContext *output_format_context,
                   AVCodecContext *output_codec_context, int *data_present)
{
    /* Packet used for temporary storage. */
		AVPacket output_packet;
		int error;
		
	init_packet(&output_packet);
    
    /* Set a timestamp based on the sample rate for the container. */
    if (frame) {
        frame->pts = pts;
        pts += frame->nb_samples;
    }

    /*
     * Encode the audio frame and store it in the temporary packet.
     * The output audio stream encoder is used to do this.
     */
     /* avcodec_encode_audio2() is deprecated we must use acodec_receive_frame()
      * or avcodec_send_packet() instead. */
    if ((error = avcodec_send_packet(output_codec_context, &output_packet)) < 0) {
        fprintf(stderr, "Could not encode frame (error '%s')\n",
                get_ffmpeg_error(error));
        av_packet_unref(&output_packet);
        return error;
    }
    //~ if ((error = avcodec_encode_audio2(output_codec_context, &output_packet,
                                       //~ frame, data_present)) < 0) {
        //~ fprintf(stderr, "Could not encode frame (error '%s')\n",
                //~ get_ffmpeg_error(error));
        //~ av_packet_unref(&output_packet);
        //~ return error;
    //~ }


    /** Write one audio frame from the temporary packet to the output file. */
    if (*data_present) {
        if ((error = av_write_frame(output_format_context, &output_packet)) < 0) {
            fprintf(stderr, "Could not write frame (error '%s')\n",
                    get_ffmpeg_error(error));
            av_packet_unref(&output_packet);
            return error;
        }
        av_packet_unref(&output_packet);
    }
    return 0;
}

/*
 * Load one audio frame from the FIFO buffer, encode and write it to the
 * output file.
 */
int
load_encode_and_write(AVAudioFifo *fifo, AVFormatContext *output_format_context,
                      AVCodecContext *output_codec_context)
{
    /* Temporary storage of the output samples of the frame written to the file. */
		AVFrame *	output_frame;
    /*
     * Use the maximum number of possible samples per frame.
     * If there is less than the maximum possible frame size in the FIFO
     * buffer use this number. Otherwise, use the maximum possible frame size
     */
		const int 	frame_size = FFMIN(av_audio_fifo_size(fifo),
										output_codec_context->frame_size);
		int 		data_written;
    
    /* Initialize temporary storage for one output frame. */
    if (init_output_frame(&output_frame, output_codec_context, frame_size))
        return AVERROR_EXIT;
        
    /*
     * Read as many samples from the FIFO buffer as required to fill the frame.
     * The samples are stored in the frame temporarily.
     */
    if (av_audio_fifo_read(fifo, (void **)output_frame->data, frame_size) < frame_size) {
        fprintf(stderr, "Could not read data from FIFO\n");
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }
    /* Encode one frame worth of audio samples. */
    if (encode_audio_frame(output_frame, output_format_context,
                           output_codec_context, &data_written)) {
        av_frame_free(&output_frame);
        return AVERROR_EXIT;
    }
    av_frame_free(&output_frame);
    return 0;
}

/* Write the header of the output file container. */
int 
write_output_file_header(AVFormatContext *output_format_context)
{
    int error;
    if ((error = avformat_write_header(output_format_context, NULL)) < 0) {
        fprintf(stderr, "Could not write output file header (error '%s')\n",
                get_ffmpeg_error(error));
        return error;
    }
    return 0;
}

/* Write the trailer of the output file container. */
int
write_output_file_trailer(AVFormatContext *output_format_context)
{
    int error;
    if ((error = av_write_trailer(output_format_context)) < 0) {
        fprintf(stderr, "Could not write output file trailer (error '%s')\n",
                get_ffmpeg_error(error));
        return error;
    }
    return 0;
}


/*-- EOF --*/
