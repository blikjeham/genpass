/*
 *    genpass - a random password generator
 *    Copyright (C) 2010 Jabik Postmus and Sjoerd Job Postmus
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * main.c
 * 
 * For the GenPas project
 *
 * $Id: main.c,v 1.16 2011/04/17 11:32:45 jabik Exp $
 */

#include "../config.h"
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "arguments.h"
#include "genpass.h"


int main(int argc, char *argv[]) 
{
	extern struct arguments *arguments;
	arguments = malloc(sizeof(struct arguments));
	memset(arguments, 0, sizeof(struct arguments));

	extern struct arguments *file_args;
	file_args = malloc(sizeof(struct arguments));
	memset(file_args, 0, sizeof(struct arguments));

	extern struct arguments *final;
	final = malloc(sizeof(struct arguments));
	memset(final, 0, sizeof(struct arguments));

	/* for the stat part we need this one. We don't use it,
	 * but we need it. */
	struct stat *bluf= malloc(sizeof(struct stat));

	int seed,i;
	int ret;
	char *filename = "/genpass.conf";

	/* Fill the var configfile */
	configfile[0]=malloc(strlen(CONFDIR)+strlen(filename)+1);
	strncat(configfile[0], CONFDIR, strlen(CONFDIR));
	strncat(configfile[0], filename, strlen(filename));
	configfile[1]=strcat(getenv("HOME"),"/.genpass");
	
	/* Set the default values. */
	set_default(final);
	set_empty(arguments);

	if (parse_arg(argc, argv))
	    exit(1);

	/* 
	 * Now we will read the configfiles.
	 * Possibilities are stored in char *configfile[MAXFILES].
	 *
	 * First we will stat() the files.
	 * Then read the configfile.
	 * Then we have to process the settings for each file.
	 * As you can see the last one overrides the previous ones,
	 * with the one from commandline as supreme ruler.
	 */
	for (i=0;i<=MAXFILES;i++) {
		if (configfile[i] != NULL) {
			verbose("Trying configfile: %s ...",
				configfile[i]);
			ret = stat(configfile[i], bluf);
			if (ret != -1) {
				verbose("ok\n");
				readConfFile(configfile[i], file_args);
				if (file_args == NULL)
					break;
				final = processSettings(file_args, final);
				if (arguments->verbose)
					printsettings(file_args);
			} else {
				verbose("no\n");
			}
		}
	}
	free(bluf);

	/*
	 * override configfile with commandline
	 * and free the file_args
	 */
	final = processSettings(arguments, final);
	free(file_args);
	
	/* 
	 * Check if there is an override in the seed.
	 * Overriding the seed should only be done to debug or test. 
	 */
	if (final->seed == 0) {
		/* 
		 * We don't want to seed with time entirely, but we have
		 * to do it for now.
		 * We are going to seed twice. Once with time, once with
		 * the output from
		 * getSeedFromDev
		 */
		srandom(time(NULL));
		seed = abs(getSeedFromDev(random() % 1024));
		srandom(seed);
	} else {
		seed = final->seed;
		srandom(seed);
	}

	if (!validsettings(final)) {
		fprintf(stderr,"Error: The sum of the tokens is larger than the desired lenght of the password.\nPlease fix this problem.\n");
		printsettingserr(final);
		exit(1);
	}
	
	if (arguments->verbose) {
		printsettings(final);
		printf("Random seed is: %d\n", seed);
		printf("\n");
	}

	for (i=0; i<final->count; i++) {
		printf("%s\n", genpass(final));
	}
	
	free(final);
	free(arguments);
	return 0;
}
