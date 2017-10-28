/**
	@file qdcp_photograms.h
	@author jean.sabatier@zanobox.net
	@brief Calls to imagemagick (convert) for preliminary image treatment
*/

#ifndef QDCP_PHOTOGRAMS_H_INCLUDED
#define QDCP_PHOTOGRAMS_H_INCLUDED

#include "quickdcp.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <pthread.h>
#include <MagickWand/MagickWand.h>
#include "qdcp_projects.h"
//~ #include "odcp_xyz.h"


/**
 * Recognized image encodings
 */
typedef enum ImgEncoding
{
	NOT_IMG,	/**< This is not an image ! */
	BMP_IMG,	/**< bitmap */
	PNG_IMG,	/**< PNG */
	JPG_IMG,	/**< Jpeg */
	TIFF_IMG,	/**< TIFF */
	J2K_IMG,	/**< jpeg2000 */
	
	/* we include also those two for possible future implementation */
	GIF_IMG,	/**< GIFF (not implemented yet) */
	SVG_IMG		/**< SVG (not implemented yet) */
} ImgEncoding;

typedef struct ImgDims
{
	int w;		/**< Image width in pixels */
	int h;		/**< Image height in pixels */
} ImgDims;

typedef struct ImgCoords
{
	int x;		/**< Image left border horizontal position from left in pixels */
	int y;		/**< Image top border vertical position from the top in pixels */
} ImgCoords;

/**
 * @brief An ImgGeom object contain both dimensions and coordinates of an image on a screen 
 */
typedef struct ImgGeom
{
	ImgDims dims;		/**< An ImgDims struct describing the image dimensions */
	ImgCoords coords;	/**< An ImgCoords struct describing the image position */
} ImgGeom;

/**
 * @brief Get an image dimensions in pixels
 * @see quickdcp.c
 */
ImgDims 
get_img_dims(
	char * file_path
	);

qdcp_bool 	
apply_background(t_project * current_project, MagickWand * wand_photogram);

qdcp_bool 
apply_fondu(int n_photogram, MagickWand * wand_photogram, t_project * pj, quickdcp_ctx * qdcp, int debut);

qdcp_bool
apply_layer_fading(int n_photogram, int max_op, t_layer * ly);

qdcp_bool 
add_layer_to_photogram(MagickWand * wand_photogram, t_layer * cur_ly, t_keyframe * frame_values, quickdcp_ctx * qdcp);

qdcp_bool 
generate_photogram(int n, t_project * pj, char * preview_photogram, char * trgt_photogram, int render_mode, quickdcp_ctx * qdcp);

qdcp_bool 
write_j2k_photogram(MagickWand * wand, const char * target_file, quickdcp_ctx * qdcp);

/** 
 * Sequence generation
 */
qdcp_bool
generate_sequence(t_project * pj, quickdcp_ctx * qdcp, int render_mode);
		
#endif	// QDCP_PHOTOGRAMS_H_INCLUDED
