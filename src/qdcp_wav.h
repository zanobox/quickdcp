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
   @file    qdcp_wav.h
   @author  jean.sabatier@zanobox.net
   @brief   Build Digital Cinema Packages in command line interface.
*/

#ifndef QDCP_WAV_H_INCLUDED
#define QDCP_WAV_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <pthread.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/parser.h>
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"
#include "libavutil/log.h"
#include "libavutil/intreadwrite.h"
#include "libavutil/display.h"
#include "libavutil/avstring.h"
#include "libavutil/replaygain.h"
#include "libavutil/stereo3d.h"
#include "libavcodec/avcodec.h"
#include "libavutil/channel_layout.h"
#include "libavutil/common.h"
//~ #include <libavutil/imgutils.h>
#include "libavutil/mathematics.h"
#include "libavutil/samplefmt.h"
#include "quickdcp.h"



/*--------------------------- QuickDCP API ---------------------------*/


/** 
 * @brief Allocate a new empty audio sub-project.
 * 
 * @param	track	A pointer to the t_wav_track structure to be fed.
 * @param	qdcp	The quickdcp context in which all this is done.
 * @return  A boolean, either QDCP_SUCCESS, or ... a segmentation fault !
 */
qdcp_bool 	
init_wav_track(
	t_wav_track * track,
	quickdcp_ctx * qdcp
	);

/**
 * @brief Create a new wav_project from an audio file. 
 * 
 * @param	nw		A pointer to the audio sub-project to configure.
 * @param	file	A pointer to a file object describing an audio file.
 * @param	qdcp	The quickdcp_ctx where everything happens.
 * @return A qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE.
 */
qdcp_bool
import_audio_file(
	t_wav_track * nw, 
	char * file, 
	quickdcp_ctx * qdcp
	);

/**
 * @brief Append an audio sub-project to an audio sub-projects list.
 * 
 * @param	track	A pointer to the t_wav_track audio sub-project.
 * @param	wpjl	A pointer to the t_track_list.
 * @return qdcp_bool : QDCP_SUCCESS or QDCP_FAILURE
 */
qdcp_bool
add_wav_to_list(
	t_wav_track * track, 
	t_track_list * wpjl
	);

qdcp_bool
save_wav_subproject(
	t_wav_track * track, 
	quickdcp_ctx * qdcp
	);

qdcp_bool
save_wav_track(
	xmlTextWriterPtr writer, 
	t_wav_track * track, 
	quickdcp_ctx * qdcp
	);

t_wav_track *
load_wav_subproject(
	const char * qdw_path, 
	t_track_list * wpl, 
	quickdcp_ctx * qdcp
	);

t_wav_track * 
load_wav_track(
	const char * qdp_file, 
	t_track_list * wpl, 
	quickdcp_ctx * qdcp
	);

t_track_list * 	
scan_wav_projects(
	quickdcp_ctx * qdcp
	);

qdcp_bool 					
add_track_to_project(
	t_wav_track * track, 
	t_project * pj, 
	quickdcp_ctx * qdcp
	);

/**
 * This is not implemented yet. Should be used to 'flatten' different
 * audio tracks on a same project, as the one and only wav file with the 
 * required number of channels.
 */
qdcp_bool 			
render_audio(
	t_project * pj, 
	quickdcp_ctx * qdcp
	);



/*---------------------------- FFmpeg API ----------------------------*/



/**
 * @brief Extract information about an audio file
 * and store it into a t_wav_track object.
 * 
 * The function looks for a duration, a number of samples, a sample rate,
 * a bit depth (bit per sample), a number of channels, some volume information,
 * some metadata (annotation text), and possibly more if available.
 * 
 * @param	track		A pointer to a t_wav_track structure.
 * @param	verbosity	Verbosity level of the function's feedback in console.
 * @return A qdcp_bool, either QDCP_SUCCESS or QDCP_FAILURE.
 */
qdcp_bool
read_audio_input(
	t_wav_track * track,
	int verbosity
	);

/**
 * Open audio stream from an input file
 * thanks to FFmpeg API.
 * 
 * Populates track->in_format_ctx, track->audio_stream, 
 * track->codec_params, track->codec_ctx
 */
qdcp_bool
open_input_file(
	t_wav_track * track
	);

/**
 * @brief Open an output file thanks to FFmpeg API.
 * 
 * The output file has a computed name from the basename of input file.
 * - Sample rate should be 48000 Hz.
 * - Channel layout would be 5.1 (6 channels) by default, with a possibility
 * of 7.1 (8 channels). For Dolby, etc. we'll see later.
 * - Bit depth should be 24 bits per sample. So we need a sample format of signed 32 bits (s32),
 * shifted like 0x12345600 (in 32 bits) to get 0x123456 (in 24 bits).
 * - I don't know about bit rate. Should it be arbitrary ? Or based on what ?
 * - Frame size also still puzzles me, together with time reference.
 * 
 * @param track A t_wav_track structure, either orphan or t_project dependant.
 * @param verbosity The level of verbosity expected from the process.
 * @return A boolean, QDCP_SUCCESS or QDCP_FAILURE.
 */
qdcp_bool
open_output_file(
	t_wav_track * track,
	int verbosity
	);

/**
 * Select the input channel layout with the maximum number
 * of channels, based on the input codec.
 */
int
select_channel_layout(
	AVCodec *codec
	);

/**
 * Make sure the given sample format is compatible with
 * the given codec.
 */
int
check_sample_fmt(
	AVCodec *codec,
	enum AVSampleFormat sample_fmt
	);

/**
 * @brief Converting an imported audio file into the required
 * format, according to current QuickDCP configuration.
 */
qdcp_bool		
convert_audio_2_wav(
	t_wav_track * track, 
	quickdcp_ctx * qdcp
	);

/** 
 * Write the header of the output file container. 
 */
int 
write_output_file_header(
	AVFormatContext *output_format_context
	);

/** 
 * Initialize one data packet for reading or writing. 
 */
static void 
init_packet(
	AVPacket *packet
	);

/** 
 * @brief Initialize one audio frame for reading from the input file.
 */
int 
init_input_frame(
	AVFrame **frame
	);
	
/**
 * @brief Initialize the audio resampler based on the input and output codec settings.
 * 
 * If the input and output sample formats differ, a conversion is required
 * libswresample takes care of this, but requires initialization.
 * For this, we need at least to have defined upstream 
 * the source sample format, channel layout and sample rate
 * and the target sample format, channel layout and sample rate.
 * 
 * @param	track		The t_wav_track which the audio conversion happens to.
 * @param	resample_context	The SwrContext structure to initialize for further resampling.
 * @param	verbosity	The required level of verbosity of the whole process (inherited from the quickdcp context).
 * @return 	A int containing an negative error value on failure, or zero on success.
 */
int 
init_resampler(
	t_wav_track * track,
    SwrContext **resample_context,
    int verbosity
    );
    
/** 
 * Initialize a FIFO buffer for the audio samples to be encoded. */
int 
init_fifo(
	AVAudioFifo **fifo, 
	AVCodecContext *output_codec_context
	);

/** 
 * Decode one audio frame from the input file. */
int 
decode_audio_frame(
	AVFrame *frame,
    AVFormatContext *input_format_context,
    AVCodecContext *input_codec_context,
    int *data_present, int *finished
    );
                               
/**
 * Initialize a temporary storage for the specified number of audio samples.
 * The conversion requires temporary storage due to the different format.
 * The number of audio samples to be allocated is specified in frame_size.
 */
int 
init_converted_samples(
	uint8_t ***converted_input_samples,
    AVCodecContext *output_codec_context,
    int frame_size
    );
    
/**
 * Convert the input audio samples into the output sample format.
 * The conversion happens on a per-frame basis, the size of which is specified
 * by frame_size.
 */
int 
convert_samples(
	const uint8_t **input_data,
    uint8_t **converted_data, 
    const int frame_size,
    SwrContext *resample_context
    );

/** 
 * Add converted input audio samples to the FIFO buffer for later processing. 
 */
int 
add_samples_to_fifo(
	AVAudioFifo *fifo,
   uint8_t **converted_input_samples,
   const int frame_size
   );

/**
 * Read one audio frame from the input file, decodes, converts and stores
 * it in the FIFO buffer.
 */
int 
read_decode_convert_and_store(
	AVAudioFifo *fifo,
	AVFormatContext *input_format_context,
    AVCodecContext *input_codec_context,
    AVCodecContext *output_codec_context,
    SwrContext *resampler_context,
    int *finished
    );
  
/**
 * Initialize one input frame for writing to the output file.
 * The frame will be exactly frame_size samples large.
 */
int 
init_output_frame(
	AVFrame **frame,
    AVCodecContext *output_codec_context,
    int frame_size
    );    

/** 
 * Encode one frame worth of audio to the output file. 
 */
int 
encode_audio_frame(
	AVFrame *frame,
    AVFormatContext *output_format_context,
    AVCodecContext *output_codec_context,
    int *data_present
    );
    
/**
 * Load one audio frame from the FIFO buffer, encode and write it to the
 * output file.
 */
int 
load_encode_and_write(
	AVAudioFifo *fifo,
    AVFormatContext *output_format_context,
    AVCodecContext *output_codec_context
    );

/** 
 * Write the trailer of the output file container.
 */
int 
write_output_file_trailer(
	AVFormatContext * output_format_context
	);                          
                                                                    

#endif // QDCP_WAV_H_INCLUDED
