/**
	@file image.c
	@author jean.sabatier@zanobox.net
	@brief Functions generating image sequences
*/

#include "qdcp_photograms.h"

/**
 * Generates a sequence of Jpeg2000 images based on a project's parameters.
 * A preview sequence can also (/instead) be generated in png format.
 */
qdcp_bool
generate_sequence(t_project * pj, quickdcp_ctx * qdcp, int render_mode) 
{
		int					n = 0;
		//~ int					pid = getpid();
		
		char 				trgt_photogram[NAME_STR_LEN] = {0};
		char 				preview_photogram[NAME_STR_LEN] = {0};
		char 				last_file[PATH_STR_LEN];
		time_t				last_rendering;
		//~ MagickBooleanType	status;
		
	/* display some information about embedded imagemagick */
	/* but this does not work in GraphicsMagick */
				//~ int			i;
				//~ size_t	*	number_options;
		//~ const 	char 	* 	sel_options = "*";
				//~ char	** 	options_array = MagickQueryConfigureOptions(sel_options, number_options);
	//~ for (i = *number_options; i != 0; i--)
	//~ {
		//~ printf("%d - %s\n", i, options_array[i]);
	//~ }
		
	
	if (qdcp->verbosity >= VERBOSE_VERBOSE)
		printf("%sTarget dir : %s\n%sPreview dir : %s\n", CLI_INDENT, pj->j2k_path, CLI_INDENT, pj->seq_path);
		
	/* check directories */
	if (render_mode == PREVIEW_ONLY || render_mode == J2K_PLUS_PREVIEW)
		check_dir(pj->seq_path, qdcp->verbosity);
	
	if (render_mode != PREVIEW_ONLY)
		check_dir(pj->j2k_path, qdcp->verbosity);
	
	/* for debugging */
	//~ printf("Fondu début : %d -> %s\n", pj->fondu_d, qdcp->fondus[pj->fondu_d]); 
	//~ printf("Fondu fin   : %d -> %s\n", pj->fondu_f, qdcp->fondus[pj->fondu_f]); 
	//~ printf("Fondu laps début : %.1f\n", pj->fondulaps_d); 
	//~ printf("Fondu laps fin   : %.1f\n", pj->fondulaps_f); 
	
	/* test if the sequence has already been generated */

	/* we check the existence of sequence's last file to ensure job was complete last time */
	sprintf(last_file, "%s%s_%ld.j2k", pj->j2k_path, pj->name, pj->nmax);
	if (check_file(last_file))
	{
		/* get last_mod of the last file */		
		last_rendering = get_mod_time(last_file);
		
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
		{
			printf("%sDernière modification :\t%d\nDernier rendu :\t\t%d\n", 
				CLI_INDENT, get_last_mod_string(pj->last_mod), get_last_mod_string(last_rendering));
		}
		//~ QDCP_LOG(LOG_INFO, "last project mod :\t%d\nlast rendering :\t%d\n", pj->last_mod, last_rendering);
		
		/* compare with last_mod of the project file (qdx) */
		if (last_rendering > pj->last_mod)
		{
			pj->status = PJ_SEQUENCE;
			
			printf("Le rendu de la séquence a déjà été effectué depuis la dernière modification du projet. \n");
			/* ask what to do */
			
			/* shall we empty the directories and do it again ? */
			
			/* or do nothing ? */
			return QDCP_SUCCESS;
		}
	}

	/* try some multithreading here ? */
	/* in GUI, we could also generate preview sequence in a forked process */

	/* init progress bar */
	printf("\n");
	if (qdcp->verbosity <= VERBOSE_VERBOSE)
	{
		qdcp_progress_bar("", 0, pj->nmax, render_mode);
	}

	/* loop all photograms */
	for (n = 1; n <= pj->nmax; n++)
	{			
		/* full name and path of the .j2k photogram */
		sprintf(trgt_photogram, "%s%s_%d.j2k", pj->j2k_path, pj->name, n);
		
		/* full name and path of preview (png) photogram */
		if (render_mode == PREVIEW_ONLY || render_mode == J2K_PLUS_PREVIEW)
		{
			sprintf(preview_photogram, "%sprev_%s_%d.png", pj->seq_path, pj->name, n);
		}
		
		/* generate the photogram here */
		generate_photogram(n, pj, preview_photogram, trgt_photogram, render_mode, qdcp);
		
		/* progress bar */
		if (qdcp->verbosity <= VERBOSE_VERBOSE)
		{
			qdcp_progress_bar("", n, pj->nmax, render_mode);
		}
	}
	pj->status = PJ_SEQUENCE;	
		
	return QDCP_SUCCESS;
}

/*
 * Generates a single photogram of a sequence.
 */
qdcp_bool 
generate_photogram(int n, t_project * pj, char * preview_photogram, char * trgt_photogram, int render_mode, quickdcp_ctx * qdcp)
{
		MagickWand	*	wand_photogram;
		
	/* open a new empty magick wand */
	wand_photogram = NewMagickWand();
	
	/* start with a clean background */
	apply_background(pj, wand_photogram);
	
	/*--------------- loop the layers for this photogram -------------*/
	t_layer * cur_ly = pj->layers->first;	
	while (cur_ly != NULL)
	{
		/* if current layer is present on this photogram and set to visible */
		if (cur_ly->visible && cur_ly->entry_point <= n && cur_ly->exit_point >= n)
		{			
			if (qdcp->verbosity >= VERBOSE_VERBOSE){
				show_layer_params(cur_ly, qdcp->verbosity);
				star_line();
			}
			/*------ get all values for this layer at this frame -----*/			
			t_keyframe * frame_values = get_frame_values(n, cur_ly);
					
			/*------- add this layer to the current photogram --------*/
			if (add_layer_to_photogram(wand_photogram, cur_ly, frame_values, qdcp) == QDCP_FAILURE)
			{
				warning("Add layer to photogram failed"); 
				continue;
			}
		}
		cur_ly = cur_ly->next;
	}
	
	/*---------------- do we have to apply a fondu ? -----------------*/
	if(pj->fondu_d > FONDU_AUCUN && (float) n <= pj->fondulaps_d * pj->framerate)
	{	
		//~ printf("Applying entering fondu\n");
		apply_fondu(n, wand_photogram, pj, qdcp, 1);
	}
	
	if(pj->fondu_f > FONDU_AUCUN && (float) n > pj->nmax - (pj->fondulaps_f * pj->framerate))
	{
		//~ printf("Applying exiting fondu\n");
		apply_fondu(n, wand_photogram, pj, qdcp, 0);
	}
	
	
	/*------- write down tmp image into a jpeg2000 photogram ---------*/
	if (render_mode == J2K_ONLY || render_mode == J2K_PLUS_PREVIEW)
	{			
		if (write_j2k_photogram(wand_photogram, trgt_photogram, qdcp) == QDCP_FAILURE)
			warning("Failed to convert a photogram into jpeg2000");
	}
	
	/*-------- "thumbnail" for a sequence preview in png -------------*/
	if (render_mode == PREVIEW_ONLY || render_mode == J2K_PLUS_PREVIEW)
	{
		/* convert tmp image into png at scale 1/4 */
		if (MagickResizeImage(wand_photogram, pj->mx_w / 4, pj->mx_h / 4, LanczosFilter) == MagickFalse)
			ThrowWandException(wand_photogram);
		
		if (MagickWriteImages(wand_photogram, preview_photogram, MagickTrue) == MagickFalse)
			ThrowWandException(wand_photogram);
	}
	
	/*---------- destroy temporary image for this photogram ----------*/
	DestroyMagickWand(wand_photogram);

	/* add the photogram to project file list */
	//~ ensure_sequential(pj->j2k_files->files, n);
	
	return QDCP_SUCCESS;
}

qdcp_bool 
add_layer_to_photogram(MagickWand * wand_photogram, t_layer * cur_ly, t_keyframe * frame_values, quickdcp_ctx * qdcp)
{
			char err_buffer[80];
		
	/*-------------------- case of an image layer --------------------*/
			
	if (cur_ly->type == LAYER_IMAGE)
	{
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			printf("This is an image layer\n");
			
		if (frame_values->modify)
		{
			/* if we are on a keyframe or between different keyframes 
			 * we resize or compare current dims with last dims if any */
					
			/* path string of the source image for this layer */
				char 		img_full_path[PATH_STR_LEN];
							
			sprintf(img_full_path, "%s%s", qdcp->workspace, cur_ly->filename);
			
			if (qdcp->verbosity >= VERBOSE_DEBUG)
				printf("%sImage source : %s\n", CLI_INDENT, img_full_path);

			/* destroy a layer's previous image state */
			if (cur_ly->wand != NULL)
				DestroyMagickWand(cur_ly->wand);
			
			/* read this layer's image */
			cur_ly->wand = NewMagickWand();

			/* make sure the source image exists */
			if (! check_file(img_full_path))
			{
				sprintf(err_buffer, "Could'nt find file %s", img_full_path);
				warning(err_buffer);
				return QDCP_FAILURE;
			}
			else
			{
				if (MagickReadImage(cur_ly->wand, img_full_path) == MagickFalse)
					ThrowWandException(cur_ly->wand);
			}					
		}
		/* else */
		// we keep last resizing result and do nothing.
		// Since first photogram of the layer is supposed to BE a keyframe
		// we should already have a MagickWand.
		/* but we could check that here to be sure */

	}
	
	/*--------------------- case of a text layer ---------------------*/

	if (cur_ly->type == LAYER_TEXT)
	{
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			printf("This is an image layer\n");
			
		if (frame_values->modify)
		{
		}
	}
	
	/*------------------- case of a vectorial layer ------------------*/
	
	if (cur_ly->type == LAYER_VECTORIAL)
	{
		/* destroy a layer's previous image state */
		if (cur_ly->wand != NULL)
			DestroyMagickWand(cur_ly->wand);
		
		/* create this layer's image */
			DrawingWand * pencil = NewDrawingWand();
			PixelWand	* bgPxWd = NewPixelWand();
			PixelWand	* fgPxWd = NewPixelWand();
			
		cur_ly->wand = NewMagickWand();
		if (MagickNewImage(cur_ly->wand, frame_values->w, frame_values->h, fgPxWd) == MagickFalse)
			ThrowWandException(cur_ly->wand);
		
		PushDrawingWand(pencil);
		
		/* line width */
		DrawSetStrokeWidth(pencil, cur_ly->line_width);
		
		/* line cap */
		// one of UndefinedCap, ButtCap, RoundCap, SquareCap
		DrawSetStrokeLineCap(pencil, RoundCap);
		
		/* line join */
		// one of UndefinedJoin, MiterJoin, RoundJoin, BevelJoin
		DrawSetStrokeLineJoin(pencil, RoundJoin);
		
		/* dash array */
		//~ DrawSetStrokeDashArray(pencil, 2, [3, 2]);
		//~ DrawSetStrokeDashArray(pencil, 0, NULL);
		//~ DrawSetStrokeDashOffset(pencil, 0);
		
		DrawSetStrokeAntialias(pencil, 1);

		/* foreground (stroke) color */
		PixelSetColor(fgPxWd, frame_values->fg_color);
		DrawSetStrokeColor(pencil, fgPxWd);
		/* background (fill) color */
		PixelSetColor(bgPxWd, frame_values->bg_color);
		DrawSetFillColor(pencil, bgPxWd);
		
		if (! cur_ly->border_on)
		{
			//~ PixelSetOpacity(fgPxWd, 0.0);
			PixelSetAlpha(fgPxWd, 0.0);
		}
		if (! cur_ly->background_on)
		{
			//~ PixelSetOpacity(bgPxWd, 0.0);
			PixelSetAlpha(bgPxWd, 0.0);
		}
		
		/* draw the required shape */
		switch(cur_ly->shape)
		{
			case SHAPE_RECTANGLE:
				DrawRectangle(pencil, 
						frame_values->x, 
						frame_values->y,
						frame_values->x + frame_values->w,
						frame_values->y + frame_values->h
					);
			break;
			case SHAPE_ROUND_RECTANGLE:
				/* Here, the last two values represent
				 * horizontal and vertical radius of rounded corners.
				 * They should be set in config and GUI */
				DrawRoundRectangle(pencil, 
						frame_values->x, 
						frame_values->y,
						frame_values->x + frame_values->w,
						frame_values->y + frame_values->h,
						10,
						10
					);							
			break;
			case SHAPE_CIRCLE:
				DrawCircle(pencil, 
						frame_values->ox, 
						frame_values->oy, 
						frame_values->ox - frame_values->w, 
						frame_values->oy + frame_values->w
					);								
			break;
			case SHAPE_ELLIPSE:
				//~ DrawEllipse(pencil,
						//~ frame_values->ox,
						//~ frame_values->oy,
						//~ rx,
						//~ ry,
						//~ start,
						//~ end
					//~ );
			break;
			case SHAPE_UNDEFINED:
			
			break;
			case '?':
			
			break;
		}			
		
		if(MagickDrawImage(cur_ly->wand, pencil) == MagickFalse)
			ThrowWandException(cur_ly->wand);
		
		PopDrawingWand(pencil);
		DestroyDrawingWand(pencil);
	}				
	
	/*------------------- case of a video layer ----------------------*/
	if (cur_ly->type == LAYER_VIDEO)
	{
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			printf("This is a video layer\n");
			
		if (frame_values->modify)
		{
			/* if we are on a keyframe or between different keyframes 
			 * we resize or compare current dims with last dims if any */
					
			/* path string of the source image for this layer */
				char 		img_full_path[PATH_STR_LEN];
							
			sprintf(img_full_path, "%s%s%s%s-%d.png", 
					qdcp->vid_dir, cur_ly->basename, SEPARATOR, cur_ly->basename, frame_values->photogram);
			
			if (qdcp->verbosity >= VERBOSE_VERBOSE)
				printf("%sImage source : %s\n", CLI_INDENT, img_full_path);

			/* make sure the source image exists */
			if (! check_file(img_full_path))
			{
				warning(CANT_READ_FILE);
				return QDCP_FAILURE;
			}

			/* destroy a layer's previous image state */
			if (cur_ly->wand != NULL)
				DestroyMagickWand(cur_ly->wand);
			
			/* read this layer's image */
			cur_ly->wand = NewMagickWand();

			if (MagickReadImage(cur_ly->wand, img_full_path) == MagickFalse)
				ThrowWandException(cur_ly->wand);
										
		}
		/* else */
		// we keep last resizing result and do nothing.
		// Since first photogram of the layer is supposed to BE a keyframe
		// we should already have a MagickWand.
		/* but we could check that here to be sure */

	}

	/*----------------- apply required transformations ---------------*/
	
	/* opacity */
	if (frame_values->opacity < 100)
	{						
			float opacity = (float) frame_values->opacity / 100;
		 
		/* This is from ImageMagick :	*/
		MagickSetImageAlphaChannel(cur_ly->wand, SetAlphaChannel);
		MagickSetImageAlpha(cur_ly->wand, opacity);
	}
	/* rotation */
	if (frame_values->rotdeg != 0.0)
	{
			PixelWand * bg_pixel = NewPixelWand();
		PixelSetColor(bg_pixel, get_color(frame_values->bg_color));
		
		/* here width and height should probably be different */
		MagickSetPage(cur_ly->wand, frame_values->w, frame_values->h, 0, 0);
		
		MagickRotateImage(cur_ly->wand, bg_pixel, frame_values->rotdeg);
	}

	/*----------------- apply required size --------------------------*/
	
	if (frame_values->resize)
	{
		MagickResetIterator(cur_ly->wand);
		while (MagickNextImage(cur_ly->wand) != MagickFalse)
		{
			if (MagickResizeImage(cur_ly->wand, frame_values->w, frame_values->h, LanczosFilter) == MagickFalse)
				ThrowWandException(cur_ly->wand);
		}
		MagickResetIterator(cur_ly->wand);
	}
	
	/*----------------- apply layer's fade in or fade out ------------*/
	
	apply_layer_fading(frame_values->photogram, frame_values->opacity, cur_ly);
			
	/*------------- add src image layer to the target wand -----------*/
	
	if (MagickCompositeImage(wand_photogram, cur_ly->wand, OverCompositeOp, MagickTrue, frame_values->x, frame_values->y) == MagickFalse)
		ThrowWandException(wand_photogram);
	
	return QDCP_SUCCESS;
}

qdcp_bool 
apply_background(t_project * pj, MagickWand * wand_photogram)
{
			int 			mx_w 		= pj->mx_w;
			int 			mx_h 		= pj->mx_h;
	const	char 		*	background 	= get_color(pj->background);
			PixelWand	*	pxWd 		= NewPixelWand();
		
	PixelSetColor(pxWd, background);
	PixelSetAlpha(pxWd, 1.0);
	
	if (MagickNewImage(wand_photogram, mx_w, mx_h, pxWd) == MagickFalse)
	{
		ThrowWandException(wand_photogram);
	}	
		
	return QDCP_SUCCESS;
}

qdcp_bool 
apply_fondu(int n, MagickWand * wand_photogram, t_project * pj, quickdcp_ctx * qdcp, int debut)
{
		int				fondu;
		float			fondulen;
		int 			factor;
		float 			taux;
		float			level;
		float			opacity;
		
		double			iris_x, iris_y;
		double			rayon_x, rayon_y;
		
		MagickWand 	*	fondu_cache = NewMagickWand();
		PixelWand	*	pxWd 		= NewPixelWand();

	PixelSetAlpha(pxWd, 1.0);
		
	/* at the beginning */
	if (debut)
	{		
		fondu = pj->fondu_d;
		
		/* 23/24 = 96/100, 22/24 = 92/100, ... , 1/24 = 4/100 */
		fondulen = pj->fondulaps_d * pj->framerate;
		factor = n;	
		
		/* applying color to the cache */
		PixelSetColor(pxWd, get_color(pj->fondu_cache_color_d));
				
		/* position and radius of the circular hole in the cache */
		if (pj->fondu_d == FONDU_IRIS)
		{
			iris_x = pj->mx_w * (pj->fondu_iris_cx_d / 100);
			iris_y = pj->mx_h * (pj->fondu_iris_cy_d / 100);
		}
	}
	/* at the end */
	else
	{
		fondu = pj->fondu_f;

		fondulen = pj->fondulaps_f * pj->framerate;
		factor = pj->nmax - n ; /* -1 to get 0 on the last photogram */
		
		/* applying color to the cache */
		PixelSetColor(pxWd, get_color(pj->fondu_cache_color_f));
				
		/* position and radius of the circular hole in the cache */
		if (pj->fondu_f == FONDU_IRIS)
		{
			iris_x = pj->mx_w * (pj->fondu_iris_cx_f / 100);
			iris_y = pj->mx_h * (pj->fondu_iris_cy_f / 100);
		}
	}
	
	/* fondus au noir */
	if (fondu == FONDU_AU_NOIR)
	{
		taux = (float) 100.0 / fondulen;
		level = factor * taux;
		/* convert opacity from percent to quantum (from 0.0 to 1.0) */
		/* 1.0 is fully opaque and 0.0 is fully transparent*/
		/* As this is the alpha of the cache, it gives the opacity of the photogram */
		opacity = (double) (1 - (level / 100));
		
		//~ printf("\nfondulen : %.1f\n", fondulen);
		//~ printf("\n taux (100/fondulen) : %.1f\n", taux);
		//~ printf("\n level (factor*taux) : %.1f\n", level);
		//~ printf("\n opacity : %.1f\n", opacity);
		
		//~ PixelSetAlpha(pxWd, opacity);
		
		/* create cache */
		if (MagickNewImage(fondu_cache, pj->mx_w, pj->mx_h, pxWd) == MagickFalse)
			ThrowWandException(fondu_cache);
		MagickSetImageAlphaChannel(fondu_cache, SetAlphaChannel);
		if (MagickSetImageAlpha(fondu_cache, opacity) == MagickFalse)
			ThrowWandException(wand_photogram);
			
		if (MagickCompositeImage(wand_photogram, fondu_cache, OverCompositeOp, MagickTrue, 0, 0) == MagickFalse)
			ThrowWandException(wand_photogram);
		
	}
	/* fondus iris */
	else if (fondu == FONDU_IRIS)
	{
			MagickWand 	* 	iris_mask 	= NewMagickWand();
			DrawingWand	*	d_wand 		= NewDrawingWand();
			
		taux = (pj->mx_w / 2) / fondulen;
		level = (factor-1) * taux;
		rayon_x  = iris_x + level;
		rayon_y  = iris_y + level;
		//~ rayon_x  = 100;
		//~ rayon_y  = 200;

		//~ printf("\n");
		//~ printf(" taux : %f\n", taux);
		//~ printf(" factor : %d\n", factor);
		//~ printf(" level : %f\n", level);
		//~ printf(" iris_x : %f\n", iris_x);
		//~ printf(" iris_y : %f\n", iris_y);
		//~ printf(" rayon_x : %f\n", rayon_x);
		//~ printf(" rayon_y : %f\n", rayon_y);

		/* create colored cache */
		//~ PixelSetColor(pxWd, "black");
		if (MagickNewImage(fondu_cache, pj->mx_w, pj->mx_h, pxWd) == MagickFalse)
			ThrowWandException(fondu_cache);
					
		/* create white mask */
		PixelSetColor(pxWd, "white");
		DrawSetStrokeOpacity(d_wand, 1);

		if (MagickNewImage(iris_mask, pj->mx_w, pj->mx_h, pxWd) == MagickFalse)
			ThrowWandException(iris_mask);		
		//~ MagickSetImageAlphaChannel(iris_mask, SetAlphaChannel);
		/* paint black circle on white mask */
		PushDrawingWand(d_wand);
			PixelSetColor(pxWd, "black");
			DrawSetStrokeColor(d_wand, pxWd);
			DrawSetFillColor(d_wand, pxWd);
			DrawSetStrokeOpacity(d_wand, 1);
			DrawSetStrokeWidth(d_wand,1);
			DrawSetStrokeAntialias(d_wand,1);
			DrawCircle(d_wand, iris_x, iris_y, rayon_x, rayon_y);
		PopDrawingWand(d_wand);
		
		if(MagickDrawImage(iris_mask, d_wand) == MagickFalse)
			ThrowWandException(iris_mask);
		
		/* apply mask to wand */
		/* arg 2, type of mask (a pixel type) can be ReadPixelMask or WritePixelMask */
		if (MagickSetImageMask(wand_photogram, WritePixelMask, iris_mask) == MagickFalse)
			ThrowWandException(fondu_cache);
		
		/* apply cache over the whole */
		if (MagickCompositeImage(wand_photogram, fondu_cache, DstOutCompositeOp, MagickTrue, 0, 0) == MagickFalse)
			ThrowWandException(wand_photogram);

		/* for debugging : */
		//~ if (MagickCompositeImage(wand_photogram, fondu_cache, OverCompositeOp, MagickTrue, 0, 0) == MagickFalse)
			//~ ThrowWandException(wand_photogram);

		if(n == pj->nmax)
		{
			DestroyPixelWand(pxWd);
			DestroyMagickWand(fondu_cache);
			DestroyMagickWand(iris_mask);
		}
	}

	
	return QDCP_SUCCESS;
}

qdcp_bool
apply_layer_fading(int n, int max_op, t_layer * ly)
{
		int entry 			= ly->entry_point;
		int exit  			= ly->exit_point;
		int fade_in			= ly->fade_in;
		int fade_out		= ly->fade_out;
		int n_in  			= n - entry;
		int n_out 			= exit - n;
		float taux;
		float fade_in_op 	= max_op;
		float fade_out_op 	= max_op;
		float fade_op		= max_op/100;
		
	/* at the beginning */
	if (n_in <= ly->fade_in)
	{
		taux = (float) max_op / fade_in;
		fade_in_op = (float) taux * n_in;
		fade_op = fade_in_op / 100;
	}
	
	if (n_out <= ly->fade_out)
	{
		taux = (float) max_op / fade_out;
		fade_out_op = (float) taux * n_out;
		
		if (fade_in_op < fade_out_op)
		{
			fade_out_op = fade_in_op;
		}
		
		fade_op = fade_out_op / 100;
	}
	
	/* This was from GraphicsMagick :
	MagickSetImageOpacity(ly->wand, fade_op); */
	/* This is from ImageMagick :	*/
	MagickSetImageAlpha(ly->wand, fade_op);

	
	return QDCP_SUCCESS;
}

qdcp_bool 
write_j2k_photogram(MagickWand * wand_photogram, const char * trgt_file, quickdcp_ctx * qdcp)
{
	/* This convert line comes from 
	 * http://www.imagemagick.org/discourse-server/viewtopic.php?f=1&t=13936
	 * 
	 * convert %%F -alpha Off -resize 2048x1080 \ 
	 * -recolor "0.412453 0.357580 0.180423 \
	 * 			 0.212671 0.715160 0.072169 \
	 * 			 0.019334 0.119193 0.950227" \
	 * -depth 12 ..\jp2k\file.j2k
	 * 
	 * Which is supposed to correspond to openjpeg command :
	 * 	image_to_j2k -cinema2K 24 -ImgDir tiff -OutFor j2c
	 * 
	 * In the same thread Wolfgang Woehl gives the following command line :
	 *  convert srgb.tiff -alpha off -depth 12 \
	 *  -gamma $degamma_2_2 -recolor "$srgb_to_xyz" -gamma 2.6 dci-xyz.tiff
	 * Where $degamma_2_2 would contain the number 1/2.2 (approximated sRGB gamma) 
	 * and $srgb_to_xyz the 3x3 matrix.
	 * 
	 * See also :
	 * http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html.
	 * where sRGB matrix to XYZ is :
	 * 0.4124564  0.3575761  0.1804375
	 * 0.2126729  0.7151522  0.0721750
	 * 0.0193339  0.1191920  0.9503041
	 * 
	 * More on imagemagick repos : http://git.imagemagick.org/repos/ImageMagick
	 * */
	 
	 /* METHODS */
	 
	/* with imagemagick 7.0.3-4 and openjpeg 2.1 *
	 * --------------------------------------- */
	//const KernelInfo * srgb2j2k_matrix;
	
	//~ if (MagickColorMatrixImage(wand_photogram, srgb2j2k_matrix) == MagickFalse)
		//~ ThrowWandException(wand_photogram);

	if(qdcp->verbosity >= VERBOSE_VERBOSE)
		printf("qdcp converting : %s\n", trgt_file); 
	
	
	const char * wandformat = MagickGetFormat(wand_photogram);
	
	/* check input colorspace */
	ColorspaceType colspace = MagickGetImageColorspace(wand_photogram);
	
	if(qdcp->verbosity >= VERBOSE_VERBOSE)
		printf("wandformat %d - colorspace %d\n", wandformat, colspace);
			
	/* apply XYZ colorspace */
	/* this one does not work well (at least without giving any colorMatrix) */
	//~ if (MagickSetImageColorspace(wand_photogram, XYZColorspace) == MagickFalse)
		//~ ThrowWandException(wand_photogram);
	
	/* try this : */	
	if (MagickTransformImageColorspace(wand_photogram, XYZColorspace) == MagickFalse)
		ThrowWandException(wand_photogram);
	
	/* write the jpeg2000 photogram */
	if (MagickWriteImages(wand_photogram, trgt_file, MagickTrue) == MagickFalse)
		ThrowWandException(wand_photogram);
		
	return QDCP_SUCCESS;
}

