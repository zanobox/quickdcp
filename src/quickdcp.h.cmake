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
 * @file    quickdcp.h
 * @author  jean.sabatier@zanobox.net
 * @brief   QuickDCP's common functions set. Header file.
*/

#ifndef QUICKDCP_H_INCLUDED
#define QUICKDCP_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#define QDCP_NAME			"@PROJECT_NAME@"
#define QDCP_VERSION_MAJ 	@VERSION_MAJOR@
#define QDCP_VERSION_MIN 	@VERSION_MINOR@
#define QDCP_VERSION_REV 	@VERSION_BUILD@
#define QDCP_VERSION 		"@QDCP_VERSION@"
#define QDCP_VERSION_DATE 	"@VERSION_DATE@"
#define QDCP_COPYRIGHT  	"@QDCP_COPYRIGHT@"
#define QDCP_LICENSE    	"@QDCP_LICENSE@"
#define QDCP_URL 			"@QDCP_URL@"

#cmakedefine UNIX			@UNIX@
#cmakedefine APPLE			@APPLE@		
#cmakedefine WIN32			@WIN32@

#cmakedefine TARGET_ARCH		"@TARGET_ARCH@"	
#cmakedefine IMAGEMAGICK7		@IMAGEMAGICK7@
#cmakedefine ENABLE_GUI			@ENABLE_GUI@

#ifndef WIN32
#include <pthread.h>
#endif
/* about syslog on windows, see http://syslog-win32.sourceforge.net */
#include <syslog.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <inttypes.h>
#include <dirent.h>
#include <sys/stat.h>
#include <math.h>
#include <libgen.h>
#include <time.h>
#include <fcntl.h>
#include <libintl.h>
#include <libxml/xmlwriter.h>
#ifdef OPENMP
	#include <omp.h>
#endif
#ifdef IMAGEMAGICK7
	#include "MagickWand/MagickWand.h"
#else
	#include "wand/MagickWand.h"
#endif
#include "libavutil/avutil.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"

//~ #include <errno.h>
//~ #include <limits.h>


/* Internationalization */
/* for internationalization */
#define  _(x)     gettext (x)
#define N_(x)     x
#define C_(ctx,x) pgettext (ctx, x)

/* Preprocessor : booleans */
#define QDCP_YES 		1
#define QDCP_NO 		0
#define QDCP_ON 		1
#define QDCP_OFF 		0
#define QDCP_TRUE 		1
#define QDCP_FALSE 		0

#define QDCP_SUCCESS	1
#define QDCP_FAILURE	0

/* Preprocessor : files and directories */

#define PROGRAM				"@PACKAGE_NAME@"
#define CFG_FILE 			"@PACKAGE_NAME@.ini"
#define DCP_DIR 			"DCP"
#define PROJ_DIR 			".projets"
#define TMPL_DIR			".templates"
#ifdef WIN32
	#define TEXT_EDITOR		"Notepad.exe"
	#define HOME_DIR 		sprintf(HOME, "%s\\%s", getenv("HOMEDRIVE"), getenv("HOMEPATH"))
	#define SEPARATOR 		"\\"
	#define CFG_DIR 		"programs\@PACKAGE_NAME@"
	#define TMP_IMG 		".tmp\img"
	#define TMP_VID			".tmp\vid"
	#define TMP_SEQ 		".tmp\seq"
	#define TMP_TIF			".tmp\tif"
	#define TMP_J2K 		".tmp\j2k"
	#define TMP_WAV 		".tmp\wav"
	#define TMP_SUB 		".tmp\sub"
	#define TMP_MXF 		".tmp\mxf"
	#define LOG_DIR 		".tmp\log"
#else
	#define CLI_EDITOR		"vim"
	#define TEXT_EDITOR		"gedit"
	#define HOME_DIR 		getenv("HOME")
	#define SEPARATOR 		"/"
	#define CFG_DIR 		".config/@PACKAGE_NAME@"
	#define TMP_DIR 		".tmp"
	#define TMP_IMG 		".tmp/img"
	#define TMP_VID			".tmp/vid"
	#define TMP_SEQ 		".tmp/seq"
	#define TMP_TIF			".tmp/tif"
	#define TMP_J2K 		".tmp/j2k"
	#define TMP_WAV 		".tmp/wav"
	#define TMP_SUB 		".tmp/sub"
	#define TMP_MXF 		".tmp/mxf"
	#define LOG_DIR 		".tmp/log"
	#define SHARE_DIR 		"@INSTALL_PREFIX@/share/@PACKAGE_NAME@"
	// #define ICON_DIR 		"@INSTALL_PREFIX@/share/@PACKAGE_NAME@/icons"
	#define ICON_DIR 		"@INSTALL_PREFIX@/share/icons/@PACKAGE_NAME@"
	#define APP_ICON 		"@INSTALL_PREFIX@/share/icons/@PACKAGE_NAME@/@PACKAGE_NAME@.svg"
	// #define APP_ICON 		"@INSTALL_PREFIX@/share/@PACKAGE_NAME@/icons/@PACKAGE_NAME@.svg"
#endif
#ifdef APPLE
	#define TEXT_EDITOR		"TextEdit"
#endif


/* Preprocessor : maximum values */

#define FILE_STR_LEN		8192
#define PATH_STR_LEN		1024
#define CMD_STR_LEN 		512
#define NAME_STR_LEN 		256
#define FULL_STR_LEN 		128
#define HALF_STR_LEN 		64
#define WORD_STR_LEN 		32
#define COLOR_STR_LEN 		18
#define SHORT_STR_LEN 		8
#define TINY_STR_LEN 		4

#define DEFAULT_PROJECT_NAME	"Nouveau"	/* Exemple */
#define DEFAULT_TRACK_NAME		"Nouvelle_Piste_Audio"

#define MAX_ASSETS          10   /**< Soft limit */
#define MAX_REELS           30   /**< Soft limit */
#define MAX_PKL             1    /**< Soft limit */
#define MAX_CPL             5    /**< Soft limit */
#define MAX_PATH_LENGTH     4095
#define MAX_FILENAME_LENGTH 254
#define MAX_AUDIO_CHANNELS  16   /**< maximum allowed audio channels */

#define FILE_READ_SIZE      16384

#define MAX_DCP_JPEG_BITRATE 250000000  /**< Maximum DCI compliant bit rate for JPEG2000 */
#define MAX_DCP_MPEG_BITRATE  80000000  /**< Maximum DCI compliant bit rate for MPEG */

#define MAX_WIDTH_2K        2048
#define MAX_HEIGHT_2K       1080

/* Preprocessor : XML strings */

#define XML_ENCODING 	"UTF-8"
#define XML_HEADER  	"<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>"

#define NS_CPL			"http://www.smpte-ra.org/schemas/429-7/2006/CPL"     	/**< SMPTE */
#define NS_CPL_3D		"http://www.smpte-ra.org/schemas/429-10/2008/Main-Stereo-Picture-CPL"   /* SMPTE */
#define NS_PKL 			"http://www.smpte-ra.org/schemas/429-8/2007/PKL"     	/**< SMPTE */
#define NS_AM 			"http://www.smpte-ra.org/schemas/429-9/2007/AM"      	/**< SMPTE */

#define DS_DSIG 		"http://www.w3.org/2000/09/xmldsig#"                    /**< digital signature */
#define DS_CMA  		"http://www.w3.org/TR/2001/REC-xml-c14n-20010315"       /**< canonicalization method */
#define DS_DMA  		"http://www.w3.org/2000/09/xmldsig#sha1"                /**< digest method */
#define DS_TMA  		"http://www.w3.org/2000/09/xmldsig#enveloped-signature" /**< transport method */
#define DS_SMA 			"http://www.w3.org/2001/04/xmldsig-more#rsa-sha256"     /**< SMPTE */

#define RATING_AGENCY	"http://www.mpaa.org/2003-ratings"
                        //~ "http://rcq.qc.ca/2003-ratings"

/* Preprocessor : CLI stuff */

#define CLI_INDENT 		" > "
#define CLI_OK 			" > Ok\n"

/* CLI colors */
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

/* CLI foreground colors */
#define CLI_COLOR			"\x1b[0m"	/*< system default CLI colors (fg & bg) */
#define CLI_BLACK			"\x1b[30m"
#define CLI_RED				"\x1b[31m"
#define CLI_GREEN			"\x1b[32m"
#define CLI_YELLOW			"\x1b[33m"
#define CLI_BLUE			"\x1b[34m"
#define CLI_MAGENTA			"\x1b[35m"
#define CLI_CYAN			"\x1b[36m"
#define CLI_WHITE			"\x1b[37m"

/* CLI bold foreground colors */
#define CLI_BOLD_BLACK		"\x1b[1;30m"
#define CLI_BOLD_RED		"\x1b[1;31m"
#define CLI_BOLD_GREEN		"\x1b[1;32m"
#define CLI_BOLD_YELLOW		"\x1b[1;33m"
#define CLI_BOLD_BLUE		"\x1b[1;34m"
#define CLI_BOLD_MAGENTA	"\x1b[1;35m"
#define CLI_BOLD_CYAN		"\x1b[1;36m"
#define CLI_BOLD_WHITE		"\x1b[1;37m"

/* CLI background colors */
#define CLI_BG_BLACK		"\x1b[40m"
#define CLI_BG_RED			"\x1b[41m"
#define CLI_BG_GREEN		"\x1b[42m"
#define CLI_BG_YELLOW		"\x1b[43m"
#define CLI_BG_BLUE			"\x1b[44m"
#define CLI_BG_MAGENTA		"\x1b[45m"
#define CLI_BG_CYAN			"\x1b[46m"
#define CLI_BG_WHITE		"\x1b[47m"

/* one of SPINNER_CLASSIC, SPINNER_BALOON, SPINNER_BOUNCING */
#define DEFAULT_SPINNER SPINNER_BOUNCING

/* Preprocessor : Default projects parameters */

#define DCP_ANNOTATION  "QuickDCP-me-computavit"
#define DCP_TITLE       "QuickDCP-stuff"
#define DCP_KIND        "test"


#define UNUSED(x) ( (void)(x) )
#define BASE_FILE (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : strrchr(__FILE__, '\\') ? strrchr(__FILE__, '\\') + 1 : __FILE__)


/* imagemagick exceptions */
#define ThrowWandException(wand) \
{ \
  char \
    *description; \
 \
  ExceptionType \
    severity; \
 \
  description=MagickGetException(wand,&severity); \
  (void) fprintf(stderr,"%s %s %lu %s\n",GetMagickModule(),description); \
  MagickRelinquishMemory(description); \
  exit(-1); \
}

#if _MSC_VER
#define snprintf _snprintf
#endif

#define QDCP_UNKNOWN_ERROR		"Erreur inconnue"
#define NO_QDCP_INIT			"Echec de l'initialisation de QuickDCP"
#define NO_QDCP_CONFIG			"Echec de la configuration de QuickDCP \navec le fichier %s"
#define CANT_CREATE_DIR			"Impossible de créer le dossier %s"
#define BAD_PATH				"Echec de l'ouverture du dossier %s"
#define CANT_OPEN_FILE			"Impossible d'ouvrir le fichier %s"
#define CANT_READ_FILE			"Impossible de lire le fichier %s"
#define NO_SOURCE_FOUND			"Pas de fichiers sources en provenance de"
#define NO_QDP_FOUND			"Aucun projet qdp n'a été trouvé dans"
#define INVALID_RELATIVE_PATH	"La fonction requiert un chemin absolu !"
#define CANT_WRITE_XML			"Impossible d'écrire le fichier XML !"
#define CANT_READ_XML			"Impossible d'ouvrir le fichier XML !"
#define ADD_FILE_FAILED			"Echec de l'enregistrement du fichier"
#define BAD_CALLOC				"Echec d'allocation de mémoire d'une structure"		
#define T_PROJECT_FAILED		"Echec de la création de la structure du projet"
#define T_PROJECT_LIST_FAILED	"Echec de la création d'une liste de projets"
#define BAD_LIST_INIT			"Echec d'initialisation de liste de fichiers."
#define NO_SETTINGS_YET			"Les réglages à la volée ne sont pas encore possibles."
#define CANT_WRITE_TIFF			"L'écriture du fichier tiff temporaire a échoué."
/* deprecated */
#define NO_SHELL				"Shell non disponible ou échec de fork"
#define COMMAND_FAILED			"Popen failed to run command !"

/* log and debug */

#ifndef CLI_MODE
#define CLI_MODE QDCP_ON
#endif

#define CLI_TALK(...) \
        do { if (CLI_MODE) printf( __VA_ARGS__); } while (0)


#define QDCP_LOG(LOG_LEVEL, ...) \
		f = fopen(LOG_DIRSEPARATORLOG_LEVEL, "w+"); \
		fprintf(f, "%s", __VA_ARGS__);

#define warning(...) do { \
			char location[80]; \
			char message[1024]; \
			sprintf(location, "%s(), %s, l. %d", \
				 __func__, strip_path(__FILE__), __LINE__); \
			sprintf(message, __VA_ARGS__); \
			warning_msg(location, message); \
			} while (0)

/* generate error messages */
#define FOREACH_QDCP_ERROR_MSG(QDCP_ERROR_MSG) \
        QDCP_ERROR_MSG(QDCP_NO_ERROR,                "No error")  \
        QDCP_ERROR_MSG(QDCP_ERROR,                   "OpenDCP error")  \
        QDCP_ERROR_MSG(QDCP_FATAL,                   "OpenDCP fatal error")  \
        QDCP_ERROR_MSG(QDCP_FILEOPEN,                "Could not open file") \
        QDCP_ERROR_MSG(QDCP_INVALID_PICTURE_TRACK,   "Invalid picture track type") \
        QDCP_ERROR_MSG(QDCP_NO_PICTURE_TRACK,        "Reel does not contain a picture track") \
        QDCP_ERROR_MSG(QDCP_MULTIPLE_PICTURE_TRACK,  "Reel contains multiple picture tracks") \
        QDCP_ERROR_MSG(QDCP_INVALID_SOUND_TRACK,     "Invalid sound track type") \
        QDCP_ERROR_MSG(QDCP_NO_SOUND_TRACK,          "Reel does not contain a sound track") \
        QDCP_ERROR_MSG(QDCP_MULTIPLE_SOUND_TRACK,    "Reel contains multiple sound tracks") \
        QDCP_ERROR_MSG(QDCP_SPECIFICATION_MISMATCH,  "DCP contains MXF Interop and SMPTE track") \
        QDCP_ERROR_MSG(QDCP_TRACK_NO_DURATION,       "Track has no duration") \
        QDCP_ERROR_MSG(QDCP_J2K_ERROR,               "JPEG2000 error") \
        QDCP_ERROR_MSG(QDCP_CALC_DIGEST,             "Could not calculate MXF digest") \
        QDCP_ERROR_MSG(QDCP_DETECT_TRACK_TYPE,       "Could not determine MXF track type") \
        QDCP_ERROR_MSG(QDCP_INVALID_TRACK_TYPE,      "Invalid MXF track type") \
        QDCP_ERROR_MSG(QDCP_UNKNOWN_TRACK_TYPE,      "Unknown MXF track type") \
        QDCP_ERROR_MSG(QDCP_INVALID_WAV_BITDEPTH,    "WAV is not 24-bit") \
        QDCP_ERROR_MSG(QDCP_INVALID_WAV_CHANNELS,    "WAV has an incorrect number of channels") \
        QDCP_ERROR_MSG(QDCP_FILEOPEN_MPEG2,          "Could not open MPEG2 file") \
        QDCP_ERROR_MSG(QDCP_FILEOPEN_J2K,            "Could not open JPEG200 file") \
        QDCP_ERROR_MSG(QDCP_FILEOPEN_J2K_S,          "Could not open stereoscopic JPEG200 file") \
        QDCP_ERROR_MSG(QDCP_FILEOPEN_WAV,            "Could not open wav file") \
        QDCP_ERROR_MSG(QDCP_FILEOPEN_TT,             "Could not open subtitle file") \
        QDCP_ERROR_MSG(QDCP_FILEWRITE_MXF,           "Could not write MXF file") \
        QDCP_ERROR_MSG(QDCP_FINALIZE_MXF,            "Could not finalize MXF file") \
        QDCP_ERROR_MSG(QDCP_PARSER_RESET,            "Could not reset MXF parser") \
        QDCP_ERROR_MSG(QDCP_STRING_LENGTH,           "Input files have differing file lengths") \
        QDCP_ERROR_MSG(QDCP_STRING_NOTSEQUENTIAL ,   "Input files are not sequential") \
        QDCP_ERROR_MSG(QDCP_MAX_ERROR,               "Maximum error string")

#define GENERATE_ENUM(ERROR, STRING) ERROR,
#define GENERATE_STRING(ERROR, STRING) STRING,
#define GENERATE_NAME(ERROR, STRING) #ERROR,
#define GENERATE_STRUCT(ERROR, STRING) { ERROR, #ERROR, STRING },


/*--------------------------------------------------------------------*
 *  						  Enumerations							  * 
 *--------------------------------------------------------------------*/

enum QDCP_ERROR_MESSAGES {
    FOREACH_QDCP_ERROR_MSG(GENERATE_ENUM)
};



/*
 * Simple types
 */

typedef unsigned char byte_t;
/* booleans */
typedef int qdcp_bool;

/*
 * QuickDCP, OpenDCP and ASDCP enums
 */

/** QuickDCP verbosity */
enum verbosity_level {
	VERBOSE_QUIET,
	VERBOSE_NORMAL,
	VERBOSE_VERBOSE,
	VERBOSE_BIGMOUTH,
	VERBOSE_DEBUG
};

/* project status */
/** 
 * project status 
 * Not bitwise anymore, it seems that it was outside
 * the capacity of a qdcp->status[9], an array of 9.
 */
enum project_status {
	PJ_INIT 		,
	PJ_CARTON 		,
	PJ_PILE			,
	PJ_SEQUENCE		,
	PJ_UNMIXED_WAV 	,
	PJ_RAW_SUBS		,
	PJ_MXF 			,
	PJ_XML 			,
	PJ_DCP 
};

/** Layers
 */
typedef enum layer_type_enum
{
	LAYER_BLANK,
	LAYER_IMAGE,
	LAYER_TEXT,
	LAYER_VECTORIAL,
	LAYER_VIDEO,
	LAYER_MXF
} layer_type;


/** Image rendering switches */
enum render_modes {
	PREVIEW_ONLY,
	J2K_ONLY,
	J2K_PLUS_PREVIEW
};

/** Recognized mime types */
enum mime_types {
	MIME_OTHER,
	MIME_IMG,
	MIME_PNG,
	MIME_JPG,
	MIME_GIF,
	MIME_SVG,
	MIME_TIF,
	MIME_J2K,
	MIME_SND,
	MIME_WAV,
	MIME_MP3,
	MIME_VID,
	MIME_MOV,
	MIME_AVI,
	MIME_SRT,
	MIME_MXF,
	MIME_QDP,
	MIME_DCP
};

/** format (image ratio) types (needed for comboboxes) */
typedef enum formats_types_enum
{
	FMT_UNKNOWN,
	FMT_FLAT,
	FMT_SCOPE,
	FMT_FULL,
	FMT_VIDEO,
	FMT_N_FORMATS
} formats_types;
	
/** resolution types (needed for comboboxes) */
typedef enum resolution_types_enum
{
	RES_UNKNOWN,
	RES_2K,
	RES_4K,
	RES_N_RESOLUTIONS
} resolution_types;
	
/** framerate types (needed for comboboxes) */
typedef enum framerate_types_enum
{
	FRATE_NONE,
	FRATE_12,
	FRATE_24,
	FRATE_25,
	FRATE_30,
	FRATE_48,
	FRATE_50,
	FRATE_60,
	FRATE_96,
	FRATE_N_FRAMERATES
} framerate_types;
	
/** fondus types */
typedef enum fondus_types_enum
{
	FONDU_AUCUN,
	FONDU_AU_NOIR,
	FONDU_IRIS,
	FONDU_N_FONDUS
} fondus_types;


/** vectorial drawing */
typedef enum shapes_types_enum {
	SHAPE_UNDEFINED,
	SHAPE_RECTANGLE,
	SHAPE_ROUND_RECTANGLE,
	SHAPE_CIRCLE,
	SHAPE_ELLIPSE,
	SHAPE_N_SHAPES
} shapes_types;

/** audio formats */
enum audio_formats {
	AUD_UNKNOWN,
	AUD_WAV,
	AUD_OGG,
	AUD_MP3,
	AUD_MIDI,
	AUD_N_FORMATS
};

/** audio encodings */
enum audio_encodings {
	AUD_ENC_UNSET,
	AUD_AAC3
};

enum accepted_samplerates {
	KHZ_UNDEF = 0,
	KHZ_48 = 48000,
	KHZ_96 = 96000
};

/** diferent types of audio mix settings */
enum mix_types {
	WAVMIX_REPARTITION,
	WAVMIX_MIXAGE,
	WAVMIX_INVERSION
};
/** status of an audio track */
enum wav_status {
	TRACK_UNCHECKED,	/**< Audio file has not been checked could be anything, even inexistant. */
	TRACK_NOTWAV,		/**< Audio file has been checked and is not wav. */
	TRACK_UNMIXED,		/**< Audio file is wav, possibly natively or converted by QuickDCP. */
	TRACK_MIXED,		/**< Audio file is mixed wav, according to its parent project's settings */
	TRACK_MXF			/**< Audio file has been put in an MXF packet,ready to go into a reel. */
};
/** channels */
enum channel_set {
	CH_UNSET	= 0,
	CH_MONO 	= 1,	/**< Audio channel settings : 1 channel AV_CH_LAYOUT_MONO in FFmpeg */
	CH_STEREO	= 2,	/**< Audio channel settings : 2 channels AV_CH_LAYOUT_STEREO in FFmpeg */
	CH_5_1		= 6,	/**< Audio channel settings : 6 channels AV_CH_LAYOUT_5POINT1 in FFmpeg */
	CH_7_1		= 7		/**< Audio channel settings : 7 channels AV_CH_LAYOUT_7POINT1 in FFmpeg */			
};

/** 
 * @brief DCP categories 
 *
 * It seems that categories names can take upper case initial (like 'Feature').
 * The members of this enumeration are a bridge between short names such as 'FTR' and 
 * usual names like 'feature' or 'psa'.
 */
enum DCP_CATEGORIES {
	CAT_NONE,
	CAT_TST,		/**< test */
	CAT_PSA,		/**< psa (public social announcement) */
	CAT_XSN,		/**< Can't remember what that means ! */
	CAT_POL,		/**< policy */
	CAT_ADV,		/**< advertisement */
	CAT_TSR,		/**< teaser */
	CAT_TLR,		/**< trailer */
	CAT_SHR,		/**< short */
	CAT_FTR,		/**< feature */
	CAT_N_CATEGORIES
};

/** DCP resolution */
enum CINEMA_PROFILE {
    DCP_CINEMA2K = 3,
    DCP_CINEMA4K = 4,
};


enum ASSET_CLASS_TYPE {
    ACT_UNKNOWN = 0,
    ACT_PICTURE,
    ACT_SOUND,
    ACT_TIMED_TEXT
};

/**
 * This enum is an implementation of an asdcplib enum
 * which describes the nature of the content of a reel asset.
 */
enum ASSET_ESSENCE_TYPE {
    AET_UNKNOWN,             /**< The file is not a supported AS-DCP essence container */
    AET_MPEG2_VES,           /**< The file contains an MPEG video elementary stream */
    AET_JPEG_2000,           /**< The file contains one or more JPEG 2000 codestreams */
    AET_PCM_24b_48k,         /**< The file contains one or more PCM audio pairs */
    AET_PCM_24b_96k,         /**< The file contains one or more PCM audio pairs */
    AET_TIMED_TEXT,          /**< The file contains an XML timed text document and one or more resources */
    AET_JPEG_2000_S,         /**< The file contains one or more JPEG 2000 codestream pairs (stereoscopic) */
    AET_DCDATA_UNKNOWN,      /**< The file contains one or more D-Cinema Data bytestreams */
    AET_DCDATA_DOLBY_ATMOS,  /**< The file contains one or more DolbyATMOS bytestreams */
};

enum DPX_MODE {
    DPX_LINEAR = 0,
    DPX_FILM,
    DPX_VIDEO
};

enum COLOR_PROFILE_ENUM {
    CP_SRGB = 0,
    CP_REC709,
    CP_P3,
    CP_SRGB_COMPLEX,
    CP_REC709_COMPLEX,
    CP_MAX
};

/**
 * This is kept for backward compatibility with some
 * code from opendcp and asdcplib.
 * Actually, QuickDCP does not really consider implementing the
 * soon to be deprecated Interop standard. So this can be 
 * overlooked as almost already deprecated, SMPTE being the only 
 * viable standard. Yet, this could be kept for rainy days, when a newer
 * standard could appear.
 */
enum XML_NAMESPACE {
    XML_NS_UNKNOWN,
    XML_NS_INTEROP,
    XML_NS_SMPTE
};

enum spinners
{
	SPINNER_CLASSIC,
	SPINNER_BALLOON,
	SPINNER_BOUNCING
};


/*--------------------------------------------------------------------*
 *  							Structures					    	  * 
 *--------------------------------------------------------------------*/

/* t_qdcp_error */
typedef struct {
    int error;
    char *name;
    char *string;
} t_qdcp_error;

/* RGB color type */
typedef struct rgb_color_t RGB_color;
struct rgb_color_t {
    float r;
    float g;
    float b;
};

/* RGBA color type */
typedef struct rgba_color_t RGBA_color;
struct rgba_color_t {
    float r;
    float g;
    float b;
	float a;
};

/*-----------------  File objects and lists of files -----------------*/

typedef struct file_obj_struct file_obj;
struct file_obj_struct
{
			int				index;					/**< Index of file in the list */
			char 			path[PATH_STR_LEN];		/**< Absolute file path with trailing slash */
			char			fname[NAME_STR_LEN];	/**< Plain filename without path */
			char 			full_path[PATH_STR_LEN];/**< Absolute file path and name */
			char			basename[NAME_STR_LEN];	/**< File basename no path, no extension */
			char			ext[SHORT_STR_LEN];		/**< As deduced from the file name's extension */
			char			mime[SHORT_STR_LEN];	/**< As deduced from the binary's header */
			time_t			mod_time;				/**< Last modification time */
			file_obj	*	prev;					/**< Previous file in the list */
			file_obj 	*	next;					/**< Next file in the list */
};

/* t_file_list */
typedef struct {
	int				size;
	file_obj	*	first;
	file_obj	*	last;
} t_file_list;

/* filenames list */
typedef struct {
    char **files;
    int  nfiles;
} t_filenames;

/* a filename of the form <prefix>N*.<ext> paired with its index. */
typedef struct {
    char *file;
    int   index;
} t_qdcp_sort;


/*---------------------- DCP related structures ----------------------*/

/* t_qdcp_cb */
typedef struct {
    int   (*callback)(void *);
    void  *argument;
} t_qdcp_cb;

/* qdcp_node_t */
typedef struct {
    void   *data;
    struct node_t *next;
} qdcp_node_t;

/* remote_t */
typedef struct {
    int            id;
    char           *host;
    char           *port;
} remote_t;

/* checksum */

/* md5_t */
typedef struct {
    uint32_t buf[4];
    uint32_t bits[2];
    unsigned char in[64];
} md5_t;

/* t_sha1 */
typedef struct {
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} t_sha1;

/* t_mxf */
typedef struct {
    int            	start_frame;
    int            	end_frame;
    int            	duration;
    int            	slide;
    int            	encrypt_header_flag;
    int            	key_flag;
    int            	delete_intermediate;
    byte_t         	key_id[16];
    byte_t         	key_value[16];
    int            	write_hmac;
    t_qdcp_cb   	frame_done;
    t_qdcp_cb   	file_done;
}t_mxf;


/* t_asset */
/** 
 * @brief The t_asset structure is used to write XML tags of different aspects
 * in CPL, PKL and ASSETMAP XML files.
 * 
 * Most of this struct members are filled by 'read_asset_info' function in asdcp_intf.cpp.
 * @see asdcp_intf.cpp
 */
typedef struct {
    char           uuid[40];					/**< Used in PKl, CPL and ASSETMAP Id fields, to obviously identify the asset. */
    int            essence_type;				/**< The essence of the asset's content, such as ESS_JPEG_2000_S, ESS_JPEG_2000, ESS_PCM_24b_48k, ESS_PCM_24b_96k, ESS_TIMED_TEXT */
    int            essence_class;				/**< One of ACT_PICTURE, ACT_SOUND, ACT_TIMED_TEXT */
    int            duration;					/**< Total number of frames/samples of the asset. */
    int            intrinsic_duration;			/**< Total number of frames/samples of the asset. */
    int            entry_point;					/**< Position of the asset's first frame in the reel. */
    int            xml_ns;						/**< Should be 2, i.e. SMPTE. */
    int            stereoscopic;				
    char           size[18];					/**< Size of the asset's file in bytes (here a char array, for XML purpose I guess). */
    char           name[128];					/**< @todo What is this for ? Can we suppress it ? */
    char           annotation[128];				/**< Needed in PKL AnnotationText field, and also in ASSETMAP file asset list. */
    char           edit_rate[20];				/**< A rationnal pair such as '24 1'. Appears as such in CPL's EditRate field. */
    char           frame_rate[20];				/**< A rationnal pair such as '24 1'. Appears as such in CPL's FrameRate field. */
    char           sample_rate[20];				/**< For audio only. Should be 48000 (Hz). */
    char           aspect_ratio[20];			/**< This is a string representation of a rationnal pair of two integers of the type '1998 1080'. It appears in CPL's ScreenAspectRatio of MainPicture. In non-SMPTE DCP, it's converted into a float (1.85) */
    char           digest[40];
    char           filename[NAME_STR_LEN];		/**< Asset's corresponding file's name, without path. Appears as such in PKL OriginalFileName field. */
    char			filepath[PATH_STR_LEN];		/**< Asset's corresponding file's full (absolute) path and name */
    int            encrypted;					/**< 1 if asset is encrypted, 0 if not see read_asset_info, asdcp_intf.cpp) */
    char           key_id[40];					/**< Filled only when asset is encrypted. Otherwise empty. */
} t_asset;

/* t_asset_list */
typedef struct {
    char filename[FILENAME_MAX];
} t_asset_list;

/* t_reel */
/**
 * @brief Reels are used in CPL files, where they are children of a 
 * ReelList tag which is usually composed of just one reel. 
 * Which in turn is composed of a list of not more than 3 assets : 
 * MainPicture (mandatory), MainSound (optionnal), MainSubtitle (optionnal).
 */
typedef struct {
    char           uuid[40];
    char           annotation[128]; 
    int            asset_count;		/**< Number of assets in this reel. */
    t_asset        main_picture;	/**< An asset whose essence_class must be ACT_PICTURE (=1). */
    t_asset        main_sound;		/**< An asset whose essence_class must be ACT_SOUND (=2). */
    t_asset        main_subtitle;	/**< An asset whose essence_class must be ACT_TIMED_TEXT (=3). */
} t_reel;

/* t_reel_list */
typedef struct {
    int          asset_count;
    t_asset_list asset_list[3];
} t_reel_list;

/* t_cpl */
typedef struct {
    char           uuid[40];
    char           annotation[128];
    char           size[18];
    char           digest[40];
    int            duration;
    int            entry_point;
    char           issuer[80];
    char           creator[80];
    char           title[80];
    char           issue_date[30];
    char           kind[32];
    char           rating[32];
    char           filename[NAME_STR_LEN];
    int            reel_count;
    t_reel         reel[MAX_REELS];
} t_cpl;

/* t_pkl */
typedef struct {
    char           uuid[40];
    char           annotation[128];
    char           size[18];
    char           issuer[80];
    char           creator[80];
    char           timestamp[30];
    char           filename[NAME_STR_LEN];	/**< This name (with its extension, but without path) is used in the Path field of Chunks in ASSETMAP file. */
    int            cpl_count;
    t_cpl          cpl[MAX_CPL];
} t_pkl;

/* t_xml_signature */
typedef struct {
    int  	sign;
    int  	use_external;
    char *	root;
    char *	ca;
    char *	signer;
    char *	private_key;
} t_xml_signature;

/* t_assetmap */
/** 
 * @brief This goes directly into t_dcp structure 
 * at the end of the process and
 * has no earlier purpose. 
 */
typedef struct {
    char           filename[NAME_STR_LEN];
} t_assetmap;

/* t_volindex */
/** 
 * @brief This goes directly into t_dcp structure 
 * at the end of the process and
 * has no earlier purpose. */
typedef struct {
    char           filename[NAME_STR_LEN];
} t_volindex;

/* t_dcp */
/** 
 * t_dcp 
 * This (t_project dependant) structure is finally 
 * filled at the moment the project has been fully 
 * rendered in order to achieve the whole process 
 * by writing all the XML files. 
 * @todo It could be completely merged into t_project
 * the question(s) being whether we'd thus lose or gain clarity
 * and if passing a bloat t_project to many little functions
 * is an acceptable memory policy.
 */
typedef struct {
    char           	basename[40];		/**< @todo this could be replaced by t_project var */ 
    char           	issuer[80];			/**< @todo this could be replaced by t_project var */
    char           	creator[80];		/**< @todo this could be replaced by t_project var */
    char           	issue_date[30];		/**< @todo this could be replaced by t_project var */
    char           	annotation[128];
    char           	title[80];			/**< @todo this could be replaced by t_project var */
    char           	kind[15];			/**< @todo this could be replaced by t_project var */
    char           	rating[6];
    char           	aspect_ratio[20];	/**< @todo this could be replaced by t_project var */
    int            	digest_flag;
    int            	pkl_count;
    t_pkl          	pkl[MAX_PKL];
    t_assetmap     	assetmap;
    t_volindex     	volindex;
    t_qdcp_cb   	sha1_update;
    t_qdcp_cb		sha1_done;
} t_dcp;




/*---------------------- QuickDCP  structures ------------------------*/

/*------------------------- Config & Context -------------------------*/

/** Config defaults (projects part) */
typedef struct{
			/* config values proper */			
			float			laps;					/**< Project's time length in seconds (and fractions) */
			char 		*	format;					/**< Flat, Scope and so on. */
			char 		* 	resolution;				/**< 2K or 4K or ... */
			int			 	framerate;		
			char 			background[18];			/**< The maximum (17 + \0) is #rrrrggggbbbbaaaa */
			char 			foreground[18];
			int				margin;					/**< Could be deprecated : margin in pixels between resized images and screen border */
			double			color_matrix[9];		/**< This contains the QuickDCP default color matrix for RGB->XYZ colorspaces translations. A square of 3x3 values between 0 and 1. The matrix can be modified on a per project basis. */

			/* dcp name */
			int			 	category;
			char 		*	issuer;
			char 		*	issuer_short;
			char		*	country_code;
			char		*	lang_code;
			char		*	subtitle_code;
			char		*	laboratory;
			char		*	dcp_type;
			char		*	version;
			qdcp_bool		name_category;
			qdcp_bool		name_3D;
			qdcp_bool		name_format;
			qdcp_bool		name_langs;
			qdcp_bool		name_country;
			qdcp_bool		name_audio;
			qdcp_bool		name_resol;
			qdcp_bool		name_issuer;
			qdcp_bool		name_date;
			qdcp_bool		name_lab;
			qdcp_bool		name_version;
			qdcp_bool		name_dcptype;

			/* fondus */
			int				fondu_d;
			int				fondu_f;
			float			fondulaps_d;
			float			fondulaps_f;
			char	 		fondu_cache_color_d[18];
			char 			fondu_cache_color_f[18];		
			float 			fondu_iris_cx_d;
			float 			fondu_iris_cy_d;
			float 			fondu_iris_cx_f;
			float 			fondu_iris_cy_f;

			/* vectorial shapes */
	const	char		*	shapes[5];
			int				line_width;
			char 			line_color[18];
			char 			fill_color[18];
	const	char	 	*	line_style;		
	const	char		*	line_caps;
	const	char		*	line_join;
			char		*	dash_arrays;

			/* text (pango) */
			char 		*	font_desc;
			int				font_size;
			char 			text_color[18];
	const	char 		*	text_style;
	const	char 		*	text_variant;
	const	char 		*	text_align;
	const	char 		*	text_weight;
	const	char 		*	text_stretch;
			int				interline;
	const	char		*	text_underline;
			qdcp_bool		text_strike;
	const	char 		*	text_script;
	const	char 		*	text_lang;

			/* audio */
			int				mix_type;
			int				audio_format;
			int				nchannels;		/**< number of channels (should be inherited from project) */
			int				channel_layout; /**< Channel layout as defined in FFmpeg's AV_CHANNEL_LAYOUT, such as AV_5POINT1 */
			int				sample_fmt;		/**< Sample format as defined in FFmpeg's AVSampleFormat enumeration, such as AV_SAMPLE_FMT_S32 */
			int				bitdepth;		/**< precision, bit/sample, bitdepth = 24 */
			float			volume;
			int				codec_id;		/**< output AVCodecID as defined in FFmpeg. Should be AV_CODEC_ID_PCM_S24DAUD. */
			int				samplerate;		/**< samples per second (48 or 96). Like kiloHertz. See enum accepted_samplerates. */
			int				bitrate;
			int				samples;
			int				encoding;

			/* video */
			qdcp_bool		vid2flat;
	const	char		*	vid_framerate;
	const	char		*	vid_format;	
} t_cfg;

/*------------------------- Project members --------------------------*/

/* t_sub_track */
/** 
 * Subtitles tracks
 */
typedef struct t_sub_track_struct t_sub_track;
struct t_sub_track_struct {
	
			/* -- track's ID -- */
			int 			s_id;
			t_sub_track	*	prev;				/** < previous track in the t_track_list */
			t_sub_track	*	next;				/** < next track in the t_track_list */
			int				creation;			/** < creation time (from 1970) for potential calculations */
			int				status;				/** < mixed, unmixed, locked (to a video layer) or whatever */
			int				index;				/** < in CLI, this allows to identify and choose a track */

			/* -- track's values -- */
	const 	char  		*	name;				/** < original file basename */
	const 	char		*	original;			/** < original file name */
	const 	char  		*	ext;				/** < original file extension */
			int				entry_point;		/** < entry point in project timeline (frame) */
			int 			duration;			/** < duration expressed in project's frames */
			int				time_length;		/** < time length expressed in samples */
			char		*	time_HHMMSS ;		/** < time length as a HHMMSS string */

			/* -- subtitle info -- */
			int				subtitle_stream;	/** < stream index from FFmpeg */
			// format ?
			// namespace ?
			// font ?
			// size ?
			
			// relative_rate ?	/** < that would be some coef for speed relatively to the actual framerate */
			
			/* -- interface params --*/
			int				diamond;		/** < Position of the reading head in GUI */
			qdcp_bool		hide;
    
};
/* t_subtitles_list */
typedef struct {
	int				size;
	char		**	filenames;
	t_sub_track	*	first;
	t_sub_track	*	last;
} t_subtitles_list;


/* t_wav_track */
/** 
 * Wav tracks
 */
/* t_wav_track */
typedef struct t_wav_track_struct t_wav_track;
struct t_wav_track_struct {
	
		/* -- track's ID -- */
		int 					w_id;
		t_wav_track			*	prev;						/** < previous track in the t_track_list */
		t_wav_track			*	next;						/** < next track in the t_track_list */
		int						creation;					/** < creation time (from 1970) for potential calculations */
		int						status;						/** < mixed, unmixed, locked (to a video layer) or whatever */
		int						index;						/** < in CLI, this allows to identify and choose a track */

		/* -- track's values -- */
		int						entry_point;				/**< entry point in project timeline (frame) */
		int 					duration;					/**< duration expressed in project's frames */
		char					wav_path[PATH_STR_LEN]; 	/**< location of temporary audio file(s) */
		int						lock_to_layer;				/**< this attaches a wav track to a layer's id, most likely a video layer */

		/* -- source info -- */
		char					filename[NAME_STR_LEN];
		char  					basename[NAME_STR_LEN];		/**< original file basename (no path, no extension) */
		char  					ext[SHORT_STR_LEN];			/**< original file extension */
		float					time_length;				/**< time length expressed in (seconds * 1000000) */
		char					time_HHMMSS[10];			/**< time length as a HHMMSS string (room is left for a point and decimals). Based on number of samples. */
		char				*	annotations;				/**< Contains a brief description (imported from the original audio file, I guess) */

		char					original[PATH_STR_LEN];		/**< original file name. The original file is supposed to have been copied into workspace. */
		int						src_encoding;				/**< source audio encoding (codec ID ?) */
		int						src_channels;				/**< Number of channels of the output audio. Can be one of CH_MONO, CH_STEREO, CH_5_1, CH_7_1 (see enum channel_set) */
		int						src_channel_layout;			/**< Reference to channel layout, from FFmpeg enumeration. */
		int						src_samples;				/**< total number of samples */
		int						src_samplerate;				/**< The original samplerate in kHz */
		int						src_sample_fmt;				/**< From FFmpeg enum AVSampleFormat */
		int						src_bitrate;
		int						src_bitdepth;				/**< The orignal source's bit per sample depth. */
		int						src_framesize;
		AVFormatContext 	*	src_format_ctx;				/**< A pointer to a file format handle from FFmpeg */
		int						src_stream_index;			/**< stream index from FFmpeg */
		AVStream 			*	src_audio_stream;			/**< A pointer to an AVStream struct from FFmpeg */
		int						src_codec_id;				/**< Encoder FFmpeg Codec ID from AVCodecID enumeration, as defined in avcodec.h. Should be AV_CODEC_ID_PCM_S24DAUD */
		AVCodecContext 		*	src_codec_ctx;				/**< A pointer to an AVCodecContext struct from FFmpeg */
		AVCodecParameters 	* 	src_codec_params;			/**< A pointer to an ACodecParameters struct from FFmpeg */
		AVCodec				*	src_codec;					/**< A pointer to an AVCodec struct from FFmpeg*/
		
		/* -- audio output settings -- */
		char					mixed_file[PATH_STR_LEN];	/**< Mixed file, in temporary wav directory */
		int						mix_type;					/**< One of Repartition, Mixage, Inversion */
		int						out_bitrate;
		int						out_channels;				/**< Number of channels of the output audio. Can be one of CH_MONO, CH_STEREO, CH_5_1, CH_7_1 (see enum channel_set) */
		int						out_channel_layout;			/**< Reference to channel layout, from FFmpeg enumeration. */
		int						out_samples;				/**< total number of samples */
		int						out_samplerate;				/**< Targeted rate in samples per second (48000 or 96000/s). See enum accepted_samplerates. By default, will be KHZ_48. */
		int						out_sample_fmt;				/**< From FFmpeg enum AVSampleFormat */
		int						out_bitdepth;				/**< precision, bit/sample, bitdepth = 24 */
		int						out_framesize;
		AVFormatContext 	*	out_format_ctx;
		int						out_stream_index;			/**< stream index from FFmpeg */
		AVStream 			*	out_audio_stream;			/**< A pointer to an AVStream struct from FFmpeg */
		int						out_codec_id;				/**< Encoder FFmpeg Codec ID from AVCodecID enumeration, as defined in avcodec.h. Should be AV_CODEC_ID_PCM_S24DAUD */
		AVCodecContext 		*	out_codec_ctx;				/**< A pointer to an AVCodecContext struct from FFmpeg */
		AVCodecParameters 	* 	out_codec_params;			/**< A pointer to an ACodecParameters struct from FFmpeg */
		AVCodec				*	out_codec;					/**< A pointer to an AVCodec struct from FFmpeg*/
		
		int						encoding;					/**< current audio encoding */
		float					volume;						/**< @todo to be clarified... */

		/* -- interface params --*/
		int						diamond;					/**< Position of the reading head in GUI */
		qdcp_bool				mute;						/**< mute/unmute the track (in GUI) */
};
/* t_track_list */
typedef struct {
	int				size;
	char		**	filenames;
	t_wav_track	*	first;
	t_wav_track	*	last;
} t_track_list;


/** 
 * Keyframes
 * 
 * t_keyframe 
 */
typedef struct t_keyframe_struct t_keyframe;
struct t_keyframe_struct
{
	t_keyframe	*	prev;				/**< Previous keyframe in the keyframe list */
	t_keyframe	*	next;				/**< Next keyframe in the keyframe list */
		int			photogram;			/**< Position of this keyframe in the sequence (n) */
		int 		ox;					/**< Horizontal position in pixels of the center of the object */
		int 		oy;					/**< Vertical position in pixels of the center of the object */				
		float 		oxp;				/**< As above, in percent */
		float 		oyp;				/**< As above, in percent */
		int 		x;					/**< Horizontal position in pixels of the left of the object */
		int 		y;					/**< Vertical position in pixels of the top of the object */
		int 		w;					/**< Total width in pixels, of the object */
		int 		h;					/**< Total height in pixels, of the object */
		float 		wp;					/**< As above in percent */
		float 		hp;					/**< As above in percent */
		int 		centerX;			/**< Horizontal position (in pixels) of the gravity center of the object (from left of object) */
		int 		centerY;			/**< Vertical position (in pixels) of the gravity center of the object, (from top of object) */
		float 		ratio; 				/**< Width / height */
		float 		rotrad;				/**< Rotation factor, in radians */
		float 		rotdeg;				/**< Rotation factor, in degrees */
		int			opacity;			/**< Alpha level of the object (100 = normal, 0 = transparent) */
		char	*	bg_color;			/**< Background color string (like ff00aa) */
		char	*	fg_color;			/**< Foreground color string (like 22cc99) */

		/* internals */
		int			modify;				/**< True if other values must be computed */
		int			rotate;				/**< True if rotation must be computed */
		int			resize;				/**< True if size must be (re)computed */
		int			transparent;		/**< True if opacity must be computed */
		int			color_grades;		/**< True if colors must be computed in a gradient */
};
/* t_kf_list */
typedef struct t_kf_list_struct
{
	int 		 size;
	t_keyframe * first;
	t_keyframe * last;
	
} t_kf_list;

/*------------------------------ t_layer -----------------------------*/
/** Layers 
 */
typedef struct t_layer_struct t_layer;
struct t_layer_struct
{
			int 				l_id;						/**< This layer's unique id, a hashed creation epoch time */
			t_layer			*	prev;						/**< Previous layer in layer's list */
			t_layer 		*	next;						/**< Next layer in layer's list */
			char 				layername[NAME_STR_LEN];	/**< Name of this layer */
			layer_type			type;						/**< Type of this layer (IMAGE, VIDEO, etc.) */
			int 				pos;						/**< Position (from bottom to top) of this layer in its pile */
			void			*	parent;						/**< The project to which this layer belongs */
			t_kf_list 		*	keyframes;					/**< This layer's keyframe list */
			
			/*2 booleans*/
			int 				visible;					/**< False when the layer is masked */
			int 				keep_ratio;					/**< Keep proportions on resizing width or height */
		   
			int 				entry_point;				/**< First photogram where this layer is treated */
			int 				exit_point;					/**< Last photogram where this layer is treated */
			int 				fade_in;					/**< Length in photograms of an entry fading */
			int 				fade_out;					/**< Length in photograms of an exit fading */
			
			char 			*	bg_color;					/**< Background color, specific to this layer */
			char 			*	fg_color;					/**< Foreground color of this layer (border, text, vectorial drawing) */
			int 				margin;						/**< (deprecated) A margin to apply to this layer */
			
			char				source_file[PATH_STR_LEN];	/**< This layer's source full path : image, video, wav or text */
			char				filename[NAME_STR_LEN];		/**< Image file name without path */
			char				basename[NAME_STR_LEN];		/**< Image file base name without path nor extension */
			char				ext[SHORT_STR_LEN];			/**< Image file extension */
			char				source_path[PATH_STR_LEN];	/**< Absolute source path without file name, but with a trailing slash */
    
			/*image values*/
			
			MagickWand		*	wand;						/**< A magick wand object for this image layer */
			ColorspaceType		source_colorspace;			/**< Colorspace of the source image (or video?) as determined by MagickWand */
			//~ char		*	source_colorspace;		/**< Colorspace of the source image (or video?) as determined by MagickWand */
	
			/*pango values*/
					
			char 			* 	text;						/**< Text to display on that layer */
			char		 	*	font_desc;				
			int 				interline;
			char		 		text_color[18];
			char 			*	text_align;
			char 			*	text_stretch;
			char 			*	text_style;
			char 			*	text_variant;
			char 			*	text_weight;
			char			*	text_underline;
			char 			*	text_lang;	

			/* vectorial values*/

			int 				shape;
			int 				line_width;
			char 			*	line_style;
			char			*	line_join;
			char			*	line_caps;
			/*2 booleans*/
			int 			 	background_on;				/**< Apply a layer background of bg_color */
			int 				border_on;					/**< Apply a layer border of fg_color */
			
			/* video values */
			
			AVFormatContext	*	src_format_ctx;
			int					stream_index;				/**< stream index from FFmpeg */
			AVStream		*	video_stream;
			AVCodecParameters*	src_codec_params;
			AVCodecContext	*	src_codec_ctx;
			int					src_codec_id;
			AVCodec			*	src_codec;
			float				time_length;
			int					src_w;
			int					src_h;
			float				src_ratio;
			AVRational			frame_rate;
			AVFrame			*	out_frame;
			AVFormatContext	*	out_format_ctx;
			int					out_codec_id;
			AVCodec			*	out_codec;
			AVCodecContext	*	out_codec_ctx;
			AVCodecParameters*	out_codec_params;
};

/* t_layer_list */

typedef struct t_layer_list_struct t_layer_list;
struct t_layer_list_struct
{
	int			size;		/**< Number of layers in this list */
	t_layer	*	first;		/**< Firt layer of this list */
	t_layer	*	last;		/**< Last layer of this list */
};

/*---------------------------- t_project -----------------------------*/
/** 
 * Projects
 */
/* t_project */
typedef struct t_project_struct t_project;
struct t_project_struct
{
			/* -- project's internals -- */
			int 			p_id;						/**< id number of the project (hashed epoch time and index) */
			t_project	 *	prev;						/**< A pointer to the previous project in the t_project_list object. */
			t_project 	 *	next;						/**< A pointer to the next project in the t_project_list object. */
			time_t		 	creation;					/**< The project creation time (seconds from the Epoch). */
			time_t		 	last_mod;					/**< The last project modification time (seconds from the Epoch). */
			int				starttime;					/**< At runtime, beginning of processing the project */
			int				endtime;					/**< At runtime, end of processing the project */
			t_layer_list *	layers;						/**< List of project's image layers */
			t_track_list *	tracks;						/**< List of project's sound (wav) tracks */
			t_track_list *	channels;					/**< List of audio channels (deprecated ?) */
			
			char 			name[NAME_STR_LEN];			/**< name of the project (used to name directories) */
			int				status;						/**< A bitwise number describing the current state of the project */
			char			dcp_name[NAME_STR_LEN];		/**< The official DCP name of the project as it is currently */
			char			issuer[WORD_STR_LEN];		/**< The author/producer of the project */
			char			issuer_short[SHORT_STR_LEN];/**< Abbreviation of the author/producer of the project, as shown in the DCP name. */
			int				category;					/**< Category of the content, such as advertisement, public announcement, feature, etc. */
			
			/* -- project's directories -- */
			char			img_path[PATH_STR_LEN];		/**< Absolute path of the temporary images produced during project's generation. With a trailing slash */
			char			seq_path[PATH_STR_LEN];		/**< Absolute path of the preview sequence in PNG, for this project, with a trailing slash. */
			char			j2k_path[PATH_STR_LEN];		/**< Absolute path of the generated jpeg2000 sequence for this project, with a trailing slash. */
			char			j2k_3D_right[PATH_STR_LEN]; /**< Absolute path of the generated jpeg2000 sequence for the right eye, when going stereoscopic. The above being the left eye. With a trailing slash. */
			char			wav_path[PATH_STR_LEN];		
			char			sub_path[PATH_STR_LEN];
			char			tmp_dcp_dir[PATH_STR_LEN];	/**< Absolute path of the generated MXF and XML files for this project, with a trailing slash. */
			char			final_dcp_dir[PATH_STR_LEN];/**< Absolute path of the complete DCP, at the end of the process. */
						
			/* -- project values -- */
			int 			mx_w;						/**< Target format width in pixels */
			int 			mx_h;						/**< Target format height in pixels */
			long 			nmax;						/**< Total number of frames of the project. */
			float 			zoom_factor;				/**< For GUI, controls the on screen zoom level of the project's preview. */
			int 			current_frame;				/**< For GUI, current frame of the sequence seen in the preview screen. */
			t_layer		*	active_layer;				/**< For GUI, the layer we are currently working on. */
			qdcp_bool		unsaved;					/**< For GUI, true when changes have been made since last time the project was saved. */
			
			float 			laps;						/**< Time length of the project in seconds. The float type allows a frame level precision. */
			char 		*	format;						/**< Format string such as 'Flat', 'Scope', 'Full', etc. */
	const	char 		*	resolution;					/**< Case sensitive string such as '2K' or '4K'. */
			int 			framerate;					/**< Number of frames per second. @todo Implement non-integer framerates (23.98) with a divisor. */
			qdcp_bool		stereoscopic;				/**< True (=1) if we want 3D output. */
			double			color_matrix[9];			/**< @todo This contains the project's proper color matrix for RGB->XYZ colorspaces translations. A square of 3x3 values between 0 and 1. */
			
			char 		*	background;
			
			int				fondu_d;
			float 			fondulaps_d;
	const	char 		*	fondu_cache_color_d;
			float 			fondu_iris_cx_d;
			float 			fondu_iris_cy_d;
	
			int				fondu_f;
			float 			fondulaps_f;
	const	char 		*	fondu_cache_color_f;
			float 			fondu_iris_cx_f;
			float 			fondu_iris_cy_f;
			
			/* dcp name */
	const	char		*	lang_code;
	const	char		*	subtitle_code;
	const	char		*	country_code;
			char		*	laboratory;
			qdcp_bool		name_category;
			qdcp_bool		name_3D;
			qdcp_bool		name_format;
			qdcp_bool		name_langs;
			qdcp_bool		name_country;
			qdcp_bool		name_audio;
			qdcp_bool		name_resol;
			qdcp_bool		name_issuer;
			qdcp_bool		name_date;
			qdcp_bool		name_lab;
	
			/* -- audio section -- */
			int				mix_type;
			int				audio_format;
			int				nchannels;					/**< number of channels (should be inherited from project) */
			int				bitdepth;					/**< precision, bit/sample, bitdepth = 24 */
			float			volume;						/**< @todo To clarify. A volume level for the whole project. */
			float			samplerate;					/**< samples per second (48000 or 96000/s) */
			int				samples;					/**< Total number of samples for this project's full length. */
			int				encoding;					/**< @todo Clarify : current|original encoding ? */
			//char		*	annotations; (should go to each track of track_list)
			char			main_wav[PATH_STR_LEN];		/**< One main unified audio track, mixing (as in "flattening") all tracks of the project */

			/* transposed or adapted from opendcp: */
						
			int             ns;							/**< ns = xml namespace */
			t_filenames	*	j2k_files;					/**< Full list of j2k photograms by file name */
			t_filenames *	j2k_righteye_files;			/**< File list of right eye images in 3D*/
			t_filenames	*	wav_files;
			t_filenames	*	subs_files;
			t_file_list *	dcp_files;
			
			remote_t        remote;
			t_mxf           mxf;
			t_dcp           dcp;
			t_xml_signature xml_signature;
};

/* t_project_list */
typedef struct t_project_list_struct
{
	int				size;
	t_project	*	first;
	t_project	*	last;
	
} t_project_list;
 

/** Quickdcp context */
typedef struct {
			/* application settings */
			int 			verbosity;
			qdcp_bool		preview;
			qdcp_bool		make_dcp;
			qdcp_bool		signature;
			qdcp_bool		beep;
			int				threads;
			char		*	autoclean;
			char		* 	img2j2k;
			char		*	j2k2mxf;
			int				loglevel;
			int				opendcp_loglevel;
			int				ffmpeg_loglevel;
			char 		*	lang;

			/* GUI interface */
			int				show_toolbar;
			int				show_statusbar;
			int				show_terminal;
			int				win_x;
			int				win_y;
			int				win_w;
			int				win_h;
			
			/* directories */
			char		*	workspace;
			char		*	dcp_out;
			char 		*	archives;
			char			proj_dir[PATH_STR_LEN];	
			char			img_dir[PATH_STR_LEN];
			char			vid_dir[PATH_STR_LEN];	/**< The default directory (with a trailing slash) receiving extracted video frames. */
			char			seq_dir[PATH_STR_LEN];	/**< The default directory (with a trailing slash) receiving PNG frames of preview sequences. */
			char			tif_dir[PATH_STR_LEN];	
			char			j2k_dir[PATH_STR_LEN];	/**< The default directory (with a trailing slash) receiving jpeg2000 frames before they are packed into an MXF file. */
			char			wav_dir[PATH_STR_LEN];	/**< The default directory (with a trailing slash) receiving mixed and unmixed WAV files before they are mixed or packed into an MXF file. */
			char			sub_dir[PATH_STR_LEN];
			char			mxf_dir[PATH_STR_LEN];	/**< The default directory (with a trailing slash) receiving the MXF and XML files. */
			char			log_dir[PATH_STR_LEN];
			char			cfg_dir[PATH_STR_LEN];
			char			cfg_file[PATH_STR_LEN];
			
			/* projects default values */
			t_cfg 		*	cfg;
			
			/* project and sub-projects lists */
			
		t_project_list 	*	proj_list;
		t_track_list	*	wav_list;
		t_subtitles_list*	subs_list;
		t_project		*	active_project;			/**< For GUI, the project we are currently working on, whose settings are displayed in the compositor tab. */
			
			/* some lists of strings (defined in src/quickdcp.c:quickdcp_init()) */
	const	char		*	yes_no[2];	
	const	char		*	on_off[2];
	const	char		*	pj_status[9];
	const	char		*	formats[5];
	const	char		*	resolutions[3];
	const	char		*	framerates[9];
	const	char		*	fondus[3];
	const	char		*	shapes[5];
	const 	char		*	mixs[3];
	const	char		*	aud_formats[4];
	const	char		*	dcp_categories[9];
	const 	char		*	short_cats[9];
			double			color_matrix[9];		/**< This contains the QuickDCP default color matrix for RGB->XYZ colorspaces translations. A square of 3x3 values between 0 and 1. The matrix can be modified on a per project basis. */

} quickdcp_ctx;



/* -----------------------------------------------------------------*
 * 						Common QuickDCP functions 					*
 * ---------------------------------------------------------------- */


/*----------------------- Logs and errors --------------------------*/

/**
 * @brief Convert an error code returned by an FFmpeg function
 * into a text message.
 * 
 * @param error Error code to be converted
 * @return Corresponding error text (not thread-safe)
 */
const char * get_ffmpeg_error(const int error);


int 
qdcp_callback_null(
	void *args
	);

/*
void 
qdcp_log(
	int level, 
	const char *file, 
	const char *function, 
	int line,  
	const char *fmt, 
	...
	);

void
qdcp_log_init(
	int level, 
	quickdcp_ctx * qdcp
	);

void 
qdcp_log_print_message(
	void *arg, 
	const char *msg
	);

void 
qdcp_log_subscribe(
	qdcp_log_cb_t *cb
	);

char *	
qdcp_log_timestamp();
*/

void
dcp_set_log_level(int log_level);

void  		
dcp_fatal(t_project * pj, char *error, ...);


void 	
warning_msg(
	char * location, 
	char * message
	);

void 	
error_msg(
	char * message, 
	char * file, 
	int line
	);

/*---------------- Stuff needed by quickdcp_init ---------------------*/

/**
 * @brief This sets some default values from the config file, 
 * after creating the latter whenever it's missing.
 * The config functions find themselves in qdcp_config.h
 */
/**
 * Init config struct with default projects values
 */
qdcp_bool 
init_default_config(
	quickdcp_ctx * qdcp
	);

/** 
 * @brief 	Allocate a new empty project list in the quickdcp_ctx struct.
 * The project related functions find themselves in qdcp_project.h
 * 
 * @return	QDCP_SUCCESS in case of success.
 */
qdcp_bool
init_project_list(quickdcp_ctx * qdcp);


/** 
 * @brief Allocate a new empty audio sub-projects list.
 * The audio related functions find themselves in qdcp_wav.h
 * 
 * @return	QDCP_SUCCESS in case of success.
 */
qdcp_bool
init_track_list(
	t_track_list * wl
	);

/**
 * @brief I'm not sure if this is really useful.
 * Just in case.
 * I mean who needs subtitles lists ?
 * (maybe for some internationalization of a project...)
 *
 * @return	QDCP_SUCCESS in case of success.
 */
qdcp_bool 	
init_subtitles_list(
	t_subtitles_list * stl
	);

/**
 * @brief Before using FFmpeg API, we need to register all available
 * formats and codecs.
 */
void
init_ffmpeg_api(void);


/*---------------------- Some CLI eye-candy ------------------------*/

void 	
star_line();

void *  	
spinner(void * signal);

/**
 * A cross-platform sleep function
 * @param	milliseconds The argument is an int value representing milliseconds.
 * @return 	NONE
 */
void 	
sleep_ms(
	int milliseconds
	);

void 	
qdcp_progress_bar(
	const char * work, 
	int val, 
	int total, 
	int task
	);

/** Check the existence of a directory and create it if need be */
qdcp_bool 	
check_dir(const char * dir_path, int verbosity);

/** Check the existence of a file */
qdcp_bool 	
check_file(const char * file_path);


/*----------------- Manipulation of file names ---------------------*/

/**
 * Get a file's path 
 */
char *
get_path_from_file(
	char * filename
	);
	
/**
 * Get a file name without any absolute or relative path
 */
char * 
strip_path(
	char const * filename
	);

/** 
 * Get a file's basename 
 */
char *	
get_basename(
	char const * filename
	);

/** 
 * Get a file's extension 
 */
char *	
get_filename_ext(
	char * filename
	);


const char * 	
get_fname(const char * filename);

qdcp_bool 	
is_filename_ascii(const char * s);

void 	
strip_string(char * s);


/*----------------------- Time and strings -------------------------*/

/** 
 * Get a file mod time in seconds from the epoch (of time_t type) 
 */
time_t
get_mod_time(
	const char * absolute_filename
	);

/** 
 * Gives a string back from a time_t value (mostly for CLI) 
 */
char *
get_last_mod_string(
	time_t  epoch
	);

/** 
 * Gives a time_t * from a string value in seconds since the epoch 
 */
qdcp_bool
get_time_from_string(
	time_t  target_time, 
	char * epoch_string
	);

/** 
 * @brief	Generate a timestamp string.
 * 
 * This function will generate a timestamp string for DCP's XML files
 * based on the current local time.
 * Timestamp has the form : 2015-03-03T09:42:49+00:00
 * 
 * @param  	timestamp A pointer to a char buffer that will hold the timestamp string.
 * @return 	nothing
 */
void  		
get_timestamp(
	char * timestamp
	);


/*------------------ Manipulation of color strings -----------------*/

char * 	
get_color(const char * raw_color);

RGB_color		
color_hex2rgb(const char * raw_color);

RGBA_color 		
color_hex2rgba(const char * raw_color);

char *	
color_rgb2hex(RGB_color rgb_color);


/* -----------------------------------------------------------------*
 * 					Specific QuickDCP functions 					*
 * ---------------------------------------------------------------- */


/** 
 * Open and populate a QuickDCP context.
 * Including useful arrays of string values
 *
 * @return 	A pointer to a new quickdcp_ctx structure allocated and populated 
 */
quickdcp_ctx * 	
quickdcp_init();

/** 
 * Close and free a QuickDCP context 
 * at the end of a QuickDCP session.
 * 
 * @param 	qdcp A pointer to the quickdcp_ctx structure to be freed
 * @return  A boolean : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool
quickdcp_close(
	quickdcp_ctx * qdcp
	);


/*-------------- Determine content type of a file ------------------*/

qdcp_bool 	
file_is_image(file_obj * file, qdcp_bool double_check);

qdcp_bool 	
file_is_audio(file_obj * file, qdcp_bool double_check);

qdcp_bool 	
file_is_video(file_obj * file, qdcp_bool double_check);

qdcp_bool 	
file_is_subtitle(file_obj * file, qdcp_bool double_check);

qdcp_bool 	
file_is_mxf(file_obj * file, qdcp_bool double_check);



/*--------------- Manipulation of file lists -----------------------*/

qdcp_bool
import_source_file(
	char * full_path,
	char * input_file, 
	quickdcp_ctx * qdcp
	);

qdcp_bool
import_media_file(
	t_layer * ly, 
	char * input_file, 
	quickdcp_ctx * qdcp
	);

qdcp_bool 
scan_source_directory(char * input_path, int depth, char * mime_filter, t_file_list * src_list);

t_file_list * 	
new_file_list();

file_obj  	* 	
new_file_object();

qdcp_bool
add_file_to_list(file_obj * file, t_file_list * src_list);

void
show_file_list(t_file_list * src_list);


char * 
get_dcp_name_from_project(t_project * pj, quickdcp_ctx * qdcp);


/* utility functions */
t_filenames *
filelist_alloc(
	int nfiles
	);

t_filenames *
get_filelist(
	const char *path, 
	const char *filter
	);

/** 
 * @brief	Free a t_filenames structure
 * 
 * This function frees memory used by filelist_t.
 * 
 * @param 	filelist a pointer to a filelist
 * @return 	nothing
 */
void        
filelist_free(
	t_filenames * filelist
	);

/** 
 * @brief	Check the extension of a file against string filter 
 */
int
file_selector(
	const char *filename, 
	const char *filter
	);

/** 
 * @brief	Ensure a list of ordered filenames are sequential
 * 
 * @param  	files is an array of file names
 * @param  	nfiles is the number of files
 * @return 	returns 0 if files are sequential, otherwise it returns the index
 *         	the first out of order file.
 */
int
ensure_sequential(
	char *files[], 
	int nfiles
	);

/** 
 * @brief Order a list of filenames using a version type sort.
 * 
 * This function will order a list of filenames of the form:
 * 
 *   \<index\>N* or N*\<index\>
 * 
 * where:
 *   \<index\> is the longest (though possibly empty) common prefix/suffix of all the
 *   files.
 *   N is some decimal number which I call its "index".
 *   N is unique for each file.
 *   1 <= N <= nfiles  OR  0 <= N < nfiles
 * 
 * @param  files is an array of file names
 * @param  nfiles is the number of files
 * @return QDCP_ERROR_CODE
 */
int         
order_indexed_files(
	char *files[], 
	int nfiles
	);


void 		common_prefix(const char *s1, const char *s2, char *prefix);
int 		file_cmp(const void *a, const void *b);

void        strnchrdel(const char *src, char *dst, int dst_len, char d);
int         strcasefind(const char *s, const char *find);
int         hex2bin(const char* str, byte_t* buf, unsigned int buf_len);

void 
prefix_of_all(char *files[], int nfiles, char *prefix);

int 
get_index(char *file, int prefix_len);


//~ /* image functions */
//~ int check_image_compliance(int profile, opendcp_image_t *image, char *file);

//~ /* J2K functions */
//~ int convert_to_j2k(opendcp_t *opendcp, char *in_file, char *out_file);



/*------------------------------------------------------------------*
 * 						ASDCPLIB functions 							*
 *------------------------------------------------------------------*/


/* MXF functions */


/**
 * @brief Write out a jpeg2000 MXF file. 
 */
qdcp_bool 
convert_j2k_to_mxf(t_project * pj, char *output_file);

/**
 * @brief Write out a 3D jpeg2000 MXF file. 
 */
qdcp_bool 
convert_j2k3D_to_mxf(t_project * pj, char *output_file);

/**
 * @brief Write out a PCM audio MXF file.
 */
qdcp_bool 
convert_pcm_to_mxf(t_project * pj, char *output_file);

/**
 * @brief Write out a timed text MXF file. 
 */
qdcp_bool 
convert_timedtext_to_mxf(t_project * pj, char *output_file);


/* get wav duration */
int
get_wav_duration(
	const char *filename, 
	int frame_rate
	);

/* get wav file info  */
int
get_wav_info(
	const char *filename, 
	int frame_rate, 
	t_wav_track *wav
	);


/*----------------------- Reels and assets -------------------------*/

/** 
 * @brief Determine a file's "essence".
 * 
 * This function will return the "essence" type of a file to become an asset.
 * The return value can be one of AET_UNKNOWN, AET_MPEG2_VES, AET_JPEG_2000, 
 * AET_PCM_24b_48k, AET_PCM_24b_96k, AET_TIMED_TEXT or AET_JPEG_2000_S
 * 
 * @param   filename	The full (absolute) path string of the file to determine.
 * @param	raw			Whether
 * @return 	int			An integer of ASSET_ESSENCE_TYPE
 */
int   		
get_file_essence_class(
	char * filename, 
	int raw
	);

/** 
 * @brief Allocate a new empty t_asset structure.
 * 
 * This function only declares and allocates a new t_asset structure.
 * 
 * @param	asset		A pointer to the t_asset structure to allocate.
 * @return int			A qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool
init_asset(
	t_asset *asset
	);

/** 
 * @brief Populating an empty t_asset structure.
 * 
 * This function will fill the fields of the given asset structure
 * 
 * @param	pj			A pointer to the current project.
 * @param	asset		A pointer to a freshly declared t_asset structure.
 * @param  	filename	A pointer to the full (absolute) path string of an MXF file.
 * @param	verbosity 	An integer value (of verbosity_level type) to control output on console.
 * @return int			A qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool   		
new_asset(
	t_project * pj, 
	t_asset * asset, 
	char * filename,
	int verbosity
	);

/** 
 * @brief Determine an asset's type.
 * 
 * This function will return the class type of an asset essence.
 * Which can be one of :
 * ACT_UNKNOWN, ACT_PICTURE, ACT_SOUND or ACT_TIMED_TEXT
 * 
 * @param  asset	Any existing asset.
 * @return int		An integer of ASSET_CLASS_TYPE
 */
int
get_asset_type(
	t_asset asset
	);

qdcp_bool 
is_valid_asset(
	t_asset asset
	);
	
/** 
 * @brief Read asset file information. 
 *
 * @see asdcp_intf.cpp
 * @param	asset A pointer to an asset structure.
 * @return  A qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool 
read_asset_info(
t_asset *asset
	);

void
show_asset_params(
	t_asset * asset
	);
	
/** 
 * @brief Attach a configured asset to its mother reel. 
 *
 * @see qdcp_xml.c
 * @param	pj		A pointer to the current project.
 * @param	reel	A pointer to the adopting mother reel.
 * @param	asset 	A pointer to an asset structure.
 * @return  A qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool   		
add_asset_to_reel(
	t_project * pj, 
	t_reel * reel, 
	t_asset asset
	);

/** 
 * @brief Create an empty reel. 
 *
 * Which is basically a list of assets with an ID.
 * And an AnnotationText (here, derived from the DCP name).
 * @param	pj		A pointer to the t_project concerned.
 * @param	reel	A pointer to the new reel.
 * @return	nothing
 */
void
create_reel(
	t_project * pj, 
	t_reel * reel
	);

int   		
validate_reel(
	t_project * pj, 
	t_reel * reel, 
	int reel_number
	);


/*------------------------------------------------------------------*
 * 						  PKL CPL functions 						*
 *------------------------------------------------------------------*/

void
create_cpl(
	t_project * pj, 
	t_cpl * cpl, 
	quickdcp_ctx * qdcp
	);

void 
add_reel_to_cpl(
	t_cpl * cpl, 
	t_reel reel
	);


/** 
 * @brief Create a pkl and add information.
 * 
 * This function populates a pkl data structure with DCP information
 * from a dcp_t structure.
 * 
 * @see		qdcp_xml.c
 * @param  	pj 	 A pointer to the t_project structure to which the PKL belongs
 * @param  	pkl  A pointer to the t_pkl structure to populate
 * @param	qdcp A pointer to the quickdcp context where everything happens
 * @return 	nothing
 */
void
create_pkl(
	t_project * pj, 
	t_pkl * pkl, 
	quickdcp_ctx * qdcp
	);

/** 
 * @brief Add Content PlayList to Packing List
 * 
 * This function populates a cpl data structure with DCP information
 * from a dcp_t structure.
 * 
 * @see		qdcp_xml.c
 * @param  	pkl	A pointer to the t_pkl structure which receives the CPL.
 * @param  	cpl	The t_cpl structure to be attached to the PKL.
 * @return nothing
 */
void 
add_cpl_to_pkl(
	t_pkl * pkl, 
	t_cpl   cpl
	);


/** 
 * @brief Add Packing List to DCP
 * 
 * This function adds a pkl to a dcp_t structure
 * 
 * @see		qdcp_xml.c
 * @param  	dcp	A pointer to the target t_dcp structure.
 * @param  	pkl	The t_pkl structure to attach to the DCP.
 * @return 	nothing
 */
void
add_pkl_to_dcp(
	t_dcp * dcp, 
	t_pkl pkl
	);


/*------------------------------------------------------------------*
 * 							XML functions 							*
 *------------------------------------------------------------------*/

qdcp_bool
finalise_dcp(
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/* write the asset to an mxf file */
qdcp_bool 
write_cpl(
	t_project * pj, 
	t_cpl * cpl
	);

/**
 * @brief Write the asset tag in the asset list of a PKL .xml file.
 * 
 * This produces the contents related to an asset in the PKL file.
 * Namely, the following xml tags : \<Id\>, \<AnnotationText\>, \<Hash\>,
 * \<Size\>, \<Type\>, \<OriginalFileName\>. 
 */
int 
write_pkl_asset(t_project * pj, xmlTextWriterPtr xml, t_asset asset);

int 
write_cpl_asset(t_project * pj, xmlTextWriterPtr xml, t_asset asset);

qdcp_bool 
write_pkl(
	t_project * pj, 
	t_pkl * pkl
	);

qdcp_bool 
write_assetmap(
	t_project * pj
	);

qdcp_bool 
write_volumeindex(
	t_project * pj
	);

qdcp_bool 
xml_verify(
	char *filename
	);

qdcp_bool
xml_sign(
	t_project * pj, 
	char * filename
	);

/**
 * @brief Generate a random UUID with KUMU.
 *
 * @param uuid A pointer to a UUID string to be filled.
 * @return nothing
 */
void 
uuid_random(
	char *uuid
	);


int
is_uuid(
	const char *s
	);

int
is_key(
	const char *s
	);

int 
is_key_value_set(
	byte_t *key, 
	int len
	);


/*------------------------------------------------------------------*
 *						 Checksum functions 						*
 *------------------------------------------------------------------*/

/**
 * @brief Calculate the SHA1 digest of a file.
 *
 * @see qdcp_xml.c
 * @param pj		A pointer to the project to which the file belongs.
 * @param filename	A char pointer to the absolute path of the file to digest.
 * @param digest	A char pointer to be filled by the result of digest.
 * @return
 */
int 
calculate_digest(
	t_project * pj, 
	const char *filename, 
	char *digest
	);

/* djb2 from http://www.cse.yorku.ca/~oz/hash.html */
unsigned long
hash(
	unsigned char *str
	);

void 		
md5_init(
	md5_t *ctx
	);

void 		
md5_update(
	md5_t *ctx, 
	unsigned char const * buf, 
	unsigned len
	);

void 		
md5_final(
	unsigned char digest[16], 
	md5_t *ctx
	);

void 		
sha1_init(
	t_sha1 *context
	);

void 		
sha1_update(
	t_sha1 *context, 
	const unsigned char * data, 
	uint32_t len
	);

void 		
sha1_final(
	unsigned char digest[20], 
	t_sha1 * context
	);

void 		
base64_encode(
	const unsigned char *src, 
	int src_len, 
	char *dst
	);

/*------------------------------------------------------------------*
 *						 Some more stuff	 						*
 *------------------------------------------------------------------*/

/** 
 * copy a file somewhere 
 *
 * (copied from caf at http://stackoverflow.com/questions/2180079/how-can-i-copy-a-file-on-unix-using-c) 
 */

int 
cp(const char *to, const char *from);

/**
 * Convert an integer size in bytes into a human readable string like 2.3Mo or 4Go
 */ 
void
get_human_size(char * string, int size);



#ifdef __cplusplus
}
#endif

#endif // COMMON_H_INCLUDED
