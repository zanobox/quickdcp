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
   @file    config.c
   @author  jean.sabatier@zanobox.net
   @brief   Manipulation of MXF files, for writing/packing as well as extracting/unpacking
 */
#include "qdcp_mxf.h"

/*
 *  MXF packing functions 
 */
qdcp_bool
write_j2k_mxf_reel(t_project * pj, quickdcp_ctx * qdcp) 
{	
		int		i;
		char	trgt_file[PATH_STR_LEN] = {0};

	/* MXF target file */
	sprintf(trgt_file, "%sv_%s.mxf", pj->tmp_dcp_dir, pj->name);


	/* get last modification of the file */
		time_t		last_rendering;
	
	if (check_file(trgt_file))
	{
		last_rendering = get_mod_time(trgt_file);
		
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
		{
			printf("Dernière modification du projet :\t%s\n", get_last_mod_string(pj->last_mod));
			printf("Dernier rendu MXF :\t\t%s\n", get_last_mod_string(last_rendering));
		}
		
		/* compare with last_mod of the project file (qdx) */
		if (last_rendering > pj->last_mod)
		{
			printf("%sLe fichier MXF a déjà été généré depuis la dernière modification du projet. \n"
						, CLI_INDENT);
			/* ask what to do */
			
			/* or do nothing ? */
			return QDCP_SUCCESS;
		}
	}

	check_dir(pj->tmp_dcp_dir, qdcp->verbosity);
	
	/* get the real file list, no matter what pj->nmax says (opendcp function) */
	pj->j2k_files = get_filelist(pj->j2k_path, "j2k,jp2,j2c");
	if (pj->j2k_files == NULL)
		warning("Liste de fichiers j2k introuvable", __FILE__, __LINE__);
		
	/* check the list */
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		for (i = 0; i < pj->nmax; i++)
		{
			printf("\r > > %d : %s\n", i, pj->j2k_files->files[i]);
		}
		printf("\n");
	}
	/* adjust nmax to the actual number of files 
	 * (if we want to force the process even with missing frames) */
	if (pj->j2k_files->nfiles < pj->nmax)
		pj->nmax = pj->j2k_files->nfiles;
	
	/* */
	int rc = ensure_sequential(pj->j2k_files->files, pj->j2k_files->nfiles);
	if (rc != QDCP_NO_ERROR) 
		warning("Filenames not sequential.", __FILE__, __LINE__);
		//~ warning("Filenames not sequential after %s.", pj->j2k_files->files[rc], __FILE__, __LINE__);

	/* in case of 3D we must get both sides (left and right) separately */
    if (pj->stereoscopic) {
		/* define and check_dir right_src_dir */
		pj->j2k_righteye_files = get_filelist(pj->j2k_3D_right, "j2c,j2k,jp2");
        if (pj->j2k_righteye_files->nfiles == 0) {
            warning("3D input detected, but missing right image input path", __FILE__, __LINE__);
        }
		/* */
		rc = ensure_sequential(pj->j2k_righteye_files->files, pj->j2k_righteye_files->nfiles);
		if (rc != QDCP_NO_ERROR) 
			warning("Filenames not sequential.", __FILE__, __LINE__);
    }
	

	/* write a jpeg2000 mxf reel with asdcp */

		pthread_t 		spin_thread;
		int				working = 1;
	
	/* pthread_create returns 0 on success */
	if (pthread_create(&spin_thread, NULL, spinner, &working))
	{
		warning("Thread failed", __FILE__, __LINE__);
		perror("pthread_create");
		return QDCP_FAILURE;
	}
	
	int class = get_file_essence_class(pj->j2k_files->files[0], 1);
	
	if (class != ACT_PICTURE)
	{
		warning("J2K -> MXF : Invalid input files", __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
		
	if (convert_j2k_to_mxf(pj, trgt_file) == QDCP_FAILURE )
	{
		pthread_cancel(spin_thread);

		warning("Could not create MXF file", __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	else 
	{
		pthread_cancel(spin_thread);
		printf("\r");

		printf("%s%s%s Fichier MXF terminé : %s%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, strip_path(trgt_file), CLI_COLOR);
			
		return QDCP_SUCCESS;
	}
}

qdcp_bool
write_wav_mxf_reel(t_project * pj, quickdcp_ctx * qdcp) 
{
	printf("Not implemented yet");

		char	src_file[PATH_STR_LEN] 	= {0};
		char	trgt_dir[PATH_STR_LEN] 	= {0};
		char	trgt_file[PATH_STR_LEN] = {0};

	check_dir(pj->tmp_dcp_dir, qdcp->verbosity);
	
	/* J2K source directory */
	sprintf(src_file, "%s%s.wav", pj->wav_path, pj->name);
	/* MXF target file */
	sprintf(trgt_file, "%sa_%s.mxf", pj->tmp_dcp_dir, pj->name);


	/* get last_mod of the mxf file */
		time_t		last_rendering;
	
	if (check_file(trgt_file))
	{
		last_rendering = get_mod_time(trgt_file);
		
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
		{
			printf("Dernière modification du projet :\t%d\n", get_last_mod_string(pj->last_mod));
			printf("Dernier rendu MXF :\t\t%d\n", get_last_mod_string(last_rendering));
		}

		/* compare with last_mod of the project file (qdx) */
		if (last_rendering > pj->last_mod)
		{
			printf("Le rendu de la séquence a déjà été effectué depuis la dernière modification du projet. \n");
			/* ask what to do */
			
			/* or do nothing ? */
			return QDCP_SUCCESS;
		}
	}
	
	

		pthread_t 		spin_thread;
		int				working = 1;
	
	/* pthread_create returns 0 on success */
	if (pthread_create(&spin_thread, NULL, spinner, &working))
	{
		warning("Thread failed", __FILE__, __LINE__);
		perror("pthread_create");
		return QDCP_FAILURE;
	}
	
	int class = get_file_essence_class(pj->j2k_files->files[0], 1);
	
	if (class != ACT_SOUND)
	{
		warning("CPM -> MXF : Invalid input files", __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
		
	if (convert_pcm_to_mxf(pj, trgt_file) == QDCP_FAILURE )
	{
		pthread_cancel(spin_thread);

		warning("Could not create MXF file", __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	else 
	{
		pthread_cancel(spin_thread);
		printf("\r");

		printf("%s%s%s Fichier MXF terminé : %s%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, strip_path(trgt_file), CLI_COLOR);
			
		return QDCP_SUCCESS;
	}

	return QDCP_SUCCESS;
}

qdcp_bool 
write_subs_mxf_reel(t_project * pj, quickdcp_ctx * qdcp) 
{
	printf("Not implemented yet");

	return QDCP_SUCCESS;
}


qdcp_bool
generate_ffmpeg_mxf_seq(t_project * pj, quickdcp_ctx * qdcp)
{
		char	trgt_mxf_file[PATH_STR_LEN];
		char	command[CMD_STR_LEN];

		pthread_t 		spin_thread;
		int 			working = 1;
	
	check_dir(pj->tmp_dcp_dir, 0);
	
	sprintf(trgt_mxf_file, "%sv_%s.mxf", pj->tmp_dcp_dir, pj->name); 
	
	sprintf(command, "ffmpeg -i %s %s", pj->seq_path, trgt_mxf_file);
	
	/* pthread_create returns 0 on success */
	if (pthread_create(&spin_thread, NULL, spinner, &working))
	{
		warning("Thread failed", __FILE__, __LINE__);
		perror("pthread_create");
		return QDCP_FAILURE;
	}

	if (system(command) < 0)
	{
		pthread_cancel(spin_thread);
		error_msg(NO_SHELL, __FILE__, __LINE__);
	}
	
	pthread_cancel(spin_thread);
	printf("\r");
	
	return QDCP_SUCCESS;
}

/*
 * MXF extracting functions 
 */

t_project *
dcp_analyze(char * path_to_dcp, quickdcp_ctx * qdcp) {
	
		t_project 	  *	dcp = calloc(1, sizeof(*dcp));
		int		 		class;
		t_file_list   * dcpfiles = new_file_list();
		file_obj	  * mxf;
		char			mxf_file[PATH_STR_LEN];
		char			buffer[80];
		
	
	/* read assetmap */
	
	
	/* list mxf contents of dcp directory */
	scan_source_directory(path_to_dcp, 0, "mxf", dcpfiles);
	
	/* for each reel found */
	mxf = dcpfiles->first;
	while (mxf != NULL)
	{
		sprintf(mxf_file, "%s%s", mxf->path, mxf->fname);
		/* get essence type */
			
		class = get_file_essence_class(mxf_file, 0);
		
		switch(class) {
			case ACT_PICTURE:
				extract_j2k_mxf_reel(mxf_file, dcp, qdcp);
				break;
							
			case ACT_SOUND:
				extract_wav_mxf_reel(mxf_file, qdcp);				
				break;
			
			case ACT_TIMED_TEXT:
				extract_subs_mxf_reel(mxf_file, qdcp);
				break;
			
			default:
				sprintf(buffer, "Could not determine essence class of %s", mxf_file);
				warning(buffer, __FILE__, __LINE__);
				break;
		}
		mxf = mxf->next;
	}
	/* return a quickdcp t_project */
	return dcp;
}

t_layer * 
extract_j2k_mxf_reel(const char * mxf_file, t_project * pj, quickdcp_ctx * qdcp) 
{	
	printf("Not implemented yet");
	
		char		trgt_dir[PATH_STR_LEN] 	= {0};
		char		trgt_file[PATH_STR_LEN] = {0};
		t_layer * 	ly = init_layer(pj);

	/* Sequence target directory */
	sprintf(trgt_dir, "%s%s", qdcp->mxf_dir, pj->name);

	return ly;
}

t_wav_track * 
extract_wav_mxf_reel(const char * mxf_file, quickdcp_ctx * qdcp) 
{
	printf("Not implemented yet");

		t_wav_track * track = NULL;
	init_wav_track(track, qdcp);
	
	save_wav_subproject(track, qdcp);
		
	return track;
}

t_sub_track * 
extract_subs_mxf_reel(const char * mxf_file, quickdcp_ctx * qdcp) {
	
		t_sub_track * st = NULL;
	
	init_sub_track(st, qdcp);

	printf("Not implemented yet");

	return st;
}
