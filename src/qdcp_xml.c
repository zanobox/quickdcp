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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgen.h>
#include <time.h>
#include <inttypes.h>
#include <sys/stat.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include "quickdcp.h"

extern int write_dsig_template(t_project * pj, xmlTextWriterPtr xml);

#ifdef WIN32
char *
strsep (char **stringp, const char *delim) 
{
    register char *s;
    register const char *spanp;
    register int c, sc;
    char *tok;

    if ((s = *stringp) == NULL)
        return (NULL);

    for (tok = s;;) {
        c = *s++;
        spanp = delim;

        do {
            if ((sc = *spanp++) == c) {
                if (c == 0)
                    s = NULL;
                else
                    s[-1] = 0;

                *stringp = s;
                return (tok);
            }
        } while (sc != 0);
    }
}
#endif

qdcp_bool
finalise_dcp(t_project * pj, quickdcp_ctx * qdcp)
{
		int 			r, a;
		char 			buffer[80];
		int  			total = 0;
		int  			read_size = 16384;
		t_pkl 			pkl;
		t_cpl 			cpl;
		t_file_list *	mxf_files = new_file_list();
		file_obj	*	mxf;
		t_reel_list 	reel_list[MAX_REELS];
		int 			reel_count = 0;

	/* dcp_t struct first actualisation */
	
	sprintf(pj->dcp.basename, 		"%s", 		pj->name);
    sprintf(pj->dcp.annotation, 	"%s", 		pj->dcp_name);
    get_timestamp(pj->dcp.issue_date);
    sprintf(pj->dcp.issuer, 		"%s", 		pj->issuer);
    sprintf(pj->dcp.creator, 		"%s-%s", 	QDCP_NAME, QDCP_VERSION);
    sprintf(pj->dcp.title, 			"%s", 		pj->dcp_name);
    sprintf(pj->dcp.kind, 			"%s", 		qdcp->dcp_categories[pj->category]);
    // ContentVersion ? Id + LabelText
	pj->dcp.rating[0] = 0;
    sprintf(pj->dcp.aspect_ratio, 	"%d %d", 	pj->mx_w, pj->mx_h);
	pj->dcp.pkl_count	= 0;
    //~ t_qdcp_cb   	sha1_update;
    //~ t_qdcp_cb		sha1_done;
	
	/* Get list of mxf files here */ 	
	scan_source_directory(pj->tmp_dcp_dir, 0, "mxf", mxf_files);
	
	/* and attach them to their reel */	
	a = 0;
	mxf = mxf_files->first;
	while (mxf != NULL)
	{
		/* For the time being, reel_list always contains only one reel (index 0).
		 * 
		 * A reel is composed of maximum 3 assets.
		 * One for images, one for sound and one for subs.
		 */
		sprintf(reel_list[reel_count].asset_list[a++].filename, "%s", mxf->fname);
		reel_list[reel_count].asset_count = a;
		
		mxf = mxf->next;
	}

    /* check cert files */
    if (pj->xml_signature.sign && pj->xml_signature.use_external == 1) {
		//~ if (qdcp->verbosity >= VERBOSE_VERBOSE)			
		
        FILE *tp;

        if (pj->xml_signature.root) {
            tp = fopen(pj->xml_signature.root, "rb");

            if (tp) {
                fclose(tp);
            }
            else {
                error_msg("Could not read root certificate", __FILE__, __LINE__);
            }
        }
        else {
            error_msg("XML digital signature certificates enabled, but root certificate file not specified", __FILE__, __LINE__);
        }

        if (pj->xml_signature.ca) {
            tp = fopen(pj->xml_signature.ca, "rb");

            if (tp) {
                fclose(tp);
            }
            else {
                error_msg("Could not read ca certificate", __FILE__, __LINE__);
            }
        }
        else {
            error_msg("XML digital signature certificates enabled, but ca certificate file not specified", __FILE__, __LINE__);
        }

        if (pj->xml_signature.signer) {
            tp = fopen(pj->xml_signature.signer, "rb");

            if (tp) {
                fclose(tp);
            }
            else {
                error_msg("Could not read signer certificate", __FILE__, __LINE__);
            }
        }
        else {
            error_msg("XML digital signature certificates enabled, but signer certificate file not specified", __FILE__, __LINE__);
        }

        if (pj->xml_signature.private_key) {
            tp = fopen(pj->xml_signature.private_key, "rb");

            if (tp) {
                fclose(tp);
            }
            else {
                error_msg("Could not read private key file", __FILE__, __LINE__);
            }
        }
        else {
            error_msg("XML digital signature certificates enabled, but private key file not specified", __FILE__, __LINE__);
        }
    }

    /* add empty pkl to the DCP (only one PKL currently supported) */
    create_pkl(pj, &pkl, qdcp);
    add_pkl_to_dcp(&pj->dcp, pkl);

    /* add empty cpl to the DCP/PKL (only one CPL currently supported) */
    create_cpl(pj, &cpl, qdcp);
    add_cpl_to_pkl(&pj->dcp.pkl[0], cpl);

    /* Add assets to the asset list of each reel and validate reel */
    for (r = 0; r <= reel_count; r++) {

			 t_reel 	reel;
        
        create_reel(pj, &reel);
        /* reel has now Id and AnnotationText fields */

		/* loop assets of each reel (but we should generally only have one reel) */
        for (a = 0; a < reel_list[r].asset_count; a++) 
        {	
				t_asset asset;
        
            new_asset(pj, &asset, reel_list[r].asset_list[a].filename, qdcp->verbosity);
            /* new asset has now a size, a filepath, an annotation
             * a name_space, a duration, a uuid, an aspect_ratio, 
             * an essence_type, an essence_class, etc. */
                                  
            add_asset_to_reel(pj, &reel, asset);
        }
	
        if (validate_reel(pj, &reel, r)) {
            add_reel_to_cpl(&pj->dcp.pkl[0].cpl[0], reel);
            printf("%s%s%sBobine %d ajoutée au CPL %s\n", 
				CLI_INDENT, CLI_GREEN, CLI_INDENT, r+1, CLI_COLOR);
        }
        else 
        {
            warning("Could not validate reel");
            return QDCP_FAILURE;
        }
    }

    /* set ASSETMAP and VOLINDEX paths */
    if (pj->ns == XML_NS_SMPTE) {
        sprintf(pj->dcp.assetmap.filename, "%s%s", pj->tmp_dcp_dir, "ASSETMAP.xml");
        sprintf(pj->dcp.volindex.filename, "%s%s", pj->tmp_dcp_dir, "VOLINDEX.xml");
    }
    else 
    {
        sprintf(pj->dcp.assetmap.filename, "%s%s", pj->tmp_dcp_dir, "ASSETMAP");
        sprintf(pj->dcp.volindex.filename, "%s%s", pj->tmp_dcp_dir, "VOLINDEX");
    }

    if (qdcp->verbosity >= VERBOSE_VERBOSE)
	{
		printf("size : %s\n", cpl.size);
		printf("digest : %s\n", cpl.digest);
		printf("duration : %d\n", cpl.duration);
		printf("entry_point : %d\n", cpl.entry_point);
		printf("kind : %s\n", cpl.kind);
		printf("rating : %s\n", cpl.rating);
	}

    /*  Write CPL File */

    if (qdcp->verbosity >= VERBOSE_NORMAL)
        printf("%s%s%sEcriture du fichier CPL%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, CLI_COLOR);
    
    if (write_cpl(pj, &pj->dcp.pkl[0].cpl[0]) != QDCP_SUCCESS)
    {
		warning("Writing composition playlist failed");
		return QDCP_FAILURE;
	}
	
    /*  Write PKL File */

    if (qdcp->verbosity >= VERBOSE_NORMAL) 
        printf("%s%s%sEcriture du fichier PKL%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, CLI_COLOR);

    if (write_pkl(pj, &pj->dcp.pkl[0]) != QDCP_SUCCESS)
    {
		warning("Writing packing list failed"); 
		return QDCP_FAILURE;
	}
	
    /* Write VOLINDEX File */
    
    if (qdcp->verbosity >= VERBOSE_NORMAL)
		printf("%s%s%sEcriture du fichier VOLINDEX%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, CLI_COLOR);

    if (write_volumeindex(pj) != QDCP_SUCCESS)
    { 
		warning("Writing volume index failed");
		return QDCP_FAILURE;
	}

    /* Write ASSETMAP File */
    
    if (qdcp->verbosity >= VERBOSE_NORMAL)
		printf("%s%s%sEcriture du fichier ASSETMAP%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, CLI_COLOR);

    if (write_assetmap(pj) != QDCP_SUCCESS)
	{
		warning("Writing asset map failed");
		return QDCP_FAILURE;
	}
   
    return QDCP_SUCCESS;
}

qdcp_bool
init_asset(t_asset *asset)
{
    memset(asset, 0, sizeof(t_asset));

    return QDCP_SUCCESS;
}

qdcp_bool 
new_asset(t_project * pj, t_asset * asset, char *filename, int verbosity) 
{
		struct 	stat st;
		FILE  *	fp;
		int    	result;
		char	absolute_file[PATH_STR_LEN];

    if (verbosity >= VERBOSE_NORMAL)
		printf("%s%s%sAjout d'un contenu : %s%s\n", 
			CLI_INDENT, CLI_GREEN, CLI_INDENT, filename, CLI_COLOR);

	sprintf(absolute_file, "%s%s", pj->tmp_dcp_dir, filename);

    /* check if file exists */
    if ((fp = fopen(absolute_file, "r")) == NULL) {
        warning("new_asset: Could not open file: %s (QDCP_FILEOPEN_ERROR)\n", absolute_file);
        return QDCP_FAILURE;
    } else {
        fclose (fp);
    }

	/* allocate the asset (declared in finalize_dcp) */
    init_asset(asset);

    /* asset : filename */
    sprintf(asset->name, "%s", filename);
    /* asset : filename */
    sprintf(asset->filename, "%s", filename);
    /* asset : filepath */
    sprintf(asset->filepath, "%s", absolute_file);
	/* asset : annotation */
    sprintf(asset->annotation, "%s", filename);

    /* asset : get file size */
    stat(absolute_file, &st);
    //~ sprintf(asset->size, "%"PRIu64, st.st_size);
    sprintf(asset->size, "%jd", (intmax_t)st.st_size);

    /* deduce asset information from the file itself (<- read_asset_info, asdcp_intf.cpp) 
     * such as :
     * 	asset.essence_type
     * 	asset.essence_class
     * 	asset.duration
     * 	asset.intrinsic_duration
     * 	asset.entry_point
     * 	asset.xml_ns
     * 	asset.uuid
     * 	asset.aspect_ratio
     * 	asset.edit_rate
     * 	asset.sample_rate
     * 	asset.frame_rate
     * 	asset.encrypted 
     */
    if (read_asset_info(asset) == QDCP_FAILURE) {
        warning("%s is not a proper essence file", filename);
        return QDCP_FAILURE;
    }

    /* force aspect ratio (as a float) */
    //~ sprintf(asset->aspect_ratio, "%.2f", pj->mx_w / pj->mx_h);
	
	//~ total = strtoimax(asset.size, NULL, 10) / read_size;

	/* get and store the SHA1 digest of the file */
	calculate_digest(pj, asset->filepath, asset->digest);

	if (verbosity >= VERBOSE_VERBOSE)
		show_asset_params(asset);
		
    return QDCP_SUCCESS;
}

void
show_asset_params(t_asset * asset)
{
	printf("%s filename : %s\n", 			CLI_INDENT, asset->filename);
	printf("%s aspect_ratio : %s\n", 		CLI_INDENT, asset->aspect_ratio);
	printf("%s duration : %d\n", 			CLI_INDENT, asset->duration);
	printf("%s essence_class : %d\n", 		CLI_INDENT, asset->essence_class);
	printf("%s essence_type : %d\n", 		CLI_INDENT, asset->essence_type);

	printf("%s intrinsic_duration : %d\n", 	CLI_INDENT, asset->intrinsic_duration);
	printf("%s entry_point : %d\n", 		CLI_INDENT, asset->entry_point);
	printf("%s xml_ns : %d\n", 				CLI_INDENT, asset->xml_ns);
	printf("%s stereoscopic : %d\n", 		CLI_INDENT, asset->stereoscopic);
	printf("%s name : %s\n", 				CLI_INDENT, asset->name);
	printf("%s size : %s\n", 				CLI_INDENT, asset->size);
	printf("%s annotation : %s\n", 			CLI_INDENT, asset->annotation);
	printf("%s edit_rate : %s\n", 			CLI_INDENT, asset->edit_rate);
	printf("%s frame_rate : %s\n", 			CLI_INDENT, asset->frame_rate);
	printf("%s sample_rate : %s\n", 		CLI_INDENT, asset->sample_rate);
	printf("%s digest : %s\n", 				CLI_INDENT, asset->digest);
	printf("%s encrypted : %d\n", 			CLI_INDENT, asset->encrypted);
	printf("%s key_id : %s\n", 				CLI_INDENT, asset->key_id);
	
	return;
}

qdcp_bool 
is_valid_asset(t_asset asset) {

    if (asset.essence_class == ACT_PICTURE ||
        asset.essence_class == ACT_SOUND ||
        asset.essence_class == ACT_TIMED_TEXT ) {

        return QDCP_YES;
    }

    return QDCP_NO;
}

/* Create an empty reel  */
void 
create_reel(t_project * pj, t_reel *reel) 
{
		char uuid_s[40];

    memset(reel, 0, sizeof(t_reel));

    /* Generate UUIDs */
    uuid_random(uuid_s);
    sprintf(reel->uuid, "%.36s", uuid_s);
    
    sprintf(reel->annotation, "%s", pj->dcp.annotation);

	return;
}

qdcp_bool 
validate_reel(t_project * pj, t_reel *reel, int reel_number) 
{
		int d = 0;
		int picture = 0;
		int duration_mismatch = 0;
		UNUSED(pj);

    /* change reel to 1 based for user */
    reel_number += 1;

    //~ QDCP_LOG(LOG_DEBUG, "validate_reel: validating reel %d", reel_number);

	/* recall of reel struct members : */
		//~ char           uuid[40];
		//~ char           annotation[128];
		//~ int            asset_count;
		//~ t_asset        main_picture;
		//~ t_asset        main_sound;
		//~ t_asset        main_subtitle;
	
    /* check if reel has a (valid) picture track (must have at least one) */
    if (reel->main_picture.essence_class == ACT_PICTURE) {
        picture++;
    }

    if (picture < 1) {
        //~ QDCP_LOG(LOG_ERROR, "Reel %d has no picture track", reel_number);
        warning("Reel has no picture track");

        return QDCP_FAILURE;
    }

    /* check reel's assets durations */
    d = reel->main_picture.duration;

    if (reel->main_sound.duration && reel->main_sound.duration != d) {
        duration_mismatch = 1;

        if (reel->main_sound.duration < d) {
            d = reel->main_sound.duration;
        }
    }

    if (reel->main_subtitle.duration && reel->main_subtitle.duration != d) {
        duration_mismatch = 1;

        if (reel->main_subtitle.duration < d) {
            d = reel->main_subtitle.duration;
        }
    }

    if (duration_mismatch) {
        reel->main_picture.duration = d;
        reel->main_sound.duration = d;
        reel->main_subtitle.duration = d;
        //~ QDCP_LOG(LOG_WARN, "Asset duration mismatch, adjusting all durations to shortest asset duration of %d frames", d);
    }

    return QDCP_SUCCESS;
}

qdcp_bool 
add_asset_to_reel(t_project * pj, t_reel *reel, t_asset asset) 
{
		int result;

    //~ QDCP_LOG(LOG_INFO, "Adding asset to reel");

    if (pj->ns == XML_NS_UNKNOWN) {
        pj->ns = asset.xml_ns;
        //~ QDCP_LOG(LOG_DEBUG, "add_asset_to_reel: Label type detected: %d", pj->ns);
        printf("add_asset_to_reel: Label type detected: %d\n", pj->ns);
        
    } else {
        if (pj->ns != asset.xml_ns) {
            warning("Warning DCP specification mismatch in assets. \nPlease make sure all assets are MXF Interop or SMPTE");
            return QDCP_FAILURE;
        }
    }

	result = get_asset_type(asset);

    switch (result) {
        case ACT_PICTURE:
            //~ QDCP_LOG(LOG_DEBUG, "add_asset_to_reel: adding picture");
            reel->main_picture = asset;
            break;

        case ACT_SOUND:
            //~ QDCP_LOG(LOG_DEBUG, "add_asset_to_reel: adding sound");
            reel->main_sound = asset;
            break;

        case ACT_TIMED_TEXT:
            //~ QDCP_LOG(LOG_DEBUG, "add_asset_to_reel: adding subtitle");
            reel->main_subtitle = asset;
            break;

        default:
            return QDCP_FAILURE;
    }

    return QDCP_SUCCESS;
}

/**
 * Creation of the CPL (Content PlayList) file of the DCP.
 * Initialization step, with default values, before we fill it with more info.
 */
void 
create_cpl(t_project * pj, t_cpl *cpl, quickdcp_ctx * qdcp) 
{
		char uuid_s[40];

    memset(cpl, 0, sizeof(t_cpl));

    uuid_random(uuid_s);
    sprintf(cpl->uuid, 			"%.36s", uuid_s);
    sprintf(cpl->annotation,	"%s", pj->dcp.annotation);
    strcpy(cpl->issue_date,  	pj->dcp.issue_date);
    strcpy(cpl->issuer,     	pj->issuer);
    strcpy(cpl->creator,    	pj->dcp.creator);
    strcpy(cpl->title,      	pj->dcp.title);
    strcpy(cpl->kind,       	qdcp->dcp_categories[pj->category]);
    // ContentVersion : Id, LabelText
    strcpy(cpl->rating,     	pj->dcp.rating);
    cpl->reel_count = 0;

    /* Generate XML filename */
    if ( !strcmp(pj->name, "") || !strcmp(pj->name, DEFAULT_PROJECT_NAME) ) {
        sprintf(cpl->filename, "%sCPL_%.20s_%.40s.xml", 
				pj->tmp_dcp_dir, pj->name, cpl->uuid);
    } else {
        sprintf(cpl->filename, "%sCPL_%.40s.xml",
				pj->tmp_dcp_dir, cpl->uuid);
    }

    return;
}

void 
add_reel_to_cpl(t_cpl *cpl, t_reel reel) 
{
    memcpy(&cpl->reel[cpl->reel_count], &reel, sizeof(t_reel));
    cpl->reel_count++;

	return;
}

/*
 * Init of the PKL (Packing List) file of the DCP.
 */
void 
create_pkl(t_project * pj, t_pkl *pkl, quickdcp_ctx * qdcp) 
{
		char uuid_s[40];

    memset(pkl, 0, sizeof(t_pkl));

    sprintf(pkl->annotation, 	"%s", pj->dcp.annotation);
    strcpy(pkl->timestamp,  	pj->dcp.issue_date);
    strcpy(pkl->issuer, 		pj->issuer);
    pkl->cpl_count = 0;

    /* Generate UUIDs */
    uuid_random(uuid_s);
    sprintf(pkl->uuid, "%.36s", uuid_s);
    
    /* Generate XML filename */
    if (strcmp(pj->name, "") == 0) 
    {
        sprintf(pkl->filename, "%sPKL_%.40s.xml", 
				pj->tmp_dcp_dir, pkl->uuid);
    }
    else
    {
        sprintf(pkl->filename, "%sPKL_%.20s_%.40s.xml", 
				pj->tmp_dcp_dir, pj->name, pkl->uuid);
    }

    return;
}

void
add_cpl_to_pkl(t_pkl *pkl, t_cpl cpl) 
{

    memcpy(&pkl->cpl[pkl->cpl_count], &cpl, sizeof(t_cpl));
    pkl->cpl_count++;

    return;
}

void 
add_pkl_to_dcp(t_dcp *dcp, t_pkl pkl)
{
    memcpy(dcp[dcp->pkl_count].pkl, &pkl, sizeof(t_pkl));
    dcp->pkl_count++;

    return;
}


char *
get_aspect_ratio(char *dimension_string) {

    char *p, *ratio;
    int n, d;
    float a = 0.0;

    ratio = malloc(sizeof(char) * 5);
    p = malloc(strlen(dimension_string) + 1);
    strcpy(p, dimension_string);
    n = atoi(strsep(&p, " "));
    d = atoi(strsep(&p, " "));

    if (d > 0) {
        a = (n * 1.00) / (d * 1.00);
    }

    if ( a >= 1.77 && a <= 1.78) {
        a = 1.77;
    }

    sprintf(ratio, "%-3.2f", a);

    return(ratio);
}



/*------------------------------ XML proper --------------------------*/

int 
write_cpl_list(t_project * pj) 
{
    UNUSED(pj);
    int placeholder = 0;
    return placeholder;
}

qdcp_bool 
write_cpl(t_project * pj, t_cpl *cpl)
{
		int 				r, rc;
		struct 				stat st;
		xmlIndentTreeOutput = 1;
		xmlDocPtr        	doc;
		xmlTextWriterPtr 	xml;

    /* create XML document */
    xml = xmlNewTextWriterDoc(&doc, 0);

    /* cpl start */
    rc = xmlTextWriterStartDocument(xml, NULL, XML_ENCODING, NULL);
    if (rc < 0) {
        warning("xmlTextWriterStartDocument failed");
        return QDCP_FAILURE;
    }

    xmlTextWriterStartElement(xml, BAD_CAST "CompositionPlaylist");
    xmlTextWriterWriteFormatAttribute(xml, BAD_CAST "xmlns", "%s", NS_CPL);

    if (pj->xml_signature.sign) {
        xmlTextWriterWriteFormatAttribute(xml, BAD_CAST "xmlns:dsig", "%s", DS_DSIG);
    }

    /* cpl attributes */
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", cpl->uuid);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "AnnotationText", "%s", cpl->annotation);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "IssueDate", "%s", pj->dcp.issue_date);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Issuer", "%s", pj->dcp.issuer);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Creator", "%s", pj->dcp.creator);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "ContentTitleText", "%s", cpl->title);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "ContentKind", "%s", cpl->kind);

    /* content version */
    if (pj->ns == XML_NS_SMPTE) {
        xmlTextWriterStartElement(xml, BAD_CAST "ContentVersion");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uri:%s_%s", cpl->uuid, cpl->issue_date);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "LabelText", "%s_%s", cpl->uuid, cpl->issue_date);
        xmlTextWriterEndElement(xml);
    }

    /* rating */
    xmlTextWriterStartElement(xml, BAD_CAST "RatingList");

    if (strcmp(cpl->rating, "")) {
        xmlTextWriterStartElement(xml, BAD_CAST "Rating");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Agency", "%s", RATING_AGENCY);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Label", "%s", cpl->rating);
        xmlTextWriterEndElement(xml);
    }

    xmlTextWriterEndElement(xml);

    /* reel(s) Start */
    xmlTextWriterStartElement(xml, BAD_CAST "ReelList");

    for (r = 0; r < cpl->reel_count; r++) {
        t_reel reel = cpl->reel[r];
        xmlTextWriterStartElement(xml, BAD_CAST "Reel");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", reel.uuid);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "AnnotationText", "%s", reel.annotation);
        xmlTextWriterStartElement(xml, BAD_CAST "AssetList");

        /* write picture first, unless stereoscopic */
        if (cpl->reel[r].main_picture.stereoscopic) {
            write_cpl_asset(pj, xml, cpl->reel[r].main_sound);
            write_cpl_asset(pj, xml, cpl->reel[r].main_subtitle);
            write_cpl_asset(pj, xml, cpl->reel[r].main_picture);
        } else {
            write_cpl_asset(pj, xml, cpl->reel[r].main_picture);
            write_cpl_asset(pj, xml, cpl->reel[r].main_sound);
            write_cpl_asset(pj, xml, cpl->reel[r].main_subtitle);
        }

        xmlTextWriterEndElement(xml);     /* end assetlist */
        xmlTextWriterEndElement(xml);     /* end reel */
    }

    xmlTextWriterEndElement(xml);         /* end reel list */

	
    if (pj->xml_signature.sign) {
        write_dsig_template(pj, xml);
    }

    xmlTextWriterEndElement(xml);         /* end compositionplaylist */

    rc = xmlTextWriterEndDocument(xml);

    if (rc < 0) {
        warning("xmlTextWriterEndDocument failed %s", cpl->filename);
        return QDCP_FAILURE;
    }

    xmlFreeTextWriter(xml);
    xmlSaveFormatFile(cpl->filename, doc, 1);
    xmlFreeDoc(doc);

    /* sign the XML file */
    if (pj->xml_signature.sign) {
        xml_sign(pj, cpl->filename);
    }

    /* store CPL file size */
    printf("writing CPL file info\n");
    
    stat(cpl->filename, &st);
    //~ sprintf(cpl->size, "%"PRIu64, st.st_size);
    sprintf(cpl->size, "%jd", (intmax_t)st.st_size);
    calculate_digest(pj, cpl->filename, cpl->digest);

    return QDCP_SUCCESS;
}

qdcp_bool 
write_cpl_asset(t_project * pj, xmlTextWriterPtr xml, t_asset asset) 
{
	/* we don't interrupt the process for an invalid asset
	 * we just ignore it (yet with a warning). */
    if (is_valid_asset(asset) == QDCP_NO) {
		//~ warning("CPL : Asset %s of class %d is invalid", asset.filename, asset.essence_class);
        return QDCP_SUCCESS;
    }

	//~ printf("%sWe're here write_cpl_asset : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

    if (asset.essence_class == ACT_PICTURE) {
        if (asset.stereoscopic) {
            xmlTextWriterStartElement(xml, BAD_CAST "msp-cpl:MainStereoscopicPicture");
            xmlTextWriterWriteAttribute(xml, BAD_CAST "xmlns:msp-cpl", BAD_CAST NS_CPL_3D);
        } else {
            xmlTextWriterStartElement(xml, BAD_CAST "MainPicture");
        }
    } else if (asset.essence_class == ACT_SOUND) {
        xmlTextWriterStartElement(xml, BAD_CAST "MainSound");
    } else if (asset.essence_class == ACT_TIMED_TEXT) {
        xmlTextWriterStartElement(xml, BAD_CAST "MainSubtitle");
    } else {
		/* we don't stop */
        return QDCP_SUCCESS;
    }

    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", asset.uuid);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "AnnotationText", "%s", asset.annotation);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "EditRate", "%s", asset.edit_rate);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "IntrinsicDuration", "%d", asset.intrinsic_duration);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "EntryPoint", "%d", asset.entry_point);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Duration", "%d", asset.duration);
    if ( asset.encrypted ) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "KeyId", "urn:uuid:%s", asset.key_id);
    }
    if ( pj->dcp.digest_flag ) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Hash", "%s", asset.digest);
    }

    if (asset.essence_class == ACT_PICTURE) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "FrameRate", "%s", asset.frame_rate);

        if (pj->ns == XML_NS_SMPTE) {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "ScreenAspectRatio", "%s", asset.aspect_ratio);
        } else {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "ScreenAspectRatio", "%s", get_aspect_ratio(asset.aspect_ratio));
        }
    }

    xmlTextWriterEndElement(xml); /* end asset */

    return QDCP_SUCCESS;
}

int 
write_pkl_list(t_project * pj) 
{
    UNUSED(pj);
    int placeholder = 0;
    return placeholder;
}

qdcp_bool 
write_pkl(t_project * pj, t_pkl *pkl) 
{
		int r, c, rc;
		struct stat st;
		xmlIndentTreeOutput = 1;
		xmlDocPtr        doc;
		xmlTextWriterPtr xml;

    /* create XML document */
    xml = xmlNewTextWriterDoc(&doc, 0);

	//~ printf("%sWe're here write_pkl : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);
	
    /* pkl start */
    rc = xmlTextWriterStartDocument(xml, NULL, XML_ENCODING, NULL);

    if (rc < 0) {
        warning("xmlTextWriterStartDocument failed");
        return QDCP_FAILURE;
    }

    xmlTextWriterStartElement(xml, BAD_CAST "PackingList");
    xmlTextWriterWriteAttribute(xml, BAD_CAST "xmlns", BAD_CAST NS_PKL);

    if (pj->xml_signature.sign) {
        xmlTextWriterWriteAttribute(xml, BAD_CAST "xmlns:dsig", BAD_CAST DS_DSIG);
    }

    /* pkl attributes */
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", pkl->uuid);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "AnnotationText", "%s", pkl->annotation);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "IssueDate", "%s", pj->dcp.issue_date);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Issuer", "%s", pj->dcp.issuer);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Creator", "%s", pj->dcp.creator);

    //~ printf("cpls: %d\n", pkl->cpl_count);

    /* asset(s) Start */
    xmlTextWriterStartElement(xml, BAD_CAST "AssetList");


    for (c = 0; c < pkl->cpl_count; c++) {
        
			t_cpl cpl = pkl->cpl[c];
        
        //~ printf("reels: %d", cpl.reel_count);

		//~ printf("%sWe're here write_pkl : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

        for (r = 0; r < cpl.reel_count; r++) {
            write_pkl_asset(pj, xml, cpl.reel[r].main_picture);
            write_pkl_asset(pj, xml, cpl.reel[r].main_sound);
            write_pkl_asset(pj, xml, cpl.reel[r].main_subtitle);
        }

       /* cpl */
        xmlTextWriterStartElement(xml, BAD_CAST "Asset");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", cpl.uuid);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Hash", "%s", cpl.digest);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Size", "%s", cpl.size);

        if (pj->ns == XML_NS_SMPTE) {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "text/xml");
        } else {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "text/xml;asdcpKind=CPL");
        }

		//~ printf("%sWe're here write_pkl : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

        xmlTextWriterWriteFormatElement(xml, BAD_CAST "OriginalFileName", "%s", basename(cpl.filename));
        xmlTextWriterEndElement(xml);      /* end cpl asset */
    }

    xmlTextWriterEndElement(xml);      /* end assetlist */

    if (pj->xml_signature.sign) {
        write_dsig_template(pj, xml);
    }

    xmlTextWriterEndElement(xml);      /* end packinglist */

    rc = xmlTextWriterEndDocument(xml);

    if (rc < 0) {
        warning("xmlTextWriterEndDocument failed %s", pkl->filename);
        return QDCP_FAILURE;
    }

	//~ printf("%sWe're here write_pkl : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

    xmlFreeTextWriter(xml);
    xmlSaveFormatFile(pkl->filename, doc, 1);
    xmlFreeDoc(doc);

    /* sign the XML file */
    if (pj->xml_signature.sign) {
        xml_sign(pj, pkl->filename);
    }

    /* store PKL file size */
    stat(pkl->filename, &st);
    //~ sprintf(pkl->size, "%"PRIu64, st.st_size);
    sprintf(pkl->size, "%jd", (intmax_t)st.st_size);

	//~ printf("%sWe're here write_pkl : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

    return QDCP_SUCCESS;
}

qdcp_bool 
write_pkl_asset(t_project * pj, xmlTextWriterPtr xml, t_asset asset) 
{
	//~ printf("%sWe're here write_pkl_asset : l. %d %s\n", CLI_RED, __LINE__, CLI_COLOR);

	/* we don't interrupt the process for an invalid asset
	 * we just ignore it (yet with a warning). 
	 * That means actually that we won't stop for a missing sound or timed_text asset.
	 * */
    if (is_valid_asset(asset) == QDCP_NO) {
        return QDCP_SUCCESS;
    }

    xmlTextWriterStartElement(xml, BAD_CAST "Asset");
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", asset.uuid);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "AnnotationText", "%s", asset.annotation);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Hash", "%s", asset.digest);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Size", "%s", asset.size);

    if (pj->ns == XML_NS_SMPTE) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "application/mxf");
    } else {
        if (asset.essence_class == ACT_PICTURE) {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "application/x-smpte-mxf;asdcpKind=Picture");
        } else if (asset.essence_class == ACT_SOUND) {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "application/x-smpte-mxf;asdcpKind=Sound");
        } else if (asset.essence_class == ACT_TIMED_TEXT) {
            xmlTextWriterWriteFormatElement(xml, BAD_CAST "Type", "%s", "application/x-smpte-mxf;asdcpKind=Subtitle");
        } else {
			/* we don't stop... */
            return QDCP_SUCCESS;
        }
    }

    xmlTextWriterWriteFormatElement(xml, BAD_CAST "OriginalFileName", "%s", asset.filename);
    xmlTextWriterEndElement(xml);      /* end asset */

    return QDCP_SUCCESS;
}

qdcp_bool 
write_assetmap_asset(xmlTextWriterPtr xml, t_asset asset) 
{
	/* we don't interrupt the process for an invalid asset
	 * we just ignore it (yet with a warning). */
    if (is_valid_asset(asset) == QDCP_NO) {
		//~ warning("ASSETMAP : Asset %s of class %d is invalid", asset.filename, asset.essence_class);
        return QDCP_SUCCESS;
    }

    if (asset.uuid != NULL) {
        xmlTextWriterStartElement(xml, BAD_CAST "Asset");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", asset.uuid);
        xmlTextWriterStartElement(xml, BAD_CAST "ChunkList");
        xmlTextWriterStartElement(xml, BAD_CAST "Chunk");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Path", "%s", basename(asset.filename));
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "VolumeIndex", "%d", 1);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Offset", "%d", 0);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Length", "%s", asset.size);
        xmlTextWriterEndElement(xml); /* end chunk */
        xmlTextWriterEndElement(xml); /* end chunklist */
        xmlTextWriterEndElement(xml); /* end cpl asset */
    }

    return QDCP_SUCCESS;
}

qdcp_bool 
write_assetmap(t_project * pj) 
{
		xmlIndentTreeOutput = 1;
		xmlDocPtr        doc;
		xmlTextWriterPtr xml;
		int              c, r, rc;
		char             uuid_s[40];
		t_reel           reel;

		t_assetmap assetmap = pj->dcp.assetmap;

    /* generate assetmap UUID */
    uuid_random(uuid_s);

    //~ printf("writing ASSETMAP file %.256s\n", assetmap.filename);

    /* create XML document */
    xml = xmlNewTextWriterDoc(&doc, 0);

    /* assetmap XML Start */
    rc = xmlTextWriterStartDocument(xml, NULL, XML_ENCODING, NULL);

    if (rc < 0) {
        warning("xmlTextWriterStartDocument failed");
        return QDCP_FAILURE;
    }

    xmlTextWriterStartElement(xml, BAD_CAST "AssetMap");
    xmlTextWriterWriteAttribute(xml, BAD_CAST "xmlns", BAD_CAST NS_AM);

    /* assetmap attributes */
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", uuid_s);

    if (pj->ns == XML_NS_SMPTE) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Creator", "%s", pj->dcp.creator);
    }

    xmlTextWriterWriteFormatElement(xml, BAD_CAST "VolumeCount", "%d", 1);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "IssueDate", "%s", pj->dcp.issue_date);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Issuer", "%s", pj->dcp.issuer);

    if (pj->ns == XML_NS_INTEROP) {
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Creator", "%s", pj->dcp.creator);
    }

    xmlTextWriterStartElement(xml, BAD_CAST "AssetList");

    //~ printf("writing ASSETMAP PKL\n");

    /* PKL */
    t_pkl pkl = pj->dcp.pkl[0];
    xmlTextWriterStartElement(xml, BAD_CAST "Asset");
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", pkl.uuid);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "PackingList", "%s", "true");
    xmlTextWriterStartElement(xml, BAD_CAST "ChunkList");
    xmlTextWriterStartElement(xml, BAD_CAST "Chunk");
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Path", "%s", strip_path(pkl.filename));
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "VolumeIndex", "%d", 1);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Offset", "%d", 0);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Length", "%s", pkl.size);
    xmlTextWriterEndElement(xml); /* end chunk */
    xmlTextWriterEndElement(xml); /* end chunklist */
    xmlTextWriterEndElement(xml); /* end pkl asset */

    //~ printf("writing ASSETMAP CPLs\n");

    /* CPL */
    for (c = 0; c < pkl.cpl_count; c++) {
        t_cpl cpl = pkl.cpl[c];
        xmlTextWriterStartElement(xml, BAD_CAST "Asset");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Id", "urn:uuid:%s", cpl.uuid);
        xmlTextWriterStartElement(xml, BAD_CAST "ChunkList");
        xmlTextWriterStartElement(xml, BAD_CAST "Chunk");
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Path", "%s", basename(cpl.filename));
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "VolumeIndex", "%d", 1);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Offset", "%d", 0);
        xmlTextWriterWriteFormatElement(xml, BAD_CAST "Length", "%s", cpl.size);
        xmlTextWriterEndElement(xml); /* end chunk */
        xmlTextWriterEndElement(xml); /* end chunklist */
        xmlTextWriterEndElement(xml); /* end cpl asset */

        /* assets(s) start */
        for (r = 0; r < cpl.reel_count; r++) {
            reel = cpl.reel[r];

            write_assetmap_asset(xml, reel.main_picture);
            write_assetmap_asset(xml, reel.main_sound);
            write_assetmap_asset(xml, reel.main_subtitle);
        }
    }

    xmlTextWriterEndElement(xml); /* end assetlist */
    xmlTextWriterEndElement(xml); /* end assetmap */

    rc = xmlTextWriterEndDocument(xml);

    if (rc < 0) {
        warning("xmlTextWriterEndDocument failed %s", pj->dcp.assetmap.filename);
        return QDCP_FAILURE;
    }

    xmlFreeTextWriter(xml);
    xmlSaveFormatFile(assetmap.filename, doc, 1);
    xmlFreeDoc(doc);

    return QDCP_SUCCESS;
}

qdcp_bool 
write_volumeindex(t_project * pj) 
{

		xmlIndentTreeOutput = 1;
		xmlDocPtr        doc;
		xmlTextWriterPtr xml;
		int              rc;

		t_volindex volindex = pj->dcp.volindex;

    //~ QDCP_LOG(LOG_INFO, "writing VOLINDEX file %.256s", volindex.filename);

    /* create XML document */
    xml = xmlNewTextWriterDoc(&doc, 0);

    /* volumeindex XML Start */
    rc = xmlTextWriterStartDocument(xml, NULL, XML_ENCODING, NULL);

    if (rc < 0) {
        warning("xmlTextWriterStartDocument failed");
        return QDCP_FAILURE;
    }

    xmlTextWriterStartElement(xml, BAD_CAST "VolumeIndex");
    xmlTextWriterWriteAttribute(xml, BAD_CAST "xmlns", BAD_CAST NS_AM);
    xmlTextWriterWriteFormatElement(xml, BAD_CAST "Index", "%d", 1);
    xmlTextWriterEndElement(xml);

    rc = xmlTextWriterEndDocument(xml);

    if (rc < 0) {
        warning("xmlTextWriterEndDocument failed");
        return QDCP_FAILURE;
    }

    xmlFreeTextWriter(xml);
    xmlSaveFormatFile(volindex.filename, doc, 1);
    xmlFreeDoc(doc);

    return QDCP_SUCCESS;
}

