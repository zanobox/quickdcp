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
   @file    qdcp_gui.c
   @author  jean.sabatier@zanobox.net
   @brief   QuickDCP builds Digital Cinema Packages from a GTK+ interface.
*/

#include "qdcp_gui.h"




/* ******************* A P P L I C A T I O N ************************ */


QuickDcpApp *
quickdcp_app_new (void)
{
	printf("Quickdcp : Application new\n");

	return g_object_new (QUICKDCP_APP_TYPE,
                       "application-id", "net.zanobox.quickdcp",
                       "flags", G_APPLICATION_HANDLES_OPEN,
                       NULL);
}

static void
quickdcp_app_init (QuickDcpApp *App)
{
	printf("Quickdcp : Application initialization\n");

	/* Instantiate ImageMagick */
	MagickWandGenesis();
}

static void
quickdcp_app_class_init (QuickDcpAppClass *class)
{
	printf("Quickdcp : Application class initialization\n");

	G_APPLICATION_CLASS (class)->activate	= quickdcp_app_activate;
	G_APPLICATION_CLASS (class)->open 	= quickdcp_app_open;
	G_APPLICATION_CLASS (class)->startup 	= quickdcp_app_startup;
}


/** Define global action entries 
 * arg0 -> name of the entry
 * arg1 -> callback function
 * arg2 -> type of argument (as a GVariant string)
 * arg3 -> state (as a GVariant string)
 * arg4 -> change-state handler
 * 
 * It seems that a parametered callback can be expressed like the following or so :
 * void (* activate) (GSimpleAction *action,
				 GVariant      *parameter,
				 gpointer       user_data);
 * Where I don't understand yet how user_data is typed and defined.
 */
static GActionEntry app_entries[] =
{
	{ "quit", 				action_quit_activated, NULL, NULL, NULL },

	{ "import_qdp",			action_import_qdp_activated, NULL, NULL, NULL },
	{ "export_qdp",			action_export_qdp_activated, NULL, NULL, NULL },
	{ "import_video",		action_import_video_activated, NULL, NULL, NULL },
	{ "import_audio",		action_import_audio_activated, NULL, NULL, NULL },
	{ "import_subtitles",	action_import_subtitles_activated, NULL, NULL, NULL },
	{ "import_DCP",			action_import_dcp_activated, NULL, NULL, NULL },

	{ "about",				action_about_activated, NULL, NULL, NULL },
};

static GActionEntry win_entries[] =
{
	{ "new_project",		action_new_project_activated, NULL, NULL, NULL },
	{ "open_project",		action_open_project_activated, NULL, NULL, NULL },
	{ "open_template",		open_template_activated, NULL, NULL, NULL },
	{ "save_project",		action_save_project_activated, NULL, NULL, NULL },
	{ "save_project_as",	action_save_project_as_activated, NULL, NULL, NULL },

	{ "preferences",		preferences_activated, NULL, NULL, NULL },
	{ "simple_carton",		simple_carton_activated, NULL, NULL, NULL },
	{ "convert_video",		convert_video_activated, NULL, NULL, NULL },
	{ "play_DCP",			dcp_preview_activated, NULL, NULL, NULL },

	{ "show_intro",			action_show_intro_activated, NULL, NULL, NULL },
	{ "show_compositor",	action_show_compositor_activated, NULL, NULL, NULL },
	{ "show_sequences",		action_show_sequences_activated, NULL, NULL, NULL },
	{ "show_audio",			action_show_audio_activated, NULL, NULL, NULL },
	{ "show_dcp",			action_show_dcp_activated, NULL, NULL, NULL },
	
	{ "zoom_in",			action_zoom_in_activated, NULL, NULL, NULL },
	{ "zoom_out",			action_zoom_out_activated, NULL, NULL, NULL },
	{ "zoom_fit",			action_zoom_fit_activated, NULL, NULL, NULL },
	
	{ "toggle_toolbar",		action_toggle_toolbar_activated, NULL, "false", NULL },
	{ "toggle_statusbar",	action_toggle_statusbar_activated, NULL, "false", NULL },
};


static void
quickdcp_app_startup (GApplication *app)
{
		GtkBuilder 			*	builder;
		GMenuModel 			*	menubar;

	printf("Quickdcp : Application startup\n");

	G_APPLICATION_CLASS (quickdcp_app_parent_class)->startup (app);

	/* menu */
	
	g_action_map_add_action_entries (G_ACTION_MAP (app),
								   app_entries, 
								   G_N_ELEMENTS (app_entries),
								   app);
					
	/* set the menubar through builder */
	builder = gtk_builder_new_from_resource ("/net/zanobox/quickdcp/gui/gtk/quickdcp_menu.ui");
	menubar = G_MENU_MODEL (gtk_builder_get_object (builder, "quickdcpmenu"));
	gtk_application_set_menubar (GTK_APPLICATION (app), menubar);
	

	g_object_unref (builder);

	/* Define global accelerators */			
	   
		/* file menu */
		const gchar *new_accels[2] 		= { "<Ctrl>N", NULL };
		const gchar *open_accels[2] 	= { "<Ctrl>O", NULL };
		const gchar *save_accels[2] 	= { "<Ctrl>S", NULL };
		const gchar *save_as_accels[2] 	= { "<Ctrl><Shift>S", NULL };
		const gchar *prefs_accels[2] 	= { "<Ctrl><Shift>P", NULL };
		const gchar *quit_accels[2] 	= { "<Ctrl>Q", NULL };
		const gchar *int_tab_accels[2] 	= { "<Ctrl>I", NULL };
		const gchar *cmp_tab_accels[2] 	= { "<Ctrl>C", NULL };
		const gchar *seq_tab_accels[2] 	= { "<Ctrl>V", NULL };
		const gchar *aud_tab_accels[2] 	= { "<Ctrl>A", NULL };
		const gchar *dcp_tab_accels[2] 	= { "<Ctrl>D", NULL };
		const gchar *zoom_in_accels[2] 	= { "<Ctrl>P", NULL };	// P stands for <Plus> which doesn't work
		const gchar *zoom_out_accels[2] = { "<Ctrl>M", NULL };	// M stands for <Minus> which doesn't work
		const gchar *zoom_fit_accels[2] = { "<Ctrl>F", NULL };	// F stands for <Period> which doesn't work
		/* tabs menu */
		/* help menu */
		//~ const gchar *about_accels[2] 	= { "<Ctrl>I", NULL };

	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.new_project",  	new_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.open_project", 	open_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.save_project", 	save_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.save_as_project",save_as_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.quit", 			quit_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.preferences", 	prefs_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show_intro",		int_tab_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show_compositor",cmp_tab_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show_sequences", seq_tab_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show_audio", 	aud_tab_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.show_dcp", 		dcp_tab_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.zoom_in", 		zoom_in_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.zoom_out", 		zoom_out_accels);
	gtk_application_set_accels_for_action (GTK_APPLICATION (app), "win.zoom_fit", 		zoom_fit_accels);
	//~ gtk_application_set_accels_for_action (GTK_APPLICATION (app), "app.about", 			about_accels);
}


static void
quickdcp_app_activate (GApplication *app)
{
		QuickDcpAppWindow *win;

	printf("Quickdcp : Application activation\n");

	win = quickdcp_app_window_new (QUICKDCP_APP (app));
	gtk_window_present (GTK_WINDOW (win));

}


static void
quickdcp_app_open (GApplication  *app,
                  GFile        **files,
                  gint           n_files,
                  const gchar   *hint)
{
	  GList *windows;
	  QuickDcpAppWindow *win;
	  int i;

	windows = gtk_application_get_windows (GTK_APPLICATION (app));
	
	if (windows)
		win = QUICKDCP_APP_WINDOW (windows->data);
	else
		win = quickdcp_app_window_new (QUICKDCP_APP (app));

	/* if there are files to open */
	for (i = 0; i < n_files; i++)
		quickdcp_app_open_command_line_arguments (win, files[i]);

  gtk_window_present (GTK_WINDOW (win));
}

static void
quickdcp_app_shutdown (GApplication *app)
{
	printf("Quickdcp : Application shutdown\n");
	
	/* If modified prefs haven't been written onto the .ini file yet, do that now.
	 * Or keep them recorded somewhere for next run, without crushing actual prefs */
	
	/* No. Don't touch the prefs, even when modified off saving.
	 * But keep in record the state of every widget, so they get tuned the same way
	 * on next run. */
	
	/* Keep also in record things like window position, magnified or not, last proj opened,
	 * history (if we ever track it), pending tasks, 
	 * what else ?
	 * */ 
}




/* ******************* A P P - W I N D O W ************************** */




static void
quickdcp_app_window_init (QuickDcpAppWindow * self)
{
			t_project 		* 	pj = NULL;
			t_wav_track 	*	nw = NULL;
			struct timeval		tv;
			char				err_buffer[80];   /* for warning and error messages */
			char 				config_path[PATH_STR_LEN] = {0};
			char 				config_file[PATH_STR_LEN] = {0};
			char 				qdcp_path[PATH_STR_LEN] = {0};
			char 				icon_path[PATH_STR_LEN] = {0};
	

	printf("Quickdcp : Application Window init \n");

	/* add some actions to the window */
	g_action_map_add_action_entries (G_ACTION_MAP (self),
									 win_entries,
									 G_N_ELEMENTS (win_entries),
									 self);
	
   	/* After this, all widgets from the app window template will be somewhat accessible : */ 
	gtk_widget_init_template (GTK_WIDGET (self));

	/* QuickDcp specific icon themes (for tools and self navigation) */
	self->icons = gtk_icon_theme_new();
	gtk_icon_theme_set_custom_theme(self->icons, "quickdcp");
	gtk_icon_theme_set_screen(self->icons, gdk_screen_get_default());

	/* icon size could/should be defined at a 'theme' level */
	sprintf(icon_path, "%s%sscalable", ICON_DIR, SEPARATOR); 
	//~ gtk_icon_theme_add_resource_path(self->icons, icon_path);
	gtk_icon_theme_prepend_search_path(self->icons, icon_path);
	
	//~ g_object_unref(); /* to unref the reference to a loaded icon */
	

	/* Init toolbar */
	
	/* Init statusbar */
	
	
	/*-------------------- Instantiate quickdcp ----------------------*/
	
	printf("Quickdcp :  - qdcp struct initialization \n");
	
	self->qdcp = quickdcp_init();
	
	
	/*-------------- Default path of configuration file --------------*/
	
    sprintf(config_path, "%s%s%s", 
				HOME_DIR, SEPARATOR, CFG_DIR);
	check_dir(config_path, self->qdcp->verbosity);
    sprintf(config_file, "%s%s%s", 
				config_path, SEPARATOR, CFG_FILE);

	/*---------------- Read and parse configuration file -------------*/
	
    read_config_file(config_file, self->qdcp);    

  	/* Now we can start recording logs */
    //~ qdcp_log_init(qdcp->loglevel, qdcp);

	/*--------- if workspace or output directories do not exist ------*/
	
	/* create them */
  	    
	/*---- configure all interface widgets with default config values ----*/
	
	if (self->qdcp->show_toolbar)
		gtk_widget_show(GTK_WIDGET (self->toolbar));
	else
		gtk_widget_hide(GTK_WIDGET (self->toolbar));

	//~ if (self->qdcp->show_statusbar)
		//~ g_action_activate(win_entries[19], NULL);
		//~ gtk_widget_show(GTK_WIDGET (self->statusbar));
	//~ else
		//~ g_action_activate(win_entries[19], NULL);
		//~ gtk_widget_hide(GTK_WIDGET (self->statusbar));

	/* some values to calibrate spin buttons */
	self->time_adjustment = gtk_adjustment_new(self->qdcp->cfg->laps, 0, 36000, 1, 1, 60); /* max. 10h ! (in seconds) */
	self->percent_adjustment = gtk_adjustment_new(0, -100, 100, 1, 5, 0); 
	self->u_percent_adjustment = gtk_adjustment_new(0, 0, 100, 1, 5, 0); 
	self->degrees_adjustment = gtk_adjustment_new(0, -360, 360, 1, 45, 0); 
	self->frames_adjustment = gtk_adjustment_new(0, 0, (36000*24), 1, 1, 24); /* max. 1 minute of 24 frames */ 
	self->pixels_adjustment = gtk_adjustment_new(0, -16000, 16000, 1, 1, 10);
	self->u_pixels_adjustment = gtk_adjustment_new(0, 0, 16000, 1, 1, 10);
	
	/* About treeviews, see : https://developer.gnome.org/gtk3/3.22/TreeWidget.html */

	/* piles treeview */		
	self->compPilesTreeStore = gtk_tree_store_new(PILES_N_COLUMNS, 	/* number of columns */
										  G_TYPE_BOOLEAN,			/* check box */
										  GDK_TYPE_PIXBUF,			/* icon */
										  G_TYPE_STRING,			/* name of file (or project?) */
										  G_TYPE_STRING,			/* file absolute path */
										  G_TYPE_INT,				/* project ID */
										  G_TYPE_STRING				/* tooltip */
										  );
	/* layers treeview */
	self->compLayersTreeStore = gtk_tree_store_new(LAYERS_N_COLUMNS, /* number of columns */
										  G_TYPE_BOOLEAN,			/* check box */
										  GDK_TYPE_PIXBUF,			/* icon */
										  G_TYPE_STRING,			/* layer's name */
										  G_TYPE_STRING,			/* layer's ID */
										  G_TYPE_STRING,			/* layer's position in pile */
										  G_TYPE_STRING				/* layer's source file (if any) */
										  );
	/* DCP treeview */	
	self->dcpTreeStore = gtk_tree_store_new(DCP_N_COLUMNS, 	/* number of columns */
										  G_TYPE_BOOLEAN,	/* check box */
										  GDK_TYPE_PIXBUF,	/* icon */
										  G_TYPE_STRING,	/* file or folder name */
										  G_TYPE_STRING,	/* name of child */
										  G_TYPE_STRING,	/* file or folder size */
										  G_TYPE_STRING,	/* zip contents as tooltip */
										  G_TYPE_STRING		/* date of creation/last modification */
										  );

	/*--------------- configure compositor tab widgets  --------------*/
	comp_init_widgets(self);
	

	/*--------------- configure video tab widgets  -------------------*/

	vid_init_widgets(self);
	

	/*--------------- configure audio tab widgets  -------------------*/

	aud_init_widgets(self);
	
	
	/*----------------- configure DCP tab widgets  -------------------*/

	dcp_init_widgets(self);


	/*---------- configure preferences tab widgets  ------------------*/

	opt_init_widgets(self);
	

	/*--------- some signals not declared in xml interface -----------*/

	g_signal_connect_swapped( G_OBJECT(gtk_entry_get_buffer(self->compBasenameEntry)),
					  "inserted-text",
					  G_CALLBACK (comp_basename_changed_cb),
					  (gpointer) self
					  );

	g_signal_connect_swapped( G_OBJECT(gtk_entry_get_buffer(self->compBasenameEntry)),
					  "deleted-text",
					  G_CALLBACK (comp_basename_changed_cb),
					  (gpointer) self
					  );
				
	/* unsaved marking of dcpname provider */		
	self->unsaved_provider = gtk_css_provider_new();

	/*----------------------------------------------------------------*/

	gtk_notebook_set_current_page(self->notebook, PAGE_COMP);

	/*----------------------------------------------------------------*/

	printf("Quickdcp : End of window init.\n");
}

static void
quickdcp_app_window_class_init (QuickDcpAppWindowClass *class)
{
	printf("Quickdcp : Application window class init\n");

	/* About setting a glade XML template and accessing widgets declared in the template, see
	 * https://developer.gnome.org/gtk3/stable/GtkWidget.html#gtk-widget-class-set-template-from-resource
	 */
	gtk_widget_class_set_template_from_resource (GTK_WIDGET_CLASS (class),
                                               "/net/zanobox/quickdcp/gui/gtk/qdcp-main.glade");

	/* binding widgets */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolbar);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, notebook);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, statusbar);

	/*-------------- Access Widgets from Window object ---------------*/
	
	/*---------------------- Compositor Tab --------------------------*/
	
	/* compositor widgets */
	/* bind :: comp :: general settings */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compBasenameEntry);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compBasenameOkBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTimeLengthSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFormatCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compResolutionCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compCategoryCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFramerateCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compBackgroundColorBtn);

	/* bind :: comp :: piles */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compLeftBox);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compPilesTreeView);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compPileCellCheck);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compPileCellPixbuf);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compNewPileBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDeletePilesBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compRefreshPilesBtn);
	/* bind :: comp :: layers */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compNewImageLayerBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compOpacitySpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compLayersTreeView);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compLayerCellCheck);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compLayerCellPixbuf);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDuplicateLayerBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDeleteLayerBtn);
	/* bind :: comp :: screen */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compGenerateSeqBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compGenerateDcpBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDrawingSW);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDrawingArea);
	//~ gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFrameLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compCurrentFrame);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compDcpName);
	//~ gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFrameLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compCurrentLayer);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTimeline);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, isKeyframeCheckBtn);

	/* bind :: comp :: tools */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compRightBox);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compRightColNotebook);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolbox);
	//~ gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolitemgroup);
	/* bind :: comp :: text/pango */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compNewTextLayerBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compPangoBox);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compInterlineSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextAlignCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextStretchCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextWeightCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextVariantCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextStyleCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFontSelector);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextScriptCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextStrikeSwitch);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextUnderlineCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compTextLangCBB);
	/* bind :: comp :: rotation */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compRotateDegreeSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compCenterXSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compCenterYSpinBtn);
	/* bind :: comp :: shapes */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compShapesLineColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compShapesFillColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compShapesLineWidthSpinBtn);
	/* bind :: comp :: transitions */	
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFonduLapsDebSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFonduCacheDebColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFonduLapsFinSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFonduCacheFinColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compEntryPointSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFadeInSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compExitPointSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compFadeOutSpinBtn);
	/* bind :: comp :: geometry */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compGeometryToolBar);
	/* bind :: comp :: coords */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compXLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compYLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compOxSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compOxPercentLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compOySpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compOyPercentLabel);
	/* bind :: comp :: dims */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compWidthSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compWidthPercentLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compHeightSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compHeightPercentLabel);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, compRatioCheckBtn);

	/* bind :: comp :: tool buttons */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolNone);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolSelection);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolDraw);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolMove);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolText);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolRotation);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolTransition);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolRectangle);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolRectangleFilled);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolCircle);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolCircleFilled);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolOval);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolOvalFilled);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolZoomIn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolZoomOut);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, toolZoomFit);
	
	/* bind :: aud :: all */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedAddSource);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedDeleteBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedRefreshBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedListenBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedConvertBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedChannelsCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, audUnprocessedMixTypeCBB);

	/* bind :: dcp :: buttons */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpCopyBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpAtticBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpZipBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpDeleteBtn);
	/* bind :: dcp :: treeview */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpTreeView);
	/* tree store is not part of the template */
	/* bind :: dcp :: image */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpIconTiff);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpIconJ2k);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpIconWav);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpIconMxf);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, dcpIconDcp);
	
	/* bind :: opt :: all */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optSaveConfigBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optCancelConfigBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optCloseBtn);

	/* GENERAL options widgets */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optFormatCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optResolutionCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optCategoryCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optFramerateCBB);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optTimelengthSpinBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optBackgroundColorBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optIssuerShortEntry);
		//~ optMarginSpinBtn
		//~ optForegroundColorBtn
		//~ optIssuerEntry
		//~ optIssuerShortEntry
		
	/* TEXT options widgets */
		//~ optTextFontBtn
		//~ optTextInterlineSpinBtn
		//~ optTextAlignCBB
		//~ optTextWeightCBB
		//~ optTextStretchCBB
		//~ optTextStyleCBB
		//~ optTextVariantCBB)
		//~ optTextUnderlineCBB
		//~ optTextStrikeSwitch
		//~ optTextScriptCBB
		//~ optTextLangCBB
		
	/* SHAPES options widgets */
		//~ optLineColorBtn
		//~ optFillColorBtn
		//~ optLineWidthSpinBtn
		//~ optLineStyleCBB
		//~ optLineCapsCBB
		//~ optLineJoinCBB
		
	/* TRANSITIONS options widgets */
		//~ optFonduDebCBB
		//~ optFonduFinCBB
		//~ optFonduLapsDebSpinBtn
		//~ optFonduLapsFinSpinBtn
		//~ optFonduCacheDebColorBtn
		//~ optFonduCacheFinColorBtn
		
	/* AUDIO-VIDEO options widgets */
		
	/* DCP NAME options widgets */
		//~ optDcpExampleLabel
		//~ optDcpCategoryCheckBtn
		//~ optDcpFormatCheckBtn
		//~ optDcpLangCheckBtn
		//~ optDcpLangCBB
		//~ optDcpSubtitlesCheckBtn
		//~ optDcpSubtitlesCBB
		//~ optDcpCountryCheckBtn
		//~ optDcpCountryCBB
		//~ optDcpResolutionCheckBtn
		//~ optDcpDateCheckBtn
		//~ optDcpIssuerCheckBtn
		
	/* QUICKDCP options widgets */
		//~ optQdcpInputdirFileBtn
		//~ optQdcpOutputdirFileBtn
		//~ optQdcpBeepSwitch
		//~ optQdcpThreadsSpinBtn
		//~ optQdcpDebugModeSwitch
		//~ optQdcpOdcpLoglevelCBB
		//~ optQdcpFFmpegLoglevelCBB
		//~ optQdcpJ2kMethodCBB
		//~ optQdcpMxfMethodCBB
		
	/* KEYBOARD SHORTCUTS options widgets */
		//~ optAccelTreeView
		
	/* CONFIG FILE EDITOR widgets */
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optConfigViewer);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optConfigBuffer);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optSaveConfigBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optCancelConfigBtn);
	gtk_widget_class_bind_template_child (GTK_WIDGET_CLASS (class), QuickDcpAppWindow, optCloseBtn);


	/*----------------------- binding callbacks ----------------------*/



	/* callbacks :: actions */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_new_project_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_open_project_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), open_template_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), preferences_activated);
	
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_import_video_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_import_audio_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), action_import_dcp_activated);
	
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), simple_carton_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), convert_video_activated);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_preview_activated);
	
	/* callbacks :: comp :: top row */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_basename_changed_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_validate_basename_clicked);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_time_spin_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_format_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_resolution_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_category_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_framerate_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_background_color_chooser_activated);
	/* callbacks :: comp :: piles tree view */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_piles_toggle_checked_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_piles_row_activated_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_new_pile_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_delete_pile_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_refresh_piles_btn_cb);
	/* callbacks :: comp :: layers treeview */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_layers_mask_layer_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_layers_focus_layer_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_layers_reorder_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_new_image_layer_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_duplicate_layer_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_delete_layer_btn_cb);
	/* callbacks :: middle box (screen) */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_generate_seq_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_generate_dcp_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), draw_drawing_area_try_layer_selection_by_click_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), draw_canvas_draw_event_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), draw_drawing_area_realize_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), draw_drawing_area_size_allocate_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), draw_drawing_area_configure_event_cb);
	/* callbacks :: comp :: pango text */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), comp_new_text_layer_btn_cb);
	/* callbacks :: comp :: geometry */

	/* callbacks :: audio :: buttons */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), aud_unprocessed_add_source_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), aud_unprocessed_delete_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), aud_unprocessed_refresh_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), aud_unprocessed_listen_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), aud_unprocessed_convert_btn_cb);

	/* callbacks :: dcp :: buttons */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_copy_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_attic_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_zip_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_delete_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_tree_toggled_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), dcp_tree_row_double_clicked_cb);
	
	/* callbacks :: opt :: all */
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_save_config_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_cancel_config_btn_cb);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_close_btn_cb);

	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_time_spin_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_format_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_resolution_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_category_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_framerate_CBB_changed);
	gtk_widget_class_bind_template_callback (GTK_WIDGET_CLASS (class), opt_background_color_chooser_activated);
	
}

QuickDcpAppWindow *
quickdcp_app_window_new (QuickDcpApp *app)
{
		QuickDcpAppWindow 	* 	self;
		char					app_icon[PATH_STR_LEN];

	printf("Quickdcp : Application Window new \n");

	/* get a window */
	self = g_object_new (QUICKDCP_APP_WINDOW_TYPE, "application", app, NULL);

	/* set a window icon */
	sprintf(app_icon,	 "%s", APP_ICON);
	
	if (check_file(app_icon) == QDCP_YES)
		gtk_window_set_default_icon_from_file(app_icon, NULL);
		
	return self;
}


                          

/* ************** A C T I O N - C A L L B A C K S ******************* */




/** Actions callbacks 
 */

/* on startup, open command line arguments, considered as project files */
void                    
quickdcp_app_open_command_line_arguments (QuickDcpAppWindow * win,
                          GFile             * file)
{
	
	/* we could use GFileInfo here. */
	/* and GContentType related funks
	 * like g_content_type_is_mime_type,
	 * g_content_type_get_icon etc...
	 */
}


/* new project */
static void
action_new_project_activated (GSimpleAction * action,
                       GVariant      * parameter,
                       gpointer        user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;
		t_project			*	np = NULL;
	
	printf("QuickDCP : Activating new project\n");

	/* in any case, go to the compositor tab */
	gtk_notebook_set_current_page(self->notebook, PAGE_COMP);	
	
	/* if a project is opened and unsaved show a dialog */
	if (self->qdcp->active_project != NULL && self->qdcp->active_project->unsaved == QDCP_TRUE)
	{
		printf("An unsaved project is active\n");
		
		/* show a save or discard dialog 
		 * or create a new project tab */
		return;
	}
	/* then set all compositor's widgets to default values */
	comp_reset_widgets(self);
	
	gtk_entry_set_text(self->compBasenameEntry, DEFAULT_PROJECT_NAME);
	
	/* get the layers treeview tab active */
	gtk_notebook_set_current_page(self->compLeftBox, 1);
	
	/* and create an empty new project */
	self->qdcp->active_project = new_project(self->qdcp->proj_list->size, self->qdcp, self->qdcp->proj_list);
	
	/* set the new project active */
	//~ self->qdcp->active_project = self->qdcp->proj_list->last;
	self->qdcp->active_project->unsaved = QDCP_TRUE;
	
	printf("Quickdcp : - %s\n", self->qdcp->active_project->format);
	
}

/* open project */
static void
action_open_project_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
		char	qdp_path[PATH_STR_LEN] = {0};
		char	arg_qdp_project[PATH_STR_LEN] = {0};

	
	printf("QuickDCP : Activating open project\n");	

	/* open a dialog to chose from all available projects */

	/* load the project given in parameter */				
	strcpy(qdp_path, arg_qdp_project);
	if (qdp_path[0] != 0 && strcmp(arg_qdp_project, "list"))
	{
			t_project 	*	qdp = NULL;
		
		qdp = load_project(qdp_path, self->qdcp->proj_list, self->qdcp);
	}		
	/* apply project's value to all compositor widgets */

	/* then go to the compositor tab */
	gtk_notebook_set_current_page(self->notebook, PAGE_COMP);

}

/* open project template */

static void
open_template_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	
	
	printf("QuickDCP : open_template_activated\n");
	/* open a dialog to chose from all available projects */
	/* load the project given in parameter */
	/* apply project's value to all compositor widgets */
	/* then go to the compositor tab */
	gtk_notebook_set_current_page(self->notebook, PAGE_COMP);
}


/* save project */
static void
action_save_project_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;

	printf("QuickDCP : Activating save project\n");	
	
	if (self->qdcp->active_project == NULL)
	{
		/* create an empty new project */
		printf("QuickDCP :  - creating a new project\n");	
		self->qdcp->active_project = new_project(self->qdcp->proj_list->size, self->qdcp, self->qdcp->proj_list);
		comp_get_widgets_values(self->qdcp->active_project, self);
		self->qdcp->active_project->unsaved = QDCP_TRUE;
	}
	
	/* name an unnamed project as default */
	if (strcmp(self->qdcp->active_project->name, "") == 0)
	{
		rename_project(DEFAULT_PROJECT_NAME, self->qdcp->active_project, self->qdcp);
	}
	
	/* check whether we already have a project with such a name */
	
	/* finally save the active project */
	if (self->qdcp->active_project->unsaved)
	{
		printf("QuickDCP : - saving active project\n");
			
		comp_get_widgets_values(self->qdcp->active_project, self);
		
		/* before saving, search in the tree store for the same project */


		save_project(self->qdcp->active_project, self->qdcp);
		
		self->qdcp->active_project->unsaved = QDCP_FALSE;
		
		/* set back the color state of dcpname widget */
		gtk_style_context_remove_provider(gtk_widget_get_style_context(GTK_WIDGET (self->compDcpName)),
										  GTK_STYLE_PROVIDER(self->unsaved_provider));
								   				
		/* if the project did not exist before, append a new row to the tree store */
		comp_scan_available_piles(self);
	}
}

/* save project AS */

static void
action_save_project_as_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;

	printf("QuickDCP : Activating save project as\n");	

	/* we need a dialog window to chose a place and a name */


	/* then we save the freshly named project */
	//~ save_project(self->qdcp->active_project, self->qdcp);

}

/* quit activated */
static void
action_quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
	printf("QuickDCP : Quitting application\n");
	g_application_quit (G_APPLICATION (user_data));
}



/** Menu Edit */

/* simples cartons */
static void
simple_carton_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
	printf("QuickDCP : simple_carton_activated\n");
}

static void
convert_video_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
	printf("QuickDCP : convert_video_activated\n");
}

/* preferences */
static void
preferences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
	/* we need a popup window */
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	

	printf("QuickDCP : Activating preferences\n");	
	
	/* keep trace of the current tab, to get back to from the preferences tab */
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_PREFS);
	
}


/* DCP preview */
static void
dcp_preview_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app)
{
	printf("QuickDCP : dcp_preview_activated\n");
}



/* analyse video */

static void
action_import_video_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app)
{
	printf("QuickDCP : action_import_video_activated\n");
}

/* import audio */

static void
action_import_audio_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app)
{
	printf("QuickDCP : action_import_audio_activated\n");
}

/* import subtitles */

static void
action_import_subtitles_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app)
{
	printf("QuickDCP : action_import_subtitles_activated\n");
}

/* analyse dcp */

static void
action_import_dcp_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app)
{
	printf("QuickDCP : action_import_dcp_activated\n");
}



/** Menu View */


/* show intro */
static void
action_show_intro_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	

	printf("QuickDCP : Activating intro tab\n");	
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_INTRO);
	
}

/* show compositor */
static void
action_show_compositor_activated (GSimpleAction *action,
						   GVariant      *parameter,
						   gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	
	
	printf("QuickDCP : Activating compositor tab\n");
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_COMP);

	/* refresh list of available piles */
	comp_refresh_piles_btn_cb(self);
	
}

/* show sequences */
static void
action_show_sequences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	

	printf("QuickDCP : Activating sequences tab\n");	
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_SEQ);
	
}

/* show audio */
static void
action_show_audio_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	

	printf("QuickDCP : Activating audio tab\n");	
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_AUD);
	
}

/* show DCP */
static void
action_show_dcp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
	
	//~ g_assert (QUICKDCP_IS_APP_WINDOW (self));	

	printf("QuickDCP : Activating DCP tab\n");	
	
	/* just go to compositor tab */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->notebook), PAGE_DCP);
	
}

/* zoom in */
static void
action_zoom_in_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
	printf("Just zoom in\n");
}

/* zoom out */
static void
action_zoom_out_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
	printf("Just zoom out\n");
}
                       
/* zoom fit */
static void
action_zoom_fit_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app)
{
	printf("Just zoom fit\n");
}


/* toggle toolbar */
static void
action_toggle_toolbar_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		GVariant * state;
		QuickDcpAppWindow 	* self = user_data;
		
	//~ printf("user_data type: %s\n", g_type_name(user_data));

	state = g_action_get_state (G_ACTION (action));
	//~ g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));
  
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));

	if (gtk_widget_is_visible(GTK_WIDGET (self->toolbar)))
	{
		printf("      -> Hiding toolbar\n");
		gtk_widget_hide(GTK_WIDGET (self->toolbar));
	}
	else
	{
		printf("      -> Showing toolbar\n");
		gtk_widget_show(GTK_WIDGET (self->toolbar));
	}
	
	
	g_variant_unref (state);
}

/* toggle statusbar */
static void
action_toggle_statusbar_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
		GVariant * state;
		QuickDcpAppWindow 	* self = user_data;
		
	printf("QuickDCP : Toggling statusbar\n");	

	state = g_action_get_state (G_ACTION (action));
	//~ g_action_change_state (G_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));
  
	g_simple_action_set_state (G_SIMPLE_ACTION (action), g_variant_new_boolean (!g_variant_get_boolean (state)));

	//~ printf("Action parameter type: %b\n", g_variant_get_type_string(state));

	if (gtk_widget_is_visible(GTK_WIDGET (self->statusbar)))
	{
		printf("      -> Hiding statusbar\n");
		gtk_widget_hide(GTK_WIDGET (self->statusbar));
	}
	else
	{
		printf("      -> Showing statusbar\n");
		gtk_widget_show(GTK_WIDGET (self->statusbar));
	}
	
	g_variant_unref (state);
}



/* import qdp */
static void
action_import_qdp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
	printf("QuickDCP : Activating import qdp\n");	
}

/* export qdp */
static void
action_export_qdp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data)
{
	printf("QuickDCP : Activating export qdp\n");	
}


/** Help menu */

/* about activated */
static void
action_about_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       user_data)
{
		QuickDcpAppWindow * self = user_data;
		
	printf("QuickDCP : About window called\n");
	/* show independent window of 'About' information */
	
	gtk_show_about_dialog(GTK_WINDOW (self),
						"program-name", PROGRAM,
						"title",		_("À propos de Quickdcp"),
						"icon_name",	"vlc",
						"version",		QDCP_VERSION,
						"web_site",		QDCP_URL,
						"licence", 		QDCP_LICENSE,
						"copyright",	QDCP_COPYRIGHT,
						NULL);	
}




/* *********** C O M P O S I T O R  - F U N C T I O N S ************* */



qdcp_bool
comp_init_widgets(QuickDcpAppWindow * self)
{
	printf("Quickdcp : Init compositor widgets \n");

	/* compositor init variables declarations */
	
	
		//~ t_project		*	empty_pj = new_project(1, self->qdcp, NULL);
		char				default_project_name[NAME_STR_LEN];
		

	/* compositor init variables definitions */

	sprintf(default_project_name, "%s", DEFAULT_PROJECT_NAME);
		

	/* comp :: top row (general settings) */
	gtk_spin_button_configure(self->compTimeLengthSpinBtn, self->time_adjustment, 1, 0);
	gtk_entry_set_placeholder_text(self->compBasenameEntry, DEFAULT_BASENAME_TEXT);

	/* comp :: piles treeview */
	comp_scan_available_piles(self);
	gtk_tree_view_set_model(self->compPilesTreeView, 
							GTK_TREE_MODEL(self->compPilesTreeStore));
	
	/* comp :: layers treeview */
	gtk_spin_button_configure(	self->compOpacitySpinBtn, self->u_percent_adjustment, 1, 0);
	gtk_tree_view_set_model(self->compLayersTreeView, 
							GTK_TREE_MODEL(self->compLayersTreeStore));

	/* comp :: middle box */
	gtk_label_set_text(	self->compCurrentFrame, "");
	gtk_label_set_text(	self->compCurrentLayer, "");
	gtk_label_set_text(	self->compDcpName, DEFAULT_DCPNAME_TEXT);

	/* comp :: time line */
	//~ GtkScale			*	compTimeline;
		
	/* comp :: pango */
	//~ GtkTextView			*	compPangoBox;
	gtk_spin_button_configure(	self->compInterlineSpinBtn, self->percent_adjustment, 1, 0);
	//~ gtk_combo_box_text_set_active(self->compTextAlignCBB, self->qdcp->cfg->text_align);
	//~ gtk_combo_box_text_set_active(self->compTextStretchCBB, self->qdcp->cfg->text_stretch);
	//~ gtk_combo_box_text_set_active(self->compTextWeightCBB, self->qdcp->cfg->text_weight);
	//~ gtk_combo_box_text_set_active(self->compTextVariantCBB, self->qdcp->cfg->text_variant);
	//~ gtk_combo_box_text_set_active(self->compTextStyleCBB, self->qdcp->cfg->text_style);
	//~ GtkFontButton		*	compFontSelector;
	//~ gtk_combo_box_text_set_active(self->compTextScriptCBB;
	//~ GtkSwitch			*	compTextStrikeSwitch;
	//~ gtk_combo_box_text_set_active(self->compTextUnderlineCBB, self->qdcp->cfg->underline);
	//~ gtk_combo_box_text_set_active(self->compTextLangCBB, self->qdcp->cfg->text_lang);

	/* comp :: shapes and rotation */
	gtk_spin_button_configure(	self->compShapesLineWidthSpinBtn, self->u_percent_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compRotateDegreeSpinBtn, self->degrees_adjustment, 1, 2);
	gtk_spin_button_configure(	self->compCenterXSpinBtn, self->pixels_adjustment, 0, 0);
	gtk_spin_button_configure(	self->compCenterYSpinBtn, self->pixels_adjustment, 0, 0);

	/* comp :: transitions */	
	gtk_spin_button_configure(	self->compFonduLapsDebSpinBtn, self->frames_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compFonduLapsFinSpinBtn, self->frames_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compEntryPointSpinBtn, self->frames_adjustment, 24, 0);
	gtk_spin_button_configure(	self->compFadeInSpinBtn, self->frames_adjustment, 6, 0);
	gtk_spin_button_configure(	self->compExitPointSpinBtn, self->frames_adjustment, 24, 0);
	gtk_spin_button_configure(	self->compFadeOutSpinBtn, self->frames_adjustment, 6, 0);

	/* comp :: geometry */
	gtk_spin_button_configure(	self->compOxSpinBtn, self->pixels_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compOySpinBtn, self->pixels_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compWidthSpinBtn, self->u_pixels_adjustment, 1, 0);
	gtk_spin_button_configure(	self->compHeightSpinBtn, self->u_pixels_adjustment, 1, 0);
	gtk_label_set_text(			self->compXLabel, "0");
	gtk_label_set_text(			self->compYLabel, "0");
	gtk_label_set_text(			self->compOxPercentLabel, "0.0");
	gtk_label_set_text(			self->compOyPercentLabel, "0.0");
	gtk_label_set_text(			self->compWidthPercentLabel, "0.0");
	gtk_label_set_text(			self->compHeightPercentLabel, "0.0");
	
	
	/* compositor widgets default settings */
	comp_reset_widgets(self);



	/* Display the default tab of the left hand notebook (piles or layers) */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->compLeftBox), 0);
	/* By default, display the tool tab of the right hand notebook (instead of text, shapes, etc.) */
	gtk_notebook_set_current_page(GTK_NOTEBOOK (self->compRightColNotebook), 0);

}

qdcp_bool
comp_reset_widgets(QuickDcpAppWindow * self)
{
	/* compositor widgets default settings */
		char				default_project_name[NAME_STR_LEN];
		int 				format;
		int 				resolution;
		int					category;
		int					framerate;
		char				str_frate[TINY_STR_LEN];
		GdkRGBA				rgba = {0};
		const gchar		*	hex_str;
		
	printf("Quickdcp : Reset compositor widgets\n");

	sprintf(default_project_name, "%s", DEFAULT_PROJECT_NAME);

	format = index_of_cbb(self->qdcp->formats, FMT_N_FORMATS, self->qdcp->cfg->format);

	resolution = index_of_cbb(self->qdcp->resolutions, RES_N_RESOLUTIONS, self->qdcp->cfg->resolution);

	category = self->qdcp->cfg->category;
	
	sprintf(str_frate, "%d", self->qdcp->cfg->framerate);
	framerate = index_of_cbb(self->qdcp->framerates, FRATE_N_FRAMERATES, str_frate);

	/* comp :: top row :: general settings */
	
	//~ gtk_entry_set_text(	self->compBasenameEntry, default_project_name);
	gtk_entry_set_text(	self->compBasenameEntry, "");
	
	gtk_spin_button_set_value(	self->compTimeLengthSpinBtn, self->qdcp->cfg->laps);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->compFormatCBB), format);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->compResolutionCBB), resolution);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->compCategoryCBB), self->qdcp->cfg->category);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->compFramerateCBB), framerate);

	hex_str = strdup(self->qdcp->cfg->background);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compBackgroundColorBtn), &rgba);
	else
		printf("Quickdcp : Could not initialize rgba color for background color chooser\n");
	
	/* comp :: piles and layers */
	
	gtk_widget_set_sensitive(GTK_WIDGET (self->compDuplicateLayerBtn), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET (self->compDeleteLayerBtn), FALSE);

	gtk_spin_button_set_value(	self->compOpacitySpinBtn, (gdouble) 90);

	/* comp :: screen */	
	gtk_label_set_text(	self->compCurrentFrame, "");
	//~ gtk_label_set_text(	self->compDcpName, get_dcp_name_from_widgets(self));
	gtk_label_set_text(	self->compDcpName, DEFAULT_DCPNAME_TEXT);
	gtk_label_set_text(	self->compCurrentLayer, "");
	
	/* time line */
	comp_time_line_set_end(self);
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (self->isKeyframeCheckBtn), FALSE);
	
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (self->compRatioCheckBtn), TRUE);

	/* comp :: geometry */

	gtk_label_set_text(			self->compXLabel, "0");
	gtk_label_set_text(			self->compYLabel, "0");
	gtk_label_set_text(			self->compOxPercentLabel, "0.0");
	gtk_label_set_text(			self->compOyPercentLabel, "0.0");
	gtk_spin_button_set_value(	self->compOxSpinBtn, 				0);
	gtk_spin_button_set_value(	self->compOySpinBtn, 				0);

	gtk_label_set_text(			self->compWidthPercentLabel, "0.0");
	gtk_label_set_text(			self->compHeightPercentLabel, "0.0");

	gtk_spin_button_set_value(	self->compWidthSpinBtn, 			0);
	gtk_spin_button_set_value(	self->compHeightSpinBtn, 			0);

	//~ GtkToolPalette		*	toolbox;
	//~ GtkToolItemGroup	*	toolitemgroup;
	
	//~ GtkTextView			*	compPangoBox;

	hex_str = strdup(self->qdcp->cfg->text_color);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compTextColorBtn), &rgba);

	gtk_spin_button_set_value(	self->compInterlineSpinBtn, 		self->qdcp->cfg->interline);

	//~ gtk_combo_box_text_set_active(self->compTextAlignCBB, self->qdcp->cfg->text_align);
	//~ gtk_combo_box_text_set_active(self->compTextStretchCBB, self->qdcp->cfg->text_stretch);
	//~ gtk_combo_box_text_set_active(self->compTextWeightCBB, self->qdcp->cfg->text_weight);
	//~ gtk_combo_box_text_set_active(self->compTextVariantCBB, self->qdcp->cfg->text_variant);
	//~ gtk_combo_box_text_set_active(self->compTextStyleCBB, self->qdcp->cfg->text_style);
	//~ GtkFontButton		*	compFontSelector;
	//~ gtk_combo_box_text_set_active(self->compTextScriptCBB;
	//~ GtkSwitch			*	compTextStrikeSwitch;
	//~ gtk_combo_box_text_set_active(self->compTextUnderlineCBB, self->qdcp->cfg->underline);
	//~ gtk_combo_box_text_set_active(self->compTextLangCBB, self->qdcp->cfg->text_lang);
	/* comp :: rotation */	

	/* comp :: shapes */

	gtk_spin_button_set_value(	self->compShapesLineWidthSpinBtn, 	self->qdcp->cfg->line_width);

	hex_str = strdup(self->qdcp->cfg->line_color);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compShapesLineColorBtn), &rgba);

	hex_str = strdup(self->qdcp->cfg->fill_color);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compTextColorBtn), &rgba);

	/* comp :: rotation */
	
	gtk_spin_button_set_value(	self->compRotateDegreeSpinBtn, 		0);
	gtk_spin_button_set_value(	self->compCenterXSpinBtn, 			0);
	gtk_spin_button_set_value(	self->compCenterYSpinBtn, 			0);

	/* comp :: transitions */	

	gtk_spin_button_set_value(	self->compFonduLapsDebSpinBtn, 		self->qdcp->cfg->fondulaps_d);
	gtk_spin_button_set_value(	self->compFonduLapsFinSpinBtn, 		self->qdcp->cfg->fondulaps_f);

	gtk_spin_button_set_value(	self->compEntryPointSpinBtn, 		1);
	gtk_spin_button_set_value(	self->compFadeInSpinBtn, 			0);
	gtk_spin_button_set_value(	self->compExitPointSpinBtn, 		1);
	gtk_spin_button_set_value(	self->compFadeOutSpinBtn, 			0);

	hex_str = strdup(self->qdcp->cfg->fondu_cache_color_d);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compFonduCacheDebColorBtn), &rgba);

	hex_str = strdup(self->qdcp->cfg->fondu_cache_color_f);
	if (gdk_rgba_parse(&rgba, hex_str))
		gtk_color_chooser_set_rgba(GTK_COLOR_CHOOSER (self->compFonduCacheFinColorBtn), &rgba);


	/* comp :: screen */
	draw_drawing_area_init(self);	

	return QDCP_SUCCESS;
}

qdcp_bool
comp_get_widgets_values(t_project * pj, QuickDcpAppWindow * self)
{
	/* exit quietly if we don't have an active project to feed */
	if (pj == NULL)
		return QDCP_SUCCESS;
		
			
	/* comp :: top row :: general settings */
	
	sprintf(pj->name, "%s", gtk_entry_get_text(	self->compBasenameEntry));
	pj->laps = (float) gtk_spin_button_get_value(self->compTimeLengthSpinBtn);
	
	self->qdcp->active_project->format 	   = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compFormatCBB));
	self->qdcp->active_project->resolution = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compResolutionCBB));
	self->qdcp->active_project->category   = (int) gtk_combo_box_get_active(GTK_COMBO_BOX (self->compCategoryCBB));
	self->qdcp->active_project->framerate = atoi(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compFramerateCBB)));

	/* we could retrieve the background color from the color chooser
	 * but it is easier to get it from the canvas */
		char	hex_color[COLOR_STR_LEN];
		//~ GdkRGBA * rgba_color = malloc(sizeof(GdkRGBA));
	//~ gtk_color_chooser_get_rgba(button, rgba_color);
	gui_color_rgba2hex(hex_color, self->canvas->bg_color);
	sprintf(self->qdcp->active_project->background, "%s", hex_color);
	
	show_project_params(pj, self->qdcp);
	
	/* exit gloriously */
	return QDCP_SUCCESS;
}

qdcp_bool
comp_set_widgets_values(t_project * pj, QuickDcpAppWindow * self)
{
}

qdcp_bool
comp_time_line_set_end(QuickDcpAppWindow * self)
{
		double total_frames;

	printf("Quickdcp : setting the timeline end value\n");
	
	if (self->qdcp->active_project)
		total_frames = (double) self->qdcp->active_project->framerate * (double) self->qdcp->active_project->laps;
	else
		total_frames = (double) self->qdcp->cfg->framerate * (double) self->qdcp->cfg->laps;

	gtk_range_set_range(GTK_RANGE (self->compTimeline), 1, total_frames);
	
	return QDCP_SUCCESS;
}




/* *********** C O M P O S I T O R - C A L L B A C K S ************** */



/* ******************** top row callbacks *************************** */

/* Basename text entry */
static void
comp_basename_changed_cb(QuickDcpAppWindow * self)
{
		char	* new_name = strdup(gtk_entry_get_text(self->compBasenameEntry));
		
	//~ printf("Quickdcp : project's basename was modified to %s\n", new_name);
	
	/* DCP name */
	gtk_label_set_text(self->compDcpName, get_dcp_name_from_widgets(self));

	if (gtk_entry_get_text_length(self->compBasenameEntry) == 0)
	{
		gtk_entry_set_placeholder_text(self->compBasenameEntry, DEFAULT_BASENAME_TEXT);
		//~ gtk_label_set_text(	self->compDcpName, DEFAULT_DCPNAME_TEXT);
		
	}
	
	/* save new name in project and all other depending variables */
	if (self->qdcp->active_project != NULL)
	{
		rename_project(new_name,
					   self->qdcp->active_project,
					   self->qdcp);
	}
}

/* Basename Ok button */
static void
comp_validate_basename_clicked(QuickDcpAppWindow * self)
{
		char	* new_name = strdup(gtk_entry_get_text(self->compBasenameEntry));

	printf("Quickdcp : Ok, project's basename was modified to %s\n", new_name);

	/* DCP name */
	gtk_label_set_text(self->compDcpName, get_dcp_name_from_widgets(self));

	/* save new name in project and all other depending variables */
	if (self->qdcp->active_project != NULL)
	{
		rename_project(new_name,
						self->qdcp->active_project,
						self->qdcp);

		set_unsaved(self);
	}
	return;
}

static void
comp_time_spin_changed(GtkSpinButton * button,
						gpointer	   user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting time laps to %f\n", gtk_spin_button_get_value(button));
		self->qdcp->active_project->laps = (float) gtk_spin_button_get_value(button);
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}

static void
comp_format_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting format to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->format = gtk_combo_box_text_get_active_text(combobox);
		
		set_unsaved(self);
	}
	
	gtk_widget_queue_draw(GTK_WIDGET (self->compDrawingArea));

	return;
}
static void
comp_resolution_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting resolution to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->resolution = gtk_combo_box_text_get_active_text(combobox);
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}
static void
comp_category_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting category to %s\n", gtk_combo_box_text_get_active_text(combobox));
		printf("Quickdcp : Setting category to %d\n", (int) gtk_combo_box_get_active(GTK_COMBO_BOX (combobox)));
		self->qdcp->active_project->category = (int) gtk_combo_box_get_active(GTK_COMBO_BOX (combobox));
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}
static void
comp_framerate_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting framerate to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->framerate = atoi(gtk_combo_box_text_get_active_text(combobox));
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}

static void
comp_background_color_chooser_activated(GtkColorChooser * button,
										gpointer		  user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;
		GdkRGBA				* 	rgba_color = malloc(sizeof(GdkRGBA));
		char					hex_color[COLOR_STR_LEN];
		
	printf("Quickdcp : Setting a new background color\n");
	
	//~ if (gtk_color_chooser_get_use_alpha(self->compBackgroundColorBtn))
		//~ printf("use alpha\n");
	//~ else
		//~ printf("no alpha\n");
		
	//~ gtk_color_chooser_set_use_alpha(self->compBackgroundColorBtn, TRUE);

	gtk_color_chooser_get_rgba(button, rgba_color);
	
	self->canvas->bg_color = gdk_rgba_copy(rgba_color);
	printf("Quickdcp : - background color was changed to %s\n", gdk_rgba_to_string(rgba_color));
			
	gtk_widget_queue_draw(GTK_WIDGET (self->compDrawingArea));

	if (self->qdcp->active_project != NULL)
	{
		gui_color_rgba2hex(hex_color, rgba_color);
			
		printf("Quickdcp : background color was changed to %s\n", hex_color);

		/* pass the returned color to the current project */
		sprintf(self->qdcp->active_project->background, "%s", hex_color);
		
		set_unsaved(self);
	}

	return;
}



/* ********************* piles callbacks **************************** */

/* piles tree view */
qdcp_bool
comp_scan_available_piles(QuickDcpAppWindow * self)
{
		GFile			*	proj_dir;
		GFileEnumerator * 	pile_list;
		GCancellable	*	cancel;
		GError			*   error = NULL;

	/* get a clean slate */
	gtk_tree_store_clear(self->compPilesTreeStore);
	
	/* Scan actual piles (i.e. .qdx projects) */
	
	proj_dir = g_file_new_for_path(self->qdcp->proj_dir);

	/* use g_file_enumerate_children_async() instead of the following */
	pile_list = g_file_enumerate_children(proj_dir, 		/* GFile 		* file */
										  "standard::*",	/* const char 	* attributes */
										  G_FILE_QUERY_INFO_NONE, /* other possible flag : G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS */
										  cancel,			/* GCancellable * cancellable if not NULL */
										  &error			/* GError ** error */
										  );
	if (error != NULL)
	{
		/* raise a notification and return */
		printf("QuickDcp : scan of %s for piles failed ! :: %s\n", self->qdcp->proj_dir, error->message);
		g_error_free(error);
		error = NULL;

		return QDCP_FAILURE;
	}
	if (pile_list == NULL)
	{
		/* raise a notification and return */
		printf("QuickDcp : scanning %s for piles failed !\n", self->qdcp->proj_dir);

		return QDCP_FAILURE;
	}

	printf("Quickdcp : Scanning default directory for piles : %s\n", self->qdcp->proj_dir);

	/* store all that with a loop */

		GtkTreeIter 	iter1;
		int				counter = 0;
	
	while (TRUE)
	{
			GFileInfo 	* info;
			GFile		* file;
		g_file_enumerator_iterate(pile_list,
										&info,
										&file,
										cancel,	/* GCancellable * cancellable if not NULL */
										&error	/* GError ** error */
										);

		/* exit the loop at the end of iteration */
		if (!info)
		{
			break;
		}
		
		/* now we can access file's attributes */
			char ** attributes;
			char *	attr;
			char *  value;
			int		cc;
			
		attributes = g_file_info_list_attributes(info, NULL);

		counter++;
		printf("QuickDcp :  - seeing %s \n", 
			g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_NAME));
		
		/* list all file attributes in console */
		//~ cc = 0;
		//~ while (attributes[cc] != NULL)
		//~ {
			//~ value = g_file_info_get_attribute_as_string(info, attributes[cc]);
			//~ printf("Quickdcp : attribute type=%d : key : %s = %s \n", 
					//~ g_file_info_get_attribute_type(info, attributes[cc]),
					//~ attributes[cc], 
					//~ value);
			//~ cc++;
		//~ }
		

		/* current file must be a qdp file to be shown in the treeview */

		if (strstr(g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_CONTENT_TYPE), "xml")
		 && strstr(g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_NAME),".qdx")
		)
		{
				const char		*	qdx_name;
				char				qdx_path[PATH_STR_LEN];
				GdkPixbuf		*	qdx_icon;
				char				qdx_icon_path[PATH_STR_LEN];
				char				tooltip[NAME_STR_LEN];
				t_project		*	pj;
				char			*	pj_name;
				GError			*	error = NULL;
				
				/* more stuff for the tooltip ? */
				//~ char				s_size[NAME_STR_LEN];
				//~ char				s_date[NAME_STR_LEN];
				//~ char				s_time[NAME_STR_LEN];
				//~ GTimeVal		*	modif_time;
				//~ int					num_layers;
				//~ int					num_wav;
				//~ int					time_length;

			qdx_name = g_file_info_get_display_name(info);
			
			sprintf(qdx_path, "%s%s", self->qdcp->proj_dir, qdx_name);
			if (load_project(qdx_path, self->qdcp->proj_list, self->qdcp))
			{
				pj = self->qdcp->proj_list->last;
				pj_name = strdup(pj->name);
				
				/* in the tooltip we could show the number of layers
				 * weather we have wav or subs, and the project status
				 */
				sprintf(tooltip, "%s", get_dcp_name_from_project(pj, self->qdcp));
			}
			else
			{
				sprintf(pj_name, "%s", qdx_name);
				sprintf(tooltip, "Impossible de charger ce projet");
			}
			
			/* get a generic icon */
			sprintf(qdx_icon_path, "%s%sscalable%sstatus%scamera-bullet.svg", ICON_DIR, SEPARATOR, SEPARATOR, SEPARATOR);
			qdx_icon = gdk_pixbuf_new_from_file_at_size(qdx_icon_path, 16, 16, &error);
			if (error)
			{
				g_critical ("Could not load pixbuf: %s\n", error->message);
				g_error_free(error);
			}

			/* set folder data in a tree store row */
			gtk_tree_store_append(self->compPilesTreeStore, &iter1, NULL);
			gtk_tree_store_set(self->compPilesTreeStore, 	&iter1,
								PILES_CHECK_COL, 	FALSE, 
								PILES_ICON_COL,  	qdx_icon,
								PILES_NAME_COL,		pj_name,
								PILES_PATH_COL,		qdx_path,
								PILES_ID_COL,		pj->p_id,
								PILES_TOOLTIP_COL,	tooltip,
								-1); /* end of the list */

			g_object_unref(qdx_icon);
		}
	}
}

static void
comp_new_pile_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : compositor, new pile button clicked");

	/* get the layers page of left notebook */
	gtk_notebook_set_current_page(self->compLeftBox, 1);
}

static void
comp_delete_pile_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : compositor, delete pile button clicked");
}

static void
comp_refresh_piles_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : refresh piles treeview\n");
}


static void
comp_piles_toggle_checked_cb(GtkCellRendererToggle * cell,
							 GtkTreePath           * path,
							 gpointer                user_data)
{
		QuickDcpAppWindow 	* self = user_data;
		gboolean			  state;
		gchar				* name;
		GtkTreeIter			* iter;

	printf("Quickdcp : toggled a check cell from piles\n");

	gtk_tree_model_get_iter(GTK_TREE_MODEL (self->compPilesTreeStore), 
							iter, 
							path);
							
	gtk_tree_model_get(GTK_TREE_MODEL (self->compPilesTreeStore), 
					   iter, 
					   PILES_CHECK_COL, state, 
					   PILES_NAME_COL, name, 
					   -1);
					   
	printf("           old state of %s = %d\n", name, state);

	/* invert the state of toggle cell */
	//~ if (state == FALSE)
	//~ {
		//~ printf("           new state of %s = %d\n", name, state);
		//~ 
		//~ gtk_tree_store_set(GTK_TREE_STORE (self->compPilesTreeStore), 
						   //~ iter,
						   //~ PILES_CHECK_COL, TRUE,
					       //~ -1);
		//~ gtk_widget_set_sensitive(GTK_WIDGET (self->compDeletePilesBtn), TRUE);
	//~ }
	//~ else
	//~ {		
		//~ gtk_tree_store_set(GTK_TREE_STORE (self->compPilesTreeStore), 
						   //~ iter,
						   //~ PILES_CHECK_COL, FALSE,
					       //~ -1);
		//~ 
		//~ /* walk the tree to check if this was the last checked row */
			//~ qdcp_bool	valid;
			//~ qdcp_bool	empty = TRUE;
			//~ 
		//~ valid = gtk_tree_model_get_iter_first (
							//~ GTK_TREE_MODEL (self->compPilesTreeStore), 
							//~ iter);
		//~ /* looping the successive iterators */
		//~ while (valid)
		//~ {
			//~ gtk_tree_model_get(
							//~ GTK_TREE_MODEL (self->compPilesTreeStore), 
							//~ iter, 
							//~ PILES_CHECK_COL, state, 
							//~ -1);
			//~ if (state)
				//~ empty = FALSE;
				//~ 
			//~ valid = gtk_tree_model_iter_next (
							//~ GTK_TREE_MODEL (self->compPilesTreeStore), 
							//~ iter);
		//~ }
		//~ if (empty)
		//~ {
			//~ /* set appropriate widgets to insensitive */
			//~ gtk_widget_set_sensitive(GTK_WIDGET (self->compDeletePilesBtn), FALSE);
		//~ }
	//~ }
}

static void
comp_piles_row_activated_cb(GtkTreeView 	  * treeview,
							GtkTreePath		  *	path,
							GtkTreeViewColumn *	col,
							gpointer 			user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;
		GtkTreeIter 		*	iter;
		char					qdx_path[PATH_STR_LEN];
        int						pj_id;
		t_project 			* 	cur_pj = self->qdcp->proj_list->first;
	
	printf("Quickdcp : activated a row from piles tree view\n");	
	
	//~ gtk_tree_model_get_iter(GTK_TREE_MODEL (self->compPilesTreeStore), 
							//~ iter, 
							//~ path);
							//~ 
	//~ gtk_tree_model_get(
					//~ GTK_TREE_MODEL(self->compPilesTreeStore),
					//~ iter, 
					//~ PILES_PATH_COL, qdx_path,
					//~ PILES_ID_COL,	pj_id,
					//~ -1);
		//~ 
	//~ /* A project listed in the treeview is supposed to be loaded already 
	 //~ * in the project list */
	//~ while (cur_pj->next != NULL)
	//~ {
		//~ if (cur_pj->p_id == pj_id)
		//~ {
			//~ printf("Quickdcp - Activating project %s", cur_pj->name);
			//~ /* set the double-clicked project active */
			//~ self->qdcp->active_project = cur_pj;
			//~ 
			//~ /* and set the compositor widgets accordingly */
			//~ comp_set_widgets_values(self->qdcp->active_project, self);
//~ 
			//~ return;
		//~ }
		//~ 
		//~ cur_pj = cur_pj->next;
	//~ }				
//~ 
	//~ return;
}


/* ********************** layers callbacks ************************** */

static void
comp_new_image_layer_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : new image layer button clicked\n");
}

static void
comp_duplicate_layer_btn_cb(QuickDcpAppWindow * self)
{

	printf("Quickdcp : duplicate current layer button clicked\n");
}

static void
comp_delete_layer_btn_cb(QuickDcpAppWindow * self)
{

	printf("Quickdcp : delete current layer button clicked\n");
}


/* when a check cell is toggled */
static void
comp_layers_mask_layer_cb(GtkCellRendererToggle * cell_renderer,
						  gchar                 * path,
						  gpointer                user_data)
{
		QuickDcpAppWindow * self = user_data;

	printf("Quickdcp : mask/unmask a layer");
}

/* when a layer is selected or single-clicked */
static void
comp_layers_focus_layer_cb(GtkTreeView 	  	  * treeview,
						   GtkTreePath		  *	path,
						   GtkTreeViewColumn *	col,
						   gpointer 			user_data)
{
		QuickDcpAppWindow * self = user_data;

	printf("Quickdcp : focus a layer\n");
}

/* when rows are dragndropped */
static void
comp_layers_reorder_cb(GtkTreeView * treeview,
					   gpointer 	 user_data)
{
		QuickDcpAppWindow * self = user_data;

	printf("Quickdcp : reorder layers in pile\n");
	//~ void
	//~ gtk_tree_model_rows_reordered (GtkTreeModel *tree_model,
								   //~ GtkTreePath *path,
								   //~ GtkTreeIter *iter,
								   //~ gint *new_order);
}


/* ******************** pango text callbacks ************************ */

static void
comp_new_text_layer_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : new text layer\n");
}

static void
comp_text_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data)
{
		QuickDcpAppWindow * self = user_data;

	/* pass the returned color to the current project */
	//~ printf("Quickdcp : text color was changed to %s\n", gdk_rgba_to_string(color));
	//~ gtk_color_chooser_get_rgba();
	//~ sprintf(self->qdcp->active_project->background, "%s", color_rgba2hex(color));
	set_unsaved(self);
	
}


/* **************** vectorial shapes callbacks ********************** */

static void
comp_shapes_line_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data)
{
		QuickDcpAppWindow * self = user_data;

	/* pass the returned color to the current project */
	//~ printf("Quickdcp : text color was changed to %s\n", gdk_rgba_to_string(color));
	//~ gtk_color_chooser_get_rgba();
	//~ sprintf(self->qdcp->active_project->background, "%s", color_rgba2hex(color));
	set_unsaved(self);
	
}

static void
comp_shapes_fill_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data)
{
		QuickDcpAppWindow * self = user_data;

	/* pass the returned color to the current project */
	//~ printf("Quickdcp : text color was changed to %s\n", gdk_rgba_to_string(color));
	//~ gtk_color_chooser_get_rgba();
	//~ sprintf(self->qdcp->active_project->background, "%s", color_rgba2hex(color));
	set_unsaved(self);
	
}


/* ******************* transitions callbacks ************************ */

static void
comp_fondu_deb_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data)
{
		QuickDcpAppWindow * self = user_data;

	/* pass the returned color to the current project */
	//~ printf("Quickdcp : text color was changed to %s\n", gdk_rgba_to_string(color));
	//~ gtk_color_chooser_get_rgba();
	//~ sprintf(self->qdcp->active_project->background, "%s", color_rgba2hex(color));
	set_unsaved(self);
	
}

static void
comp_fondu_fin_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data)
{
		QuickDcpAppWindow * self = user_data;

	/* pass the returned color to the current project */
	//~ printf("Quickdcp : text color was changed to %s\n", gdk_rgba_to_string(color));
	//~ gtk_color_chooser_get_rgba();
	//~ sprintf(self->qdcp->active_project->background, "%s", color_rgba2hex(color));
	set_unsaved(self);
	
}


static void
comp_generate_seq_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : generate sequence button clicked\n");
}

static void
comp_generate_dcp_btn_cb(QuickDcpAppWindow * self)
{
	printf("Quickdcp : generate DCP button clicked\n");
}




/* ********* D R A W I N G   A R E A  -  C A L L B A C K S ********** */



/* other signals from drawing area :
 * drag-begin
 * drag-data-delete
 * drag-data-get
 * drag-data-received
 * drag-drop
 * drag-end
 * drag-failed
 * drag-leave
 * drag-motion
 * enter-notify-event
 * event
 * event-after
 * focus
 * focus-in-event
 * focus-out-event
 * grab-*
 * key-*
 * leave-notify-event
 * map
 * map-event
 * motion-notify-event
 * move-focus
 * screen-changed
 * scroll-event
 * selection-*
 * show / hide
 * touch-event
 * unmap
 * unmap-event
 * unrealize
 * visibility-notify-event
 * window-state-event
 *   and some more...
 */
static void
draw_drawing_area_configure_event_cb(GtkDrawingArea * dwa,
									 GdkEvent	 	* event,
									 gpointer		  user_data)
{
		QuickDcpAppWindow * self = user_data;
		
	printf("Quickdcp : configure-event on drawing area\n");
		
	return;
}

static void
draw_drawing_area_realize_cb(GtkDrawingArea * dwa,
							 GdkEvent		* event,
							 gpointer		  user_data)
{
		
		
	printf("Quickdcp : realize signal sent from the drawing area\n");

	/* deprecated : */
	//~ self->canvas->cairo = gdk_cairo_create();
	
	//~ cairo_region = gdk_cairo_region_create_from_surface();
	//~ 
	//~ drw_ctx = gdk_window_begin_draw_frame(gdk_event_get_window(event), self->canvas->cairo);
	//~ 
	//~ if (gdk_drawing_context_is_valid)
	//~ {
		//~ printf("Quickdcp : got a valid drawing context\n");
		//~ self->canvas->cairo = gdk_drawing_context_get_cairo_context(drw_ctx);
	//~ 
	//~ 
	//~ }


	/* when we're finished, draw the whole work all at once */
	//~ gdk_window_end_draw_frame(gdk_event_get_window(event), drw_ctx);
	
	return;
}

static void
draw_on_resize_event_cb(GtkDrawingArea  * dwa,
			 		    GdkEvent		* event,
						gpointer		  user_data)
{
		QuickDcpAppWindow * self = user_data;
		
	printf("Quickdcp : canvas resize_event\n");	

	self->canvas->win_w = gtk_widget_get_allocated_width (GTK_WIDGET (dwa));
	self->canvas->win_h = gtk_widget_get_allocated_height (GTK_WIDGET (dwa));

	//~ printf("Quickdcp : resized win_w = %f  win_h = %f\n", self->canvas->win_w, self->canvas->win_h);
}		 	

static void
draw_drawing_area_size_allocate_cb(GtkDrawingArea * dwa,
								   GdkEvent		  * event,
								   gpointer		    user_data)
{
		QuickDcpAppWindow * self = user_data;

	printf("Quickdcp : size-allocate signal sent from the drawing area\n");
	self->canvas->win_w = gtk_widget_get_allocated_width (GTK_WIDGET (dwa));
	self->canvas->win_h = gtk_widget_get_allocated_height (GTK_WIDGET (dwa));

	//~ printf("Quickdcp : allocated win_w = %f  win_h = %f\n", self->canvas->win_w, self->canvas->win_h);
}

static void
draw_canvas_draw_event_cb(GtkDrawingArea  * dwa,
						  cairo_t		  * cairo,
						  gpointer		    user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;
		cairo_region_t		*	cairo_region;
		cairo_pattern_t		*	empty_ptn;
		cairo_surface_t		*	empty_ptn_surf;
		char					png_file[PATH_STR_LEN] = {0};
		
	//~ printf("Quickdcp : canvas redraw event\n");
	
	/* first of all we'll keep a cairo reference for later operations */
	self->canvas->cairo = cairo_reference(cairo);
	
	/* we can print here some cairo information for debugging purposes */
	
	
	/* in any case we fill the whole area with 'empty' pattern */	

	cairo_translate(cairo, 0, 0);
	
	//~ sprintf(png_file, "%s%spatterns%sempty.png", SHARE_DIR, SEPARATOR, SEPARATOR);
	sprintf(png_file, "%s%spatterns%sempty-dark.png", SHARE_DIR, SEPARATOR, SEPARATOR);

	empty_ptn_surf = cairo_image_surface_create_from_png(png_file);
	empty_ptn = cairo_pattern_create_for_surface(empty_ptn_surf);	
	cairo_pattern_set_extend(empty_ptn, CAIRO_EXTEND_REPEAT);
	self->canvas->empty_ptn = cairo_pattern_reference(empty_ptn);		
	cairo_set_source(self->canvas->cairo, self->canvas->empty_ptn);
	cairo_paint(self->canvas->cairo);
		
	/* provisoirement avant le test de projet actif : */
	
	/* compute screen size using drawing area width */
	self->canvas->screen_w = 2048;
	self->canvas->screen_h = 1080;
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(self->compFormatCBB)) == FMT_FLAT)
		self->canvas->screen_w = 1998;
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(self->compFormatCBB)) == FMT_SCOPE)
		self->canvas->screen_h = 858;
	if (gtk_combo_box_get_active(GTK_COMBO_BOX(self->compFormatCBB)) == FMT_VIDEO)
		self->canvas->screen_w = 1920;
		
	//~ printf("win w  		:  %f\n", self->canvas->win_w);
	//~ printf("win h  		:  %f\n", self->canvas->win_h);
	//~ printf("screen w  	:  %d\n", self->canvas->screen_w);
	//~ printf("screen h  	:  %d\n", self->canvas->screen_h);

	/* default size is 'zoom-fit' based on the drawing area width */
	self->canvas->w = self->canvas->win_w;
	self->canvas->h = self->canvas->screen_h * (self->canvas->win_w / self->canvas->screen_w);

	/* center the screen representation */
	self->canvas->x = 0;
	self->canvas->y = round(self->canvas->win_h / 2) - round(self->canvas->h / 2);


	//~ printf("x	:  %d\n", self->canvas->x);
	//~ printf("y	:  %d\n", self->canvas->y);
	//~ printf("w	:  %f\n", self->canvas->w);
	//~ printf("h	:  %f\n", self->canvas->h);
	
	//~ gdk_cairo_set_source_rgba(cr, GdkRGBA);
	//~ cairo_set_source_surface(canvas->cairo, canvas->surf, 1, 1);
	cairo_set_source_rgb(cairo, 
						 self->canvas->bg_color->red,
						 self->canvas->bg_color->green,
						 self->canvas->bg_color->blue
						 );

	/* Fill the background with solid bg color */
	cairo_rectangle(cairo, 
					self->canvas->x, 
					self->canvas->y, 
					self->canvas->w, 
					self->canvas->h
					);	

	cairo_fill(cairo);
	
		
	/* now, we continue only if we have an active project */
	if (self->qdcp->active_project == NULL)
		return;
	
	gtk_color_chooser_get_rgba(self->compBackgroundColorBtn, self->canvas->bg_color);
	
	//~ self->canvas->win_h = self->canvas->win_w * (self->qdcp->active_project->mx_w / self->qdcp->active_project->mx_h);
	
	/* draw the background */
	
		//~ rectangle = Gdk.Rectangle()
		//~ rectangle.x = self.win_x
		//~ rectangle.y = self.win_y
		//~ rectangle.width = self.win_w
		//~ rectangle.height = self.win_h
		//~ 
		//~ #~ self.window.invalidate_rect(rectangle)
		//~ self.queue_draw_area(0, 0, self.win_w, self.win_h)
		
	return;
}

static void
draw_enter_canvas_set_cursor(GtkDrawingArea * 	dwa,
							 gpointer			user_data)
{
	printf("Quickdcp : entering the working area\n");
}

static void
draw_leave_canvas_reset_cursor(GtkDrawingArea * dwa,
							   gpointer			user_data)
{
	printf("Quickdcp : entering the working area\n");
}

static void
draw_drawing_area_try_layer_selection_by_click_cb(GtkDrawingArea * 	dwa,
												  gpointer			user_data)
{
	printf("Quickdcp : tried to select a layer in the drawing area\n");
}




/* ********* D R A W I N G   A R E A  -  F U N C T I O N S ********** */



qdcp_bool
draw_drawing_area_init(QuickDcpAppWindow * self)
{
	/* https://developer.gnome.org/gtk3/stable/ch01s05.html */
			canvas_ctx 	 	*	cvs = calloc(1, sizeof(*cvs));
	
	printf("Quickdcp : drawing area init\n");
	/* new_project, layers list, wav_tracks list, j2k_list, wav_files list, subtitles list */
	if (cvs == NULL)
	{
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	}
	
	self->canvas = cvs;
	self->canvas->cairo = NULL;
	self->canvas->empty_ptn = NULL;
	self->canvas->win_w = gtk_widget_get_allocated_width (GTK_WIDGET (self->compDrawingArea));
	self->canvas->win_h = gtk_widget_get_allocated_height (GTK_WIDGET (self->compDrawingArea));
	self->canvas->x = 0;
	self->canvas->y = 0;
	self->canvas->scroll_x = 0;
	self->canvas->scroll_y = 0;
	self->canvas->bg_color = malloc(sizeof(GdkRGBA));
	if (self->canvas->bg_color == NULL)
	{
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	}
	gtk_color_chooser_get_rgba(self->compBackgroundColorBtn, self->canvas->bg_color);
	self->canvas->zoom_factor = 1.0;
	self->canvas->selecting = QDCP_FALSE;
	self->canvas->dragging = QDCP_FALSE;
	self->canvas->tool_mode = NULL;
	self->canvas->tool_ox = 0;
	self->canvas->tool_oy = 0;
	self->canvas->tool_xx = 0;
	self->canvas->tool_yy = 0;
	self->canvas->layer_group = 0;
	self->canvas->selected_box = 0;
	//~ self->canvas->error = NULL;
	
	gtk_widget_set_events(GTK_WIDGET (self->compDrawingArea), 
						  gtk_widget_get_events(GTK_WIDGET (self->compDrawingArea))
						  | GDK_BUTTON_PRESS_MASK
						  | GDK_POINTER_MOTION_MASK
						  );
						//~ | Gdk.EventMask.EXPOSURE_MASK
						//~ | Gdk.EventMask.LEAVE_NOTIFY_MASK
						//~ | Gdk.EventMask.FOCUS_CHANGE_MASK
						//~ | Gdk.EventMask.SCROLL_MASK
						//~ | Gdk.EventMask.TOUCH_MASK
						//~ | Gdk.EventMask.BUTTON_PRESS_MASK
						//~ | Gdk.EventMask.BUTTON_MOTION_MASK
						//~ | Gdk.EventMask.BUTTON_RELEASE_MASK
						//~ | Gdk.EventMask.POINTER_MOTION_MASK
						//~ | Gdk.EventMask.TOUCHPAD_GESTURE_MASK
						
	/* signal to retrieve a surface from a newly created or resized gdk window */
	//~ self->canvas->surf = g_connect_signal(
							//~ gtk_widget_get_window(GTK_WIDGET (self->compDrawingArea)),
							//~ gtk_widget_get_allocated_width(GTK_WIDGET (self->compDrawingArea)),
							//~ gtk_widget_get_allocated_height(GTK_WIDGET (self->compDrawingArea)),
							//~ get_a_gdk_window_surface_on_creation,
							//~ self);
	

	//~ if (gdk_display_supports_cursor_alpha(GdkDisplay * display)
	//~ gdk_display_supports_cursor_color(); /* or B&W */
	//~ gdk_display_get_default_cursor_size(display);
	//~ gdk_cursor_new_from_pixbuf();
	//~ gdk_window_set_cursor();
}

qdcp_bool
draw_set_background(canvas_ctx * canvas, t_project * pj)
{
	/* Draw a cairo background in Canvas */
	printf("Quickdcp : + painting background\n");
		
	printf("Not here 2342\n");
		int w = (int) ceil(pj->mx_w * canvas->zoom_factor);
	printf("Not here 2344\n");
		int h = (int) ceil(pj->mx_h * canvas->zoom_factor);

	printf("Not here 2345\n");
	if (canvas->surf == NULL)
	{
		printf("Quickdcp : draw_set_background() : canvas surface not initialized\n");
		return QDCP_FAILURE;
	}
		
	printf("Not here 2352\n");
	
	//~ gdk_cairo_set_source_rgba(cr, GdkRGBA);
	
	//~ gdk_cairo_surface_create_from_pixbuf(pixbuf, scale, GdkWindow * for_window);
	
	//~ gdk_cairo_set_source_pixbuf(cr, pixbuf, gdouble x, gdouble y);
	
	//~ gdk_cairo_rectangle(cr, GdkRectangle * rectangle);
	
	//~ cairo_set_source_surface(canvas->cairo, canvas->surf, 1, 1);
	cairo_set_source_rgb(canvas->cairo, 
						 canvas->bg_color->red,
						 canvas->bg_color->green,
						 canvas->bg_color->blue
						 );
	printf("Not here 2360\n");

	/* Fill the background with solid bg color */
	cairo_rectangle(canvas->cairo, 0, 0, w, h);
	printf("Not here 2364\n");

	cairo_fill(canvas->cairo);

}


qdcp_bool
draw_get_geometry(canvas_ctx * canvas, t_project * pj)
{
		//~ """This is called when updating screen, before updating widgets"""
		//~ 
		//~ l_id = ly.params['l_id']
		//~ 
		//~ if frame is None:
			//~ say("+ Computing frame failed : frame is None")
			//~ return
						//~ 
		//~ say("+ Computing layer " + str(ly.params['l_id']) )
//~ 
		//~ # this is where we gonna stock the values
		//~ cur_geom = {}
			//~ 
		//~ # if we have only one keyframe we stick to that
		//~ if len(ly.params['keyframes']) == 1:
			//~ # force the keyframe to be an int when it comes from a recorded project
			//~ ep = unicode(ly.params['entry_point'])
//~ 
			//~ cur_geom = ly.params[ep]
					//~ 
		//~ # if self.current.frame is a keyframe for the layer, we have it already computed
		//~ elif frame in ly.params['keyframes']:
			//~ 
			//~ cur_geom = ly.params[unicode(frame)]
			//~ 
		//~ # else, between keyframes, we have to compute
		//~ else:
			//~ # get previous and following keyframe
			//~ prev_kf = 0
			//~ next_kf = 0
			//~ for kf in sorted(ly.params['keyframes']):
				//~ if kf < frame:
					//~ prev_kf = kf
				//~ if kf > frame:
					//~ next_kf = kf
					//~ break
			//~ 
			//~ # if there is no following frame, we keep the previous one as is
			//~ if next_kf == 0:
				//~ 
				//~ cur_geom = ly.params[unicode(prev_kf)]
				//~ 
			//~ # ... or we really do have to compute
			//~ else:
				//~ # interpolate parameters where prev and next KFs are different
				//~ cur_geom = self.compute_frame(frame, ly, prev_kf, next_kf)
//~ 
		//~ # we treat entry and exit fade in and fade out here,
		//~ # over other keyframes parameters
		//~ fade_in = int(ly.params['fade_in'])
		//~ fade_out = int(ly.params['fade_out'])
		//~ exit_point = int(ly.params['exit_point'])
		//~ cur_geom['fade_opacity'] = cur_geom['opacity']
		//~ if frame < fade_in:
			//~ cur_geom['fade_opacity'] = (cur_geom['opacity'] / fade_in) * (frame - 1)
			//~ 
		//~ if frame > exit_point - fade_out:
			//~ cur_geom['fade_opacity'] = (cur_geom['opacity'] / fade_out) * (exit_point - frame)
					//~ 
		//~ return cur_geom
}

qdcp_bool
draw_compute_frame(canvas_ctx * canvas, t_project * pj)
{
		//~ # length of the interpolation between the to keyframes
		//~ trip = next_kf - prev_kf
		//~ pos = frame - prev_kf
		//~ 
		//~ say(" prev_kf = " + str(prev_kf) + " next_kf = " + str(next_kf) + " pos = " + str(pos))
		//~ 
		//~ geometry = {}
		//~ ## Translation
		//~ # ox
		//~ geometry['ox'] = self.compute_value('ox', frame, ly, prev_kf, next_kf)
		//~ # oy
		//~ geometry['oy'] = self.compute_value('oy', frame, ly, prev_kf, next_kf)
		//~ # in %
		//~ geometry['oxp'], geometry['oyp'] = self.pix2perc(geometry['ox'], geometry['oy'])
//~ 
		//~ ## Scaling
		//~ # width
		//~ geometry['w'] = self.compute_value('w', frame, ly, prev_kf, next_kf)
		//~ # height
		//~ geometry['h'] = self.compute_value('h', frame, ly, prev_kf, next_kf)				
		//~ # in %
		//~ geometry['wp'], geometry['hp'] = self.pix2perc(geometry['w'], geometry['h'])
		//~ # ratio
		//~ # dirty hack to avoid division by zero :
		//~ if geometry['h'] == 0: geometry['h'] = 1	
		//~ geometry['ratio'] = geometry['w'] / geometry['h']
		//~ 
		//~ 
		//~ ## Rotation
		//~ # centerX
		//~ geometry['centerX'] = self.compute_value('centerX', frame, ly, prev_kf, next_kf)
		//~ # centerY
		//~ geometry['centerY'] = self.compute_value('centerY', frame, ly, prev_kf, next_kf)
		//~ # rotrad
		//~ geometry['rotrad'] = self.compute_value('rotrad', frame, ly, prev_kf, next_kf)
		//~ # rotdeg
		//~ #~ self.cur_geom['rotdeg'] = self.cur_geom.degrees
		//~ geometry['rotdeg'] = geometry['rotrad'] * (180 / math.pi)
		//~ #~ self.geometry['rotrad'] = self.geometry['rotdeg'] * (math.pi / 180)
//~ 
		//~ ## Opacity
		//~ geometry['opacity'] = self.compute_value('opacity', frame, ly, prev_kf, next_kf)
		//~ 
		//~ ## Topleft deduction
		//~ geometry['x'] = geometry['ox'] - int(geometry['w'] * (geometry['centerX'] / 100))
		//~ geometry['y'] = geometry['oy'] - int(geometry['h'] * (geometry['centerY'] / 100))
				//~ 
		//~ return geometry
}
										
qdcp_bool
draw_compute_value(canvas_ctx * canvas, t_project * pj)
{
		//~ """ """
		//~ trip = next_kf - prev_kf
		//~ pos = frame - prev_kf
		//~ prev_dix = ly.params[unicode(prev_kf)]
		//~ next_dix = ly.params[unicode(next_kf)]
		//~ 
		//~ if prev_dix[option] != next_dix[option]:
			//~ diff = next_dix[option] - prev_dix[option]
			//~ factor = ( diff / trip)
			//~ value = prev_dix[option] + (factor * pos)
			//~ say(" -Computed " + option + " current value : " + str(value))
			//~ say("  previous value = " + str(prev_dix[option]) + " next value = " + str(next_dix[option]) + " factor = " + str(factor))
			//~ say("  difference = " + str(diff))
		//~ else:
			//~ value = prev_dix[option]
			//~ #~ say(" -Did not compute " + option)
			//~ 
		//~ return value
}
												 
qdcp_bool
draw_compute_translation(canvas_ctx * canvas, t_project * pj)
{
		//~ say("Compute translation (and zoom)")
//~ 
		//~ #~ if self.win_w > self.mx_w * self.factor:
		//~ x = ((self.win_w - (self.mx_w * self.factor)) / 2) + self.scroll_x
		//~ #~ if self.win_h > self.mx_h * self.factor:
		//~ y = ((self.win_h - (self.mx_h * self.factor)) / 2) + self.scroll_y
//~ 
		//~ #~ say(" x = " + str(x))
		//~ #~ say(" y = " + str(y))
		//~ 
		//~ return (x, y)
}
		
qdcp_bool
draw_pix2perc(int x, int y)
{
		//~ mx_w = self.current.pile.mx_w
		//~ mx_h = self.current.pile.mx_h
		//~ xp = float(x) / float(mx_w) * 100
		//~ yp = float(y) / float(mx_h) * 100
		//~ return (xp, yp)
}

qdcp_bool
draw_zoom_in(canvas_ctx * canvas, t_project * pj)
{
		//~ if self.current.pile is None:
			//~ return
		//~ say("Zooming in 10%")
		//~ self.current.pile.zf = float(self.current.pile.zf) * 1.1
		//~ self.current.pile.save()
		//~ self.redraw()
}
		
qdcp_bool
draw_zoom_out(canvas_ctx * canvas, t_project * pj)
{
		//~ if self.current.pile is None:
			//~ return
		//~ say("Zooming out 10%")
		//~ self.current.pile.zf = float(self.current.pile.zf) * 0.9
		//~ self.current.pile.save()
		//~ self.redraw()
}
		
qdcp_bool
draw_zoom_fit(canvas_ctx * canvas, t_project * pj)
{
		//~ if self.current.pile is None:
			//~ return
		//~ say("Resizing the canvas to fit the drawing area")
		//~ self.current.pile.zf = 1
		//~ control_factor = (float(self.win_w) / self.mx_w)
		//~ if self.mx_h * control_factor > self.win_h:
			//~ self.current.pile.zf = (float(self.win_h) / self.mx_h) / control_factor
		//~ self.current.pile.save()
		//~ self.redraw()
}


/* ***************** S E Q U E N C E  - W D G T S ******************* */



qdcp_bool
vid_init_widgets(QuickDcpAppWindow * self)
{
	printf("Quickdcp : init video widgets\n");
}
	



/* ******************* A U D I 0 - W D G T S ************************ */



qdcp_bool
aud_init_widgets(QuickDcpAppWindow * self)
{
	printf("Quickdcp : init audio widgets\n");
}

qdcp_bool
aud_unprocessed_add_source_btn_cb(QuickDcpAppWindow * self)
{
}

qdcp_bool
aud_unprocessed_delete_btn_cb(QuickDcpAppWindow * self)
{
}

qdcp_bool
aud_unprocessed_refresh_btn_cb(QuickDcpAppWindow * self)
{
}

qdcp_bool
aud_unprocessed_listen_btn_cb(QuickDcpAppWindow * self)
{
}

qdcp_bool
aud_unprocessed_convert_btn_cb(QuickDcpAppWindow * self)
{
}


/* ****************** D C P - W I D G E T S ************************* */



qdcp_bool
dcp_init_widgets(QuickDcpAppWindow * self)
{
	dcp_scan_dcps(self, FALSE);
	
	gtk_tree_view_collapse_all(self->dcpTreeView);
}

qdcp_bool
dcp_config_widgets(QuickDcpAppWindow * self)
{
}

qdcp_bool	
dcp_store_dcps(QuickDcpAppWindow * self, GtkTreeStore * store)
{
	
	/* set data in a specific cell */
	//~ gtk_tree_store_set_value(store,
							 //~ iter,
							 //~ col,
							 //~ value
							 //~ );
							
//~ 
		//~ # Get a snapshot of output directory
		//~ self.scan_dcps()
		//~ 
		//~ ## Fill the Store with unfinished DCPs
		//~ 
		//~ for bname in self.unfinished:
//~ 
			//~ ## Modification date
			//~ if os.path.exists(os.path.join(var.OUTPUTDIR, bname, ASSETMAP.xml)):
				//~ date_reference = os.path.join(var.OUTPUTDIR, bname, ASSETMAP.xml)
			//~ else:
				//~ date_reference = os.path.join(var.OUTPUTDIR, bname)			
			//~ mod_time = os.path.getmtime(date_reference)
			//~ mod_time = datetime.datetime.fromtimestamp(mod_time)
			//~ mod_time = mod_time.strftime(' %d/%m/%Y à %H:%M:%S ')
//~ 
			//~ # let's try to get a time length
			//~ pfile = os.path.join(var.PROJETS, 'mxf_' + bname + '.qdp')
			//~ if os.path.exists(pfile):
				//~ td = ws.read_project(pfile)
				//~ # duree
				//~ laps = int(td.nmax) / int(td.p_params['framerate'])
				//~ s = str(laps % 60)
				//~ m = str(laps / 60)
				//~ duree = m + "’" + s + "”"
				//~ 
				//~ # tooltip = dcpname
				//~ tooltip = td.make_dcp_name
				//~ 
			//~ else:
				//~ duree = " - "
				//~ tooltip = mod_time
			//~ 
			//~ ## Dir size
			//~ size = 16000	# (size in bytes of an empty directory)
			//~ for z in glob.glob(os.path.join(var.WORKSPACE, '.mxf', bname, '*')):
				//~ size += os.path.getsize(z)
			//~ size = get_human_size(size)
//~ 
			//~ # DCP listStore item
			//~ icon = GdkPixbuf.Pixbuf.new_from_file_at_size(os.path.join(ROOT_DIR, 'icons','bullets','bad-dcp.png'), 25,25)
			//~ this_row = [False, icon, bname, duree, size, tooltip, mod_time]
			//~ self.dcp_store.append(None, this_row)
			//~ 
			//~ say("-stored unfinished DCP " + bname) 
					//~ 
		//~ ## Fill the store with finished DCPs
//~ 
		//~ for bname in self.dcp_list:
//~ 
			//~ ## Dir size
			//~ size = 16000	# (size in bytes of an empty directory)
			//~ for z in glob.glob(var.OUTPUTDIR + bname + '/*'):
				//~ size += os.path.getsize(z)
			//~ size = get_human_size(size)
//~ 
			//~ ## Modification date and time
			//~ if os.path.exists(var.OUTPUTDIR + bname + '/ASSETMAP.xml'):
				//~ date_reference = var.OUTPUTDIR + bname + '/ASSETMAP.xml'
			//~ else:
				//~ date_reference = var.OUTPUTDIR + bname
			//~ mod_time = os.path.getmtime(date_reference)
			//~ mod_time = datetime.datetime.fromtimestamp(mod_time)
			//~ mod_time = mod_time.strftime(' %d/%m/%Y à %H:%M:%S ')
//~ 
			//~ # Let's try to get project information
			//~ pfile = os.path.join(var.PROJETS, 'mxf_' + bname + '.qdp')
			//~ if os.path.exists(pfile):
				//~ td = self.gui.src.read_project(pfile)
				//~ # duree
				//~ laps = int(td.nmax) / int(td.p_params['framerate'])
				//~ s = str(laps % 60)
				//~ m = str(laps / 60)
				//~ duree = m + "’" + s + "”"
				//~ 
				//~ ## Tooltip = dcp name
				//~ tooltip = td.make_dcp_name()
				//~ 
			//~ else:
				//~ duree = " - "
				//~ tooltip = mod_time
							//~ 
			//~ # DCP listStore item
			//~ icon = GdkPixbuf.Pixbuf.new_from_file_at_size(os.path.join(ROOT_DIR, 'icons','bullets','dcp.png'), 24,24)
			//~ this_row = [False, icon, bname, duree, size, tooltip, mod_time]
			//~ newiter = self.dcp_store.append(None, this_row)
			//~ 
			//~ for ff in sorted(glob.glob(var.OUTPUTDIR + bname + '/*')):
				//~ childbname = os.path.basename(ff)
				//~ childsize = os.path.getsize(ff)
				//~ if childsize > 1073741824:
					//~ childsize /= 1073741824
					//~ childsize = ' ' + str(round(childsize,2)) + ' G'
				//~ elif childsize > 1048576:
					//~ childsize /= 1048576
					//~ childsize = ' ' + str(round(childsize,2)) + ' M'
				//~ elif childsize > 1024:
					//~ childsize/= 1024
					//~ childsize = ' ' + str(round(childsize,1)) + ' k'
				//~ else:
					//~ childsize = ' ' + str(childsize) + 'b'
//~ 
				//~ childdate = datetime.datetime.fromtimestamp(os.path.getmtime(ff)).strftime('%d-%m-%Y à %H:%M:%S')
				//~ childrow = [None, None, childbname, None, childsize, None, childdate]
				//~ childiter = self.dcp_store.insert_before(newiter, None, childrow)   
			//~ 
			//~ say("-stored complete DCP " + bname) 
//~ 
		//~ ## Fill the store with zipped files			
			//~ 
		//~ for thisZip in glob.glob(os.path.join(var.OUTPUTDIR, '*.zip')):
//~ 
			//~ ## Name
			//~ basename = os.path.basename(thisZip)
//~ 
			//~ ## File size
			//~ size = get_human_size(os.path.getsize(thisZip))
//~ 
			//~ ## File contents
			//~ with zipfile.ZipFile(thisZip,'r') as zqdp:
				//~ tooltip = _("Contents :")
				//~ for info in zqdp.namelist():
					//~ tooltip += '\n  ' + str(info)
//~ 
			//~ ## Modification date
			//~ mod_date = os.path.getmtime(thisZip)
			//~ mod_date = datetime.datetime.fromtimestamp(mod_date)
			//~ mod_date = mod_date.strftime('%d-%m-%y à %H:%M:%S')
//~ 
			//~ # DCP listStore item
			//~ icon = GdkPixbuf.Pixbuf.new_from_file_at_size(os.path.join(ROOT_DIR, 'icons','32','colis.png'), 24,24)
			//~ this_row = [False, icon, basename, None, size, tooltip, mod_date]
			//~ self.dcp_store.append(None, this_row)
//~ 
			//~ say("-stored zip file " + basename) 
		//~ 
		//~ ## Also store here .qdp exported projects
		//~ 
		//~ for thisQdp in glob.glob(var.OUTPUTDIR + '*.qdp'):
//~ 
			//~ ## Icon 
			//~ icon = GdkPixbuf.Pixbuf.new_from_file_at_size(os.path.join(ROOT_DIR, 'icons','24','quickdcp.png'), 24,24)
//~ 
			//~ ## Name
			//~ basename = os.path.basename(thisQdp)
//~ 
			//~ ## File size
			//~ size = get_human_size(os.path.getsize(thisQdp))
			//~ 
			//~ ## File contents
			//~ with zipfile.ZipFile(thisQdp,'r') as zqdp:
				//~ tooltip = _("Contents :")
				//~ for filename in zqdp.namelist():
					//~ tooltip += '\n  ' + filename
					//~ 
			//~ ## Modification date
			//~ mod_date = os.path.getmtime(thisQdp)
			//~ mod_date = datetime.datetime.fromtimestamp(mod_date)
			//~ mod_date = mod_date.strftime('%d-%m-%y à %H:%M:%S')
//~ 
//~ 
			//~ # DCP listStore item
			//~ this_row = [False, icon, basename, None, size, tooltip, mod_date]
			//~ self.dcp_store.append(None, this_row)
//~ 
			//~ say("-stored qdp file " + basename) 
//~ 
		//~ ## If store is empty, display a short message
		//~ if len(self.dcp_store) < 1:
			//~ #~ cleanWindow(self.dcpContainer)
			//~ noJobs = Gtk.Label("Aucun DCP dans le dossier de sortie")
			//~ noJobs.set_justify(Gtk.Justification.CENTER)
			//~ self.dcpContainer.add_with_viewport(noJobs)
//~ 
		//~ ## else update or recreate the treeview
		//~ else:
			//~ try:
				//~ self.dcpTreeView.set_model(self.dcp_store)
				//~ 
			//~ except AttributeError:		
				//~ # on crée le TreeView s'il n'existe pas déjà
				//~ #~ cleanWindow(self.dcpContainer)
				//~ self.dcpTreeView = Gtk.TreeView()
				//~ self.initDcpTreeView()
				//~ self.dcpTreeView.set_model(self.dcp_store)
				//~ self.dcpContainer.add_with_viewport(self.dcpTreeView)
				//~ 
			//~ # recocher les éléments précédemment selectionnés
			//~ if keep_checked:
				//~ new_checked = []
				//~ for cs in const.checked_dcps:
					//~ if os.path.exists(cs):
						//~ new_checked.append(cs)
				//~ const.checked_dcps = new_checked
			//~ else:
				//~ const.checked_dcps = []
			//~ 
		//~ self.dcpContainer.show_all()					
}

qdcp_bool
dcp_scan_dcps(QuickDcpAppWindow * self, qdcp_bool keep_checked)
{
		GFile			*	dcp_out;
		GFileEnumerator * 	dcp_list;
		GFileEnumerator * 	dcp_children;
		GCancellable	*	cancel;
		GError			*   error = NULL;

	/* Scan actual DCPs */
	
	dcp_out = g_file_new_for_path(self->qdcp->dcp_out);

	/* use g_file_enumerate_children_async() instead of the following */
	dcp_list = g_file_enumerate_children(dcp_out, 		/* GFile 		* file */
										"standard::*",	/* const char 	* attributes */
										G_FILE_QUERY_INFO_NONE, /* other possible flag : G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS */
										cancel,			/* GCancellable * cancellable if not NULL */
										&error			/* GError ** error */
										);
									
	if (error != NULL)
	{
		/* raise a notification and return */
		printf("QuickDcp : scan of %s for DCPs failed ! :: %s\n", self->qdcp->dcp_out, error->message);
		g_error_free(error);
		error = NULL;

		return QDCP_FAILURE;
	}
	if (dcp_list == NULL)
	{
		/* raise a notification and return */
		printf("QuickDcp : scanning %s for DCPs failed !\n", self->qdcp->dcp_out);

		return QDCP_FAILURE;
	}

	printf("Quickdcp : Scanning default directory for DCPs : %s\n", self->qdcp->dcp_out);

	/* store all that with a loop */

		GtkTreeIter 	iter1;
		GtkTreeIter 	iter2;
		int				counter = 0;
	
	while (TRUE)
	{
			GFileInfo 	* info;
			GFile		* file;
		g_file_enumerator_iterate(dcp_list,
										&info,
										&file,
										cancel,			/* GCancellable * cancellable if not NULL */
										&error			/* GError ** error */
										);

		/* exit the loop at the end of iteration */
		if (!info)
			break;
		
		/* list all file attributes in console */
			char ** attributes;
			char *	attr;
			char *  value;
			int		cc;
			
		attributes = g_file_info_list_attributes(info, NULL);

		//~ cc = 0;
		//~ while (attributes[cc] != NULL)
		//~ {
			//~ value = g_file_info_get_attribute_as_string(info, attributes[cc]);
			//~ printf("Quickdcp : attribute key : %s = %s\n", attributes[cc], value);
			//~ printf("Quickdcp : attribute type :%d\n", g_file_info_get_attribute_type(info, attributes[cc]));
			//~ cc++;
		//~ }
		
		counter++;
		printf("QuickDcp :  - seeing %s \n", 
			g_file_info_get_attribute_as_string(info, G_FILE_ATTRIBUTE_STANDARD_NAME));
		

		/* current file must be a folder or a zip file to be shown in the treeview */

		if (g_file_info_get_attribute_uint32(info, G_FILE_ATTRIBUTE_STANDARD_TYPE) == G_FILE_TYPE_DIRECTORY)
		{
				GFileEnumerator * 	dcp_children;
				GdkPixbuf		*	dir_icon;
				char				icon_path[PATH_STR_LEN];
				const char		*	dir_name;
				char				content[NAME_STR_LEN];
				char			*	mime;
				char				s_dir_size[NAME_STR_LEN];
				char				dir_date[NAME_STR_LEN];
				char				s_dir_time[NAME_STR_LEN];
				char			*	dir_time;
				GTimeVal		*	modif_time;
				char				tooltip[PATH_STR_LEN];
				GError			*	error = NULL;
				
				int					total_dir_size;
				
			total_dir_size = g_file_info_get_attribute_uint64(info, "standard::size");
				
			dir_name = g_file_info_get_display_name(info);
			get_human_size(s_dir_size, g_file_info_get_size(info));
			sprintf(content, "%s", g_file_info_get_content_type(info));
			mime = strstr(content, "/");
			mime++;

			/* get a generic icon */
			sprintf(icon_path, "%s%sscalable%sstatus%sdcp-bullet.svg", ICON_DIR, SEPARATOR, SEPARATOR, SEPARATOR);
			dir_icon = gdk_pixbuf_new_from_file_at_size(icon_path, 16, 16, &error);
			if (error)
			{
				g_critical ("Could not load pixbuf: %s\n", error->message);
				g_error_free(error);
			}
			//~ g_file_info_get_modification_time(info, modif_time);
			//~ strftime(s_dir_time, sizeof(s_dir_time), "%d %M %Y %H:%M:%S", modif_time->tv_sec);
			/* TODO : get a date string */
			//~ sprintf(s_dir_time, "%s", modif_time->tv_sec);
				
			sprintf(tooltip, "%s", g_file_info_get_content_type(info));
			
			/* set folder data in a tree store row */
			gtk_tree_store_append(self->dcpTreeStore, &iter1, NULL);
			gtk_tree_store_set(self->dcpTreeStore, &iter1,
								DCP_CHECK_COL, 	FALSE, 
								DCP_ICON_COL,  	dir_icon,
								DCP_NAME_COL,	dir_name,
								DCP_CONTENT_COL,mime,
								DCP_SIZE_COL,	s_dir_size,
								DCP_TOOLTIP_COL,tooltip,
								DCP_DATE_COL,	NULL,
								-1); /* end of the list */

			g_object_unref(dir_icon);
			
			/* use g_file_enumerate_children_async() instead of the following */
			dcp_children = g_file_enumerate_children(file, 		/* GFile 		* file */
												"standard::*",	/* const char 	* attributes */
												G_FILE_QUERY_INFO_NONE, /* other possible flag : G_FILE_QUERY_INFO_NOFOLLOW_SYMLINKS */
												cancel,			/* GCancellable * cancellable if not NULL */
												&error			/* GError ** error */
												);		

			/* loop children files */
			while (TRUE)
			{
					GFileInfo 	* child_info;
					GFile		* child_file;
				g_file_enumerator_iterate(dcp_children,
										&child_info,
										&child_file,
										cancel,			/* GCancellable * cancellable if not NULL */
										&error			/* GError ** error */
										);

				/* exit the loop at the end of iteration */
				if (!child_info)
					break;

				printf("QuickDcp :  -  -  child : %s \n", g_file_info_get_attribute_as_string(child_info, G_FILE_ATTRIBUTE_STANDARD_NAME));
				
				/* child's attributes */
				attributes = g_file_info_list_attributes(child_info, NULL);
				//~ cc = 0;
				//~ while (attributes[cc] != NULL)
				//~ {
					//~ value = g_file_info_get_attribute_as_string(child_info, attributes[cc]);
					//~ printf("Quickdcp : child  -- attribute key : %s = %s\n", attributes[cc], value);
					//~ cc++;
				//~ }
				
					const char		*	file_name;
					char				file_date[NAME_STR_LEN];
					//~ GTimeVal		*	modif_time;
					GdkPixbuf		*	file_icon;
					char			*	mime;
				
				file_icon = gtk_image_get_pixbuf(GTK_IMAGE (gtk_image_new_from_icon_name("directory",16)));
				
				file_name = g_file_info_get_display_name(child_info);
				get_human_size(s_dir_size, g_file_info_get_size(child_info));
				sprintf(content, "%s", g_file_info_get_content_type(child_info));
				mime = strstr(content, "/");
				mime++;
					
				total_dir_size += g_file_info_get_size(child_info);
				
				//~ g_file_info_get_modification_time(child_info, modif_time);
				//~ sprintf(file_date, "%s", get_last_mod_string(modif_time));

				gtk_tree_store_append(self->dcpTreeStore, &iter2, &iter1);
				gtk_tree_store_set(self->dcpTreeStore, &iter2,
									DCP_CHECK_COL, 	NULL, 
									DCP_ICON_COL,  	file_icon,
									DCP_NAME_COL,	file_name,
									DCP_CONTENT_COL,mime,
									DCP_SIZE_COL,	s_dir_size,
									DCP_TOOLTIP_COL,tooltip,
									DCP_DATE_COL,	NULL,
									-1); /* end of the list */

			}

			/* if we have a corresponding project
			 * retrieve an icon, dcp name and time length */
				t_project * pj;
			 if (find_project_by_name(pj, dir_name, self->qdcp))
			 {
					char 	laps[NAME_STR_LEN];
				sprintf(laps, "%f''", pj->laps);	
				gtk_tree_store_set(self->dcpTreeStore, &iter1, 
									DCP_CONTENT_COL, laps,
									DCP_TOOLTIP_COL, pj->dcp_name,
									-1);		 
			 //~ free(pj);
			 }
			/* else, test that all parts of a valid DCP are there.
			 * an actual DCP gets a generic DCP icon */
			else if (dcp_is_actual_dcp(dcp_children))
			{
				printf("Quickdcp : is an actual DCP\n");
				/* get icon generic DCP */
			}
			else
			{
				printf("Quickdcp : is not a valid DCP\n");
				/* get icon bad-dcp.png */
			}
			
					char s_total_size[NAME_STR_LEN];
			get_human_size(s_total_size, total_dir_size);
			gtk_tree_store_set(self->dcpTreeStore, &iter1, 
								DCP_SIZE_COL, s_total_size,
								-1);
			/* for 1st level directories we can replace 'content' (always directory)
			 * by 'length' if we have a corresponding project.
			 * */
		}
	}


	/* manage error from iteration */
	
	/* free enumerators */
	//~ g_object_unref(dcp_children);
	g_object_unref(dcp_list);
	//~ g_file_enumerator_close(unfinished, NULL, error);
	//~ g_file_enumerator_close(dcp_list, NULL, error);
	
	/* sort the tree by names and finished/unfinished */
	
	return QDCP_SUCCESS;
}

qdcp_bool
dcp_is_actual_dcp(GFileEnumerator * dcp_children)
{
	/* list all file attributes in console */
		char 			** 	attributes;
		char 			*	attr;
		char 			*  	value;
		GFileInfo 		* 	info;
		GCancellable	* 	cancel;
		GError			** 	error;

	while (TRUE)
	{
		g_file_enumerator_iterate(dcp_children,
										&info,
										NULL,
										cancel,			/* GCancellable * cancellable if not NULL */
										error			/* GError ** error */
										);
		/* exit the loop at the end of iteration */
		if (!info)
			break;

		attributes = g_file_info_list_attributes(info, NULL);
		while (attr != NULL)
		{
			value = g_file_info_get_attribute_as_string(info, attr);
			printf("Quickdcp : attribute key : %s = %s\n", attr, value);
		}
	}
		/* vérifier la présence de tous les fichiers necessaires */
		//~ is_actual = True

		/* at least 1 mxf */
		//~ if len(glob.glob(os.path.join(var.OUTPUTDIR, myd_bname, '*.mxf'))) < 1:
			//~ is_actual = False

		/* 1 assetmap */
		//~ if len(glob.glob(os.path.join(var.OUTPUTDIR, myd_bname, 'ASSETMAP*'))) != 1:
			//~ is_actual = False

		/* 1 volindex */
		//~ if len(glob.glob(os.path.join(var.OUTPUTDIR, myd_bname, 'VOLINDEX*'))) != 1:
			//~ is_actual = False

		/* at least 1 cpl */ 
		//~ if len(glob.glob(os.path.join(var.OUTPUTDIR, myd_bname, 'CPL_*.xml'))) < 1:
			//~ is_actual = False

		/* at least 1 pkl */ 
		//~ if len(glob.glob(os.path.join(var.OUTPUTDIR, myd_bname, 'PKL_*.xml'))) < 1:
			//~ is_actual = False

	return QDCP_SUCCESS;
}

qdcp_bool	
dcp_check_item(QuickDcpAppWindow * self, GtkCellRendererText * cell, char * path, GtkListStore * store)
{
		//~ store = self.dcpTreeView.get_model()
//~ 
		//~ if path is not None:
			//~ it = store.get_iter(path)
			//~ store[it][0] = not store[it][0]
			//~ if store[it][0] == True:
				//~ const.checked_dcps.append(store[it][2])
			//~ else:
				//~ popin = const.checked_dcps.index(store[it][2])
				//~ const.checked_dcps.pop(popin)
//~ 
		//~ if len(const.checked_dcps) > 0:
			//~ self.dcpZipBtn.set_sensitive(True)
			//~ self.dcpCopyBtn.set_sensitive(True)
			//~ self.dcpAtticBtn.set_sensitive(True)
			//~ self.dcpDeleteBtn.set_sensitive(True)
		//~ else:
			//~ self.dcpZipBtn.set_sensitive(False)
			//~ self.dcpCopyBtn.set_sensitive(False)
			//~ self.dcpAtticBtn.set_sensitive(False)
			//~ self.dcpDeleteBtn.set_sensitive(False)
			//~ 
}




/* ********************* D C P - C A L L B A C K S ****************** */



static void	
dcp_start_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller)
{
		//~ say("StartDCP")
		//~ # qdcp.wakeup_dcp() doesn't work here, don't know why
		//~ qdcp.notebook.set_current_page(qdcp.dcp_page)
		//~ qdcp.updateToolButtons(current=qdcp.dcp_toolbtn)
//~ 
		//~ self.current.dcp = self.current.sequence
						//~ 
		//~ ## Titre
		//~ self.dcp_title.set_text(self.current.dcp.make_dcp_name())
				//~ 
		//~ ## Nettoyage du screen de monitor
		//~ cleanWindow(self.stdoutFrame)
//~ 
		//~ ## Fenetre de terminal
		//~ self.dcpTerm = Monitor()
		//~ self.stdoutFrame.add(self.dcpTerm)
		//~ self.dcpTerm.config(self.current.dcp)
		//~ self.stdoutFrame.show_all()
		//~ 
		//~ ## Appels au shell-script:
				//~ 
		//~ # PNG -> TIFF
		//~ 
		//~ if self.current.dcp.mime == 'png':
			//~ # we start over afresh
			//~ cleanWindow(self.dcpIconBox)
//~ 
			//~ self.dcpIconTiff.show()
//~ 
			//~ ## project data
			//~ self.current.dcp.action='png2tif'
			//~ ## Script
			//~ self.dcpTerm.process(cmd='conv_png2tif', projet=self.current.dcp)
//~ 
		//~ # TIFF -> JPEG2000
		//~ 
		//~ if self.current.dcp.mime == 'tiff':
			//~ 
			//~ # j2k
			//~ self.dcpIconTiff.show()
			//~ self.dcpIconFleche1.show()
			//~ self.dcpIconJ2k.show()
//~ 
			//~ ## project data
			//~ self.current.dcp.action='tif2j2k'
			//~ ## Script
			//~ self.dcpTerm.process(cmd='conv_tif2j2k', projet=self.current.dcp)
		//~ 
		//~ # WAV -> MXF
		//~ 
		//~ if self.current.dcp.mime == 'wav':
//~ 
			//~ # audio-mxf
			//~ self.dcpIconFleche2.show()
			//~ self.dcpIconWav.show()
//~ 
			//~ ## project data
			//~ self.current.dcp.action='wav2mxf'
			//~ ## Script
			//~ self.dcpTerm.process(cmd='conv_wav2mxf', projet=self.current.dcp)
				//~ 
		//~ # JPEG2000 -> MXF
			//~ 
		//~ if self.current.dcp.mime == 'j2k':
//~ 
			//~ # mxf
			//~ self.dcpIconFleche3.show()
			//~ self.dcpIconMxf.show()
			//~ 
			//~ ## project data
			//~ self.current.dcp.action='j2k2mxf'
			//~ ## Script
			//~ self.dcpTerm.process(cmd='conv_j2k2mxf', projet=self.current.dcp)
			//~ 
		//~ # MXF -> DCP
		//~ 
		//~ if self.current.dcp.mime == 'mxf':
//~ 
			//~ # dcp
			//~ self.dcpIconFleche4.show()
			//~ self.dcpIconXml.show()
			//~ 
			//~ ## project data
			//~ self.current.dcp.action='mxf2dcp'
			//~ ## Script
			//~ self.dcpTerm.process(cmd='mk_DCP', projet=self.current.dcp)
//~ 
		//~ # Finalisation et conclusion
		//~ if self.current.dcp.mime == 'DCP':
//~ 
			//~ ## Afficher le treeview des DCP
			//~ self.storeDCPs()
//~ 
			//~ infoDialog(self.gui, msg="Job terminé")
			//~ 
			//~ ## Destruction ou archivage du tiff unique de carton
			//~ if os.path.exists(SEQ_TIF + self.b_name + '.tiff'):
				//~ ## Suppression
				//~ if var.config.get('qdcp','autoclean') in ['wash', 'vacuum']:
					//~ #confirmation de suppression
					//~ msg_suppr = "Supprimer le fichier ?"
					//~ if confirmDialog(self.gui, msg=msg_suppr):
						//~ try:
							//~ os.remove(SEQ_TIF + self.b_name + '.tiff')
							//~ CUR_STATE = "Le fichier " + self.b_name + ".tiff a été supprimé."
						//~ except:
							//~ CUR_STATE = "Echec de la suppression de " + SEQ_TIF + self.b_name + '.tiff' + " !"
							//~ errorDialog(self.gui, msg=CUR_STATE)
//~ 
						//~ self.gui.updateStatusBar(CUR_STATE, False)
//~ 
				//~ ## Archivage du fichier de carton
				//~ else:
					//~ if not os.path.isdir(var.WORKSPACE + 'archives/'):
						//~ os.makedirs(var.WORKSPACE + 'archives/')
						//~ 
					//~ shutil.move(SEQ_TIF + b_name + '.tiff', var.WORKSPACE + 'archives/' + b_name + '.tiff')
}
										
static void
dcp_copy_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller)
{
		//~ # select a target folder
		//~ dialog = Gtk.FileChooserDialog(
			//~ title=u'Copier les DCP vers...', 
			//~ parent=None, 
			//~ action=Gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER, 
			//~ buttons=(Gtk.STOCK_CANCEL, Gtk.RESPONSE_CANCEL,
             //~ "Select", Gtk.RESPONSE_OK), 
			//~ backend=None)
		//~ #~ self.XportFolder.set_action(Gtk.FILE_CHOOSER_ACTION_SELECT_FOLDER)
		//~ #~ self.XportFolder.set_action(Gtk.FILE_CHOOSER_ACTION_OPEN)
		//~ #~ self.XportFolder.set_width_chars(15)
		//~ #~ self.XportFolder.set_tooltip_text(self.dcpCopyBtn.get_file())
		//~ #~ self.XportFolder.set_current_folder(str(var.config.get('qdcp','outputdir')))
		//~ #~ self.XportFolder.set_filename(var.OUTPUTDIR)
		//~ 
		//~ response = dialog.run()
		//~ say("Dialog response = " + str(response))
			//~ 
		//~ # copy the selection into the target folder
		//~ if response == -5:
			//~ target_dir = dialog.get_filename()
			//~ say('Copying into ' + target_dir)
				//~ 
			//~ copy_error_dialog = False
			//~ for thisDCP in const.checked_dcps:
				//~ try:
					//~ shutil.copytree(os.path.join(var.OUTPUTDIR, thisDCP), 
									//~ os.path.join(target_dir, thisDCP))
					//~ say('Qdcp : Copied ' + thisDCP + ' in ' + target_dir)
				//~ except OSError as why:
					//~ say("Copy error " + thisDCP + " : " + str(why))
					//~ copy_error_dialog = True
					//~ 
			//~ if copy_error_dialog:
				//~ errorDialog(self.gui, msg="Echec de copie :\n" + str(why))				
		//~ # close		
		//~ dialog.destroy()
}
		
static void
dcp_zip_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller)
{
		//~ for thisDCP in const.checked_dcps:
//~ 
			//~ if sys.version_info <= (2,6):
				//~ import zipfile
				//~ zipf = zipfile.ZipFile(os.path.join(var.OUTPUTDIR, thisDCP + '.zip'), 'w')
				//~ for root, dirs, files in os.walk(os.path.join(var.OUTPUTDIR, thisDCP)):
					//~ for file in files:
						//~ zipf.write(os.path.join(root, file))
				//~ zipf.close()
				//~ say( thisDCP + " zipped with zipfile")
//~ 
			//~ else:
				//~ shutil.make_archive(thisDCP, 'zip', var.OUTPUTDIR + thisDCP)
				//~ shutil.move(thisDCP + '.zip', var.OUTPUTDIR)
				//~ say('Zipped ' + thisDCP + ' with shutil')
				//~ 
		//~ self.storeDCPs()
}

static void
dcp_attic_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller)
{
		//~ target_dir = os.path.join(var.OUTPUTDIR, var.ATTIC)
		//~ 
		//~ if not os.path.exists(target_dir):
			//~ os.makedirs(target_dir)
			//~ 
		//~ for thisDCP in const.checked_dcps:
			//~ shutil.move( var.OUTPUTDIR + thisDCP, target_dir + thisDCP)
			//~ say('Copied ' + thisDCP + ' in ' + target_dir)
		//~ self.storeDCPs()
		//~ 
	//~ def trashThemAll(self, button=None):
		//~ """Delete all selected DCPs after some confirmation"""
		//~ shallwe = confirmDialog(self.gui, msg="Supprimer tous les DCP séléctionnés ?")
		//~ if shallwe.run() == Gtk.ResponseType.ACCEPT:
			//~ for thisDCP in const.checked_dcps:
				//~ if os.path.isdir(thisDCP):
					//~ shutil.rmtree(var.OUTPUTDIR + thisDCP)
				//~ else:
					//~ os.remove(var.OUTPUTDIR + thisDCP)
				//~ say('deleted ' + thisDCP)
		//~ self.storeDCPs()
}				

static void
dcp_delete_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller)
{
		//~ shallwe = confirmDialog(self.gui, msg="Supprimer tous les DCP séléctionnés ?")
		//~ if shallwe.run() == Gtk.ResponseType.ACCEPT:
			//~ for thisDCP in const.checked_dcps:
				//~ if os.path.isdir(thisDCP):
					//~ shutil.rmtree(var.OUTPUTDIR + thisDCP)
				//~ else:
					//~ os.remove(var.OUTPUTDIR + thisDCP)
				//~ say('deleted ' + thisDCP)
		//~ self.storeDCPs()
}

/* dcp treeview callbacks */
static void	
dcp_tree_selection_cb(GtkTreeSelection * select, 
					  gpointer * user_data)
{
		GtkTreeIter 	iter;
        GtkTreeModel *	model;
        gchar 		 * 	basename;

	if (gtk_tree_selection_get_selected (select, &model, &iter))
	{
		gtk_tree_model_get (model, &iter, DCP_NAME_COL, &basename, -1);

			g_print ("Quickdcp : dcpTreeView : selected %s\n", basename);

			g_free (basename);
	}
}

static void
dcp_tree_toggled_cb(GtkCellRendererToggle *cell_renderer,
					   gchar                 *path,
					   gpointer               user_data)
{
		QuickDcpAppWindow * self = user_data;
		
	printf("Quickdcp : Toggled a row in dcp tree view\n");
}

static void
dcp_tree_row_double_clicked_cb(GtkTreeView 	  * treeview,
							GtkTreePath		  *	path,
							GtkTreeViewColumn *	col,
							gpointer 			user_data)
{
		QuickDcpAppWindow * self = user_data;
		
	printf("Quickdcp : Double-clicked row in dcp tree view\n");
}



/*------------- P R E F E R E N C E S - F U N C T I O N S ------------*/



qdcp_bool
opt_init_widgets(QuickDcpAppWindow * self)
{
		int 				format;
		int 				resolution;
		int					category;
		int					framerate;
		char				str_frate[TINY_STR_LEN];
		GdkRGBA				rgba = {0};
		const gchar		*	hex_str;

	printf("Quickdcp : init options widgets\n");

	format = index_of_cbb(self->qdcp->formats, FMT_N_FORMATS, self->qdcp->cfg->format);
	resolution = index_of_cbb(self->qdcp->resolutions, RES_N_RESOLUTIONS, self->qdcp->cfg->resolution);
	category = self->qdcp->cfg->category;	
	sprintf(str_frate, "%d", self->qdcp->cfg->framerate);
	framerate = index_of_cbb(self->qdcp->framerates, FRATE_N_FRAMERATES, str_frate);

	gtk_combo_box_set_active( GTK_COMBO_BOX (self->optFormatCBB), format);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->optResolutionCBB), resolution);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->optCategoryCBB), self->qdcp->cfg->category);
	gtk_combo_box_set_active( GTK_COMBO_BOX (self->optFramerateCBB), framerate);

	gtk_spin_button_set_value(	self->optTimelengthSpinBtn, self->qdcp->cfg->laps);
		//~ optMarginSpinBtn
		//~ optBackgroundColorBtn
		//~ optForegroundColorBtn
		//~ optIssuerEntry
		//~ optIssuerShortEntry
		
	/* TEXT options widgets */
		//~ optTextFontBtn
		//~ optTextInterlineSpinBtn
		//~ optTextAlignCBB
		//~ optTextWeightCBB
		//~ optTextStretchCBB
		//~ optTextStyleCBB
		//~ optTextVariantCBB)
		//~ optTextUnderlineCBB
		//~ optTextStrikeSwitch
		//~ optTextScriptCBB
		//~ optTextLangCBB
		
	/* SHAPES options widgets */
		//~ optLineColorBtn
		//~ optFillColorBtn
		//~ optLineWidthSpinBtn
		//~ optLineStyleCBB
		//~ optLineCapsCBB
		//~ optLineJoinCBB
		
	/* TRANSITIONS options widgets */
		//~ optFonduDebCBB
		//~ optFonduFinCBB
		//~ optFonduLapsDebSpinBtn
		//~ optFonduLapsFinSpinBtn
		//~ optFonduCacheDebColorBtn
		//~ optFonduCacheFinColorBtn
		
	/* AUDIO-VIDEO options widgets */
		
	/* DCP NAME options widgets */
		//~ optDcpExampleLabel
		//~ optDcpCategoryCheckBtn
		//~ optDcpFormatCheckBtn
		//~ optDcpLangCheckBtn
		//~ optDcpLangCBB
		//~ optDcpSubtitlesCheckBtn
		//~ optDcpSubtitlesCBB
		//~ optDcpCountryCheckBtn
		//~ optDcpCountryCBB
		//~ optDcpResolutionCheckBtn
		//~ optDcpDateCheckBtn
		//~ optDcpIssuerCheckBtn
		
	/* QUICKDCP options widgets */
		//~ optQdcpInputdirFileBtn
		//~ optQdcpOutputdirFileBtn
		//~ optQdcpBeepSwitch
		//~ optQdcpThreadsSpinBtn
		//~ optQdcpDebugModeSwitch
		//~ optQdcpOdcpLoglevelCBB
		//~ optQdcpFFmpegLoglevelCBB
		//~ optQdcpJ2kMethodCBB
		//~ optQdcpMxfMethodCBB
		
	/* KEYBOARD SHORTCUTS options widgets */
		//~ optAccelTreeView
		
	/* CONFIG FILE EDITOR widgets */
	self->optConfigBuffer = gtk_text_view_get_buffer(self->optConfigViewer);
	
		g_autoptr(GFile) 			config_file = g_file_new_for_path(self->qdcp->cfg_file);
		g_autoptr(GFileInputStream) config_input = g_file_read(config_file, NULL, NULL);
		
		if (!config_input)
			return QDCP_FAILURE;
		
		gssize	read;
		char	config_text[FILE_STR_LEN];	
		gint	text_len = 0;
		
	while(TRUE)
	{
		read = g_input_stream_read(G_INPUT_STREAM(config_input), config_text, G_N_ELEMENTS(config_text) - 1, NULL, NULL);
		text_len = G_N_ELEMENTS(config_text) - 1;
		if (read < 0)
		{
			config_text[read] = '\0';
			g_print("%s", config_text);
		}
		else if (read < 0)
			return QDCP_FAILURE;
		else
			break;
	}
	//~ g_file_read_async();
	gtk_text_buffer_set_text(self->optConfigBuffer, config_text, text_len);
	g_object_unref(config_file);
	
		//~ optSaveConfigBtn
		//~ optCancelConfigBtn


	return QDCP_SUCCESS;
}



/* *********** P R E F E R E N C E S - C A L L B A C K S ************ */


static void
opt_save_config_btn_cb(QuickDcpAppWindow * self)
{
}

static void
opt_cancel_config_btn_cb(QuickDcpAppWindow * self)
{
}

static void
opt_close_btn_cb(QuickDcpAppWindow * self)
{
}

static void
opt_time_spin_changed(GtkSpinButton * button,
						gpointer	   user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting time laps to %f\n", gtk_spin_button_get_value(button));
		self->qdcp->active_project->laps = (float) gtk_spin_button_get_value(button);
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}

static void
opt_format_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting format to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->format = gtk_combo_box_text_get_active_text(combobox);
		
		set_unsaved(self);
	}
	
	gtk_widget_queue_draw(GTK_WIDGET (self->compDrawingArea));

	return;
}
static void
opt_resolution_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting resolution to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->resolution = gtk_combo_box_text_get_active_text(combobox);
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}
static void
opt_category_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting category to %s\n", gtk_combo_box_text_get_active_text(combobox));
		printf("Quickdcp : Setting category to %d\n", (int) gtk_combo_box_get_active(GTK_COMBO_BOX (combobox)));
		self->qdcp->active_project->category = (int) gtk_combo_box_get_active(GTK_COMBO_BOX (combobox));
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}
static void
opt_framerate_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data)
{
		QuickDcpAppWindow 	* self = user_data;

	if (self->qdcp->active_project != NULL)
	{
		printf("Quickdcp : Setting framerate to %s\n", gtk_combo_box_text_get_active_text(combobox));
		self->qdcp->active_project->framerate = atoi(gtk_combo_box_text_get_active_text(combobox));
		
		set_unsaved(self);
		return;
	}
	//~ printf("No active project, did nothing\n");
	return;
}

static void
opt_background_color_chooser_activated(GtkColorChooser * button,
										gpointer		  user_data)
{
		QuickDcpAppWindow 	* 	self = user_data;
		GdkRGBA				* 	rgba_color = malloc(sizeof(GdkRGBA));
		char					hex_color[COLOR_STR_LEN];
		
	printf("Quickdcp : Setting a new background color\n");
	
	//~ if (gtk_color_chooser_get_use_alpha(self->compBackgroundColorBtn))
		//~ printf("use alpha\n");
	//~ else
		//~ printf("no alpha\n");
		
	//~ gtk_color_chooser_set_use_alpha(self->compBackgroundColorBtn, TRUE);

	gtk_color_chooser_get_rgba(button, rgba_color);
	
	self->canvas->bg_color = gdk_rgba_copy(rgba_color);
	printf("Quickdcp : - background color was changed to %s\n", gdk_rgba_to_string(rgba_color));
			
	gtk_widget_queue_draw(GTK_WIDGET (self->compDrawingArea));

	if (self->qdcp->active_project != NULL)
	{
		gui_color_rgba2hex(hex_color, rgba_color);
			
		printf("Quickdcp : background color was changed to %s\n", hex_color);

		/* pass the returned color to the current project */
		sprintf(self->qdcp->active_project->background, "%s", hex_color);
		
		set_unsaved(self);
	}

	return;
}



/* ***************** M I S C E L L A N E O U S ********************** */


int
index_of_cbb(const char ** haystack, int len, const char * needle)
{
		int 			index = 0;
		
	for (index = 0; index < len; ++index)
	{
		//~ printf("haystack[index] : %s at index %d\n", haystack[index], index);
		
		if (strcmp(haystack[index], needle) == 0)
		{
			//~ printf("found needle %s at index %d\n", haystack[index], index);
			return index;
		}
	}
	
	return 0;
}


void
set_unsaved(QuickDcpAppWindow * self)
{
	if (self->qdcp->active_project != NULL)
	{
		if (self->qdcp->active_project->unsaved != QDCP_TRUE)
		{
			printf("Quickdcp : set the active project to unsaved\n");
			
			/* show the unsaved state somewhere : a * in the title, tab label, or else? */
			/* for example print the dcpname label in red */

			
			//~ gtk_style_context_save(style_ctx);
			//~ state_flags = gtk_widget_get_state_flags(GTK_WIDGET (self->compDcpName));
			//~ printf("DCP label state flags = %d", state_flags);
			
			gtk_style_context_add_provider(gtk_widget_get_style_context(
													GTK_WIDGET (self->compDcpName)),
										   GTK_STYLE_PROVIDER(self->unsaved_provider),
										   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
									   
			gtk_css_provider_load_from_data(self->unsaved_provider,
											"#compDcpName,GtkLabel {color: red;}",
											-1,
											NULL);
		}
		
		self->qdcp->active_project->unsaved = QDCP_TRUE;
		
	}
}

qdcp_bool
gui_color_rgba2hex(char * hex_color, GdkRGBA * rgba_color) 
{	
	//~ printf("rgba : %f %f %f %f\n", 	rgba_color->red, 
									//~ rgba_color->green, 
									//~ rgba_color->blue, 
									//~ rgba_color->alpha);
									
	int hex_red		= (int) ceil(rgba_color->red 	* 255);
	int hex_green	= (int) ceil(rgba_color->green 	* 255);
	int hex_blue	= (int) ceil(rgba_color->blue 	* 255);
	int hex_alpha	= (int) ceil(rgba_color->alpha 	* 255);
	
	sprintf(hex_color, "#%.2X%.2X%.2X%.2X", hex_red, hex_green, hex_blue, hex_alpha);
		
	//~ printf("hex color : %s\n", hex_color);
	
	return QDCP_SUCCESS;
}


/* Generate a regular DCP name based on config options */
char *
get_dcp_name_from_widgets(QuickDcpAppWindow * self) 
{
		char	category_str[SHORT_STR_LEN] = {0};		
		char 	format_str[TINY_STR_LEN] 	= {0};
		char 	lang_sub_str[SHORT_STR_LEN]	= {0};
		char 	country_str[TINY_STR_LEN] 	= {0};
		char 	audio_str[TINY_STR_LEN] 	= {0};
		char 	channels[TINY_STR_LEN] 		= {0};
		char 	resol_str[TINY_STR_LEN] 	= {0};
		char  	issuer_str[SHORT_STR_LEN] 	= {0};
		char 	date_str[WORD_STR_LEN] 		= {0};
		char 	lab_str[SHORT_STR_LEN] 		= {0};
		char 	type_str[SHORT_STR_LEN] 	= {0};
		char 	version_str[TINY_STR_LEN] 	= {0};
		
		char	dcp_name[NAME_STR_LEN]		= {0};
	
	/* category */
	if (self->qdcp->cfg->name_category)
	{
		sprintf(category_str, "_%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compCategoryCBB)));
	}
	else{ category_str[0] = 0; }
	
	/* -3D (added to category) */
	
	/* format */
	if (self->qdcp->cfg->name_format)
	{
		char c = (strcmp(gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compFormatCBB)), "Full") == 0) 
				? 'C' 
				: gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compFormatCBB))[0];
		sprintf(format_str, "_%c%c", c, '\0');
	}
	else{ format_str[0] = 0; }
	
	/* langs, subtitles */
	if (self->qdcp->cfg->name_langs)
	{
		sprintf(lang_sub_str, "_%s-%s", self->qdcp->cfg->lang_code, self->qdcp->cfg->subtitle_code); 
		/* OCAP or HOH should be added to subtitle string */
	}
	else{ lang_sub_str[0] = 0; }
	
	/* country of release */
	if (self->qdcp->cfg->name_country)
	{
		sprintf(country_str, "_%s", self->qdcp->cfg->country_code);
	}
	else{ country_str[0] = 0; }

	/* audio (+ VI, etc) */
	//~ if (self->qdcp->cfg->name_audio && pj->tracks->size > 0)
	//~ {
		//~ if (pj->audio_format == CH_5_1)
			//~ sprintf(channels, "51");
		//~ if (pj->audio_format == CH_7_1)
			//~ sprintf(channels, "71");
		//~ 
		//~ sprintf(audio_str, "_%s", channels);
	//~ }
	//~ else{ sprintf(audio_str, " "); }
	
	/* resolution */
	if (self->qdcp->cfg->name_resol)
	{
		sprintf(resol_str, "_%s", gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (self->compResolutionCBB)));
	}
	else{ resol_str[0] = 0; }

	/* issuer */
	if (self->qdcp->cfg->name_issuer)
	{
		sprintf(issuer_str, "_%s", self->qdcp->cfg->issuer_short);
	}
	else{ issuer_str[0] = 0; } 

	/* date */
	if (self->qdcp->cfg->name_date)
	{
				//~ char	*	date;
				time_t 		t;
		struct	tm		*	tmp;
				int		seconds;
	
		t = time(NULL);
		tmp = localtime(&t);
		if (tmp == NULL)
			perror("localtime");
		
		strftime(date_str, 10, "_%Y%m%d", tmp);
		
		//~ sprintf(date_str, "_%s", date);
	}
	else { date_str[0] = 0;}

	/* laboratory */
	if (self->qdcp->cfg->name_lab)
	{
		sprintf(lab_str, "_%s", self->qdcp->cfg->laboratory);
	}
	else { lab_str[0] = 0;}

	/* type (SMPTE / IOP */
	if (self->qdcp->cfg->name_dcptype)
	{
		sprintf(type_str, "_%s", "SMPTE");
	}
	else { type_str[0] = 0;}
	
	/* version (OV / VF */
	if (self->qdcp->cfg->name_version)
	{
		sprintf(version_str, "_%s", self->qdcp->cfg->version);
	}
	else { version_str[0] = 0;}
	

	/* example : TITLE_TST_F_FR_2K_QDCP_20161205_ZNBX_OV */
	
	//~ printf("langs.......%s.........\n", lang_sub_str);
	//~ printf("count.......%s.........\n", country_str);
	//~ printf("audio.......%s.........\n", audio_str);
	//~ printf("resol.......%s.........\n", resol_str);
	//~ printf("issu .......%s.........\n", issuer_str);
	//~ printf("date .......%s.........\n", date_str);
	//~ printf("labo .......%s.........\n", lab_str);
	//~ printf("vers .......%s.........\n", version_str);

	/* aggregated name */
	sprintf(dcp_name, "%s%s%s%s%s%s%s%s%s%s%s%s", 
						gtk_entry_get_text(self->compBasenameEntry), 
						category_str,
						format_str, 
						lang_sub_str,
						country_str,
						audio_str,		/* 6 */
						resol_str,
						issuer_str,
						date_str,
						lab_str,
						type_str,
						version_str		/* 11 */
					);
	
	return strdup(dcp_name);
}


/* ******************** M A I N   &   E O F ************************* */

/**
 * Main obviously
 */
int main(int argc, char *argv[]) 
{		
	return g_application_run (G_APPLICATION (quickdcp_app_new ()), argc, argv);	
}


/* ****************************************************************** */
