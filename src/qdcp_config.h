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
   @file    config.h
   @author  jean.sabatier@zanobox.net
   @brief   Build Digital Cinema Packages in command line interface.
*/

#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "quickdcp.h"
#include "iniparser.h"
#include "dictionary.h"


/**---------------------- *
 * Config file operations *
 * ---------------------- */

//~ /**
 //~ * Init config struct with default projects values
 //~ */
//~ qdcp_bool 
//~ init_default_config(
	//~ quickdcp_ctx * qdcp
	//~ );

/**
 * Read or create a config file and pass it to a config_t structure
 */
qdcp_bool 
read_config_file(
	const char * config_file, 
	quickdcp_ctx * qdcp
	);

/**
 * Write a new config file from current values
 */
qdcp_bool 
write_config_file(
	char const config_path[], 
	quickdcp_ctx * qdcp
	);

/**
 * Display all projects related default parameters
 */
void 
show_default_params(
	quickdcp_ctx * qdcp, 
	qdcp_bool show_all
	);


#endif // CONFIG_H_INCLUDED
