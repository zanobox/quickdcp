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
   @brief   Build Digital Cinema Packages in command line interface.
*/

#include "qdcp_config.h"


/*
 * Init config struct with default projects values
 */
qdcp_bool   
init_default_config(quickdcp_ctx * qdcp)
{
		t_cfg	* cfg = calloc(1, sizeof(*cfg));

	if (cfg == NULL)
	{
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	}
	qdcp->cfg = cfg;
	
	/* Declare defaults values *
	 * ----------------------- */
	
	qdcp->cfg->format 			= "Flat";
	qdcp->cfg->resolution 		= "2K";
	qdcp->cfg->framerate 		= 24;
	qdcp->cfg->laps 			= 10.0;
	qdcp->cfg->margin 			= 0;
	sprintf(qdcp->cfg->foreground, "#ffffff");
	sprintf(qdcp->cfg->background, "#000000");

	qdcp->cfg->fondu_d 			= FONDU_AU_NOIR;
	qdcp->cfg->fondu_f 			= FONDU_AU_NOIR;
	qdcp->cfg->fondulaps_d 		= 1.5; 	
	qdcp->cfg->fondulaps_f 		= 1.5;
	sprintf(qdcp->cfg->fondu_cache_color_d, "#000000");
	sprintf(qdcp->cfg->fondu_cache_color_f, "#000000");
	qdcp->cfg->fondu_iris_cx_d 	= 50.0;
	qdcp->cfg->fondu_iris_cy_d 	= 50.0;
	qdcp->cfg->fondu_iris_cx_f 	= 50.0;
	qdcp->cfg->fondu_iris_cy_f 	= 50.0;
	
	qdcp->cfg->issuer 			= "QuickDCP";
	qdcp->cfg->issuer_short 	= "QDCP";
	qdcp->cfg->category 		= CAT_TST;
	qdcp->cfg->country_code 	= "FR";
	qdcp->cfg->lang_code 		= "FR";
	qdcp->cfg->subtitle_code 	= "XX";
	qdcp->cfg->laboratory		= "ZNBX";
	qdcp->cfg->dcp_type			= "SMPTE";
	qdcp->cfg->version			= "OV";

	qdcp->cfg->name_category 	= QDCP_YES;
	qdcp->cfg->name_3D		 	= QDCP_NO;
	qdcp->cfg->name_format 		= QDCP_YES;
	qdcp->cfg->name_langs 		= QDCP_NO;
	qdcp->cfg->name_country 	= QDCP_YES;
	qdcp->cfg->name_audio 		= QDCP_NO;
	qdcp->cfg->name_resol 		= QDCP_YES;
	qdcp->cfg->name_issuer 		= QDCP_YES;
	qdcp->cfg->name_date 		= QDCP_YES;
	qdcp->cfg->name_lab 		= QDCP_NO;
	qdcp->cfg->name_version 	= QDCP_NO;
	
		char  	inputdir[PATH_STR_LEN], outputdir[PATH_STR_LEN];
		char	logdir[PATH_STR_LEN], archives[PATH_STR_LEN];
	sprintf(inputdir, "%s%sQuickDCP%s", HOME_DIR, SEPARATOR, SEPARATOR);
	sprintf(outputdir, "%s%sDCP%s", HOME_DIR, SEPARATOR, SEPARATOR);
	sprintf(logdir, "%s%sQuickDCP%s%s%s", HOME_DIR, SEPARATOR, SEPARATOR, LOG_DIR, SEPARATOR);
	sprintf(archives, "grenier%s", SEPARATOR);
	qdcp->workspace				= strdup(inputdir);
	qdcp->dcp_out				= strdup(outputdir);
	//~ qdcp->log_dir				= strdup(logdir);
	qdcp->archives				= strdup(archives);
	
	qdcp->preview 				= QDCP_YES;
	qdcp->make_dcp 				= QDCP_YES;
	qdcp->autoclean 			= "wash";
	qdcp->beep 					= QDCP_YES;
	qdcp->threads 				= 1;
	qdcp->loglevel 				= 2;
	qdcp->opendcp_loglevel 		= 0;
	qdcp->ffmpeg_loglevel 		= 0;
	qdcp->img2j2k 				= "imagick";
	qdcp->j2k2mxf 				= "asdcp";
	qdcp->lang 					= "fr";
	qdcp->signature				= 0;
	
	qdcp->show_toolbar 			= QDCP_YES;
	qdcp->show_statusbar 		= QDCP_YES;
	qdcp->show_terminal 		= QDCP_YES;
	qdcp->win_x 				= 0;
	qdcp->win_y 				= 0;
	qdcp->win_w 				= 969;
	qdcp->win_h 				= 563;
	
	qdcp->cfg->line_width 		= 1;
	qdcp->cfg->line_style 		= "solid";
	qdcp->cfg->line_caps 		= "arrondi";
	qdcp->cfg->line_join 		= "arrondi";
	sprintf(qdcp->cfg->line_color, "#999999");
	sprintf(qdcp->cfg->fill_color, "#ff5522");
	
	qdcp->cfg->font_desc		= "Comfortaa 40";
	qdcp->cfg->font_size 		= 40;
	sprintf(qdcp->cfg->text_color, "#ffffff");
	qdcp->cfg->text_style 		= "normal";
	qdcp->cfg->text_variant 	= "normal";
	qdcp->cfg->text_align 		= "center";
	qdcp->cfg->text_weight 		= "normal";
	qdcp->cfg->text_stretch 	= "normal";
	qdcp->cfg->interline 		= 20;
	qdcp->cfg->text_underline 	= "none";
	qdcp->cfg->text_strike 		= QDCP_NO;
	qdcp->cfg->text_script 		= "latin";
	qdcp->cfg->text_lang 		= "fr";
	
	qdcp->cfg->mix_type 		= WAVMIX_REPARTITION;
	qdcp->cfg->encoding			= 0;
	qdcp->cfg->bitrate			= 64000;
	qdcp->cfg->audio_format		= CH_5_1;
	qdcp->cfg->samplerate		= KHZ_48;
	qdcp->cfg->sample_fmt		= AV_SAMPLE_FMT_S32;
	qdcp->cfg->channel_layout	= AV_CH_LAYOUT_5POINT1;
	qdcp->cfg->codec_id			= AV_CODEC_ID_PCM_S24LE;  
	//~ qdcp->cfg->codec_id			= AV_CODEC_ID_PCM_S24DAUD;  /**< FFmpeg codec ID from AVCodecID enumeration, as defined in avcodec.h */
	qdcp->cfg->volume			= 1.0;						/**< @todo to be clarified... */
	
	qdcp->cfg->vid2flat 		= QDCP_YES;
	qdcp->cfg->vid_framerate 	= "inconnu";
	qdcp->cfg->vid_format 		= "inconnu";

	return QDCP_SUCCESS;
}

/*
 * Read or create a config file and pass it to a config_t structure
 */
qdcp_bool read_config_file(const char * config_file, quickdcp_ctx * qdcp)
{
		FILE		*	config_fd = NULL;
		dictionary	*	ini_dict;
	
		int				i;
		
	/* ---------------------------------- *
	 * Try to open the configuration file *
	 * ---------------------------------- */
    config_fd = fopen(config_file, "r");
    if(config_fd == NULL)
    {
		/* ---------------------------------- *
		 * Try to write the configuration file *
		 * ---------------------------------- */
        write_config_file(config_file, qdcp);
        
        config_fd = fopen(config_file, "r");
		if(config_fd == NULL){
			error_msg(NO_QDCP_CONFIG, __FILE__, __LINE__);
		}
    }
    
	/* --------------------------------- *
	 * Parse the .ini configuration file *
	 * --------------------------------- */  
	fprintf(stdout, "Quickdcp :  - Lecture du fichier de configuration\n");
	
	ini_dict = iniparser_load(config_file);
	if (ini_dict == NULL)
	{
		error_msg(CANT_READ_FILE, __FILE__, __LINE__);
	}
			
	fclose(config_fd);
    
    /* debug iniparser */
    //iniparser_dump(ini_dict, stderr);

	/* application settings */
	qdcp->threads 	= iniparser_getint(ini_dict, 			"qdcp:threads", 1);
	qdcp->beep 		= iniparser_getboolean(ini_dict, 		"qdcp:beep", 1);
	qdcp->preview 	= iniparser_getboolean(ini_dict, 		"qdcp:preview", 1);
	qdcp->make_dcp 	= iniparser_getboolean(ini_dict,		"qdcp:make_dcp", 1);
	qdcp->img2j2k 	= iniparser_getstring(ini_dict, 		"qdcp:img2j2k", NULL);
	qdcp->j2k2mxf 	= iniparser_getstring(ini_dict, 		"qdcp:j2k2mxf", NULL);
	qdcp->autoclean = iniparser_getstring(ini_dict, 		"qdcp:autoclean", NULL);
	qdcp->archives 	= iniparser_getstring(ini_dict, 		"qdcp:archives", NULL);
	qdcp->lang 		= iniparser_getstring(ini_dict, 		"qdcp:lang", NULL);
	qdcp->signature	= iniparser_getboolean(ini_dict,		"qdcp:signature", 0);
	qdcp->loglevel	= iniparser_getint(ini_dict, 			"qdcp:loglevel", 0);
	qdcp->opendcp_loglevel = iniparser_getint(ini_dict, 	"qdcp:opendcp_loglevel", 0);
	qdcp->ffmpeg_loglevel = iniparser_getint(ini_dict, 		"qdcp:ffmpeg_loglevel", 0);

	/* graphical interface values */
	qdcp->show_toolbar 	= iniparser_getboolean(ini_dict,	"interface:show_toolbar", 1);
	qdcp->show_statusbar = iniparser_getboolean(ini_dict,	"interface:show_statusbar", 1);
	qdcp->show_terminal	= iniparser_getboolean(ini_dict,	"interface:show_terminal", 1);
	qdcp->win_x 	= iniparser_getint(ini_dict,			"interface:win_x", 1);
	qdcp->win_y 	= iniparser_getint(ini_dict,			"interface:win_y", 1);
	qdcp->win_w 	= iniparser_getint(ini_dict,			"interface:win_w", 1);
	qdcp->win_h 	= iniparser_getint(ini_dict,			"interface:win_h", 1);
	
	/* directories */
	sprintf(qdcp->cfg_file, "%s", config_file);
	qdcp->workspace = iniparser_getstring(ini_dict, 		"qdcp:workspace", NULL);
	qdcp->dcp_out 	= iniparser_getstring(ini_dict, 		"qdcp:dcp_out", NULL);
	sprintf(qdcp->proj_dir, "%s%s%s", qdcp->workspace, PROJ_DIR, SEPARATOR);
	sprintf(qdcp->img_dir, "%s%s%s", qdcp->workspace, TMP_IMG, SEPARATOR);
	sprintf(qdcp->vid_dir, "%s%s%s", qdcp->workspace, TMP_VID, SEPARATOR);
	sprintf(qdcp->seq_dir, "%s%s%s", qdcp->workspace, TMP_SEQ, SEPARATOR);
	sprintf(qdcp->j2k_dir, "%s%s%s", qdcp->workspace, TMP_J2K, SEPARATOR);
	sprintf(qdcp->wav_dir, "%s%s%s", qdcp->workspace, TMP_WAV, SEPARATOR);
	sprintf(qdcp->sub_dir, "%s%s%s", qdcp->workspace, TMP_SUB, SEPARATOR);
	sprintf(qdcp->mxf_dir, "%s%s%s", qdcp->workspace, TMP_MXF, SEPARATOR);
	sprintf(qdcp->log_dir, "%s%s%s", qdcp->workspace, LOG_DIR, SEPARATOR);
	sprintf(qdcp->cfg_dir, "%s%s%s%s", HOME_DIR, SEPARATOR, CFG_DIR, SEPARATOR);
		
	/* projects default values */
	qdcp->cfg->laps 			= iniparser_getdouble(ini_dict, 	"projects:laps", 1.0);
	qdcp->cfg->format 			= iniparser_getstring(ini_dict, 	"projects:format", NULL);
	qdcp->cfg->resolution 		= iniparser_getstring(ini_dict, 	"projects:resolution", NULL);
	qdcp->cfg->framerate 		= iniparser_getint(ini_dict, 		"projects:framerate", 24);
	sprintf(qdcp->cfg->foreground, "#%s", iniparser_getstring(ini_dict, "projects:foreground", NULL));
	sprintf(qdcp->cfg->background, "#%s", iniparser_getstring(ini_dict, "projects:background", NULL));
	qdcp->cfg->margin 			= iniparser_getint(ini_dict, 		"projects:margin", 0);
	for (i = 0; i <= FONDU_IRIS; i++)
	{
		if (strcmp(qdcp->fondus[i], iniparser_getstring(ini_dict, "projects:fondu_d", NULL)) == 0)
			qdcp->cfg->fondu_d = i; 
		if (strcmp(qdcp->fondus[i], iniparser_getstring(ini_dict, "projects:fondu_f", NULL)) == 0)
			qdcp->cfg->fondu_f = i; 
	}
	//~ qdcp->cfg->fondu_d 			= iniparser_getstring(ini_dict, 	"projects:fondu_d", NULL);
	//~ qdcp->cfg->fondu_f 			= iniparser_getstring(ini_dict,		"projects:fondu_f", NULL);
	qdcp->cfg->fondulaps_d 		= iniparser_getdouble(ini_dict, 	"projects:fondulaps_d", 1);
	qdcp->cfg->fondulaps_f 		= iniparser_getdouble(ini_dict, 	"projects:fondulaps_f", 1);
	sprintf(qdcp->cfg->fondu_cache_color_d, "#%s", iniparser_getstring(ini_dict, "projects:fondu_cache_color_d", NULL));
	sprintf(qdcp->cfg->fondu_cache_color_f, "#%s", iniparser_getstring(ini_dict, "projects:fondu_cache_color_f", NULL));
	qdcp->cfg->fondu_iris_cx_d 	= iniparser_getdouble(ini_dict, 	"projects:fondu_iris_cx_d", 1.0);
	qdcp->cfg->fondu_iris_cy_d 	= iniparser_getdouble(ini_dict, 	"projects:fondu_iris_cy_d", 1.0);
	qdcp->cfg->fondu_iris_cx_f 	= iniparser_getdouble(ini_dict, 	"projects:fondu_iris_cx_f", 1);
	qdcp->cfg->fondu_iris_cy_f 	= iniparser_getdouble(ini_dict, 	"projects:fondu_iris_cy_f", 1);
	/* vectorial shapes */
	qdcp->cfg->line_width 		= iniparser_getint(ini_dict, 		"shapes:line_width", 1);
	sprintf(qdcp->cfg->line_color, "#%s", iniparser_getstring(ini_dict, "shapes:line_color", NULL));
	sprintf(qdcp->cfg->fill_color, "#%s", iniparser_getstring(ini_dict, "shapes:fill_color", NULL));
	qdcp->cfg->line_style 		= iniparser_getstring(ini_dict, 	"shapes:line style", NULL);
	qdcp->cfg->line_caps 		= iniparser_getstring(ini_dict, 	"shapes:line_caps", NULL);
	qdcp->cfg->line_join 		= iniparser_getstring(ini_dict, 	"shapes:line_join", NULL);
	qdcp->cfg->dash_arrays 		= iniparser_getstring(ini_dict, 	"shapes:dash_arrays", NULL);
	/* text (text) */
	qdcp->cfg->font_desc 		= iniparser_getstring(ini_dict, 	"text:font_desc", NULL);
	qdcp->cfg->font_size 		= iniparser_getint(ini_dict, 		"text:font_size", 10);
	sprintf(qdcp->cfg->text_color, "#%s", iniparser_getstring(ini_dict, "text:text_color", NULL));
	qdcp->cfg->text_style 		= iniparser_getstring(ini_dict, 	"text:text_style", NULL);
	qdcp->cfg->text_variant		= iniparser_getstring(ini_dict, 	"text:text_variant", NULL);
	qdcp->cfg->text_align 		= iniparser_getstring(ini_dict, 	"text:text_align", NULL);
	qdcp->cfg->text_weight 		= iniparser_getstring(ini_dict, 	"text:text_weight", NULL);
	qdcp->cfg->text_stretch		= iniparser_getstring(ini_dict, 	"text:text_stretch", NULL);
	qdcp->cfg->interline 		= iniparser_getint(ini_dict, 		"text:interline", 0);
	qdcp->cfg->text_underline 	= iniparser_getstring(ini_dict, 	"text:underline", NULL);
	qdcp->cfg->text_strike 		= iniparser_getboolean(ini_dict, 	"text:text_strike", 0);
	qdcp->cfg->text_script 		= iniparser_getstring(ini_dict, 	"text:text_script", NULL);
	qdcp->cfg->text_lang 		= iniparser_getstring(ini_dict, 	"text:text_lang", NULL);
	/* audio */
	for (i = 0; i <= WAVMIX_INVERSION; i++)
	{
		if (strcmp(qdcp->mixs[i], iniparser_getstring(ini_dict, 	"audio:mix_type", NULL)) == 0)
			qdcp->cfg->mix_type = i; 
	}
	qdcp->cfg->audio_format 	= iniparser_getint(ini_dict, 		"audio:audio_format", 1);	
	qdcp->cfg->samplerate		= iniparser_getint(ini_dict, 		"audio:samplerate", 1);
	qdcp->cfg->encoding			= iniparser_getint(ini_dict,		"audio:encoding", 1);
	/* video */
	qdcp->cfg->vid2flat 		= iniparser_getboolean(ini_dict, 	"video:vid2flat", 1);
	qdcp->cfg->vid_framerate 	= iniparser_getstring(ini_dict, 	"video:vid_framerate", NULL);
	qdcp->cfg->vid_format 		= iniparser_getstring(ini_dict,		"video:vid_format", NULL);
	/* dcp_name */
	for (i = 0; i < CAT_N_CATEGORIES; i++)
	{
		if (strcmp(qdcp->short_cats[i], iniparser_getstring(ini_dict, "dcpname:category", NULL)) == 0)
		{
			qdcp->cfg->category = i; 
		}
		else if (strcmp(qdcp->dcp_categories[i], iniparser_getstring(ini_dict, "dcpname:category", NULL)) == 0)
		{
			qdcp->cfg->category = i; 
		}
	}
	qdcp->cfg->country_code 	= iniparser_getstring(ini_dict, 	"dcpname:country_code", NULL);
	qdcp->cfg->lang_code 		= iniparser_getstring(ini_dict, 	"dcpname:lang_code", NULL);
	qdcp->cfg->subtitle_code 	= iniparser_getstring(ini_dict, 	"dcpname:subtitle_code", NULL);
	qdcp->cfg->issuer 			= iniparser_getstring(ini_dict,		"dcpname:issuer", NULL);
	qdcp->cfg->issuer_short 	= iniparser_getstring(ini_dict,		"dcpname:issuer_short", NULL);
	qdcp->cfg->laboratory 		= iniparser_getstring(ini_dict,		"dcpname:laboratory", NULL);
	qdcp->cfg->version 			= iniparser_getstring(ini_dict,		"dcpname:version", NULL);
	qdcp->cfg->dcp_type 		= iniparser_getstring(ini_dict,		"dcpname:dcp_type", NULL);

	qdcp->cfg->name_category 	= iniparser_getboolean(ini_dict, 	"dcpname:name_category", 1);
	qdcp->cfg->name_3D		 	= iniparser_getboolean(ini_dict, 	"dcpname:name_3D", 1);
	qdcp->cfg->name_format 		= iniparser_getboolean(ini_dict, 	"dcpname:name_format", 1);
	qdcp->cfg->name_langs 		= iniparser_getboolean(ini_dict, 	"dcpname:name_langs", 0);
	qdcp->cfg->name_country 	= iniparser_getboolean(ini_dict, 	"dcpname:name_country", 0);
	qdcp->cfg->name_audio 		= iniparser_getboolean(ini_dict, 	"dcpname:name_audio", 0);
	qdcp->cfg->name_resol 		= iniparser_getboolean(ini_dict, 	"dcpname:name_resol", 1);
	qdcp->cfg->name_issuer 		= iniparser_getboolean(ini_dict, 	"dcpname:name_issuer", 1);
	qdcp->cfg->name_date 		= iniparser_getboolean(ini_dict, 	"dcpname:name_date", 0);
	qdcp->cfg->name_lab 		= iniparser_getboolean(ini_dict, 	"dcpname:name_lab", 0);
	qdcp->cfg->name_version 	= iniparser_getboolean(ini_dict, 	"dcpname:name_version", 0);
	qdcp->cfg->name_dcptype 	= iniparser_getboolean(ini_dict, 	"dcpname:name_dcptype", 0);
	 

	return EXIT_SUCCESS;
}

/*
 * Display all projects related default parameters
 */
void show_default_params(quickdcp_ctx * qdcp, qdcp_bool show_all)
{
	printf("\n");
	printf(" > Dossier d'entrée  : %s\n", 	qdcp->workspace);
	printf(" > Dossier de sortie : %s\n", 	qdcp->dcp_out);
	printf(" > Rendu de l'aperçu  : %s\n", 	qdcp->yes_no[qdcp->preview]);
	printf("\n");
    printf(" > Durée :\t\t%.2f\"\n", 		qdcp->cfg->laps);
    printf(" > Format :\t\t%s\n", 			qdcp->cfg->format);
    printf(" > Catégorie :\t\t%s\n", 		qdcp->dcp_categories[qdcp->cfg->category]);
    if (show_all)
    {
		printf(" > Résolution :\t\t%s\n",	qdcp->cfg->resolution);
		printf(" > Framerate :\t\t%d\n", 	qdcp->cfg->framerate);
		printf("\n");
		printf(" > Couleur de fond :\t#%s\t(couleur)\n", qdcp->cfg->background);	/*color*/
		printf(" > Premier plan :\t#%s\t(couleur)\n",	 qdcp->cfg->foreground);	/*color*/
		printf(" > Marge auto :\t\t%d\n", 	qdcp->cfg->margin);
	}
	printf("\n");
    printf(" > Fondu de début :\t%s\n", 	qdcp->fondus[qdcp->cfg->fondu_d]);
    printf(" > Fondu de fin :\t%s\n",	 	qdcp->fondus[qdcp->cfg->fondu_f]);
    if (show_all)
    {
		printf(" > Fondu début :\t%.1f\"\n", 	qdcp->cfg->fondulaps_d);
		printf(" > Fondu fin :\t\t%.1f\"\n",  qdcp->cfg->fondulaps_f);
		printf(" > Cache fondu d. :\t#%s\t(couleur)\n", qdcp->cfg->fondu_cache_color_d);		/*color*/
		printf(" > Cache fondu f. :\t#%s\t(couleur)\n", qdcp->cfg->fondu_cache_color_f);		/*color*/
		/* values of fondu iris */
		if (qdcp->cfg->fondu_d == FONDU_IRIS)
		{
			printf(" > Centre fondu iris (début) x : %.1f%%\n",  qdcp->cfg->fondu_iris_cx_d);
			printf(" > Centre fondu iris (début) y : %.1f%%\n",  qdcp->cfg->fondu_iris_cy_d);
		}
		if (qdcp->cfg->fondu_f == FONDU_IRIS)
		{
			printf(" > Centre fondu iris (fin) x : %.1f%%\n",  qdcp->cfg->fondu_iris_cx_f);
			printf(" > Centre fondu iris (fin) y : %.1f%%\n",  qdcp->cfg->fondu_iris_cy_f);
		}
		
	}
	printf("\n");
			
}

/*
 * Write a new config file from current values
 */
qdcp_bool write_config_file(char const config_path[], quickdcp_ctx * qdcp)
{
    FILE* config_file = NULL;
    
	/* Open a new empty file *
	 * --------------------- */
	printf("Ecriture du fichier de configuration");
    
    config_file = fopen(config_path, "a");
    if(config_file == NULL)
    {
		char * config_failed = "Echec de la création du fichier de configuration";
		error_msg(config_failed, __FILE__, __LINE__);
	}
 
	 
	/* Print values in file *
	 * ---------------------- */
	fprintf(config_file,
	"\n## Ceci est le fichier de configuration de QuickDCP"
	"\n## Sa syntaxe est identique à celle de tous les fichiers '.ini':"
	"\n## des sections dont le nom est entre crochets, suivi de variales"
	"\n## auxquelles sont affectées une valeur."
	"\n## Le caractère '#' rend tout ce qui se trouve à sa droite sur la même ligne, "
	"\n## invisible au programme qui le lira."
	"\n## Une variable ne se trouvant pas directement sous le nom de sa section ne sera"
	"\n## lue. Si deux valeurs différentes sont données à la même variable dans la même"
	"\n## section, la plus proche de la fin du fichier sera retenue." 
	"\n "
	"\n"
	"\n"
	"\n# [projects]"
	"\n# Dans la section [projects] sont réunies les valeurs par défaut utilisées lors de la création d'un nouveau projet QDP."
	"\n#"
	"\n# Les COULEURS sont exprimées par une séquence (concaténée) de 3 nombres hexadécimaux allant chacun de 0 à FFF (=4096)."
	"\n# Il s'agit de la même notation qu'en HTML par exemple et elle est également utilisée dans les principaux logiciels"
	"\n# de traitement d'image, tels que GIMP. Dans la mesure où le symbole '#' dans ce fichier est réservé aux lignes de "
	"\n# commentaires, nous notons les couleurs sans le '#' initial habituel."
	"\n#"
	"\n# 'framerate' — fréquence d'images par seconde, peut prendre les valeurs entières suivantes :"
	"\n# 23, 24, 25, 30, 48, 50, 60."
	"\n# Dans le cas de la 3D, le taux sera implicitement doublé, c'est donc le taux pour chaque"
	"\n# oeil qui doit être indiqué ici."
	"\n#"
	"\n# Si 'signature' est activé, les DCP produits seront signés avec la clef de cryptage RSA de QuickDCP (en fait celle d'OpenDCP provisoirement)."
	"\n"
	"\n[projects]"
	"\nformat = %s", 			qdcp->cfg->format);
	fprintf(config_file,
	"\nresolution = %s", 		qdcp->cfg->resolution);
	fprintf(config_file,
	"\nframerate = %d", 		qdcp->cfg->framerate);
	fprintf(config_file,
	"\nlaps = %.2f", 			qdcp->cfg->laps);
	fprintf(config_file,
	"\nbackground = %s   # (couleur RVB)", qdcp->cfg->background);
	fprintf(config_file,
	"\nforeground = %s   # (couleur RVB)", qdcp->cfg->foreground);
	fprintf(config_file,
	"\nmargin = %d", 			qdcp->cfg->margin);
	fprintf(config_file,
	"\n# Valeurs acceptées pour les fondus :"
	"\n# %d, %d, %d", FONDU_AUCUN, FONDU_AU_NOIR, FONDU_IRIS);
	fprintf(config_file,
	"\nfondu_d = %s", 			qdcp->fondus[qdcp->cfg->fondu_d]);
	fprintf(config_file,
	"\nfondu_f = %s", 			qdcp->fondus[qdcp->cfg->fondu_f]);
	fprintf(config_file,
	"\n# Durée des fondus,  en secondes :"
	"\nfondulaps_d = %.1f", 	qdcp->cfg->fondulaps_d);
	fprintf(config_file,
	"\nfondulaps_f = %.1f", 	qdcp->cfg->fondulaps_f);
	fprintf(config_file,
	"\n# Couleur du cache des fondus :"
	"\nfondu_cache_color_d = %s   # (couleur RVB)", qdcp->cfg->fondu_cache_color_d);
	fprintf(config_file,
	"\nfondu_cache_color_f = %s   # (couleur RVB)", qdcp->cfg->fondu_cache_color_f);
	fprintf(config_file,
	"\n# Position du centre (en %%) pour le fondu iris (début et fin)"
	"\nfondu_iris_cx_d = %.1f", qdcp->cfg->fondu_iris_cx_d);
	fprintf(config_file,
	"\nfondu_iris_cy_d = %.1f", qdcp->cfg->fondu_iris_cy_d);
	fprintf(config_file,
	"\nfondu_iris_cx_f = %.1f", qdcp->cfg->fondu_iris_cx_f);
	fprintf(config_file,
	"\nfondu_iris_cy_f = %.1f", qdcp->cfg->fondu_iris_cy_f);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n# [dcpname]"
	"\n# Les champs suivants déterminent si l'élément concerné doit "
	"\n# entrer ou non dans la composition du nom générique de DCP."
	"\n# Par exemple :"
	"\n#   MonCarton_F_2K"
	"\n#   MonCarton_PSA_S_JA-XX_FR_51_4K_QDCP_20160309_MonLabo_SMPTE_OV"
	"\n#"
	"\n# Category should be one of the following :"
	"\n#  TST (test),"
	"\n#  PSA (psa = public social announcement),"
	"\n#  XSN (transitional),"
	"\n#  POL (policy),"
	"\n#  TSR (teaser),"
	"\n#  ADV (advertisement),"
	"\n#  TLR (trailer),"
	"\n#  SHR (short) or"
	"\n#  FTR (feature)."
	"\n"
	"\n"
	"\n[dcpname]"
	"\nissuer = %s", 			qdcp->cfg->issuer);
	fprintf(config_file,
	"\nissuer_short = %s", 		qdcp->cfg->issuer_short);
	fprintf(config_file,
	"\ncategory = %s", 			qdcp->dcp_categories[qdcp->cfg->category]);
	fprintf(config_file,
	"\ncountry_code = %s", 		qdcp->cfg->country_code);
	fprintf(config_file,
	"\nlang_code = %s", 		qdcp->cfg->lang_code);
	fprintf(config_file,
	"\nsubtitle_code = %s", 	qdcp->cfg->subtitle_code);
	fprintf(config_file,
	"\nlaboratory = %s", 		qdcp->cfg->laboratory);
	fprintf(config_file,
	"\ndcp_type = %s",		 	qdcp->cfg->dcp_type);
	fprintf(config_file,
	"\nversion = %s",		 	qdcp->cfg->version);
	fprintf(config_file,
	"\n"
	"\nname_category = %d", 	qdcp->cfg->name_category);
	fprintf(config_file,
	"\nname_3D = %d",		 	qdcp->cfg->name_3D);
	fprintf(config_file,
	"\nname_format = %d", 		qdcp->cfg->name_format);
	fprintf(config_file,
	"\nname_langs = %d", 		qdcp->cfg->name_langs);
	fprintf(config_file,
	"\nname_country = %d", 		qdcp->cfg->name_country);
	fprintf(config_file,
	"\nname_audio = %d", 		qdcp->cfg->name_audio);
	fprintf(config_file,
	"\nname_resol = %d", 		qdcp->cfg->name_resol);
	fprintf(config_file,
	"\nname_issuer = %d", 		qdcp->cfg->name_issuer);
	fprintf(config_file,
	"\nname_date = %d", 		qdcp->cfg->name_date);
	fprintf(config_file,
	"\nname_lab = %d",		 	qdcp->cfg->name_lab);
	fprintf(config_file,
	"\nname_dcptype = %d",		qdcp->cfg->name_dcptype);
	fprintf(config_file,
	"\nname_version = %d",		qdcp->cfg->name_version);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n# [qdcp]"
	"\n#  Cette section définit une série de variables utiles au fonctionnement de QuickDCP,"
	"\n#  vous permettant d'en modifier le comportement selon vos préférences."
	"\n"
	"\n#  Produire un DCP et ou une séquence d'aperçu"
	"\n#  Si make_dcp = no, la valeur de preview sera forcée à yes,"
	"\n#  et seule une séquence d'aperçu sera produite."
	"\n#"
	"\n#  Set quickdcp log level : 0 = quiet, 1 = Error, 2 = warn, 3 = info, 4 = debug"  
	"\n#"
	"\n#  Set opendcp log level : 0 = quiet, 1 = Error, 2 = warn, 3 = info, 4 = debug"  
	"\n#"
	"\n#  Set ffmpeg log level : "
	"\n#  -8 = quiet, 0 = panic, 8 = fatal, 16 = error, 24 = warning, 32 = info, 40 = verbose, 48 = debug"
	"\n#"
	"\n# Conversion methods : "
	"\n# Choose a utility to convert an image into jpeg2000"
	"\n# Default is imagemagick on recent platforms (as in debian-jessie, 2015)"
	"\n# Former versions of IM use jasper whick is said to present security holes"
	"\n# openjpeg can be a good fallback, or opendcp which ships its own j2k encoders."
	"\n# img2j2k : imagick | openjpeg | opendcp | jasper"
	"\n# Choose a utility to pack a jpeg2000 sequence into mxf"
	"\n# Default is asdcp proper, with a fallback onto opendcp when installed (which uses its own port of asdcp)"
	"\n# j2k2mxf : asdcp | opendcp"
	"\n"
	"\n"
	"\n[qdcp]"
	"\nworkspace = %s", 	qdcp->workspace);
	fprintf(config_file,
	"\ndcp_out = %s", 		qdcp->dcp_out);
	fprintf(config_file,
	"\narchives = %s", 		qdcp->archives);
	fprintf(config_file,
	"\n## Produire un aperçu de séquence en png à l'échelle 1/4"
	"\npreview = %d", 		qdcp->preview);
	fprintf(config_file,
	"\nmake_dcp = %d", 		qdcp->make_dcp);
	fprintf(config_file,
	"\nautoclean = %s", 	qdcp->autoclean);
	fprintf(config_file,
	"\nbeep = %d", 			qdcp->beep);
	fprintf(config_file,
	"\nthreads = %d", 		qdcp->threads);
	fprintf(config_file,
	"\nverbosity = %d", 	qdcp->verbosity);
	fprintf(config_file,
	"\nloglevel = %d", 		qdcp->loglevel);
	fprintf(config_file,
	"\nopendcp_loglevel = %d", qdcp->opendcp_loglevel);
	fprintf(config_file,
	"\nffmpeg_loglevel = %d", qdcp->ffmpeg_loglevel);
	fprintf(config_file,
	"\nimg2j2k = %s", qdcp->img2j2k);
	fprintf(config_file,
	"\nj2k2mxf = %s", qdcp->j2k2mxf);
	fprintf(config_file,
	"\nsignature = %d", 	qdcp->signature);
	fprintf(config_file,
	"\nlang = %s", 			qdcp->lang);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n[audio]"
	"\n# mix_type: Repartition | Mixage | Inversion"
	"\nmix_type = %s", 			qdcp->mixs[qdcp->cfg->mix_type]);
	fprintf(config_file,
	"\n# channels: Mono | Stereo | 5.1 | 7.1"
	"\naudio_format = %s", 		qdcp->aud_formats[qdcp->cfg->audio_format]);
	fprintf(config_file,
	"\nsamplerate = %d", 		qdcp->cfg->samplerate);
	fprintf(config_file,
	"\nencoding = %d", 			qdcp->cfg->encoding);
	//~ int		nchannels;		/** < number of channels (should be inherited from project) */
	//~ int		precision;		/** < precision, bit/sample, bitdepth = 24 */
	//~ int		volume;
	//~ float	samplerate;		/** < samples per second (48000 or 96000/s) */
	//~ int		samples;
	//~ int		encoding;
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n[video]"
	"\n# vid2flat : 1920x1080 -> 1998x1080"
	"\nvid2flat = %d", 			qdcp->cfg->vid2flat);
	fprintf(config_file,
	"\nvid_framerate = %s", 	qdcp->cfg->vid_framerate);
	fprintf(config_file,
	"\nvid_format = %s", 		qdcp->cfg->vid_format);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n[shapes]"
	"\nline_width = %d", 	qdcp->cfg->line_width);
	fprintf(config_file,
	"\nline_style = %s", 	qdcp->cfg->line_style);
	fprintf(config_file,
	"\nline_caps = %s", 	qdcp->cfg->line_caps);
	fprintf(config_file,
	"\nline_join = %s", 	qdcp->cfg->line_join);
	fprintf(config_file,
	"\nline_color = %s   # (couleur RVB)", qdcp->cfg->line_color);
	fprintf(config_file,
	"\nfill_color = %s   # (couleur RVB)", qdcp->cfg->fill_color);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n[text]"
	"\nfont_desc = %s", 	qdcp->cfg->font_desc);
	fprintf(config_file,
	"\nfont_size = %d", 	qdcp->cfg->font_size);
	fprintf(config_file,
	"\ntext_color = %s   # (couleur RVB)", qdcp->cfg->text_color);
	fprintf(config_file,
	"\ntext_style = %s", 	qdcp->cfg->text_style);
	fprintf(config_file,
	"\ntext_variant = %s", 	qdcp->cfg->text_variant);
	fprintf(config_file,
	"\ntext_align = %s", 	qdcp->cfg->text_align);
	fprintf(config_file,
	"\ntext_weight = %s", 	qdcp->cfg->text_weight);
	fprintf(config_file,
	"\ntext_stretch = %s", 	qdcp->cfg->text_stretch);
	fprintf(config_file,
	"\ninterline = %d", 	qdcp->cfg->interline);
	fprintf(config_file,
	"\ntext_underline = %s", qdcp->cfg->text_underline);
	fprintf(config_file,
	"\ntext_strike = %d", 	qdcp->cfg->text_strike);
	fprintf(config_file,
	"\ntext_script = %s", 	qdcp->cfg->text_script);
	fprintf(config_file,
	"\ntext_lang = %s", 	qdcp->cfg->text_lang);
	fprintf(config_file,
	"\n"
	"\n"
	"\n"
	"\n[interface]"
	"\nshow_toolbar = %d", 	qdcp->show_toolbar);
	fprintf(config_file,
	"\nshow_statusbar = %d", qdcp->show_statusbar);
	fprintf(config_file,
	"\nshow_terminal = %d", qdcp->show_toolbar);
	fprintf(config_file,
	"\nwin_x = %d", 		qdcp->win_x);
	fprintf(config_file,
	"\nwin_y = %d", 		qdcp->win_y);
	fprintf(config_file,
	"\nwin_w = %d", 		qdcp->win_w);
	fprintf(config_file,
	"\nwin_h = %d", 		qdcp->win_h);
	fprintf(config_file,
	"\n"
	"\n##EOF" 
	);

	fclose(config_file);

	printf(" > OK !\n");
	
	return EXIT_SUCCESS;

}

