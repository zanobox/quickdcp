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
   @file    qdcp_video.c
   @author  jean.sabatier@zanobox.net
   @brief   QuickDCP's video functions set.
*/
#include "qdcp_video.h"

/**
 * @brief Analyse a video file and prepare a layer
 * 
 * @param	ly		A pointer to a t_layer object
 * @param	pj		A pointer to a t_project object
 * @param	qdcp	A pointer to the current quickdcp context
 * @return A boolean, either QDCP_SUCCESS or QDCP_FAILURE.
 */
qdcp_bool
get_video_info(t_layer * ly, t_wav_track * track, t_project * pj, quickdcp_ctx * qdcp) 
{

		int 		i = 0;
		int			error;
		int 		video_stream_index 		= -1;
		int 		audio_stream_index 		= -1;
		int 		subtitle_stream_index 	= -1;
		uint8_t		verbosity = qdcp->verbosity;
	
	/* get file's format context */

	if (avformat_open_input(&ly->src_format_ctx, ly->source_file, NULL, NULL) != 0)
	{
		warning("Impossible d'ouvrir la video");	
		return QDCP_FAILURE;
	}
	 
	printf("Caractéristiques de la video source :\n");
	
	/* retrieve streams information */
	
	if (avformat_find_stream_info(ly->src_format_ctx, NULL) < 0)
	{
		warning("Impossible de déterminer les flux");	
		return QDCP_FAILURE;
	}

	/* dump format info on std error (do that after avformat_find_stream_info(), to show more info) */
	if (verbosity >= VERBOSE_VERBOSE)
	{
		printf("FFmpeg format dump\n");
		av_dump_format(ly->src_format_ctx, 0, ly->source_file, 0);
	}
	
	for (i=0; i<ly->src_format_ctx->nb_streams; i++)
	{
		if (ly->src_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream_index = i;
		}
		if (ly->src_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = i;
			/* create new track */
			/* lock track to the video layer */
			/* read audio input */
		}
		if (ly->src_format_ctx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_SUBTITLE)
		{
			subtitle_stream_index = i;
			/* create new subtitle track */
			/* lock subs track to the video layer */
		}
	}

	/*----------------- taking care of video stream ------------------*/
	
	if (video_stream_index == -1)
	{
		warning("Pas de flux video");	
		return QDCP_FAILURE;
	}
	
	printf("\n%sTrouvé un flux video\n\n", CLI_INDENT);

	/* stream index in format's streams array */
	ly->stream_index = video_stream_index;
	ly->video_stream = ly->src_format_ctx->streams[video_stream_index];
	
	/* codec parameters from the stream */
	ly->src_codec_params = ly->video_stream->codecpar;
	
	if (!(ly->src_codec = avcodec_find_decoder(ly->src_codec_params->codec_id))) 
	{
		warning("Could not find the stream's decoder from codec ID");
		warning("Codec (codec_id = %d) non pris en charge par cette instance de FFmpeg !",
				ly->src_codec_id);
		return QDCP_FAILURE;
	}
	
	/* codec context */
	if (!(ly->src_codec_ctx = avcodec_alloc_context3(ly->src_codec)))
	{
		warning("Could not alloc a context for the decoder codec");
		return QDCP_FAILURE;
	}
			
	/* codec context from parameters */
	if (avcodec_parameters_to_context(ly->src_codec_ctx, ly->src_codec_params) < 0)
	{
		warning("Could not retrieve codec context");
		return QDCP_FAILURE;
	}	
		
	ly->src_w = ly->src_codec_ctx->width;
	ly->src_h = ly->src_codec_ctx->height;
	ly->src_ratio = (float) ly->src_w / ly->src_h;

	/* fade_in, fade_out are zero, overriding default, but not locked */
	ly->fade_in = 0;
	ly->fade_out = 0;
	
	/* if project's requested framerate is diferent from video's frame rate 
	 * we deal with it later */
	ly->frame_rate = ly->video_stream->r_frame_rate;
	
	/* layer members to be set */	
	//~ time_length
	
	ly->time_length = (ly->video_stream->duration / pj->framerate);
	
	if (ly->video_stream->nb_frames > pj->nmax)
	{
		pj->nmax = ly->video_stream->nb_frames;
		//~ pj->nmax = ly->src_format_ctx->streams[videoStream]->duration;
		pj->laps = (float) pj->nmax / pj->framerate;	
	}

	// entry_point, exit_point
	ly->entry_point = 1;
	ly->exit_point = pj->nmax;
	
	printf("Verbosity : %d\n", verbosity);
	
	printf("Nombre de frames :\t%"PRId64"\n", ly->video_stream->nb_frames);	
	printf("Time base :\t\t%d / %d\n", 
			ly->video_stream->time_base.num, 
			ly->video_stream->time_base.den);
			
	printf("Duration :\t\t%.2f s\n", (float) ly->video_stream->duration / ly->video_stream->time_base.den);

	if (verbosity >= VERBOSE_NORMAL)
		printf("Sample aspect ratio :\t%d/%d\n", 
			ly->video_stream->sample_aspect_ratio.num, 
			ly->video_stream->sample_aspect_ratio.den);

	if (verbosity >= VERBOSE_VERBOSE)
		printf("Average framerate :\t%d/%d\n", 
			ly->video_stream->avg_frame_rate.num, 
			ly->video_stream->avg_frame_rate.den);
	
	if (verbosity >= VERBOSE_NORMAL)
		printf("Real framerate :\t%d/%d\n", 
			ly->video_stream->r_frame_rate.num, 
			ly->video_stream->r_frame_rate.den);

	if (verbosity >= VERBOSE_NORMAL)
		printf("Display asp. ratio :\t%dx%d\n", 
			ly->src_w, ly->src_h);
			//~ ly->video_stream->display_aspect_ratio.num, 
			//~ ly->video_stream->display_aspect_ratio.den);

	// printf("Metadata :\t\t%s\n", ly->format_ctx->streams[videoStream]->metadata);
	//~ if (ly->format_ctx->streams[videoStream]->metadata.count > 0)
	//~ {
		//~ for (i=0; i<ly->format_ctx->streams[videoStream]->metadata.count; i++)
		//~ {
			//~ /* loop AVDictionnary elems whick are key->value pairs */
		//~ }
	//~ }

	// format -> mx_w, mx_h (calculate size and position of frames in screen)

	/* needed (or cool) info derived from AVFormatContext */
	/* see http://ffmpeg.org/doxygen/trunk/structAVFormatContext.html */
	
	//bit_rate
	//packet_size
	//max_picture_buffer
	//fps_probe_size
	//codec_whitelist (list of allowed decoders)
	//format_whitelist (list of allowed demuxers) 
	
	/* useful info that can get retrieved from stream struct */
	
	// time_base
	// start_time 
	// duration
	// nb_frames
	// disposition (AV_DISPOSITION bit field)
	// sample_aspect_ratio
	// metadata
	// avg_frame_rate
	// attached_pic (if AV_DISPOSITION_ATTACHED_PIC)
	// side_data (array of side data ?)
	// info -> codec_par codec parameters ->
	// 
	// codec_info_nb_frames (int)
	// r_frame_rate (real base framrate of the stream)
	// skip_to_keyframe (int)
	// skip_samples (etc)
	// display_aspect_ratio
	
	/* interesting values of AVCodecContext struct : 
	 * https://ffmpeg.org/doxygen/2.7/structAVCodecContext.html */
	
	// codec_type (AVMediaType)
	// codec (AVCodec)
	// codec_id (AVCodecID)
	// codec_tag (unsigned int)
	// opaque (void, to carry user's or app's things)
	// bit_rate (int)
	// global_quality (if not per frame) (int)
	// compression_level (int)
	// flags, flags2, extradata
	// ticks_per_frame (int)
	// delay (int)
	// width (int)
	// height (int)
	// coded_width (int)
	// coded_height (int)
	// gop_size (size of group of pictures)
	// pix_fmt (AVPixelFormat)
	// me_method (motion estimation algorithm)
	// void(* 	draw_horiz_band )(struct AVCodecContext *s, const AVFrame *src, int offset[AV_NUM_DATA_POINTERS], int y, int type, int height)
	//  AVPixelFormat(* 	get_format )(struct AVCodecContext *s, const enum AVPixelFormat *fmt)
	// colorspace (AVColorSpace)
	// color_range (AVColorRange)
	// slices (int) number of slices
	// channels (int) number of audio channels
	// sample_rate (int) samples per second
	
	// and many, many more...
	
	

	/*------------------ take care of audio stream -------------------*/

	if (audio_stream_index == -1)
	{
		warning("Pas de flux audio détecté");	
	}
	else
	{
		printf("\n%sTrouvé un flux audio\n\n", CLI_INDENT);
		
		if (! import_audio_file(track, ly->source_file, qdcp))
			warning("Import audio échoué");
			
		/* extract video info towards layer values */
		if (! read_audio_input(track, qdcp->verbosity))
			warning("Could not retrieve audio info");
		
		
		/* add file and stream to the new track */
		printf("w_id : %d\n", track->w_id);
		printf("filename : %d\n", track->filename);
		printf("basename : %d\n", track->basename);
		printf("ext : %d\n", track->ext);
		
		
	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);

		track->src_audio_stream = ly->src_format_ctx->streams[audio_stream_index];

	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);

		track->lock_to_layer = ly->l_id;

	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);

		if (!extract_video_sound(track, ly, pj, qdcp))
		{
			warning("Failed to extract sound from video");
			return QDCP_FAILURE;
		}
	}
	
	if (subtitle_stream_index > -1)
	{
		warning("Sous-titres détectés mais non gerés par cette version de QuickDCP");	
	}
	
	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);


	return QDCP_SUCCESS;
}


qdcp_bool
extract_video_images(t_layer * ly, t_project * pj, quickdcp_ctx * qdcp) 
{
		int error;
		
	/* https://en.wikibooks.org/wiki/Category:FFMPEG_An_Intermediate_Guide */
		
	/* this is the command we need to emulate :
	 * ffmpeg -i ly->source_file -vf fps=pj->framerate  image-%03d.j2k */	
	
		char command[CMD_STR_LEN];
	sprintf(command, "/usr/local/ffmpeg -i %s -vf fps=%d  %s%s-%%06d.png",
				ly->source_file, pj->framerate, pj->seq_path, ly->basename);
	
	if (system(command) < 0)
	{
		error_msg(NO_SHELL, __FILE__, __LINE__);
	}
	
	//~ return QDCP_SUCCESS;

	/*----------------*/
		
	/* for png output */
	/* ffmpeg command dump gives :
	 * Output #0, image2
	 * encoder : Lavf56.25.101
	 * stream 0#0 : Video:png, rgb24, wxh, q=2-31, 200 kb/s, 24fps, 24tbn, 24 tbc.
	 * Based only on the input and output file names and pj->framerate. */
	 
	/* find an output codec */
		
	ly->out_codec = avcodec_find_encoder(ly->out_codec_id);
	if (! ly->out_codec)
	{
		warning("Failed to open png codec");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	
	/* create a codec context */
	
	ly->out_codec_ctx = avcodec_alloc_context3(ly->out_codec);
	if (ly->out_codec_ctx == NULL)
	{
		warning("Failed to open png codec context");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	
	/* codec context further settings */
	
	ly->out_codec_ctx->width = ly->src_w; 
	ly->out_codec_ctx->height = ly->src_h;

	ly->out_codec_ctx->time_base.num = 1;
	ly->out_codec_ctx->time_base.den = pj->framerate;
	
	ly->out_codec_ctx->pix_fmt = AV_PIX_FMT_RGBA;
	//~ ly->out_codec_ctx->pix_fmt = AV_PIX_FMT_XYZ12BE;
	//~ ly->out_codec_ctx->pix_fmt = AV_PIX_FMT_YUV422P; //< planar YUV 4:2:2, 16bpp, (1 Cr & Cb sample per 2x1 Y samples)
	/* see all pixel formats there :
	 * https://www.ffmpeg.org/doxygen/3.2/pixfmt_8h.html#a57d4742788f6c0860dc4d1f20b528558
	 */

	ly->out_codec_ctx->bit_rate = 80000000; // 80 Mb/s
	ly->out_codec_ctx->rc_max_rate = ly->out_codec_ctx->bit_rate;
	ly->out_codec_ctx->rc_min_rate = ly->out_codec_ctx->bit_rate;
	
	//~ ly->out_codec_ctx->sample_aspect_ratio = vform;
	//~ ly->out_codec_ctx->compression_level = 100;
	//~ ly->out_codec_ctx->flags2 =0;

	/* we disable multi-threading when getting separate images */
	ly->out_codec_ctx->thread_count=1;
	
	//~ av_dump_format(NULL, 0, ly->source_file, 0);
	//~ av_dump_format(ly->src_format_ctx, 0, ly->source_file, 0);
	//~ av_dump_format(ly->out_format_ctx, 0, ly->source_file, 0);
	
	/* open the output codec context */
	if (avcodec_open2(ly->out_codec_ctx, ly->out_codec, NULL) < 0)
	{
		warning("Failed to open codec");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	
	/* get a packet */
	
		AVPacket	*	pkt;
		
	//~ av_new_packet(&pkt, 0x200000);
	//~ av_init_packet(pkt);
	//~ pkt->size = 0x200000;;
	//~ vform.num = 16; vform.den = 9;
	//~ pkt->data = tempBuff;
	
	/* get a frame */
	
		AVFrame		*	frame;
		
	frame = av_frame_alloc();
	if (!frame)
	{
		warning("Failed to allocate frame");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
	}
	
	frame->interlaced_frame = 1;
	frame->top_field_first = 1;
	frame->width = ly->src_w;
	frame->height = ly->src_h;
	
	//~ frame->data[0]=&LumaBuffer[doffsl]; 	frame->linesize[0]=1920;
	//~ frame->data[1]=&RowCr1Buffer[doffscr]; 	frame->linesize[1]=960;
	//~ frame->data[2]=&RowCr2Buffer[doffscr]; 	frame->linesize[2]=960;

	error = avcodec_send_packet(ly->src_codec_ctx, pkt);
	if (error < 0)
    {
 		warning("Failed to send packet");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
    }	

	error = avcodec_receive_frame(ly->src_codec_ctx, frame);
	if (error < 0)
    {
 		warning("Failed to decode frame");
		fprintf(stderr, "%s %s %s", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
    }	
	
	error = av_read_frame(ly->src_format_ctx, pkt);
	if (error < 0)
    {
 		warning("Failed to read frame");
		fprintf(stderr, "%s %s %d", get_ffmpeg_error(error), __FILE__, __LINE__);
		return QDCP_FAILURE;
    }

	/* close codec context, format, etc... */
	av_frame_free(&frame);
    av_packet_free(&pkt);	


	return QDCP_SUCCESS;
}

qdcp_bool
extract_video_sound(t_wav_track * track, t_layer * ly, t_project * pj, quickdcp_ctx * qdcp) 
{
		int error;
		
/* https://en.wikibooks.org/wiki/Category:FFMPEG_An_Intermediate_Guide */
	
	//~ track->status = TRACK_LOCKED
	//~ track->index =
	
	sprintf(track->basename, "%s", basename);
	sprintf(track->original, "%s", ly->source_file);
	//~ sprintf(track->wav_path, "%s", );

	track->entry_point = ly->entry_point;
	track->diamond = 0;
	track->mute = QDCP_NO;
	
	//~ track->audio_stream;	/** < stream index from FFmpeg */
	track->mix_type = pj->mix_type;
	track->out_channel_layout = pj->audio_format;

	/* using sox (later ffmpeg I think) to retrieve wav features */
	//~ sox_check = sox_open_read(trgt_file, NULL, NULL, NULL);

	//~ track->nchannels	= sox_check->signal.channels;
	//~ track->samplerate	= sox_check->signal.rate;
	//~ track->precision	= sox_check->signal.precision;
	//~ track->duration		= sox_check->signal.length;
	track->time_length	= track->src_samples / track->src_samplerate;
	//~ track->time_HHMMSS

		//~ int				volume;
		//~ int				samples;
		//~ int				encoding;
		//~ char		*	annotations;
			
	printf("\n");
		
	printf(" > channels :\t\t%d channels\n",				track->src_channels);
	printf(" > precision :\t\t%d bits per sample\n",		track->src_bitdepth);
	printf(" > samplerate :\t\t%.1f samples per second\n",	track->src_samplerate);
	printf(" > duration :\t\t%d samples\n",					track->src_samples);
	printf(" > time length :\t%d seconds\n",				track->time_length);
	
	add_track_to_project(track, pj, qdcp);

	/*-----------------*/
	
		/* source codec_id */
		track->src_codec_id = track->src_audio_stream->codecpar->codec_id;

	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);
		/* decoder */
		if (!(track->src_codec = avcodec_find_decoder(
						track->src_audio_stream->codecpar->codec_id))) {
			fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
			return QDCP_FAILURE;
		}
		
	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);
		/* codec context */
		track->src_codec_ctx = avcodec_alloc_context3(track->src_codec);
		if (! track->src_codec_ctx) {
			fprintf(stderr, "%s %s %d\n", get_ffmpeg_error(error), __FILE__, __LINE__);
			return QDCP_FAILURE;
		}
			
		/* source codec parameters and format context */
		
	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);
		/* duration in seconds */
		track->time_length = track->src_format_ctx->duration;

		/* source number of channels */
		track->src_channels = track->src_audio_stream->codecpar->channels;
		
		/* source channel_layout (from select_channel_layout() ) */
		track->src_channel_layout = select_channel_layout(track->src_codec);

		/* source sample format */
		track->src_sample_fmt = track->src_audio_stream->codecpar->format;

		/* bits per coded sample */
		track->src_bitdepth = track->src_audio_stream->codecpar->bits_per_coded_sample;
										
		/* source sample rate */
		track->src_samplerate = track->src_audio_stream->codecpar->sample_rate;

		/* source frame size */
		track->src_framesize = track->src_audio_stream->codecpar->frame_size;
		
	printf("%s%s() (%s, l.%d)\n", CLI_INDENT, __func__, strip_path(__FILE__), __LINE__);
		if (qdcp->verbosity >= VERBOSE_VERBOSE)
			printf("%s%sFrames already demuxed : \t%d\n", CLI_INDENT, CLI_INDENT, 
				track->src_audio_stream->codec_info_nb_frames); 
		
		printf("Stream->Nb_frames : %d\n",  track->src_audio_stream->nb_frames);

		/* add track to the project and lock to the video layer */
		track->lock_to_layer = ly->l_id;
		add_track_to_project(track, pj, qdcp);

	return QDCP_SUCCESS;
}

qdcp_bool 
extract_video_subtitles(AVStream * stream, t_sub_track * subs, t_project * pj, quickdcp_ctx * qdcp) 
{
	
	/* subtitles decoders from FFmpeg
	 * - dvbsub
	 * - dvdsub (DVD, VobSub, Matroska)
	 * - libzvbi-teletext
	 */
	 
	/* ffmpeg -probesize 100M -analyzeduration 120M -i input.vob  , etc. */
	
	/* other example : */
	/* ffmpeg -i /path/to/file -an -vn -map 0:2 -c:s:0 srt -c:s:1 srt sub0.srt */
	/* -c:s:0 srt  works for the first subs-stream, -c:s:1 srt for the second, and so on */
	/* see also the use of -map */
	printf("Subtitles extraction not implemented yet\n");

	return QDCP_SUCCESS;
}


int 
import_video_sequence(char * video_file, t_layer * ly, t_project * pj, quickdcp_ctx * qdcp) 
{	
	/* open file */
	//~ printf("Opening video file\n");
	//~ if (avformat_open_input(&ly->format_ctx, ly->source_file, NULL, NULL) != 0)
		//~ error_msg("Impossible d'ouvrir la video", __FILE__, __LINE__);	
	
	/* open Codec */
	printf("Opening video codec\n");
	if (avcodec_open2(ly->src_codec_ctx, ly->src_codec, NULL) < 0)
		error_msg("Impossible d'ouvrir le codec video", __FILE__, __LINE__);	
	
	/* store the frame */
	printf("Storing the frames\n");
	
		AVFrame * 	pFrame 		= NULL;
		AVFrame * 	pFrameRGB 	= NULL;
		AVFrame * 	pFrameXYZ 	= NULL;
		uint8_t * 	bufferRGB 	= NULL;
		uint8_t * 	bufferXYZ 	= NULL;
		int 		numRGBBytes;
		//~ int			numXYZBytes;
	
	/* allocate */
	pFrame = av_frame_alloc();
	/* in the example they allocate a new frame for a converted frame in 24 bits RGB
	 * but I will want some JPEG2000 most certainly, and/or a preview */
	printf("Allocating RGB frame\n");
	pFrameRGB = av_frame_alloc();
	if (pFrameRGB == NULL)
		error_msg(BAD_CALLOC, __FILE__, __LINE__);
	//~ printf("Allocating XYZ frame\n");
	//~ pFrameXYZ = av_frame_alloc();
	//~ if (pFrameXYZ == NULL)
		//~ error_msg(BAD_CALLOC, __FILE__, __LINE__);
		
	/* interesting members of AVFrame struct (there's really a lot) : 
	 * 
	 * uint8_t	data[AV_NUM_DATA_POINTERS] 	 	(pointer to the picture (/channel) planes)	[fundamental]
	 * int		linesize[AV_NUM_DATA_POINTERS]  (size in bytes of each picture line)		[fundamental]
	 * uint8_t	extended_data 					(only meant for audio. in video, points to data)
	 * int 		width
	 * int 		height
	 * int 		nb_samples 						(audio, samples per channel, described by this frame)
	 * int 		format 							(video = AVPixelFormat, audio = AVSampleFormat)
	 * int		key_frame						(that's a boolean: 1=yes, 0=no)
	 * AVPictureType	pict_type				(mpeg_video.h -> https://ffmpeg.org/doxygen/2.7/structPicture.html)
	 * AVRational		sample_aspect_ratio		(0/1 if unknown)
	 * int64_t	pts								(presentation timestamp in time_base units (time when frame should be shown to user))
	 * int64_t	pkt_pts							(PTS copied from the AVPacket that was decoded to produce this frame)
	 * int64_t	pkt_dts							(DTS copied from the AVPacket that triggered returning this frame)
	 * int		coded_picture_number
	 * int		display_picture_number
	 * int		quality							(quality (between 1 (good) and FF_LAMBDA_MAX (bad)))
	 * AVBufferRef	buf[AV_NUM_DATA_POINTERS]	(AVBuffer references backing the data for this frame)
	 * AVColorRange	color_range					(MPEG vs JPEG YUV range)
	 * AVColorSpace	color_space					(YUV colorspace type. It must be accessed using av_frame_get_colorspace() and av_frame_set_colorspace())
	 * AVChromaLocation	chroma_location
	 * int64_t	best_effort_timestamp
	 * int64_t	pkt_pos	(reordered pos from the last AVPacket that has been input into the decoder Code outside libavcodec should access this field using: av_frame_get_pkt_pos(frame))
	 * int64_t	pkt_duration					(duration of the corresponding packet, expressed in AVStream->time_base units, 0 if unknown)
	 * AVDictionary	metadata
	 * int 	decode_error_flags
	 * int 	channels
	 * int	pkt_size							(size of the corresponding packet containing the compressed frame)
	*/
	
	/* some AVBuffer reference should also be useful : https://ffmpeg.org/doxygen/2.7/structAVBuffer.html 
	 * uint8_t		data		(data described by the buffer)
	 * int			size		(in bytes obviously)
	 * volatile int	refcount	(number of existing AVBufferRef instances referring to this buffer)
	 * void(* 	free )(void *opaque, uint8_t *data) callback for freeing the data (buffer_internal.h, line 48.)
	 */
	
	
	/* then we still have to allocate room for rawdata during conversion */
	
	/* determine buffer size (for one frame of the targetted type) */
	/* all pixel format declarations : https://ffmpeg.org/doxygen/2.7/pixfmt_8h.html#a9a8e335cf3be472042bc9f0cf80cd4c5 */
	
	/* AV_PIX_FMT_RGB24 || AV_PIX_FMT_RGBA */
	/* 32 is for align, an int which is default (or advised ?) value for the assumed linesize alignment */
	//~ numRGBBytes = av_image_get_buffer_size(AV_PIX_FMT_RGBA, ly->codec_ctx->width, ly->codec_ctx->height, 32);
	/* buffer size for XYZ little endian */
	// big endian : AV_PIX_FMT_XYZ12BE
	//~ numXYZBytes = av_image_get_buffer_size(AV_PIX_FMT_XYZ12LE, ly->codec_ctx->width, ly->codec_ctx->height, 32);

	printf("Allocating buffers\n");
	numRGBBytes = av_image_alloc(pFrameRGB->data, pFrameRGB->linesize, ly->src_codec_ctx->width, ly->src_codec_ctx->height, AV_PIX_FMT_RGBA, 32);
	//~ numXYZBytes = av_image_alloc(pFrameXYZ->data, pFrameXYZ->linesize, ly->codec_ctx->width, ly->codec_ctx->height, AV_PIX_FMT_XYZ12LE, 32);

	bufferRGB = (uint8_t *) av_malloc(numRGBBytes * sizeof(uint8_t));
	//~ bufferXYZ = (uint8_t *) av_malloc(numXYZBytes * sizeof(uint8_t));
	
	/* associate (assign the appropriate parts of) the frame 
	 * with (to) the new buffer */
	 
	/* AVPicture is a subset of AVFrame struct (same beginning) 
	 * pFrameRGB is an AVFrame, but AVFrame is a superset of AVPicture.
	 * It's a cast. */
	/* deprecated : */
	//~ avpicture_fill((AVPicture *)pFrameRGB, bufferRGB, AV_PIX_FMT_RGBA,
					//~ ly->codec_ctx->width, ly->codec_ctx->height);
	//~ avpicture_fill((AVPicture *)pFrameXYZ, bufferXYZ, AV_PIX_FMT_XYZ12LE,
					//~ ly->codec_ctx->width, ly->codec_ctx->height);

	printf("Filling RGB image arrays\n");
	av_image_fill_arrays(pFrameRGB->data, pFrameRGB->linesize, bufferRGB, AV_PIX_FMT_RGBA, ly->src_codec_ctx->width, ly->src_codec_ctx->height, 32);
	//~ printf("Filling XYZ image arrays\n");
	//~ av_image_fill_arrays(pFrameXYZ->data, pFrameXYZ->linesize, bufferXYZ, AV_PIX_FMT_XYZ12LE, ly->codec_ctx->width, ly->codec_ctx->height, 32);
	
	/* now we're ready to read the stream */
	
	/* so we create a .tmp/j2k/<project_name> directory 
	 * and init a J2K filenames list*/
	
	/* and a .tmp/seq/<project_name> directory 
	 * and init a J2K filenames list*/

	
	/* and start to read the stream */
	
		struct 	SwsContext 	* 	sws_ctx_RGBA = NULL;
		//~ struct 	SwsContext 	* 	sws_ctx_XYZ = NULL;
				int 			frameFinished;
				AVPacket 		packet;
		
	printf("Reading the stream...\n");
	sws_ctx_RGBA = sws_getContext(
			ly->src_codec_ctx->width,
			ly->src_codec_ctx->height,
			ly->src_codec_ctx->pix_fmt,
			ly->src_codec_ctx->width,
			ly->src_codec_ctx->height,
			AV_PIX_FMT_RGBA,
			SWS_BILINEAR,
			NULL,
			NULL,
			NULL
			);
			
		int i = 0;
		int count = 0;
	while (av_read_frame(ly->src_format_ctx, &packet) >= 0)
	{
		fflush(stdout);
		
		if (packet.stream_index == ly->stream_index)
		{
			count++;
			printf(" > Frame %d\n", count);
			
			/* decode video frame */
			// deprecated in FFmpeg 3.1 :
			//~ avcodec_decode_video2(ly->codec_ctx, pFrameRGB, &frameFinished, &packet);
			// replaced by :
			avcodec_receive_frame(ly->src_codec_ctx, pFrameRGB);
			
			/* did we get a video frame ? */
			if (frameFinished)
			{
				/* convert the image from its native format to RGBA (later J2K) */
				sws_scale(sws_ctx_RGBA, (uint8_t const * const *)pFrameRGB->data,
							pFrameRGB->linesize, 0, ly->src_codec_ctx->height,
							pFrameRGB->data, pFrameRGB->linesize);
							
				/* save the frame to disk */
				if (++i <= pj->nmax)
				{
					save_video_frame(pFrameRGB, ly->src_codec_ctx->width,
									ly->src_codec_ctx->height, i, qdcp->seq_dir);
				}
			}
		}
		
		//~ av_packet_unref(&packet);
		//~ av_frame_unref(pFrameRGB);
		//~ av_frame_unref(pFrameXYZ);
	}

	/* good bye */

	/* clean buffers */
	//~ av_free_packet(&packet); /* deprecated */
	av_packet_unref(&packet);
	av_free(bufferRGB);
	av_free(bufferXYZ);
	/* clean frames */
	av_frame_free(&pFrame);
	av_frame_free(&pFrameRGB);
	av_frame_free(&pFrameXYZ);
	/* close the codecs */
	avcodec_close(ly->src_codec_ctx);
	/* close the video file */
	avformat_close_input(&ly->src_format_ctx);
	
	return QDCP_SUCCESS;

	/* find a way to link, whatever happens (unless explicitly demanded), 
	 * sound track and video layer (and subs).
	 * - entry_points must be the same
	 * - framerate and samplerate must be closely tied together
	 * - duration must be carefuly controled and synchronized if ever modified
	 * 
	 * For example the sound track could be locked to its video layer,
	 * thus forbidding any modification, but those derived from video alterations.
	 *  */
	 
}
	
void
save_video_frame(AVFrame * pFrame, int width, int height, int iFrame, char * trgt_dir) 
{

		FILE * 	pFile;
		char 	szFilename[32];
		int 	y;
		
	printf(" > saving frame");
	/* open file */
	sprintf(szFilename, "%sframe%d.png", trgt_dir, iFrame);
	pFile = fopen(szFilename, "wb");
	if (pFile == NULL)
		return;
	/* write header (PPM) */
	fprintf(pFile, "P6\n%d %d\n255\n", width, height);
	
	/* write pixel data */
	for (y = 0; y<height; y++)
		fwrite(pFrame->data[0]+y*pFrame->linesize[0], 1, width*3, pFile);
	/* close file */
	fclose(pFile);
}


/* Convert all video streams to mxf with ffmpeg, no quickdcp project */
qdcp_bool
convert_video_to_mxf(const char * video_file, quickdcp_ctx * qdcp) {

	/* close codec context, format, etc... */
	
	return QDCP_SUCCESS;
}


