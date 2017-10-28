/**
   @file    qdcp_gui.h
   @author  jean.sabatier@zanobox.net
   @brief   quickdcp builds Digital Cinema Packages from a GTK+ interface.
*/

#ifndef QDCP_GUI_H_INCLUDED
#define QDCP_GUI_H_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <inttypes.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include "quickdcp.h"
#include "qdcp_config.h"
#include "qdcp_projects.h"
#include "qdcp_photograms.h"
#include "qdcp_mxf.h"


/* about time */
#ifdef WIN32
	#include <windows.h>
#elif _POSIX_C_SOURCE >= 199309L
	#include <time.h> // for nanosleep
	//~ #else
	//~ #include <unistd.h> // for usleep
#endif

/* a few gui macros */
#define DEFAULT_BASENAME_TEXT "Saisir un nom"
#define DEFAULT_DCPNAME_TEXT  "Pas de projet actif"


#define QUICKDCP_APP_TYPE (quickdcp_app_get_type ())
#define QUICKDCP_APP(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), QUICKDCP_APP_TYPE, QuickDcpApp))

#define QUICKDCP_APP_WINDOW_TYPE (quickdcp_app_window_get_type ())
#define QUICKDCP_APP_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), QUICKDCP_APP_WINDOW_TYPE, QuickDcpAppWindow))
//~ //G_DECLARE_FINAL_TYPE (QuickDcpAppWindow, quickdcp_app_window, QUICKDCP, APP_WINDOW, GtkApplicationWindow)

//~ #define QUICKDCP_DRAWING_AREA_TYPE (quickdcp_drawing_area_get_type ())
//~ #define QUICKDCP_DRAWING_AREA(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), QUICKDCP_DRAWING_AREA_TYPE, QuickDcpDrawingArea))

typedef struct _QuickDcpApp       		QuickDcpApp;
typedef struct _QuickDcpAppClass  		QuickDcpAppClass;

typedef struct _QuickDcpAppWindow        QuickDcpAppWindow;
typedef struct _QuickDcpAppWindowClass   QuickDcpAppWindowClass;

//~ typedef struct _QuickDcpDrawingArea        QuickDcpDrawingArea;
//~ typedef struct _QuickDcpDrawingAreaClass   QuickDcpDrawingAreaClass;

enum notebook_pages
{
	PAGE_INTRO,
	PAGE_COMP,
	PAGE_SEQ,
	PAGE_AUD,
	PAGE_DCP,
	PAGE_PREFS
};

enum comp_piles_store_model {
	PILES_CHECK_COL,
	PILES_ICON_COL,
	PILES_NAME_COL,
	PILES_PATH_COL,
	PILES_ID_COL,
	PILES_TOOLTIP_COL,
	PILES_N_COLUMNS
};

enum comp_layers_store_model {
	LAYERS_CHECK_COL,
	LAYERS_ICON_COL,
	LAYERS_NAME_COL,
	LAYERS_ID_COL,
	LAYERS_POS_COL,
	LAYERS_FILE_COL,
	LAYERS_N_COLUMNS
};

enum dcp_store_model {
	DCP_CHECK_COL, 
	DCP_ICON_COL,
	DCP_NAME_COL,
	DCP_CONTENT_COL,
	DCP_SIZE_COL,
	DCP_TOOLTIP_COL,
	DCP_DATE_COL,	
	DCP_N_COLUMNS
};


typedef struct {
	cairo_t			*	cairo;
	cairo_surface_t	*	surf;
	cairo_format_t  *	px_fmt;
	cairo_pattern_t	*	empty_ptn;
	float				win_w;
	float				win_h;
	int					screen_w;
	int					screen_h;
	float				w;
	float				h;
	int					x;
	int					y;
	int					scroll_x;
	int					scroll_y;
	GdkRGBA			*	bg_color;
	float				zoom_factor;
	qdcp_bool			selecting;
	qdcp_bool			dragging;
	char 			*	tool_mode;
	int					tool_ox;
	int					tool_oy;
	int					tool_xx;
	int					tool_yy;
	int					layer_group;
	int					selected_box;
} canvas_ctx;


/* ******************* A P P L I C A T I O N ************************ */




/** Application */
struct _QuickDcpApp
{
  GtkApplication parent;
};

/** Application Class */
struct _QuickDcpAppClass
{
  GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(QuickDcpApp, quickdcp_app, GTK_TYPE_APPLICATION);





/* App */

GType           		
quickdcp_app_get_type    (void);

static void
quickdcp_app_init (QuickDcpApp *app);

static void
quickdcp_app_class_init (QuickDcpAppClass *class);

/**
 * quickdcp_app_new() is directly called from main 
 * through g_application_run() 
 * see https://developer.gnome.org/gio/stable/GApplication.html
 * */
QuickDcpApp *	
quickdcp_app_new         (void);


/** On startup we initialize keyboard accesses. But we can also initialize 
 * all we need for our first application instance before application activation. 
 * Such as application level menu, actions, settings, etc.
 * */
static void
quickdcp_app_startup (GApplication *app);


/** This is called when the application launches in the desktop environment (or equivalent)
 * Typically, it opens an application window.
 * */
static void
quickdcp_app_activate (GApplication *app);


/** This handles files to be opened and processed by the application.
 * Typically here, we can process .qdp and .qdx files, 
 * but also images, audio, video, subtitle and mxf files
 * to be treated as new projects or subprojects.
 * 
 * We can also process other command-line arguments here.
 * (see https://wiki.gnome.org/HowDoI/GtkApplication
 * section 'Dealing with the command line')
 */
static void
quickdcp_app_open (GApplication  *app,
                  GFile        **files,
                  gint           n_files,
                  const gchar   *hint);


/** This should be called just before shutdown
 * in order to close and unref everything possible
 * (MagickWand, FFmpeg, threads?, etc.)
 * --- not implemented yet !! ---
 * */
static void
quickdcp_app_shutdown (GApplication *app);





/* ******************* A P P - W I N D O W ************************** */




/** Window */
struct _QuickDcpAppWindow
{
	GtkApplicationWindow 	parent;
	/* internal instance */
	quickdcp_ctx		*	qdcp;
	int						last_seen_tab;
	GtkIconTheme		*	icons;
	GtkCssProvider		*	unsaved_provider;
	/* main widgets */
	GtkToolbar			*	toolbar;
	GtkNotebook			*	notebook;
	GtkStatusbar		*	statusbar;
	/* compositor widgets */
	/* comp :: general settings */
	GtkEntry			*	compBasenameEntry;
	GtkButton			*	compBasenameOkBtn;
	GtkSpinButton		*	compTimeLengthSpinBtn;
	GtkComboBox			*	compFormatCBB;
	GtkComboBox			*	compResolutionCBB;
	GtkComboBox			*	compCategoryCBB;
	GtkComboBox			*	compFramerateCBB;
	GtkColorChooser		*	compBackgroundColorBtn;
	/* comp :: piles */	
	GtkNotebook			*	compLeftBox;
	GtkTreeView			*	compPilesTreeView;
	GtkTreeStore		*	compPilesTreeStore;
	GtkCellRendererToggle * compPileCellCheck;
	GtkCellRendererPixbuf * compPileCellPixbuf;
	GtkButton			*	compNewPileBtn;
	GtkButton			*	compDeletePilesBtn;
	GtkButton			*	compRefreshPilesBtn;
	/* comp :: layers */
	GtkButton			*	compNewImageLayerBtn;
	GtkSpinButton		*	compOpacitySpinBtn;
	GtkTreeView			*	compLayersTreeView;
	GtkTreeStore		*	compLayersTreeStore;
	GtkCellRendererToggle * compLayerCellCheck;
	GtkCellRendererPixbuf * compLayerCellPixbuf;
	GtkButton			*	compDuplicateLayerBtn;
	GtkButton			*	compDeleteLayerBtn;
	/* comp :: screen */	
	GtkButton			*	compGenerateSeqBtn;
	GtkButton			*	compGenerateDcpBtn;
	GtkScrolledWindow	*	compDrawingSW;
	GtkDrawingArea		*	compDrawingArea;
	canvas_ctx			*   canvas;
	//~ GtkLabel			*	compFrameLabel;
	GtkLabel			*	compCurrentFrame;
	GtkLabel			*	compDcpName;
	//~ GtkLabel			*	compFrameLabel;
	GtkLabel			*	compCurrentLayer;
	GtkScale			*	compTimeline;
	GtkAdjustment		*	compTimelineAdj;
	GtkCheckButton		*	isKeyframeCheckBtn;
	/* comp :: tools */	
	GtkBox				*	compRightBox;
	GtkNotebook			*	compRightColNotebook;
	GtkFlowBox			*	toolbox;
	//~ GtkToolItemGroup	*	toolitemgroup;
	/* comp :: text/pango */	
	GtkButton			*	compNewTextLayerBtn;
	GtkTextView			*	compPangoBox;
	GtkColorChooser		*	compTextColorBtn;
	GtkSpinButton		*	compInterlineSpinBtn;
	GtkComboBoxText		*	compTextAlignCBB;
	GtkComboBoxText		*	compTextStretchCBB;
	GtkComboBoxText		*	compTextWeightCBB;
	GtkComboBoxText		*	compTextVariantCBB;
	GtkComboBoxText		*	compTextStyleCBB;
	GtkFontButton		*	compFontSelector;
	GtkComboBoxText		*	compTextScriptCBB;
	GtkSwitch			*	compTextStrikeSwitch;
	GtkComboBoxText		*	compTextUnderlineCBB;
	GtkComboBoxText		*	compTextLangCBB;
	/* comp :: rotation */	
	GtkSpinButton		*	compRotateDegreeSpinBtn;
	GtkSpinButton		*	compCenterXSpinBtn;
	GtkSpinButton		*	compCenterYSpinBtn;
	/* comp :: shapes */
	GtkColorChooser		*	compShapesLineColorBtn;
	GtkColorChooser		*	compShapesFillColorBtn;
	GtkSpinButton		*	compShapesLineWidthSpinBtn;
	/* comp :: transitions */	
	GtkSpinButton		*	compFonduLapsDebSpinBtn;
	GtkColorChooser		*	compFonduCacheDebColorBtn;
	GtkSpinButton		*	compFonduLapsFinSpinBtn;
	GtkColorChooser		*	compFonduCacheFinColorBtn;
	GtkSpinButton		*	compEntryPointSpinBtn;
	GtkSpinButton		*	compFadeInSpinBtn;
	GtkSpinButton		*	compExitPointSpinBtn;
	GtkSpinButton		*	compFadeOutSpinBtn;
	/* comp :: geometry */
	GtkBox				*	compGeometryToolBar;
	/* comp :: coords */
	GtkLabel			*	compXLabel;
	GtkLabel			*	compYLabel;
	GtkSpinButton		*	compOxSpinBtn;
	GtkLabel			*	compOxPercentLabel;
	GtkSpinButton		*	compOySpinBtn;
	GtkLabel			*	compOyPercentLabel;
	/* comp :: dims */
	GtkSpinButton		*	compWidthSpinBtn;
	GtkLabel			*	compWidthPercentLabel;
	GtkSpinButton		*	compHeightSpinBtn;
	GtkLabel			*	compHeightPercentLabel;
	GtkCheckButton		*	compRatioCheckBtn;
	/* comp :: tool buttons */
	GtkToolButton		*	toolNone;
	GtkToolButton		*	toolSelection;
	GtkToolButton		*	toolDraw;
	GtkToolButton		*	toolMove;
	GtkToolButton		*	toolText;
	GtkToolButton		*	toolRotation;
	GtkToolButton		*	toolTransition;
	GtkToolButton		*	toolRectangle;
	GtkToolButton		*	toolRectangleFilled;
	GtkToolButton		*	toolCircle;
	GtkToolButton		*	toolCircleFilled;
	GtkToolButton		*	toolOval;
	GtkToolButton		*	toolOvalFilled;
	GtkToolButton		*	toolZoomIn;
	GtkToolButton		*	toolZoomOut;
	GtkToolButton		*	toolZoomFit;
	
	
	/* seq :: all */
	GtkTreeView			*	seqSequencesTreeView;
	GtkTreeView			*	seqWavTreeView;
	
	
	/* aud :: all */
	GtkButton			*	audUnprocessedAddSource;
	GtkButton			*	audUnprocessedDeleteBtn;
	GtkButton			*	audUnprocessedRefreshBtn;
	GtkButton			*	audUnprocessedListenBtn;
	GtkButton			*	audUnprocessedConvertBtn;
	GtkComboBoxText		*	audUnprocessedChannelsCBB;
	GtkComboBoxText		*	audUnprocessedMixTypeCBB;
	GtkTreeView			*	audProcessedTreeView;
	GtkTreeView			*	audUnprocessedTreeView;
	
	
	/* dcp :: buttons */
	GtkButton			*	dcpCopyBtn;
	GtkButton			*	dcpAtticBtn;
	GtkButton			*	dcpZipBtn;
	GtkButton			*	dcpDeleteBtn;
	/* dcp :: treeview */
	GtkTreeView			*	dcpTreeView;
	GtkTreeStore		*	dcpTreeStore;
	/* dcp :: images */
	GtkImage			*	dcpIconTiff;
	GtkImage			*	dcpIconJ2k;
	GtkImage			*	dcpIconWav;
	GtkImage			*	dcpIconSubs;
	GtkImage			*	dcpIconMxf;
	GtkImage			*	dcpIconDcp;

	
	/* opt :: all */
	GtkButton			*	optCloseBtn;

	GtkComboBoxText		*	optFormatCBB;
	GtkComboBoxText		*	optResolutionCBB;
	GtkComboBoxText		*	optCategoryCBB;
	GtkComboBoxText		*	optFramerateCBB;
	GtkSpinButton		*	optTimelengthSpinBtn;
	GtkSpinButton		*	optMarginSpinBtn;
	GtkColorChooser		*	optBackgroundColorBtn;
	GtkColorChooser		*	optForegroundColorBtn;
	GtkEntry		*	optIssuerEntry;
	GtkEntry		*	optIssuerShortEntry;
							
	/* opt :: TEXT options */
	GtkFontChooser		*	optTextFontBtn;
	GtkSpinButton		*	optTextInterlineSpinBtn;
	GtkComboBoxText		*	optTextAlignCBB;
	GtkComboBoxText		*	optTextWeightCBB;
	GtkComboBoxText		*	optTextStretchCBB;
	GtkComboBoxText		*	optTextStyleCBB;
	GtkComboBoxText		*	optTextVariantCBB;
	GtkComboBoxText		*	optTextUnderlineCBB;
	GtkSwitch			*	optTextStrikeSwitch;
	GtkComboBoxText		*	optTextScriptCBB;
	GtkComboBoxText		*	optTextLangCBB;
							
	/* opt :: SHAPES options widgets */
	GtkColorChooser		*	optLineColorBtn;
	GtkColorChooser		*	optFillColorBtn;
	GtkSpinButton		*	optLineWidthSpinBtn;
	GtkComboBoxText		*	optLineStyleCBB;
	GtkComboBoxText		*	optLineCapsCBB;
	GtkComboBoxText		*	optLineJoinCBB;
							
	/* opt :: TRANSITIONS options widgets */
	GtkComboBoxText		*	optFonduDebCBB;
	GtkComboBoxText		*	optFonduFinCBB;
	GtkSpinButton		*	optFonduLapsDebSpinBtn;
	GtkSpinButton		*	optFonduLapsFinSpinBtn;
	GtkColorChooser		*	optFonduCacheDebColorBtn;
	GtkColorChooser		*	optFonduCacheFinColorBtn;
							
	/* opt :: AUDIO-VIDEO options widgets */
							
	/* opt :: DCP NAME options widgets */
	GtkLabel			*	optDcpExampleLabel;
	GtkCheckButton		*	optDcpCategoryCheckBtn;
	GtkCheckButton		*	optDcpFormatCheckBtn;
	GtkCheckButton		*	optDcpLangCheckBtn;
	GtkComboBoxText		*	optDcpLangCBB;
	GtkCheckButton		*	optDcpSubtitlesCheckBtn;
	GtkComboBoxText		*	optDcpSubtitlesCBB;
	GtkCheckButton		*	optDcpCountryCheckBtn;
	GtkComboBoxText		*	optDcpCountryCBB;
	GtkCheckButton		*	optDcpResolutionCheckBtn;
	GtkCheckButton		*	optDcpDateCheckBtn;
	GtkCheckButton		*	optDcpIssuerCheckBtn;
	
	/* QUICKDCP options widgets */
	GtkFileChooser		*	 optQdcpInputdirFileBtn;
	GtkFileChooser		*	 optQdcpOutputdirFileBtn;
	GtkSwitch			*	optQdcpBeepSwitch;
	GtkSpinButton		*	optQdcpThreadsSpinBtn;
	GtkSwitch			*	optQdcpDebugModeSwitch;
	GtkComboBoxText		*	optQdcpOdcpLoglevelCBB;
	GtkComboBoxText		*	optQdcpFFmpegLoglevelCBB;
	GtkComboBoxText		*	optQdcpJ2kMethodCBB;
	GtkComboBoxText		*	optQdcpMxfMethodCBB;
	
	/* KEYBOARD SHORTCUTS options widgets */
	GtkTreeView			*	optAccelTreeView;
	
	/* CONFIG FILE EDITOR widgets */
	GtkTextView			*	optConfigViewer;
	GtkTextBuffer		*	optConfigBuffer;
	GtkButton			*	optSaveConfigBtn;
	GtkButton			*	optCancelConfigBtn;

		GtkAdjustment	*	time_adjustment;
		GtkAdjustment	*	percent_adjustment;
		GtkAdjustment	*	u_percent_adjustment;
		GtkAdjustment	*	degrees_adjustment;
		GtkAdjustment	*	frames_adjustment;
		GtkAdjustment	*	pixels_adjustment;
		GtkAdjustment	*	u_pixels_adjustment;
};

/** Window Class */
struct _QuickDcpAppWindowClass
{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(QuickDcpAppWindow, quickdcp_app_window, GTK_TYPE_APPLICATION_WINDOW);


GType                   
quickdcp_app_window_get_type     (void);

QuickDcpAppWindow *	
quickdcp_app_window_new (QuickDcpApp *app);

void                    
quickdcp_app_open_command_line_arguments (QuickDcpAppWindow * win,
                         GFile             * file);
                          
static void
quickdcp_app_window_init (QuickDcpAppWindow * self);

/** Application window class initialization, 
 * including access to template's widgets
 * and bindings  to actions callbacks.
 * */
static void
quickdcp_app_window_class_init (QuickDcpAppWindowClass * class);




/* ************** A C T I O N - C A L L B A C K S ******************* */



/** File menu */

/* new project */
static void
action_new_project_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);	
/* open project */
static void
action_open_project_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);	
/* save project */
static void
action_save_project_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);	
/* save project AS */
static void
action_save_project_as_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);
/* import qdp */
static void
action_import_qdp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);
/* export qdp */
static void
action_export_qdp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);

/* quit activated */
static void
action_quit_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/** Edit menu */

/* preferences */
static void
preferences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);

/** View menu */

/* show intro */
static void
action_show_intro_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);
                       
/* show compositor */
static void
action_show_compositor_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);
                       
/* show sequences */
static void
action_show_sequences_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);
/* show audio */
static void
action_show_audio_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);
/* show DCP */
static void
action_show_dcp_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);


/* zoom in */
static void
action_zoom_in_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);
/* zoom out */
static void
action_zoom_out_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);
/* zoom fit */
static void
action_zoom_fit_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);


/* toggle toolbar */
static void
action_toggle_toolbar_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       app);

/* toggle statusbar */
static void
action_toggle_statusbar_activated (GSimpleAction *action,
                       GVariant      *parameter,
                       gpointer       user_data);


/** Help menu */

/* about activated */
static void
action_about_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app);





/* ***************** I N T R O  - C A L L B A C K S ***************** */




/**  Quick start (intro) buttons callbacks
 */
 
/* treat all workspace (into static DCPs or conversions or else...) */
static void
simple_carton_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

static void
convert_video_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* analyse video */

static void
action_import_video_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* analyse dcp */

static void
import_DCP_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* import audio */

static void
action_import_audio_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* import subtitles */

static void
action_import_subtitles_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* import dcp */
static void
action_import_dcp_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app);

/* open project template */

static void
open_template_activated (GSimpleAction *action,
                GVariant      *parameter,
                gpointer       app);

/* DCP preview */
static void
dcp_preview_activated (GSimpleAction *action,
						GVariant      *parameter,
						gpointer       app);



/* ************ C O M P O S I T O R  - F U N C T I O N S ************ */




qdcp_bool
comp_init_widgets(QuickDcpAppWindow * self);

qdcp_bool
comp_scan_available_piles(QuickDcpAppWindow * self);



/** Reset all compositor widgets values to defaults 
 * */
qdcp_bool
comp_reset_widgets(QuickDcpAppWindow * self);

/** Feed a project with current compositor widgets values
 * */
qdcp_bool
comp_get_widgets_values(t_project * pj, QuickDcpAppWindow * self);

/** Set all compositor widgets values from a t_project object
 * */
qdcp_bool
comp_set_widgets_values(t_project * pj, QuickDcpAppWindow * self);



qdcp_bool
comp_time_line_set_end(QuickDcpAppWindow * self);


/* ************ C O M P O S I T O R  - C A L L B A C K S ************ */


static void
comp_basename_changed_cb(QuickDcpAppWindow * self);

static void
comp_validate_basename_clicked(QuickDcpAppWindow * self);

static void
comp_time_spin_changed(GtkSpinButton * button,
						gpointer	   user_data);

static void
comp_format_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
static void
comp_resolution_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
static void
comp_category_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
static void
comp_framerate_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);

static void
comp_background_color_chooser_activated(GtkColorChooser * button,
										gpointer		  user_data);

static void
comp_text_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data);

static void
comp_shapes_line_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data);

static void
comp_shapes_fill_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data);

static void
comp_fondu_deb_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data);

static void
comp_fondu_fin_color_chooser_activated(GtkColorChooser	* button,
								  GdkRGBA 			* color,
								  gpointer		  	  user_data);

static void
comp_new_pile_btn_cb(QuickDcpAppWindow * self);

static void
comp_delete_pile_btn_cb(QuickDcpAppWindow * self);

static void
comp_refresh_piles_btn_cb(QuickDcpAppWindow * self);

static void
comp_new_image_layer_btn_cb(QuickDcpAppWindow * self);

static void
comp_duplicate_layer_btn_cb(QuickDcpAppWindow * self);

static void
comp_delete_layer_btn_cb(QuickDcpAppWindow * self);


static void
comp_piles_toggle_checked_cb(GtkCellRendererToggle * cell_renderer,
  							 GtkTreePath		   * path,
							 gpointer                user_data);

static void
comp_piles_row_activated_cb(GtkTreeView 	  * treeview,
							GtkTreePath		  *	path,
							GtkTreeViewColumn *	col,
							gpointer 			user_data);
/* -- */

static void
comp_generate_seq_btn_cb(QuickDcpAppWindow * self);

static void
comp_generate_dcp_btn_cb(QuickDcpAppWindow * self);

static void
comp_new_text_layer_btn_cb(QuickDcpAppWindow * self);



/** when a check cell is toggled */
static void
comp_layers_mask_layer_cb(GtkCellRendererToggle * cell_renderer,
						  gchar                 * path,
						  gpointer                user_data);

/** when a layer is selected or single-clicked */
static void
comp_layers_focus_layer_cb(GtkTreeView 	  	  * treeview,
						   GtkTreePath		  *	path,
						   GtkTreeViewColumn *	col,
						   gpointer 			user_data);

/** when rows are dragndropped */
static void
comp_layers_reorder_cb(GtkTreeView * treeview,
							gpointer user_data);




/* ********* D R A W I N G   A R E A  -  F U N C T I O N S ********** */



qdcp_bool
draw_drawing_area_init(QuickDcpAppWindow * self);

qdcp_bool
draw_set_background(canvas_ctx * canvas, t_project * pj);

qdcp_bool
draw_get_geometry(canvas_ctx * canvas, t_project * pj);

qdcp_bool
draw_compute_frame(canvas_ctx * canvas, t_project * pj);
										
qdcp_bool
draw_compute_value(canvas_ctx * canvas, t_project * pj);
												 
qdcp_bool
draw_compute_translation(canvas_ctx * canvas, t_project * pj);
		
qdcp_bool
draw_zoom_in(canvas_ctx * canvas, t_project * pj);
		
qdcp_bool
draw_zoom_out(canvas_ctx * canvas, t_project * pj);
		
qdcp_bool
draw_zoom_fit(canvas_ctx * canvas, t_project * pj);

qdcp_bool
draw_pix2perc(int x, int y);



/* ********* D R A W I N G   A R E A  -  C A L L B A C K S ********** */


/** A callback to retrieve a newly created surface from an offscreen
 * gdk window upon its realization or resizing ('create-surface' signal)
 * */
//~ CairoSurface *
//~ get_a_gdk_window_surface_on_creation(GdkWindow * window,
									 //~ gint		 width,
									 //~ gint		 height,
									 //~ gpointer	 user_data);

/** On a click event in the drawing area
 * we try to select the top (visible) layer extending to the pointer coordinates.
 * To select a lower (and covered) or currently hidden layer, 
 * we'll need to use the layers tree view.
 * */
static void
draw_drawing_area_try_layer_selection_by_click_cb(GtkDrawingArea * dwa,
												  gpointer			    user_data);

static void
draw_canvas_draw_event_cb(GtkDrawingArea  * dwa,
							cairo_t			* cr,
							gpointer	      user_data);

static void
draw_drawing_area_realize_cb(GtkDrawingArea * dwa,
							 GdkEvent		* event,
							 gpointer		  user_data);

static void
draw_drawing_area_size_allocate_cb(GtkDrawingArea * dwa,
								   GdkEvent		  * event,
								   gpointer		    user_data);

static void
draw_drawing_area_configure_event_cb(GtkDrawingArea * dwa,
									 GdkEvent		* event,
									 gpointer	      user_data);



/* ****************** D C P - C A L L B A C K S ********************* */


/* dcp buttons callbacks */

/** Initiate DCP creation process
 * */
static void
dcp_start_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller);

/** Copy selected DCP to some place (e.g. an USB drive)
 * */
static void
dcp_copy_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller);

/** Generate a zip compressed file from selected DCP(s)
 * */
static void
dcp_zip_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller);

/** Send selected DCPs in a known and fit place (folder)
 * where we won't see them anymore.
 * Let's call that 'the Attic'.
 * */
static void
dcp_attic_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller);

/** Delete selected DCP(s) from the hard drive, 
 * after some confirmation dialog.
 * */
static void
dcp_delete_btn_cb(QuickDcpAppWindow * self, GtkWidget * caller);


/* dcp treeview callbacks */
static void	
dcp_tree_selection_cb(GtkTreeSelection * select, 
					  gpointer * user_data);

static void
dcp_tree_toggled_cb(GtkCellRendererToggle * cell_renderer,
					gchar                 * path,
					gpointer                user_data);

static void
dcp_tree_row_double_clicked_cb(GtkTreeView		 * 	treeview,
							   GtkTreePath		 *	path,
							   GtkTreeViewColumn *	col,
							   gpointer 			user_data);




/* *********** P R E F E R E N C E S - F U N C T I O N S ************ */



qdcp_bool
opt_init_widgets(QuickDcpAppWindow * self);



/* *********** P R E F E R E N C E S - C A L L B A C K S ************ */




static void
opt_save_config_btn_cb(QuickDcpAppWindow * self);

static void
opt_cancel_config_btn_cb(QuickDcpAppWindow * self);

static void
opt_close_btn_cb(QuickDcpAppWindow * self);

static void
opt_time_spin_changed(GtkSpinButton * button,
						gpointer	   user_data);
static void
opt_format_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
						
static void
opt_resolution_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
						
static void
opt_category_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);
						
static void
opt_framerate_CBB_changed(GtkComboBoxText * combobox,
						gpointer		user_data);

static void
opt_background_color_chooser_activated(GtkColorChooser * button,
										gpointer		  user_data);



/* ***************** S E Q U E N C E  - W D G T S ******************* */



qdcp_bool
vid_init_widgets(QuickDcpAppWindow * self);
	




/* ******************* A U D I 0 - W D G T S ************************ */




qdcp_bool
aud_init_widgets(QuickDcpAppWindow * self);

qdcp_bool
aud_unprocessed_add_source_btn_cb(QuickDcpAppWindow * self);

qdcp_bool
aud_unprocessed_delete_btn_cb(QuickDcpAppWindow * self);

qdcp_bool
aud_unprocessed_refresh_btn_cb(QuickDcpAppWindow * self);

qdcp_bool
aud_unprocessed_listen_btn_cb(QuickDcpAppWindow * self);

qdcp_bool
aud_unprocessed_convert_btn_cb(QuickDcpAppWindow * self);





/* ******************** D C P - W D G T S *************************** */




qdcp_bool
dcp_init_widgets(QuickDcpAppWindow * self);

qdcp_bool
dcp_config_widgets(QuickDcpAppWindow * self);


/*---------------------------- TREEVIEW ------------------------------*/

/** Stuffing the treeview with DCP jobs list
 * */
qdcp_bool	
dcp_store_dcps(QuickDcpAppWindow * self, GtkTreeStore * store);

/** Read the output directory to retrieve a pending and complete jobs list.
 * */
qdcp_bool
dcp_scan_dcps(QuickDcpAppWindow * self, qdcp_bool keep_checked);

/** Make sure a DCP folder contains the minimum files 
 * to make a valid DCP */
qdcp_bool
dcp_is_actual_dcp(GFileEnumerator * dcp_children);

/** This is called when a dcp is selected in the dcp treeview.
 * */
qdcp_bool	
dcp_check_item(QuickDcpAppWindow * self, GtkCellRendererText * cell, char * path, GtkListStore * store);



/* ***************** M I S C E L L A N E O U S ********************** */


/** Get the index of a string in a string array.
 * This is used to retrieve indices for comboboxes configuration
 * */
int
index_of_cbb(const char ** haystack, int len, const char * needle);

void
set_unsaved(QuickDcpAppWindow * self);

qdcp_bool
gui_color_rgba2hex(char * hex_color, GdkRGBA * rgba_color);

char *
get_dcp_name_from_widgets(QuickDcpAppWindow * self);


/* ************************** M A I N ******************************* */



#endif // QDCP_GUI_H_INCLUDED
