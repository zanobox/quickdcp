/*
    OpenDCP: Builds Digital Cinema Packages
    Copyright (c) 2010-2013 Terrence Meiczinger, All Rights Reserved

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
	@file asdcp_intf.cpp
	@author jean.sabatier@zanobox.net
	@brief Interface to asdcp C++ library (adapted from OpenDCP's own interface) 
*/

#include <AS_DCP.h>
#include <KM_fileio.h>
#include <KM_prng.h>
#include <KM_memio.h>
#include <KM_util.h>
#include <WavFileWriter.h>
#include <iostream>
//~ #include <assert.h> /* already included in quickdcp.h */

#include "quickdcp.h"


using namespace ASDCP;

const ui32_t FRAME_BUFFER_SIZE = 4 * Kumu::Megabyte;

typedef struct {
    WriterInfo    info;
    AESEncContext *aes_context;
    HMACContext   *hmac_context;
} writer_info_t;


Result_t fill_writer_info(t_project * pj, writer_info_t *writer_info);



/* To avoid pure virtual functions error :
 * (see http://stackoverflow.com/questions/920500/what-is-the-purpose-of-cxa-pure-virtual
 * and http://stackoverflow.com/questions/99552/where-do-pure-virtual-function-call-crashes-come-from?lq=1 )
 * 
 * The __cxa_pure_virtual function is an error handler that is invoked when a pure virtual function is called.
 * If you are writing a C++ application that has pure virtual functions you must supply 
 * your own __cxa_pure_virtual error handler function. 
 * For example: */
extern "C" void __cxa_pure_virtual() { while (1); }

/* generate a random UUID */
extern "C" void uuid_random(char *uuid) 
{
		char buffer[64];
		Kumu::UUID TmpID;
		
	Kumu::GenRandomValue(TmpID);
	//~ printf("%s", TmpID.EncodeHex(buffer, 40));
    sprintf(uuid, "%.36s", TmpID.EncodeHex(buffer, 40));
}

/* calculate the SHA1 digest of a file */
extern "C" int calculate_digest(t_project * pj, const char *filename, char *digest) 
{
    using namespace Kumu;

		FileReader    reader;
		t_sha1        sha_context;
		ByteString    read_buffer(FILE_READ_SIZE);
		ui32_t        read_length;
		const ui32_t  sha_length = 20;
		byte_t        byte_buffer[sha_length];
		char          sha_buffer[64];
		Result_t      result = RESULT_OK;

    result = reader.OpenRead(filename);
    sha1_init(&sha_context);

    while (ASDCP_SUCCESS(result)) {
        read_length = 0;
        result = reader.Read(read_buffer.Data(), read_buffer.Capacity(), &read_length);

        if (ASDCP_SUCCESS(result)) {
            sha1_update(&sha_context, read_buffer.Data(), read_length);

            /* update callback (also check for interrupt) */
            if (pj->dcp.sha1_update.callback(pj->dcp.sha1_update.argument)) {
                return QDCP_CALC_DIGEST;
            }
        }
    }

    if (result == RESULT_ENDOFFILE) {
        result = RESULT_OK;
    }

    if (ASDCP_SUCCESS(result)) {
        sha1_final(byte_buffer, &sha_context);
        sprintf(digest, "%.36s", base64encode(byte_buffer, sha_length, sha_buffer, 64));
    }

    if (pj->dcp.sha1_done.callback(pj->dcp.sha1_done.argument)) {
        return QDCP_CALC_DIGEST;
    }

    if (ASDCP_SUCCESS(result)) {
        return QDCP_NO_ERROR;
    } else {
        return QDCP_CALC_DIGEST;
    }
}

/* get wav duration */
extern "C" int 
get_wav_duration(const char *filename, int framerate) 
{
    PCM::WAVParser       pcm_parser;
    PCM::AudioDescriptor audio_desc;
    Result_t             result   = RESULT_OK;
    ui32_t               duration = 0;

    Rational edit_rate(framerate, 1);

    result = pcm_parser.OpenRead(filename, edit_rate);

    if (ASDCP_SUCCESS(result)) {
        pcm_parser.FillAudioDescriptor(audio_desc);
        duration = audio_desc.ContainerDuration;
    }

    return duration;
}

/* get wav file info  */
extern "C" int get_wav_info(const char *filename, int framerate, t_wav_track *wav) 
{
    PCM::WAVParser       pcm_parser;
    PCM::AudioDescriptor audio_desc;
    Result_t             result   = RESULT_OK;

    Rational edit_rate(framerate, 1);

    result = pcm_parser.OpenRead(filename, edit_rate);

    if (ASDCP_SUCCESS(result)) {
        pcm_parser.FillAudioDescriptor(audio_desc);
        wav->src_samples	= audio_desc.ContainerDuration;
        wav->src_channels	= audio_desc.ChannelCount;
        wav->src_bitdepth  	= audio_desc.QuantizationBits;
        wav->src_samplerate = audio_desc.AudioSamplingRate.Numerator;
    } else {
        return QDCP_FILEOPEN_WAV;
    }

    return QDCP_NO_ERROR;
}

/* get the essence class of a file */
extern "C" int get_file_essence_class(char *filename, int raw) 
{
    Result_t      result = RESULT_OK;
    EssenceType_t essence_type;

    //~ QDCP_LOG(LOG_DEBUG, "Reading file EssenceType: %s", filename);

    if (raw) {
        result = ASDCP::RawEssenceType(filename, essence_type);
    } else {
        result = ASDCP::EssenceType(filename, essence_type);
    }

    /* If type is unknown, return */
    if (ASDCP_FAILURE(result) || essence_type == ESS_UNKNOWN) {
        //~ QDCP_LOG(LOG_DEBUG, "Unable to determine essence type");
        return ACT_UNKNOWN;
    }

    switch (essence_type) {
        case ESS_JPEG_2000:
        case ESS_JPEG_2000_S:
        case ESS_MPEG2_VES:
            return  ACT_PICTURE;
            break;

        case ESS_PCM_24b_48k:
        case ESS_PCM_24b_96k:
            return ACT_SOUND;
            break;

        case ESS_TIMED_TEXT:
            return ACT_TIMED_TEXT;
            break;

        default:
            return ACT_UNKNOWN;
            break;
    }

    return ACT_UNKNOWN;
}

/* get the essence type of an asset */
extern "C" int get_file_essence_type(char *filename) 
{
    Result_t result = RESULT_OK;
    EssenceType_t essence_type;

    result = ASDCP::RawEssenceType(filename, essence_type);

    if (ASDCP_FAILURE(result)) {
        return AET_UNKNOWN;
    }

    switch(essence_type) {
        case ESS_MPEG2_VES:
            return AET_MPEG2_VES;
            break;

        case ESS_JPEG_2000:
            return AET_JPEG_2000;
            break;

        case ESS_PCM_24b_48k:
            return AET_PCM_24b_48k;
            break;

        case ESS_PCM_24b_96k:
            return AET_PCM_24b_96k;
            break;

        case ESS_TIMED_TEXT:
            return AET_TIMED_TEXT;
            break;

        case ESS_DCDATA_UNKNOWN:
            return AET_DCDATA_UNKNOWN;
            break;

        case ESS_DCDATA_DOLBY_ATMOS:
            return AET_DCDATA_DOLBY_ATMOS;
            break;

        default:
            return AET_UNKNOWN;
            break;
    }

    return AET_UNKNOWN;
}

/* read asset file information */
extern "C" qdcp_bool read_asset_info(t_asset *asset) 
{
		EssenceType_t essence_type;
		WriterInfo info;
		Result_t result = RESULT_OK;
		Kumu::UUID uuid;
		char uuid_buffer[64];

    result = ASDCP::EssenceType(asset->filepath, essence_type);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_DETECT_TRACK_TYPE"));
        return QDCP_FAILURE;
    }

    switch (essence_type) {
        case ESS_MPEG2_VES:
        {
            MPEG2::MXFReader reader;
            MPEG2::VideoDescriptor desc;
            result = reader.OpenRead(asset->filepath);

            if (ASDCP_FAILURE(result)) {
				warning(_("QDCP_FILEOPEN_MPEG2"));
				return QDCP_FAILURE;
            }

            reader.FillVideoDescriptor(desc);
            reader.FillWriterInfo(info);

            asset->essence_type       = essence_type;
            asset->essence_class      = ACT_PICTURE;
            asset->duration           = desc.ContainerDuration;
            asset->intrinsic_duration = desc.ContainerDuration;
            asset->entry_point        = 0;
            asset->xml_ns             = info.LabelSetType;
            sprintf(asset->uuid, "%.36s", Kumu::bin2UUIDhex(info.AssetUUID, 16, uuid_buffer, 64));
            sprintf(asset->aspect_ratio, "%d %d", desc.AspectRatio.Numerator, desc.AspectRatio.Denominator);
            sprintf(asset->edit_rate, "%d %d", desc.EditRate.Numerator, desc.EditRate.Denominator);
            sprintf(asset->sample_rate, "%d %d", desc.SampleRate.Numerator, desc.SampleRate.Denominator);
            sprintf(asset->frame_rate, "%d", desc.FrameRate);
            break;
        }

        case ESS_JPEG_2000_S:
        {
            JP2K::MXFSReader reader;
            JP2K::PictureDescriptor desc;
            result = reader.OpenRead(asset->filepath);

            if (ASDCP_FAILURE(result)) {
				warning(_("QDCP_FILEOPEN_J2K_S"));
				return QDCP_FAILURE;
            }

            reader.FillPictureDescriptor(desc);
            reader.FillWriterInfo(info);

            asset->stereoscopic       = 1;
            asset->essence_type       = essence_type;
            asset->essence_class      = ACT_PICTURE;
            asset->duration           = desc.ContainerDuration;
            asset->intrinsic_duration = desc.ContainerDuration;
            asset->entry_point        = 0;
            asset->xml_ns             = info.LabelSetType;
            sprintf(asset->uuid, "%.36s", Kumu::bin2UUIDhex(info.AssetUUID, 16, uuid_buffer, 64));
            sprintf(asset->aspect_ratio, "%d %d", desc.AspectRatio.Numerator, desc.AspectRatio.Denominator);
            sprintf(asset->edit_rate, "%d %d", desc.EditRate.Numerator, desc.EditRate.Denominator);
            sprintf(asset->sample_rate, "%d %d", desc.SampleRate.Numerator, desc.SampleRate.Denominator);
            sprintf(asset->frame_rate, "%d %d", desc.SampleRate.Numerator, desc.SampleRate.Denominator);
            break;
        }

        case ESS_JPEG_2000:
        {
            JP2K::MXFReader reader;
            JP2K::PictureDescriptor desc;
            result = reader.OpenRead(asset->filepath);

            /* Try 3D MXF Interop */
            if (ASDCP_FAILURE(result)) {
                JP2K::MXFSReader reader;
                result = reader.OpenRead(asset->filepath);
                asset->stereoscopic   = 1;

                if ( ASDCP_FAILURE(result) ) {
					warning(_("QDCP_FILEOPEN_J2K"));
					return QDCP_FAILURE;
                }

                reader.FillPictureDescriptor(desc);
                reader.FillWriterInfo(info);
            } else {
                reader.FillPictureDescriptor(desc);
                reader.FillWriterInfo(info);
            }

            asset->essence_type        	= essence_type;
            asset->essence_class       	= ACT_PICTURE;
            asset->duration           	= desc.ContainerDuration;
            asset->intrinsic_duration  	= desc.ContainerDuration;
            asset->entry_point    		= 0;
            asset->xml_ns         		= info.LabelSetType;
            sprintf(asset->uuid, "%.36s", Kumu::bin2UUIDhex(info.AssetUUID, 16, uuid_buffer, 64));
            sprintf(asset->aspect_ratio, "%d %d", desc.AspectRatio.Numerator, desc.AspectRatio.Denominator);
            sprintf(asset->edit_rate, "%d %d", desc.EditRate.Numerator, desc.EditRate.Denominator);
            sprintf(asset->sample_rate, "%d %d", desc.SampleRate.Numerator, desc.SampleRate.Denominator);
            sprintf(asset->frame_rate, "%d %d", desc.SampleRate.Numerator, desc.SampleRate.Denominator);
            break;
        }

        case ESS_PCM_24b_48k:
        case ESS_PCM_24b_96k:
        {
            PCM::MXFReader reader;
            PCM::AudioDescriptor desc;
            result = reader.OpenRead(asset->filepath);

            if (ASDCP_FAILURE(result)) {
				warning(_("QDCP_FILEOPEN_WAV"));
				return QDCP_FAILURE;
            }

            reader.FillAudioDescriptor(desc);
            reader.FillWriterInfo(info);

            asset->essence_type       = essence_type;
            asset->essence_class      = ACT_SOUND;
            asset->duration           = desc.ContainerDuration;
            asset->intrinsic_duration = desc.ContainerDuration;
            asset->entry_point    	= 0;
            asset->xml_ns         	= info.LabelSetType;
            sprintf(asset->uuid, "%.36s", Kumu::bin2UUIDhex(info.AssetUUID, 16, uuid_buffer, 64));
            sprintf(asset->edit_rate, "%d %d", desc.EditRate.Numerator, desc.EditRate.Denominator);
            sprintf(asset->sample_rate, "%d %d", desc.AudioSamplingRate.Numerator, desc.AudioSamplingRate.Denominator);
            break;
        }

        case ESS_TIMED_TEXT:
        {
            TimedText::MXFReader reader;
            TimedText::TimedTextDescriptor desc;
            result = reader.OpenRead(asset->filepath);

            if (ASDCP_FAILURE(result)) {
				warning(_("QDCP_FILEOPEN_TT"));
				return QDCP_FAILURE;
            }

            reader.FillTimedTextDescriptor(desc);
            reader.FillWriterInfo(info);

            asset->essence_type       = essence_type;
            asset->essence_class      = ACT_TIMED_TEXT;
            asset->duration           = desc.ContainerDuration;
            asset->intrinsic_duration = desc.ContainerDuration;
            asset->entry_point    = 0;
            asset->xml_ns         = info.LabelSetType;
            sprintf(asset->uuid, "%.36s", Kumu::bin2UUIDhex(info.AssetUUID, 16, uuid_buffer, 64));
            sprintf(asset->edit_rate, "%d %d", desc.EditRate.Numerator, desc.EditRate.Denominator);
            break;
        }

        default:
        {
			warning(_("QDCP_UNKNOWN_TRACK_TYPE"));
			return QDCP_FAILURE;
            break;
        }
    }

    /* add encrypted info, if applicable */
    if (info.EncryptedEssence) {
        asset->encrypted  = 1;
        sprintf(asset->key_id, "%.36s", Kumu::bin2UUIDhex(info.CryptographicKeyID, 16, uuid_buffer, 64));
    } else {
        asset->encrypted  = 0;
    }

    return QDCP_SUCCESS;
}


Result_t fill_writer_info(t_project * pj, writer_info_t *writer_info) 
{

		Kumu::FortunaRNG        rng;
		byte_t                  iv_buf[CBC_BLOCK_SIZE];
		Result_t                result = RESULT_OK;

    writer_info->info.ProductVersion = QDCP_VERSION;
    writer_info->info.CompanyName = "zanobox.net";
    writer_info->info.ProductName = QDCP_NAME;

    /* set the label type */
    /* INTEROP is deprecated we won't implement it.
     * (sorry for nostalgic digital cinema operators) */
    writer_info->info.LabelSetType = LS_MXF_SMPTE;

    /* generate a random UUID for this essence */
    Kumu::GenRandomUUID(writer_info->info.AssetUUID);

    /* start encryption, if set */
    if (pj->mxf.key_flag) {
        Kumu::GenRandomUUID(writer_info->info.ContextID);
        writer_info->info.EncryptedEssence = true;

        if (is_key_value_set(pj->mxf.key_id, sizeof(pj->mxf.key_id))) {
            memcpy(writer_info->info.CryptographicKeyID, pj->mxf.key_id, UUIDlen);
        } else {
            rng.FillRandom(writer_info->info.CryptographicKeyID, UUIDlen);
        }

        writer_info->aes_context = new AESEncContext;
        result = writer_info->aes_context->InitKey(pj->mxf.key_value);

        if (ASDCP_FAILURE(result)) {
            return result;
        }

        result = writer_info->aes_context->SetIVec(rng.FillRandom(iv_buf, CBC_BLOCK_SIZE));

        if (ASDCP_FAILURE(result)) {
            return result;
        }

        if (pj->mxf.write_hmac) {
            writer_info->info.UsesHMAC = true;
            writer_info->hmac_context = new HMACContext;
            result = writer_info->hmac_context->InitKey(pj->mxf.key_value, writer_info->info.LabelSetType);

            if (ASDCP_FAILURE(result)) {
                return result;
            }
        }
    }

    return result;
}


/* write out j2k mxf file */
extern "C" qdcp_bool 
convert_j2k_to_mxf(t_project * pj, char *output_file) 
{
	
		JP2K::MXFWriter         mxf_writer;
		JP2K::PictureDescriptor picture_desc;
		JP2K::CodestreamParser  j2k_parser;
		JP2K::FrameBuffer       frame_buffer(FRAME_BUFFER_SIZE);
		writer_info_t           writer_info;
		Result_t                result = RESULT_OK;
		ui32_t                  start_frame;
		ui32_t                  mxf_duration;
		ui32_t                  slide_duration = 0;

		EssenceType_t 			essence_type;

    result = ASDCP::RawEssenceType(pj->j2k_files->files[0], essence_type);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_DETECT_TRACK_TYPE"));
        return QDCP_FAILURE;
    }

    /* set the starting frame */
    start_frame = 0;
  
    //~ QDCP_LOG(LOG_DEBUG, "j2k_parser.OpenReadFrame(%s)", pj->j2k_files->files[start_frame]);
    result = j2k_parser.OpenReadFrame(pj->j2k_files->files[start_frame], frame_buffer);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_FILEOPEN_J2K"));
        return QDCP_FAILURE;
    }

    Rational edit_rate(pj->framerate, 1);
    j2k_parser.FillPictureDescriptor(picture_desc);
    picture_desc.EditRate = edit_rate;

    fill_writer_info(pj, &writer_info);

    result = mxf_writer.OpenWrite(output_file, writer_info.info, picture_desc);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    /* set the duration of the output mxf */
    mxf_duration = pj->nmax;

    ui32_t read  = 1;
    ui32_t i = start_frame;

    /* read each input frame and write to the output mxf until duration is reached */
    while ( ASDCP_SUCCESS(result) && mxf_duration--) {
        if (read) {
            result = j2k_parser.OpenReadFrame(pj->j2k_files->files[i], frame_buffer);

            if (pj->mxf.delete_intermediate) {
                unlink(pj->j2k_files->files[i]);
            }

            if (ASDCP_FAILURE(result)) {
				warning(_("QDCP_FILEOPEN_J2K"));
				return QDCP_FAILURE;
            }

            if (pj->mxf.encrypt_header_flag) {
                frame_buffer.PlaintextOffset(0);
            }
        }

        i++;

        /* write the frame */
        result = mxf_writer.WriteFrame(frame_buffer, writer_info.aes_context, writer_info.hmac_context);

        /* frame done callback (also check for interrupt) */
        if (pj->mxf.frame_done.callback(pj->mxf.frame_done.argument)) {
            return QDCP_SUCCESS;
        }
    }

    if (result == RESULT_ENDOFFILE) {
        result = RESULT_OK;
    }

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_FILEWRITE_MXF"));
        return QDCP_FAILURE;
    }

    result = mxf_writer.Finalize();

    /* file done callback */
    pj->mxf.file_done.callback(pj->mxf.file_done.argument);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_FINALIZE_MXF"));
        return QDCP_FAILURE;
    }

    return QDCP_SUCCESS;
}

/* write out 3D j2k mxf file */
extern "C" qdcp_bool
convert_j2k3D_to_mxf(t_project * pj, char *output_file) 
{
		JP2K::MXFSWriter        mxf_writer;
		JP2K::PictureDescriptor picture_desc;
		JP2K::CodestreamParser  j2k_parser_left;
		JP2K::CodestreamParser  j2k_parser_right;
		JP2K::FrameBuffer       frame_buffer_left(FRAME_BUFFER_SIZE);
		JP2K::FrameBuffer       frame_buffer_right(FRAME_BUFFER_SIZE);
		writer_info_t           writer_info;
		Result_t                result = RESULT_OK;
		ui32_t                  start_frame;
		ui32_t                  mxf_duration;
		ui32_t                  slide_duration = 0;

		EssenceType_t 			essence_type;

    result = ASDCP::RawEssenceType(pj->j2k_files->files[0], essence_type);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_DETECT_TRACK_TYPE"));
        return QDCP_FAILURE;
    }

    /* set the starting frame */
    if (pj->mxf.start_frame && pj->j2k_files->nfiles >= (pj->mxf.start_frame - 1)) {
        start_frame = pj->mxf.start_frame - 1; /* adjust for zero base */
    } else {
        start_frame = 0;
    }

    result = j2k_parser_left.OpenReadFrame(pj->j2k_files->files[0], frame_buffer_left);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_J2K;
    }

    result = j2k_parser_right.OpenReadFrame(pj->j2k_files->files[1], frame_buffer_right);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_J2K;
    }

    Rational edit_rate(pj->framerate, 1);
    j2k_parser_left.FillPictureDescriptor(picture_desc);
    picture_desc.EditRate = edit_rate;

    fill_writer_info(pj, &writer_info);

    result = mxf_writer.OpenWrite(output_file, writer_info.info, picture_desc);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    /* set the duration of the output mxf, set to half the filecount since it is 3D */
    if ((pj->j2k_files->nfiles / 2) < pj->nmax || !pj->nmax) {
        mxf_duration = pj->j2k_files->nfiles / 2;
    } else {
        mxf_duration = pj->nmax;
    }

    /* set the duration of the output mxf, set to half the filecount since it is 3D */
    if (pj->mxf.slide) {
        mxf_duration = pj->mxf.duration;
        slide_duration = mxf_duration / (pj->j2k_files->nfiles / 2);
    } else if (pj->mxf.duration && (pj->j2k_files->nfiles / 2 >= pj->mxf.duration)) {
        mxf_duration = pj->mxf.duration;
    } else {
        mxf_duration = pj->j2k_files->nfiles / 2;
    }

    ui32_t read = 1;
    ui32_t i = start_frame;

    /* read each input frame and write to the output mxf until duration is reached */
    while (ASDCP_SUCCESS(result) && mxf_duration--) {
        if (read) {
            result = j2k_parser_left.OpenReadFrame(pj->j2k_files->files[i], frame_buffer_left);

            if (pj->mxf.delete_intermediate) {
                unlink(pj->j2k_files->files[i]);
            }

            if (ASDCP_FAILURE(result)) {
                return QDCP_FILEOPEN_J2K;
            }

            i++;

            result = j2k_parser_right.OpenReadFrame(pj->j2k_files->files[i], frame_buffer_right);

            if (pj->mxf.delete_intermediate) {
                unlink(pj->j2k_files->files[i]);
            }

            if (ASDCP_FAILURE(result)) {
                return QDCP_FILEOPEN_J2K;
            }

            if (pj->mxf.encrypt_header_flag) {
                frame_buffer_left.PlaintextOffset(0);
            }

            if (pj->mxf.encrypt_header_flag) {
                frame_buffer_right.PlaintextOffset(0);
            }

            if (pj->mxf.slide) {
                read = 0;
            }
        }

        if (pj->mxf.slide) {
            if (mxf_duration % slide_duration == 0) {
                i++;
                read = 1;
            }
        } else {
            i++;
        }

        /* write the frame */
        result = mxf_writer.WriteFrame(frame_buffer_left, JP2K::SP_LEFT, writer_info.aes_context, writer_info.hmac_context);

        /* frame done callback (also check for interrupt) */
        pj->mxf.frame_done.callback(pj->mxf.frame_done.argument);

        /* write the frame */
        result = mxf_writer.WriteFrame(frame_buffer_right, JP2K::SP_RIGHT, writer_info.aes_context, writer_info.hmac_context);

        /* frame done callback (also check for interrupt) */
        pj->mxf.frame_done.callback(pj->mxf.frame_done.argument);
    }

    if (result == RESULT_ENDOFFILE) {
        result = RESULT_OK;
    }

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    result = mxf_writer.Finalize();

    pj->mxf.file_done.callback(pj->mxf.file_done.argument);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FINALIZE_MXF;
    }

    return QDCP_NO_ERROR;
}

/* write out pcm audio mxf file */
extern "C" qdcp_bool 
convert_pcm_to_mxf(t_project * pj, char *output_file) 
{
	
		PCM::FrameBuffer     frame_buffer;
		PCM::AudioDescriptor audio_desc;
		PCM::MXFWriter       mxf_writer;
		writer_info_t        writer_info;
		Result_t             result = RESULT_OK;
		ui32_t               mxf_duration;
		i32_t                file_index = 0;

		PCM::WAVParser       pcm_parser_channel[MAX_AUDIO_CHANNELS];
		PCM::FrameBuffer     frame_buffer_channel[MAX_AUDIO_CHANNELS];
		PCM::AudioDescriptor audio_desc_channel[MAX_AUDIO_CHANNELS];

		EssenceType_t 			essence_type;

    result = ASDCP::RawEssenceType(pj->j2k_files->files[0], essence_type);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_DETECT_TRACK_TYPE"));
        return QDCP_FAILURE;
    }

    Rational edit_rate(pj->framerate, 1);

    /* read first file */
    result = pcm_parser_channel[0].OpenRead(pj->tracks->filenames[0], edit_rate);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_WAV;
    }

    /* read audio descriptor */
    pcm_parser_channel[0].FillAudioDescriptor(audio_desc);
    audio_desc.ChannelCount = 0;
    audio_desc.BlockAlign   = 0;

    for (file_index = 0; file_index < pj->tracks->size; file_index++) {
        result = pcm_parser_channel[file_index].OpenRead(pj->tracks->filenames[file_index], edit_rate);

        if (ASDCP_FAILURE(result)) {
            //~ QDCP_LOG(LOG_ERROR, "could not open %s", pj->tracks->filenames[file_index]);
            return QDCP_FILEOPEN_WAV;
        }

        pcm_parser_channel[file_index].FillAudioDescriptor(audio_desc_channel[file_index]);

        if (audio_desc_channel[file_index].AudioSamplingRate != audio_desc.AudioSamplingRate) {
            //~ QDCP_LOG(LOG_ERROR, "mismatched sampling rate");
            return QDCP_FILEOPEN_WAV;
        }

        if (audio_desc_channel[file_index].QuantizationBits != audio_desc.QuantizationBits) {
            //~ QDCP_LOG(LOG_ERROR, "mismatched bit rate");
            return QDCP_FILEOPEN_WAV;
        }

        if (audio_desc_channel[file_index].ContainerDuration != audio_desc.ContainerDuration) {
            //~ QDCP_LOG(LOG_ERROR, "mismatched duration");
            return QDCP_FILEOPEN_WAV;
        }

        frame_buffer_channel[file_index].Capacity(PCM::CalcFrameBufferSize(audio_desc_channel[file_index]));
    }

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_WAV;
    }

    /*  set total audio characteristics */
    for (file_index = 0; file_index < pj->tracks->size; file_index++) {
        audio_desc.ChannelCount += audio_desc_channel[file_index].ChannelCount;
    }

    audio_desc.BlockAlign = audio_desc.ChannelCount * 24 / 8;
    audio_desc.EditRate = edit_rate;
    audio_desc.AvgBps   = audio_desc.AvgBps * pj->tracks->size;

    /* set total frame buffer size */
    frame_buffer.Capacity(PCM::CalcFrameBufferSize(audio_desc));
    frame_buffer.Size(PCM::CalcFrameBufferSize(audio_desc));

    /* fill write info */
    fill_writer_info(pj, &writer_info);
    result = mxf_writer.OpenWrite(output_file, writer_info.info, audio_desc);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    /* set duration */
    if (pj->wav_files->nfiles == 0) {
        mxf_duration = 0xffffffff;
    } else {
        mxf_duration = pj->nmax;
    }

    /* start parsing */
    while (ASDCP_SUCCESS(result) && mxf_duration--) {
        byte_t *data_s = frame_buffer.Data();
        byte_t *data_e = data_s + frame_buffer.Capacity();
        byte_t sample_size = PCM::CalcSampleSize(audio_desc_channel[0]);
        int    offset = 0;

        /* read a frame from each file */
        for (file_index = 0; file_index < pj->tracks->size; file_index++) {
            memset(frame_buffer_channel[file_index].Data(), 0, frame_buffer_channel[file_index].Capacity());
            result = pcm_parser_channel[file_index].ReadFrame(frame_buffer_channel[file_index]);

            if (ASDCP_FAILURE(result)) {
                continue;
            }

            if (frame_buffer_channel[file_index].Size() != frame_buffer_channel[file_index].Capacity()) {
                //~ QDCP_LOG(LOG_INFO, "frame size mismatch, expect size: %d did match actual size: %d",
                //~ 		frame_buffer_channel[file_index].Capacity(), frame_buffer_channel[file_index].Size());
                result = RESULT_ENDOFFILE;
                continue;
            }
        }

        /* write sample from each frame to output buffer */
        if (ASDCP_SUCCESS(result)) {
            while (data_s < data_e) {
                for (file_index = 0; file_index < pj->tracks->size; file_index++) {
                    byte_t *frame = frame_buffer_channel[file_index].Data() + offset;
                    memcpy(data_s, frame, sample_size);
                    data_s += sample_size;
                }

                offset += sample_size;
            }

            /* write the frame */
            result = mxf_writer.WriteFrame(frame_buffer, writer_info.aes_context, writer_info.hmac_context);

            /* frame done callback (also check for interrupt) */
            pj->mxf.frame_done.callback(pj->mxf.frame_done.argument);
        }
    }

    if (result == RESULT_ENDOFFILE) {
        result = RESULT_OK;
    }

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    /* write footer information */
    result = mxf_writer.Finalize();

    pj->mxf.file_done.callback(pj->mxf.file_done.argument);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FINALIZE_MXF;
    }

    return QDCP_NO_ERROR;
}

/* write out timed text mxf file */
extern "C" qdcp_bool 
convert_timedtext_to_mxf(t_project * pj, char *output_file) 
{
		TimedText::DCSubtitleParser    tt_parser;
		TimedText::MXFWriter           mxf_writer;
		TimedText::FrameBuffer         frame_buffer(FRAME_BUFFER_SIZE);
		TimedText::TimedTextDescriptor tt_desc;
		TimedText::ResourceList_t::const_iterator resource_iterator;
		writer_info_t                  writer_info;
		std::string                    xml_doc;
		Result_t                       result = RESULT_OK;

 		EssenceType_t 			essence_type;

    result = ASDCP::RawEssenceType(pj->j2k_files->files[0], essence_type);

    if (ASDCP_FAILURE(result)) {
		warning(_("QDCP_DETECT_TRACK_TYPE"));
        return QDCP_FAILURE;
    }

   result = tt_parser.OpenRead(pj->subs_files->files[0]);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_TT;
    }

    tt_parser.FillTimedTextDescriptor(tt_desc);

    fill_writer_info(pj, &writer_info);

    result = mxf_writer.OpenWrite(output_file, writer_info.info, tt_desc);
    result = tt_parser.ReadTimedTextResource(xml_doc);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEOPEN_TT;
    }

    result = mxf_writer.WriteTimedTextResource(xml_doc, writer_info.aes_context, writer_info.hmac_context);

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    resource_iterator = tt_desc.ResourceList.begin();

    while (ASDCP_SUCCESS(result) && resource_iterator != tt_desc.ResourceList.end()) {
        result = tt_parser.ReadAncillaryResource((*resource_iterator++).ResourceID, frame_buffer);

        if (ASDCP_FAILURE(result)) {
            return QDCP_FILEOPEN_TT;
        }

        result = mxf_writer.WriteAncillaryResource(frame_buffer, writer_info.aes_context, writer_info.hmac_context);
    }

    if (result == RESULT_ENDOFFILE) {
        result = RESULT_OK;
    }

    if (ASDCP_FAILURE(result)) {
        return QDCP_FILEWRITE_MXF;
    }

    result = mxf_writer.Finalize();

    if (ASDCP_FAILURE(result)) {
        return QDCP_FINALIZE_MXF;
    }

    return QDCP_NO_ERROR;
}

