/*
 * arguments.h
 *
 * This file is part of genpass.
 *
 * genpass is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * genpass is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with genpass.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $Id$
 * $URL$
 */

#ifndef ARGUMENTS_H
#define ARGUMENTS_H 1

#include "config.h"

#define MAXLINE 255

/* Define the maximum number of config files.
 * Not counting the one given from commandline.
 * The one from commandline is at position MAXFILES in the array.
 */
#define MAXFILES 2
char *configfile[MAXFILES+1];

#ifndef CONFDIR
#define CONFDIR "/usr/local"
#endif /* CONFDIR */

struct arguments {
	int verbose;
	int count;
	int length;
	int upper;
	int lower;
	int digits;
	int extras;
	int seed;
} *arguments;

struct arguments *final;
struct arguments *file_args;

void foutje(void);
void printsettings(struct arguments *);
void printsettingserr(struct arguments *);

int readConfFile(char *, struct arguments *);

int parse_arg(int , char **);

void set_default(struct arguments *);
void set_empty(struct arguments *);

#endif /* ARGUMENTS_H */
