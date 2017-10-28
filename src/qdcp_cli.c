/*
 * QuickDCP: Swiss knife of movie theater operator.
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
   @file    qdcp_cli.c
   @author  jean.sabatier@zanobox.net
   @brief   quickdcp-cli builds Digital Cinema Packages in command line interface.
*/

#include "qdcp_cli.h"

/** @brief Main obviously
 * 
 * This is the main function for the command line interface
 * version of QuickDCP.
 */
int main(int argc, char *argv[]) 
{
		quickdcp_ctx*	qdcp;
		t_project	*	np = NULL;
		t_project 	* 	empty_pj = NULL;
		t_project 	* 	pj = NULL;
		t_wav_track *	nw = NULL;
		struct timeval	tv;
				
		int 			i = 0;
		int 			opt = 0;
		char			err_buffer[80];   /* for warning and error messages */
	
		char			answer[2] = {0};
		char 			config_path[PATH_STR_LEN] = {0};
		char 			config_file[PATH_STR_LEN] = {0};
		char 			qdcp_path[PATH_STR_LEN] = {0};
		char			arg_configpath[PATH_STR_LEN] = {0};
		
		char		*	arg_inputdir = NULL;
		char		*	arg_outputdir = NULL;
		char			arg_qdp_project[PATH_STR_LEN] = {0};
		char			arg_image_file[PATH_STR_LEN] = {0};
		char			arg_audio_file[PATH_STR_LEN] = {0};
		char 			arg_subtt_file[PATH_STR_LEN] = {0};
		char			arg_video_file[PATH_STR_LEN] = {0};
		char			arg_video_to_convert[PATH_STR_LEN] = {0};
		char			arg_diaporama[PATH_STR_LEN] = {0};
			
	/*-------------------- Instantiate quickdcp ----------------------*/
	
	qdcp = quickdcp_init();

	/*-------------- Default path of configuration file --------------*/
	
    sprintf(config_path, "%s%s%s", 
				HOME_DIR, SEPARATOR, CFG_DIR);
	check_dir(config_path, qdcp->verbosity);
    sprintf(config_file, "%s%s%s", 
				config_path, SEPARATOR, CFG_FILE);

    
	/*---------------- Parsing options from arguments ----------------*/
	      
    while((opt = getopt(argc, argv, "hvqVdlC:w:o:i:a:s:m:p:d:c:")) != -1)
    {
        i++;
        switch(opt) {
            case 'h':
				/* show help */
                qdcp_show_help();
            break;
            case 'v':
                printf(" > QuicDCP version %d.%d.%d (%s)\n", 
							QDCP_VERSION_MAJ, 
							QDCP_VERSION_MIN, 
							QDCP_VERSION_REV, 
							QDCP_VERSION_DATE);
                printf(" > Compilation : %s\n", __DATE__);
                exit(0);
            break;
            case 'q':
				/* quiet mode, no or little output on stdout */
                printf("QuickDCP : Quiet mode activated (not implemented yet)\n");
                qdcp->verbosity = VERBOSE_QUIET;
            break;
            case 'V':
				/* verbose mode, more detailed output on stdout */
                printf("QuickDCP : Verbose mode activated.\n");
                qdcp->verbosity = VERBOSE_VERBOSE;
            break;
            case 'D':
				/* debug mode, most detailed output on stdout */
                printf("QuickDCP : Debug mode activated (not implemented yet)\n");
				qdcp->verbosity = VERBOSE_DEBUG;
			break;
			
            case 'C':
				/* alternative config file */
				// test here the validity of passed directory ?
				sprintf(config_path, "%s", optarg);
				// printf("Invalid alternative config path, falling back to default");
			break;
            case 'w':
				/* alternative path to a source directory (workspace) */
                arg_inputdir = optarg;
            break;
            case 'o':
				/* alternative output directory (for dcp) */
                arg_outputdir = optarg;
            break;
            
            case 'i':
				/* process an image file */
				sprintf(arg_image_file, "%s", optarg);
                //~ printf("QuickDCP : Process an image file %s\n", arg_image_file);
			break;
            case 'a':
				/* process an audio file */
				sprintf(arg_audio_file, "%s", optarg);
			break;
            case 's':
				/* process a subtitle file */
				sprintf(arg_subtt_file, "%s", optarg);
               //~ printf("QuickDCP : Process a subtitle file %s\n", arg_subtt_file);
			break;
            case 'm':
				/* process a video (movie) file */
				sprintf(arg_video_file, "%s", optarg);
                //~ printf("QuickDCP : Process a video file %s\n", arg_video_file);
			break;
            case 'p':
				/* treat a particular project file */
				sprintf(arg_qdp_project, "%s", optarg);
            break;
            case 'd':
				/* process all images in that directory into a diaporama */
				sprintf(arg_diaporama, "%s", optarg);
            case 'l':
				/* choose from list of project files */
                sprintf(arg_qdp_project, "list");
            break;
            case 'c':
				/* convert a video file */
				sprintf(arg_video_to_convert, "%s", optarg);
			break;
            
            case '?':
                //printf("QuickDCP : Unknown argument given");
                qdcp_show_usage();
            break;
        }
    }

	/*------------------------ Say hello -----------------------------*/
	
    qdcp_intro_block();

	/*---------------- Read and parse configuration file -------------*/
	
    read_config_file(config_file, qdcp);    

	if (arg_inputdir != NULL)
	{
		printf("Printing workspace argument");
		qdcp->workspace = arg_inputdir;
	}
	if (arg_outputdir != NULL)
	{
		qdcp->dcp_out = arg_outputdir;
	}

	/*--------- if workspace or output directories do not exist ------*/
	
		int test_dirs = 0;
	if (! check_dir(qdcp->workspace, -1))
	{
		printf("L'espace de travail %s%s%s n'existe pas\n", CLI_BLUE, qdcp->workspace, CLI_COLOR);
		test_dirs = 1;
	}
	if (! check_dir(qdcp->dcp_out, -1))
	{
		printf("Le dossier de sortie DCP %s%s%s n'existe pas\n", CLI_BLUE, qdcp->workspace, CLI_COLOR);
		test_dirs = 1;
	}
	if (test_dirs)
	{
		printf("%s1 - \tCréer automatiquement les dossiers par défaut\n", CLI_INDENT);
		printf("%s2 - \tEditer le fichier de configuration avec %s\n", CLI_INDENT, CLI_EDITOR);
		printf("%s3 - \tEditer le fichier de configuration avec %s\n", CLI_INDENT, TEXT_EDITOR);

		printf("\nRéponse > ");
		qdcp_get_answer(answer, 1);
		
		if (strcmp(answer, "1") == 0)
		{
			check_dir(qdcp->workspace, 1);
			check_dir(qdcp->dcp_out, 1);
		}
		else if (strcmp(answer, "2") == 0)
		{
				char command[CMD_STR_LEN];
			sprintf(command, "%s %s", CLI_EDITOR, config_file);
			
			if (system(command) < 0)
			{	
				sprintf(err_buffer, "Text editor %s failed to open", CLI_EDITOR);
				warning(err_buffer);
			}else{
				read_config_file(config_file, qdcp);    
			}
		}
		else if (strcmp(answer, "3") == 0)
		{
				char command[CMD_STR_LEN];
			sprintf(command, "%s %s", TEXT_EDITOR, config_file);
			
			if (system(command) < 0)
			{
				sprintf(err_buffer, "Text editor %s failed to open", TEXT_EDITOR);
				warning(err_buffer);
			}else{
				read_config_file(config_file, qdcp);    
			}
		}
  	}
  	
  	/* Now we can start recording logs */
    //~ qdcp_log_init(qdcp->loglevel, qdcp);

	/* Instantiate ImageMagick */
	MagickWandGenesis();
		
	/*-------------- Display some useful values ------------------*/

	/* this is for a phony DCP name */
	empty_pj = new_project(1, qdcp, NULL);

	if (qdcp->verbosity >= VERBOSE_NORMAL)
		show_default_params(qdcp, QDCP_NO);

	printf(" > Exemple de nom : %s", get_dcp_name_from_project(empty_pj, qdcp));
	free_project(empty_pj);
	printf("\n");
		
	/*------------ Propose to modify configuration ---------------*/

	printf("\n[Répondre * pour voir tous les paramètres]\n");
	
	printf("\n%sModifier ces réglages ? (o|N) > %s", CLI_YELLOW, CLI_COLOR);
	qdcp_get_answer(answer, 1);
	
	if (strcmp(answer, "*") == 0)
	{
		show_default_params(qdcp, QDCP_YES);

		printf(" > Exemple de nom : %s", get_dcp_name_from_project(empty_pj, qdcp));
		free_project(empty_pj);
		printf("\n");

		printf("\nModifier ces réglages ? (o|N) > ");
		qdcp_get_answer(answer, 1);
	}
	
	if (strcmp(answer, "o") == 0 || strcmp(answer, "y") == 0)
	{
			char	command[CMD_STR_LEN];
		sprintf(command, "%s %s", CLI_EDITOR, config_file);

		if (system(command) < 0)
		{
			warning("Text editor failed to open");
			printf("Continuer ? (o/n) > ");
			qdcp_get_answer(answer, 1);
			if (strcmp(answer, "o") != 0)
			{
				printf("Arret de QuickDCP.\n");
				exit(QDCP_SUCCESS);
			}
		}else{
			read_config_file(config_file, qdcp);    
		}
	}	
 
	star_line();

	/*------------- We may process an existing project ---------------*/
	
    if (arg_qdp_project[0] != 0)
    {
			char	qdp_path[PATH_STR_LEN] = {0};
				
		if (arg_qdp_project[0] == '/' || arg_qdp_project[0] == '~')
		{
			strcpy(qdp_path, arg_qdp_project);
		}
		else
		{
			/* try it from a relative path */
			sprintf(qdp_path, "%s%s", qdcp->proj_dir, arg_qdp_project);
		}
		
		if (qdp_path[0] != 0 && strcmp(arg_qdp_project, "list"))
		{
				t_project 	*	qdp = NULL;
			
			qdp = load_project(qdp_path, qdcp->proj_list, qdcp);
		}		
		/* List all available qdp projects to choose one *
		 * --------------------------------------------- */
		else
		{
				file_obj		*	current_qdp = NULL;
				t_file_list		*	file_list = new_file_list();
		
			/* Scan projects directory for qdp files *
			 * ------------------------------------- */			
			star_line();

			printf("Contenus du dossier projets %s :\n\n", qdcp->proj_dir);
			
			if (scan_source_directory(qdcp->proj_dir, 0, "qdp,qdx", file_list) != QDCP_SUCCESS)
			{
				error_msg(NO_QDP_FOUND, __FILE__, __LINE__);
			}
			
			show_file_list(file_list);
			
			ask_again:
			
			printf("\nChoisir un numéro ou * pour tout traiter > ");
			qdcp_get_answer(answer, 2);
			
			int int_answer = strtoimax(answer, NULL, 10); 
		
			if (strcmp(answer, "*") != 0 && int_answer < 1 && int_answer > file_list->size)
			{
				goto ask_again;
			}

			int count;
			current_qdp = file_list->first;
			while (current_qdp != NULL)
			{
				/* load selected project(s) */
				if (strcmp(answer, "*") == 0 || current_qdp->index == int_answer)
				{
						char				qdp_path[PATH_STR_LEN] = {0};
						t_project		*	current_proj = NULL;
					
					count++;
					star_line();
					
					sprintf(qdp_path, "%s%s%s", current_qdp->path, SEPARATOR, current_qdp->fname);
					
					current_proj = load_project(qdp_path, qdcp->proj_list, qdcp);
					
				}
				current_qdp = current_qdp->next;
			}			
		}
	}

	/*--------------- or open a brand new project --------------------*/
	else if (i > 0)
	{
		printf("Création d'un nouveau projet\n");
		np = new_project(0, qdcp, qdcp->proj_list);	
	}

	
	/*-------------- If we process a given audio file ----------------*/

	if (strcmp(arg_audio_file, "") != 0)
	{		
			char 			file_path[PATH_STR_LEN];
			t_wav_track *	nw = NULL;
		init_wav_track(nw, qdcp);

		/* do we have an absolute path or just a file name ? */
		if (import_source_file(file_path, arg_audio_file, qdcp) == QDCP_FAILURE)
			error_msg("Could not import image file", __FILE__, __LINE__);
		
		//~ rename_project(get_basename(strip_path(arg_audio_file)), np, qdcp);
				
		if (process_audio_file(file_path, nw, np, qdcp) == QDCP_FAILURE)
		{
			warning("Processing audio file failed");
			return QDCP_FAILURE;
		}
	}

	/*-------------- or process a given video file -------------------*/

	if (strcmp(arg_video_file, "") != 0)
	{		
			char 			file_path[PATH_STR_LEN];
			//~ t_project	*	np = new_project(0, qdcp, qdcp->proj_list);

		
		/* do we have an absolute path or just a file name ? */
		if (import_source_file(file_path, arg_video_file, qdcp) == QDCP_FAILURE)
			error_msg("Could not import image file", __FILE__, __LINE__);

		/* since we have only one video file
		 * we can rename the project after the video */
		if (! rename_project(strip_path(get_basename(file_path)), np, qdcp))
		{
			error_msg("Renaming project failed", __FILE__, __LINE__);
		}
			
		/* we create a wav track in case we need to retrieve audio */	
		if (init_wav_track(nw, qdcp) == QDCP_FAILURE)
		{
			warning("Failed to alloc a new wav track");
			return QDCP_FAILURE;
		}
			
		if (process_video_file(file_path, np, nw, qdcp) == QDCP_FAILURE)
			error_msg("Processing video file failed", __FILE__, __LINE__);

		//~ if (process_audio_file(file_path, nw, np, qdcp) == QDCP_FAILURE)
			//~ error_msg("Processing video file failed", __FILE__, __LINE__);
	}

	/*-------------- or process a given image file -------------------*/
	
	if (strcmp(arg_image_file, "") != 0)
	{		
			char 	file_path[PATH_STR_LEN];
			
		/* do we have an absolute path or just a file name ? */
		if (import_source_file(file_path, arg_image_file, qdcp) == QDCP_FAILURE)
			error_msg("Could not import image file", __FILE__, __LINE__);
		
		rename_project(strip_path(get_basename(file_path)), np, qdcp);

		if (process_image_file(file_path, np, qdcp) == QDCP_FAILURE)
			error_msg("Processing image file failed", __FILE__, __LINE__);
		
	}

	/*-------------- or convert a given video file -------------------*/
	
	if (strcmp(arg_video_to_convert, "") != 0)
	{		
			char 	file_path[PATH_STR_LEN];
			
		/* do we have an absolute path or just a file name ? */
		if (import_source_file(file_path, arg_video_to_convert, qdcp) == QDCP_FAILURE)
			error_msg("Could not import image file", __FILE__, __LINE__);
		
		/* since we have only one video file
		 * we can rename the project after the video */
		if (! rename_project(strip_path(get_basename(file_path)), np, qdcp))
		{
			warning("Renaming project failed");
		}

		if (convert_video_to_mxf(file_path, np, qdcp) == QDCP_FAILURE)
			error_msg("Processing video file failed", __FILE__, __LINE__);
		
	}


	/*------------- or search for file(s) to process -----------------*/

	if (qdcp->proj_list->size == 0)
	{
		/*----------- Scan input directory for source files ----------*/

			t_file_list		*	src_list = new_file_list();
			int					counter = 0;
			file_obj   		*	current_file = NULL;
			//~ t_project 		* 	np = NULL;
		
		ask_for_more:
		
		star_line();	
		printf("Contenus du dossier source %s :\n\n", qdcp->workspace);
		
		if (scan_source_directory(qdcp->workspace, 0, NULL, src_list) != QDCP_SUCCESS)
		{
			error_msg(NO_SOURCE_FOUND, __FILE__, __LINE__);
		}
		
		show_file_list(src_list);
		
		printf("\nChoisir un numéro ou * pour tout traiter > ");
		qdcp_get_answer(answer, 2);
		
			int int_answer = strtoimax(answer, NULL, 10); 
		
		if (strcmp(answer, "*") != 0 && int_answer < 1 && int_answer > src_list->size)
		{
			goto ask_for_more;
		}

		/*------------------ Looping through files -------------------*/
		
		current_file = src_list->first;
		
		while (current_file != NULL)
		{
			if (strcmp(answer, "*") == 0 || current_file->index == int_answer)
			{
				star_line();
				
					/* do we double check, beyond its extension, the true nature of the file ? */
					qdcp_bool 	double_check = QDCP_NO;
				
					/* declare a non const name of the file */
					char		file_basename[PATH_STR_LEN];
					
				sprintf(file_basename, "%s", current_file->basename);
				
				/*----------- if we have an image file ---------------*/
				
				if (file_is_image(current_file, double_check))
				{
					printf("Création d'un projet pour %s\n", current_file->fname);
					//~ QDCP_LOG(LOG_INFO, "Création d'un nouveau projet");
	
					np = new_project(counter, qdcp, qdcp->proj_list);
					if (np == NULL)
					{
						warning(T_PROJECT_FAILED);
						
						current_file = current_file->next;
						continue;
					}

					/* since we have only one image layer 
					 * we can rename the project after the image */
					if (! rename_project(strip_path(current_file->basename), np, qdcp))
					{
						warning("Renaming project failed");
					}

					if (! process_image_file(current_file->full_path, np, qdcp))
					{
						warning("Failed processing image %s", current_file->full_path);
						current_file = current_file->next;
						continue;
					}

					/* save that good work */
					save_project(np, qdcp);
				}
				
				/*------------ if we have an audio file --------------*/
				
				else if (file_is_audio(current_file, double_check)) 	
				{
						t_wav_track *	nw = NULL;
					init_wav_track(nw, qdcp);

					/* since we have only one video file
					 * we can rename the project after the video */
					if (! rename_project(strip_path(current_file->basename), np, qdcp))
					{
						warning("Renaming project failed");
					}

					/* import audio file and get a new wav project */
					if (! process_audio_file(current_file->full_path, nw, np, qdcp))
					{
						warning("Import audio échoué");
						continue;
					}

				}
				
				/*----------- if we have a subtitle file -------------*/
				
				else if (file_is_subtitle(current_file, double_check)) 	
				{
					/* import audio file and get a new wav project */
					printf("Création d'un sous-projet de piste audio :\n");
					
						t_sub_track *	nst = calloc(1, sizeof(*nst));
	
					if (nst == NULL) {
						warning(BAD_CALLOC);
						return QDCP_FAILURE;
					}

					init_sub_track(nst, qdcp);
						
					if (! import_subtitle_file(nst, current_file, qdcp))
						error_msg("Failed to import subtitle file", __FILE__, __LINE__);
					
				}
				
				/*------------ if we have a video file ---------------*/
				
				else if (file_is_video(current_file, double_check))
				{	
					/* create a new project for this video */
					
					printf("Création d'un nouveau projet video\n");
					
					np = new_project(counter, qdcp, qdcp->proj_list);
					if (np == NULL)
					{
						warning(T_PROJECT_FAILED);						
						current_file = current_file->next;
						continue;
					}

					/* since we have only one video file
					 * we can rename the project after the video */
					if (! rename_project(strip_path(current_file->basename), np, qdcp))
					{
						warning("Renaming project failed");
					}
						
					/* we create a wav track in case we need to retrieve audio */	
					if (init_wav_track(nw, qdcp) == QDCP_FAILURE)
					{
						warning("Failed to alloc a new wav track");
						return QDCP_FAILURE;
					}

					if (! process_video_file(current_file->full_path, np, nw, qdcp))
					{
						warning("Failed processing video %s", current_file->full_path);
						current_file = current_file->next;
						continue;
					}
					/* save that good work */
					save_project(np, qdcp);
				}

				/*--------- if we have an MXF (jpeg2000) file --------*/
				
				else if (file_is_mxf(current_file, double_check))
				{		
					/* determine the essence of the mxf content */
					
					printf("%sCréation d'un nouveau projet video\n", CLI_INDENT);
					//~ QDCP_LOG(LOG_INFO, "%sCréation d'un nouveau projet video", CLI_INDENT);
					
					np = new_project(counter, qdcp, qdcp->proj_list);
					if (np == NULL)
					{
						warning(T_PROJECT_FAILED);
						
						current_file = current_file->next;
						continue;
					}
					/* since we have only one video file
					 * we can rename the project after the video */
					rename_project(strip_path(file_basename), np, qdcp);
					
					printf("%s%sCréation d'un nouveau calque video%s\n", 
								CLI_INDENT, CLI_GREEN, CLI_COLOR);
					//~ QDCP_LOG(LOG_INFO, "Création d'un nouveau calque d'image");
					
					/* create and populate a new layer */
						t_layer * nl 	= init_layer(np);
					if (! create_new_video_layer(nl, current_file->fname, np, qdcp))
					{
						warning("Echec de création d'un calque video (mxf)");
						continue;
					}
					
					/* save that good work */
					save_project(np, qdcp);
														
					if (qdcp->verbosity >= VERBOSE_VERBOSE)
					{
						show_layer_params(nl, qdcp->verbosity);
						star_line();
					}
				}
				/* else file has an unexpected extension */
				else
				{
					warning("File %s has an unexpected extension", current_file->fname);
				}

			} /* if answer */
			current_file = current_file->next;
		} /* while current_file != NULL */
		//~ free(current_file);
		//~ free(sources);		
				
	} /* if (proj_list->size == 0) */


	/*----------------- Looping through project list -----------------*/

	pj = qdcp->proj_list->first;
	
	while(pj != NULL)
	{
		star_line();

		printf("Traitement du projet %s.qdx\n", pj->name);
		
		/* we could ask for showing project's params 
		 * answers : y : yes, n : don't ask again today, N : never ask.
		 * default is yes.
		 */
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			show_project_params(pj, qdcp);
		
		gettimeofday(&tv, NULL);
		pj->starttime = tv.tv_sec;

		/* generate a preview sequence from the project data */	
		
			/* render mode in CLI should be 
			 * J2K_ONLY but can otherwise be 
			 * J2K_PLUS_PREVIEW or
			 * PREVIEW_ONLY for tests purpose */
		
		if (! qdcp->make_dcp)
			qdcp->preview = 1;
		
		if (qdcp->preview)
		{
			printf("\n\n%sRendu de la séquence d'aperçu :\n", CLI_INDENT);

			if (! generate_sequence(pj, qdcp, PREVIEW_ONLY))
				error_msg("Echec de la génération des images d'aperçu", __FILE__, __LINE__);

			gettimeofday(&tv, NULL);
			pj->endtime = tv.tv_sec;
		
			qdcp_compte_rendu(pj, PREVIEW_ONLY);
		}
		
		/* go for the whole dcp making process */
		if (qdcp->make_dcp)
		{
			
			/* Generate a jpeg2000 sequence *
			 * ---------------------------- */
			printf("\n\n%sRendu de la séquence J2K :\n", CLI_INDENT);

			if (! generate_sequence(pj, qdcp, J2K_ONLY))
				error_msg("Echec de la génération des images j2k", __FILE__, __LINE__);
			
			gettimeofday(&tv, NULL);
			pj->endtime = tv.tv_sec;
		
			qdcp_compte_rendu(pj, J2K_ONLY);
			 
			/* Generate and mix wav tracks *
			 * ---------------------------- */
			if (pj->tracks->size > 0)
			{
					t_wav_track * cur_track = pj->tracks->first;
				while (cur_track != NULL)
				{
					/* check if the track was already processed and when */
					if (cur_track->status < TRACK_MIXED)
					{
						/* (re)process the track */
						printf("%sTraitement et calibrage des pistes son\n", 
							CLI_INDENT);
						if (convert_audio_2_wav(cur_track, qdcp) == QDCP_FAILURE)
							warning("Failed to convert an audio track");
							cur_track = cur_track->next;
							continue;
					}
					cur_track = cur_track->next;
				}
				
				/* flatten all tracks in just one */
				render_audio(pj, qdcp);
	
				/* pack the PCM in an MXF asset */
				printf("%sEmpaquetage des pistes son dans un fichier MXF\n", CLI_INDENT);
				if (write_wav_mxf_reel(pj, qdcp) == QDCP_FAILURE)
				{
					warning("Failed to convert PCM into MXF");
				}				
			}
			
			 
			/* MXF packaging with asdcplib *
			 * --------------------------- */			 
			printf("\n%sEmpaquetage des images dans un fichier mxf\n", CLI_INDENT);
			
			if (! write_j2k_mxf_reel(pj, qdcp))
				error_msg("Echec de l'empaquetage MXF", __FILE__, __LINE__);


			/* Generating XML with asdcplib *
			 * ---------------------------- */
			printf("\n%sÉcriture des fichiers XML : \n", CLI_INDENT);
			
			if (! finalise_dcp(pj, qdcp))
				error_msg("Echec de la finalisation du DCP", __FILE__, __LINE__);

			/* Debriefing all that good work *
			 * ----------------------------- */
			 
			/* move new DCP directory and its contents to output directory */

					file_obj	*	file = NULL;
					char			source_file[PATH_STR_LEN];
					char			target_file[PATH_STR_LEN];
							
			printf("\n%sMoving new DCP to target directory : %s\n", 
				CLI_INDENT, pj->final_dcp_dir);
			 			
			check_dir(pj->final_dcp_dir, qdcp->verbosity);
			
			scan_source_directory(pj->tmp_dcp_dir, 0, NULL, pj->dcp_files);
			
			file = pj->dcp_files->first;
			while(file != NULL)
			{	
				sprintf(source_file, "%s%s", pj->tmp_dcp_dir, file->fname);
				sprintf(target_file, "%s%s", pj->final_dcp_dir, file->fname);

				/* rename() is in stdio.h but may not work well (or at all) between diferent partitions */
				/* in the latter case, use copy_file and delete the source */
				if(rename(source_file, target_file))
				{
					/* if we deal with different partitions */
					if (errno == EXDEV)
					{
						printf("Destination is on another filesystem, QuickDCP needs to address this problem !");
						/* copy file and data */
						if (! cp(target_file, source_file))
						{	
							sprintf(err_buffer, "Failed to copy ", source_file);
							warning(err_buffer);
						}
						/* then remove source */
					}
					else
					{
						perror("rename"); exit(QDCP_FAILURE);
					}
				}
				file = file->next;
			}
			//~ destroy_file_list(tmp_list);

			
			/* move source files to the attic directory */
			/* remove temporary files */
			
			gettimeofday(&tv, NULL);
			pj->endtime = tv.tv_sec;

			qdcp_compte_rendu(pj, -1);
		}
			 
		/* show must go on */
		pj = pj->next;
	}	
	
    
    /*-------------------------- Conclusion --------------------------*/
	
	
	/* Quit ImageMagick context instance */
	MagickWandTerminus();
	
    //~ iniparser_freedict(cfg);
	//~ free(qdcp->proj_list);
	//~ free_project(pj);
	
	/* free quickdcp instance */
	quickdcp_close(qdcp);

    return QDCP_SUCCESS;
}

qdcp_bool
process_image_file(char * file_path, t_project * pj, quickdcp_ctx * qdcp)
{
	/* file_path here is supposed to be the absolute path and full name of the image */
	
	/* Create a new layer for this image */

	printf("%s%sCréation d'un nouveau calque d'image%s\n", 
					CLI_INDENT, CLI_GREEN, CLI_COLOR);
	//~ QDCP_LOG(LOG_INFO, "Création d'un nouveau calque d'image");

		t_layer * nl = init_layer(pj);
		
	if (! create_new_image_layer(nl, file_path, pj, qdcp))
	{
		warning("Echec de création d'un calque image");
		return QDCP_FAILURE;
	}
	
	/* show parameters to curious people */
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		show_project_params(pj, qdcp);
		star_line();

		show_layer_params(nl, qdcp->verbosity);
		star_line();
	}
	
	return QDCP_SUCCESS;						
}

qdcp_bool
process_audio_file(char * file_path, t_wav_track * nw, t_project * pj, quickdcp_ctx * qdcp)
{
	init_ffmpeg_api();
	
	/* import audio file and get a new wav project */
	printf("Création d'un sous-projet de piste audio :\n");
	
	/* if the file is not in our workspace get a copy there */
	if (import_audio_file(nw, file_path, qdcp) == QDCP_FAILURE)
		error_msg("Import audio échoué", __FILE__, __LINE__);
	
	/* this is where ffmpeg API begins */		
			
	/* convert the file and get wav info into wav project */
	if (convert_audio_2_wav(nw, qdcp) == QDCP_FAILURE)
		error_msg("Audio conversion failed", __FILE__, __LINE__);
		
					
	/* Add the audio track to the project */
	printf("%s%sAjout de pistes son au nouveau projet%s\n", 
				CLI_INDENT, CLI_GREEN, CLI_COLOR);
	add_track_to_project(nw, pj, qdcp);

	/* we can rename the project after the audio track */
	if (! rename_project(strip_path(get_basename(file_path)), pj, qdcp))
		warning("Could not rename audio project as %s", get_basename(file_path));

	save_project(pj, qdcp);		
										
	return QDCP_SUCCESS;						
}

qdcp_bool
process_video_file(char * file_path, t_project * pj, t_wav_track * track, quickdcp_ctx * qdcp)
{
	/* When we extract a video we must retrieve or create :
	 * - a copy of the original file in the workspace.
	 * - a project where images of the sequence will form a layer of the LAYER_VIDEO type.
	 * - a full image sequence in j2k. Those images must be kept on the hard-disk 
	 *   or the project would be broken (think about qdp exports).
	 * - optionnally, a small preview sequence in png.
	 * - a wav track object attached to the project, representing the sound channels of the video, if any.
	 * - the mixed sound as a wav file to be packed in mxf, referenced by the above wav track object.
	 * - a subtitle track object attached to the project, representing the subtitles tracks of the video, if any.
	 * - the XML file containing the subtitles, ready to be modified or packed in mxf.
	
	* In case we dont't need to interact with the extracted video materials
	* we don't import it and directly convert it with convert_video_to_mxf.
	*/

	/* Tutorial in http://dranger.com/ffmpeg/tutorial01.html */

		char			answer[2] = {0};
		int				counter = 0;
		t_layer 	* 	ly 	= init_layer(pj);
		
	/* register libav codecs and formats 
	 * (needed only once, so, better, may be, in main()? ) */
	init_ffmpeg_api();

	/* if the file is not in our workspace get a copy there */
	if (! import_media_file(ly, file_path, qdcp))
		warning("Import video échoué");
	
	/* extract video info towards layer values */
	if (! get_video_info(ly, track, pj, qdcp))
		warning("Could not retrieve video info");
	
	/* create the video layer */	
	if (! create_new_video_layer(ly, file_path, pj, qdcp))
		warning("Echec de création d'un calque video");
														
	/* extract an image sequence from the video */
	if (! extract_video_images(ly, pj, qdcp))
		error_msg("Failed to extract video frames", __FILE__, __LINE__);
		
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		show_layer_params(ly, qdcp->verbosity);
		star_line();
	}

	/*--------------------------------*/
					
	
	
	return QDCP_SUCCESS;
}


/* Get a user's answer from standard in */
qdcp_bool
qdcp_get_answer(char * string, int length) 
{
		char *pos_newline;
	
	length++; // add room to the demanded max string length for the \0 character
	
	if (fgets(string, length, stdin) != NULL)
	{
		pos_newline = strchr(string, '\n');
		
		if (pos_newline != NULL)
		{
			*pos_newline = '\0';
		}
		else
		{
			qdcp_clean_buffer();
		}
		return QDCP_SUCCESS;
	}
	else
	{
		qdcp_clean_buffer();
		return QDCP_FAILURE;
	}
}

/* Clean standard in buffer */
void 
qdcp_clean_buffer() 
{
	int c = 0;
	while (c != '\n' && c != EOF)
	{
		c = getchar();
	}
}

/* What we say when we say hello */
static void 
qdcp_intro_block() 
{
	/* Color codes
	 
	for %d in an inside string expression like \x1b[%dm

	0: reset colors/style
	1: bold
	4: underline
	30 - 37: black, red, green, yellow, blue, magenta, cyan, and white text
	40 - 47: black, red, green, yellow, blue, magenta, cyan, and white background
	
	ex:	\x1b[31m
		\x1b[40;32m
		\x1b[0m		-> remet aussi bien le fond que le texte aux valeurs par défaut
		
	*/

	star_line();
	printf("\x1b[34m");
	printf("      quickdcp-cli version %d.%d.%d\n", QDCP_VERSION_MAJ, QDCP_VERSION_MIN, QDCP_VERSION_REV);
	printf("    %s \n", QDCP_COPYRIGHT);
	printf("\x1b[36m");
    printf("   Ctrl-C pour quitter à tout moment.\n");
    printf("  	Aide : 'quickdcp-cli -h'\n");
	printf("\x1b[0m");
    star_line();
}

/* What we say when job is done */
static void 
qdcp_compte_rendu(t_project * pj, int preview) 
{
		char  		time_str[HALF_STR_LEN];	
		int 		worktime = pj->endtime - pj->starttime;
		file_obj *	file = NULL;
	
	if (worktime > 0)
	{
		int s = worktime % 60;
		int m = worktime / 60;
		int h = worktime / 3600;
		
		if (h > 0)
		{
			m %= 60;
			sprintf(time_str, "%d:%d:%d", h, m, s);
		}
		else
		{
			sprintf(time_str, "%d'%d\"", m, s);
		}
	}
	else
	{
		sprintf(time_str, "moins d'une seconde");
	}
	
	if (preview == PREVIEW_ONLY)
	{
		printf("\n\n%sRendu de la séquence d'aperçu en %s.\n", CLI_INDENT, time_str);
		
		return;
	}
	if (preview == J2K_ONLY)
	{
		printf("\n\n%sRendu de la séquence jpeg2000 en %s.\n", CLI_INDENT, time_str);
		
		return;
	}
	
	/* check if the DCP is for real */
	
	printf(	"\n\n \x1b[34m============================================================\n"
			" \x1b[34m|\n"
			" \x1b[34m|\x1b[0m\tCRÉATION DU DCP TERMINÉE\n"
			" \x1b[34m|\x1b[0m\n");
	printf( " \x1b[34m|\x1b[0m\t en %s. \n", time_str);
	printf( " \x1b[34m|\x1b[0m\n"
			" \x1b[34m|\x1b[0m\tContenu du dossier %s :\n", pj->name);
	printf( " \x1b[34m|\x1b[0m\n");
	file = pj->dcp_files->first;
	while(file != NULL)
	{	
		printf(" \x1b[34m|\x1b[0m\t\t\x1b[33m%s\x1b[0m\n", file->fname);
		file = file->next;
	}
	//printf("|\t$nc"
	//dcp_size=`du -sh $outputdir/$b_name | awk '{print $1}'`
	//printf("|\tLe nouveau DCP pèse $dcp_size et se trouve "
	//printf("|\tdans le dossier $outputdir."
	printf( " \x1b[34m|\x1b[0m\n"
			" \x1b[34m|\x1b[0m\tCopier le dossier sur un disque externe ou une \n"
			" \x1b[34m|\x1b[0m\tclef USB pour ingest sur un serveur.\n"
			" \x1b[34m|\x1b[0m\tSi la taille du DCP dépasse 4Go, le disque ou \n"
			" \x1b[34m|\x1b[0m\tla clef doivent être formatés en NTFS.\n"
			" \x1b[34m|\x1b[0m\n"
			" \x1b[34m============================================================\x1b[0m\n\n");

}

/* Display some helpful explanations */
static void 
qdcp_show_help() 
{
    printf(
    "\n******************** AIDE DE QUICKDCP ********************"
    "\n"
    "\nNOM"
    "\n\tquickdcp_cli - Création de DCP simples pour les opérateurs de cinéma"
    "\n"
    "\nSYNOPSIS"
    "\n\tquickdcp_cli [-h] [-v] [-i INPUT] [-o OUTPUT] [-c FILE ]"
    "\n"
    "\nDESCRIPTION"
    "\n"
    "\nOPTIONS"
    "\n    -c FILE"
    "\n\tUtilise le fichier FILE (chemin absolu) comme fichier de configuration."
    "\n"
    "\n    -h"
    "\n\tAffiche cette page."
    "\n"
    "\n    -i DIR"
    "\n\tDétermine le dossier source DIR,contenant les fichiers à traiter."
    "\n"
    "\n    -o DIR"
    "\n\tDétermine le dossier cible DIR, recevant les fichiers traités."
    "\n"
    "\n    -p FILE"
    "\n\tOuvre et lit le fichier de projet QuickDCP (.qdp ou .qdx) FILE, puis tente de générer le DCP correspondant."
    "\n\tSi FILE n'est pas précisé, quickdcp-cli cherchera les fichiers .qdp disponibles dans le dossier par défaut."
    "\n"
    "\n    -l"
    "\n\tPropose une liste numérotée de tous les projets existants présents dans l'espace de travail."
    "\n"
    "\n    -q"
    "\n\tMode muet (verbose). Aucun retour n'est envoyé sur la sortie standard."
    " La sortie d'erreur reste active."
    "\n\tCette option peut-être utile pour appeler QuickDCP depuis un script en tache de fond par exemple."
    "\n"
    "\n    -v"
    "\n\tAffiche le numero de la version installée de QuickDCP."
    "\n"
    "\n    -V"
    "\n\tMode bavard (verbose). Fournit des informations sur l'activité de QuickDCP sur la sortie standard."
    "\n\tUtile pour repérer des bugs avant de les signaler."
    "\n"
    "\nFICHIER DE CONFIGURATION PAR DEFAUT"
    "\n\t/home/<utilisateur>/.config/quickdcprc"
    "\n"
    "\nBUGS"
    "\n\t(liens à venir)"
    "\n"
    "\nAUTEUR"
    "\n\tjean.sabatier@zanobox.net,"
    "\n");
    
    exit(0);
}

/* Display a short usage synopsis */
static void 
qdcp_show_usage() 
{
    printf(
    "\nUsage :"
    "\nquickdcp-cli [-h] [-v] [-V] [-i input_directory] [-o output_directory] [-c alt_config_file]"
    "\n"
    "\n -h\tAffiche plus d'informations sur l'usage de QuickDCP."
    "\n -v\tAffiche la version installée de QuickDCP."
    "\n -V\tLance QuickDCP en mode verbeux (deboggage)."
    "\n -q\tLance QuickDCP en mode muet (scripts)."
    "\n -i\tChemin et nom du dossier où se trouvent les images à traiter."
    "\n -o\tChemin et nom du dossier destiné à recevoir les DCP terminés."
    "\n -c\tLance QuickDCP en utilisant le fichier de configuration alternatif 'alt_config_file'."
    "\n");
    exit(0);
}

